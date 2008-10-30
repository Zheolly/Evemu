/*
	------------------------------------------------------------------------------------
	LICENSE:
	------------------------------------------------------------------------------------
	This file is part of EVEmu: EVE Online Server Emulator
	Copyright 2006 - 2008 The EVEmu Team
	For the latest information visit http://evemu.mmoforge.org
	------------------------------------------------------------------------------------
	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU Lesser General Public License as published by the Free Software
	Foundation; either version 2 of the License, or (at your option) any later
	version.

	This program is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place - Suite 330, Boston, MA 02111-1307, USA, or go to
	http://www.gnu.org/copyleft/lesser.txt.
	------------------------------------------------------------------------------------
	Author:		Captnoord
*/

#include "EvemuPCH.h"

#include "ascent-shared/ByteBuffer.h"

// Dummy authorization handshake function
static const byte handshakeFunc[] = {
	0x74, 0x04, 0x00, 0x00, 0x00, 0x4E, 0x6F, 0x6E, 0x65	//marshaled Python string "None"
};

EveClientSocket::EveClientSocket(SOCKET fd) : Socket(fd, CLIENTSOCKET_SENDBUF_SIZE, CLIENTOCKET_RECVBUF_SIZE)
{
	mAuthed = false;
	mQueued = false;			// don't support authorization queue for now
	mNagleEanbled = false;		// don't enable 'nagle' for now

	mRemaining = 0;

	mRequest = NULL;
	mSession = NULL;

	mCurrentStateMachine = &EveClientSocket::_authStateHandshake;
}

EveClientSocket::~EveClientSocket()
{
	/* delete the buffers in the send buffer queue */
	// disabled for now as we don't have a send queue yet
	//WorldPacket * pck;
	//while((pck = _queue.Pop()))
	//	delete pck;

	/* delete the 'auth' request packet if it still exists */
	if (mRequest != NULL)
		delete mRequest;

	/* remove the link between the current socket and its client session */
	if (mSession)
	{
		mSession->SetSocket(NULL);
		mSession = NULL;
	}
}

void EveClientSocket::OnDisconnect()
{
	sSpace.mAcceptedConnections--;
	if(mSession)
	{
		mSession->SetSocket(0);
		mSession = NULL;
	}
}

// sends initial handshake containing the version info
void EveClientSocket::_sendHandShake()
{
	uint32 authCount = (uint32)sSpace.GetAuthorizedCount();

	VersionExchange version;
	version.birthday = EVEBirthday;
	version.macho_version = MachoNetVersion;
	version.user_count = authCount;
	version.version_number = EVEVersionNumber;
	version.build_version = EVEBuildVersion;
	version.project_version = EVEProjectVersion;

	//sLog.outDebug("%s: Sending Low Level Version Exchange:", GetRemoteIP().c_str());

	PyRep *rep = version.Encode();
	OutPacket(rep);
}

//send position in queue
void EveClientSocket::_sendQueuePos( int queuePos )
{
	// send a python integer back as a response on the queue query
	PyRepInteger packet(queuePos);
	OutPacket(&packet);
}

// 'auth' commands
//OK = Reading Client 'Crypto' Context OR Queue Check
//OK CC = 'Crypto' Context Complete
//CC = 'Crypto' Context

/* send handshake accept */
void EveClientSocket::_sendAccept()
{
	PyRepString packet("OK CC"); // doesn't seem to matter what we send... (at this point)
	OutPacket(&packet);
}

/* send a python integer as a responce on the password type query */
void EveClientSocket::_sendRequirePasswordType(int passwordType)
{
	PyRepInteger packet(passwordType);
	OutPacket(&packet);
}

/* low level packet sending, uses burst */
OUTPACKET_RESULT EveClientSocket::_outPacket(EVENetPacket * packet)
{
	uint32 len = packet->length;

	ByteBuffer packetcheck;
	packetcheck.append(packet->data, packet->length);

	//printf("\nsend:\n");
	//packetcheck.LogPacket();

	if(IsConnected() == false)
		return OUTPACKET_RESULT_NOT_CONNECTED;

	BurstBegin();
	if( GetWriteBuffer().GetSpace() < (len+4) )
	{
		BurstEnd();
		return OUTPACKET_RESULT_NO_ROOM_IN_BUFFER;
	}

	// Packet logger :)
	//sSpaceLog.LogPacket((uint32)len, opcode, (const uint8*)data, 1);

	// Pass the size of the packet to our send buffer
	bool rv = BurstSend((const uint8*)&len, 4);

	// Pass the rest of the packet to our send buffer (if there is any)
	if(len > 0 && rv == true)
	{
		rv = BurstSend((const uint8*)packet->data, (uint32)len);
	}

	if(rv == true)
	{
		BurstPush();
	}
	BurstEnd();
	return rv ? OUTPACKET_RESULT_SUCCESS : OUTPACKET_RESULT_SOCKET_ERROR;
}

// TODO loose the new packet and delete packet stuff..
/*  */
void EveClientSocket::OutPacket(PyRep * packet )
{
	EVENetPacket * netData = new EVENetPacket;
	netData->data = Marshal(packet, netData->length);
	_outPacket(netData);
	delete netData;
}

void EveClientSocket::OnConnect()
{
	sSpace.mAcceptedConnections++;
	//_latency = getMSTime();

	/* send initial 'auth' handshake */
	_sendHandShake();
}

void EveClientSocket::OnRead()
{
	for(;;)
	{
		//Check for the header if we don't have any bytes to wait for.
		if(mRemaining == 0)
		{
			if(GetReadBuffer().GetSize() < 4)
			{
				// No header in the packet, let's wait.
				return;
			}

			// copy the packet size from the buffer
			uint32 packetSize;
			GetReadBuffer().Read((void*)&packetSize, 4);
			mRemaining = packetSize;
		}

		if(mRemaining > 0)
		{
			if( GetReadBuffer().GetSize() < mRemaining )
			{
				// We have a fragmented packet. Wait for the complete one before proceeding.
				return;
			}
		}

		ByteBuffer * packet = NULL;

		/* TODO we can skip this new ByteBuffer by feeding the circular buffer into the 'unmarshal' function
		 * requires to redesign the 'unmarshal' function
		 */
		if(mRemaining > 0)
		{
			packet = new ByteBuffer;
			packet->resize(mRemaining);

			// Copy from packet buffer into our actual buffer.
			GetReadBuffer().Read(packet->data(), mRemaining);
			mRemaining = 0;
		}

		// packet log
		//sWorldLog.LogPacket(mSize, mOpcode, mSize ? Packet->contents() : NULL, 0);

		PyRep *recvPyPacket = InflateAndUnmarshal(packet->contents(), packet->size());

		printf("\nrecv packet with opcode:%d and Type:%s and size:%d\n", ((PyPacket*)recvPyPacket)->type, recvPyPacket->TypeString(), packet->size());
		packet->LogPacket();

		// the state machine magic
		(this->*mCurrentStateMachine)(recvPyPacket);

		// this is the end of the road for the used read buffer
		if ( packet != NULL)
		{
			delete packet;
		}
	}
}

// the client sends back its server info...
// we should compare this with our own to make sure we can block unsupported clients
void EveClientSocket::_authStateHandshake(PyRep* packet)
{
	VersionExchange ve;

	PyRep *fp = packet;
	if(!ve.Decode(&fp)) {
		//_log(NET__PRES_ERROR, "%s: Received invalid version exchange!", GetRemoteIP().c_str());
		Disconnect();
		return;
	}

	Log.Debug("ClientSocket","%s: Received Low Level Version Exchange:\n", GetRemoteIP().c_str());

	if(ve.birthday != EVEBirthday) {
		sLog.outDebug("%s: Client's birthday does not match ours!", GetRemoteIP().c_str());
		Disconnect();
	}
	if(ve.macho_version != MachoNetVersion) {
		sLog.outDebug("%s: Client's macho_version not match ours!", GetRemoteIP().c_str());
		Disconnect();
	}
	if(ve.version_number != EVEVersionNumber) {
		sLog.outDebug("%s: Client's version_number not match ours!", GetRemoteIP().c_str());
		Disconnect();
	}
	if(ve.build_version != EVEBuildVersion) {
		sLog.outDebug("%s: Client's build_version not match ours!", GetRemoteIP().c_str());
		Disconnect();
	}
	if(ve.project_version != EVEProjectVersion) {
		sLog.outDebug("%s: Client's project_version not match ours!", GetRemoteIP().c_str());
		Disconnect();
	}

	Log.Debug("AuthStateMachine","State changed into StateQueueCommand");
	mCurrentStateMachine = &EveClientSocket::_authStateQueueCommand;
}

void EveClientSocket::_authStateQueueCommand(PyRep* packet)
{
	//check if it actually is tuple
	if(!packet->CheckType(PyRep::Tuple)) {
		sLog.outDebug("%s: Invalid packet during waiting for command (tuple expected).", GetRemoteIP().c_str());
		Disconnect();
		return;
	}
	PyRepTuple *t = (PyRepTuple *)packet;
	
	//decode
	if(t->items.size() == 2)
	{
		//GetLogonQueuePosition

		//QC = Queue Check
		NetCommand_QC cmd;
		if(!cmd.Decode(&t)) {
			sLog.outDebug("%s: Failed to decode 2-arg command.", GetRemoteIP().c_str());
			Disconnect();
			return;//break;
		}
		if(cmd.command != "QC") {
			sLog.outDebug("%s: Unknown 2-arg command '%s'.", GetRemoteIP().c_str(), cmd.command.c_str());
			Disconnect();
			return;//break;
		}
		sLog.outDebug("%s: Got Queue Check command.", GetRemoteIP().c_str());
		

		//return position in queue, which is 1 for now until we implemented a real login queue
		_sendQueuePos(1);
		//_sendQueuePos(100);

		//now act like client just connected
		//send out handshake again
		_sendHandShake();

		//and set proper state
		Log.Debug("AuthStateMachine","State changed into StateHandshake");
		mCurrentStateMachine = &EveClientSocket::_authStateHandshake;
		return;
	} 
	else if(t->items.size() == 3) 
	{
		//this is sent when client is logging in
		NetCommand_VK cmd;
		if(!cmd.Decode(&t)) {
			sLog.outDebug("%s: Failed to decode 3-arg command.", GetRemoteIP().c_str());
			Disconnect();
			return;//break;
		}
		// VK == VIP mode
		if(cmd.command != "VK") {
			sLog.outDebug("%s: Unknown 3-arg command '%s'.", GetRemoteIP().c_str(), cmd.command.c_str());
			Disconnect();
			return;//break;
		}
		sLog.outDebug("%s: Got VK command, vipKey=%s.", GetRemoteIP().c_str(), cmd.vipKey.c_str());
		Log.Debug("AuthStateMachine","State changed into StateStateNoCrypto");
		mCurrentStateMachine = &EveClientSocket::_authStateNoCrypto;
		return;
	}
	else 
	{
		Log.Debug("EVE Socket","StateQueueCommand received a invalid packet");
		sLog.outDebug("%s: Received invalid command packet:", GetRemoteIP().c_str());
		Disconnect();
	}
}

void EveClientSocket::_authStateNoCrypto(PyRep* packet)
{
	CryptoRequestPacket cr;

	PyRep * data = packet;

	if(!cr.Decode(&data)) {
		sLog.outDebug("%S: Received invalid crypto request!", GetRemoteIP().c_str());
		Disconnect();
		return;//break;
	}

	if(cr.keyVersion == "placebo")
	{
		sLog.outDebug("%s: Received Placebo crypto request, accepting.", GetRemoteIP().c_str());
		Log.Debug("AuthStateMachine","State changed into StateCryptoChallenge");
		mCurrentStateMachine = &EveClientSocket::_authStateCryptoChallenge;

		_sendAccept();
	}
	else
	{
		//i'm sure cr.keyVersion can specify either CryptoAPI or PyCrypto, but its all binary so im not sure how.
		PyRep *params = cr.keyParams.Clone();
		CryptoAPIRequestParams car;
		if(!car.Decode(&params)) {
			sLog.outDebug("%s: Received invalid CryptoAPI request!", GetRemoteIP().c_str());
			Disconnect();
			return;
		}

		sLog.outDebug("%s: Unhandled CryptoAPI request: hashmethod=%s sessionkeylength=%d provider=%s sessionkeymethod=%s", GetRemoteIP().c_str(), car.hashmethod.c_str(), car.sessionkeylength, car.provider.c_str(), car.sessionkeymethod.c_str());
		sLog.outDebug("%s: You must change your client to use Placebo crypto in common.ini to talk to this server!\n", GetRemoteIP().c_str());
	}
}

void EveClientSocket::_authStateCryptoChallenge(PyRep* packet)
{
	//just to be sure
	if(mRequest != NULL)
		delete mRequest;
	mRequest = new CryptoChallengePacket;

	PyRep* data = packet;

	if(!mRequest->Decode(&data)) {
		sLog.outDebug("%s: Received invalid crypto challenge!", GetRemoteIP().c_str());
		Disconnect();
		return;
	}

	sLog.outDebug("%s: Received Client Challenge.", GetRemoteIP().c_str(), mRequest->user_name.c_str());

	if(mRequest->user_password->CheckType(PyRep::None)) {
		//this is little wrong because on live they send password version always, but only once,
		//but we send password version when we get request with hashed password ...
		sLog.outDebug("%s: Got hashed password, requesting plain.", GetRemoteIP().c_str());
		delete mRequest;
		mRequest = NULL;

		//send passwordVersion required: 1=plain, 2=hashed
		_sendRequirePasswordType(1);
		return;
	}

	sLog.outDebug("%s:		username='%s'", GetRemoteIP().c_str(), mRequest->user_name.c_str());

	//send our handshake
	CryptoServerHandshake server_shake;
	server_shake.serverChallenge = "hi";
	server_shake.func_marshaled_code = new PyRepBuffer(handshakeFunc, sizeof(handshakeFunc));
	server_shake.macho_version = MachoNetVersion;
	server_shake.boot_version = EVEVersionNumber;
	server_shake.boot_build = EVEBuildVersion;
	server_shake.boot_codename = EVEProjectCodename;
	server_shake.boot_region = EVEProjectRegion;
	server_shake.cluster_usercount = 100;//m_userCount;
	server_shake.proxy_nodeid = 0xFFAA;
	server_shake.user_logonqueueposition = 1;
	server_shake.challenge_responsehash = "654"; // binascii.crc_hqx of 64 zero bytes in a string, in a single element tuple, marshaled

	data = server_shake.Encode();
	OutPacket(data);
	delete data;
	
	Log.Debug("AuthStateMachine","State changed into HandservershakeSend");
	mCurrentStateMachine = &EveClientSocket::_authStateHandshakeSend;
}

void EveClientSocket::_authStateHandshakeSend(PyRep* packet)
{
	CryptoHandshakeResult hr;
	PyRep * data = packet;
	if(!hr.Decode(&data)) {
		sLog.outDebug("%s: Received invalid crypto handshake result!", GetRemoteIP().c_str());
		Disconnect();
		return;
	}

//	mClient = new Client();

	//this is a bit crappy ...
	//client->Login(m_request);

	/* now comes code that should be in the client class, but as we are hacking anyway why care at all */
	sLog.outDebug("Login with %s", mRequest->user_name.c_str());

	if(!mRequest->user_password->CheckType(PyRep::PackedObject2)) {
		sLog.outDebug("Failed to get password: user password is not PackedObject2.");
		Disconnect();
		return;
	}
	PyRepPackedObject2 *obj = (PyRepPackedObject2 *)mRequest->user_password;
	//we can check type, should be "util.PasswordString"

	Call_SingleStringArg pass;
	if(!pass.Decode(&obj->args1)) {
		Disconnect();
		sLog.outDebug("Failed to decode password.");
		return;
	}

	// db check stuff
	/*if(!m_services->GetServiceDB()->DoLogin(mRequest->user_name.c_str(), pass.arg.c_str(), m_accountID, m_role)) {
		_log(CLIENT__MESSAGE, "%s: Login rejected by DB", mRequest->user_name.c_str());

		PyRepPackedObject1 *e = new PyRepPackedObject1("exceptions.GPSMACHONETMSG_TYPE_TRANSPORTCLOSED");
		e->args = new PyRepTuple(1);
		e->args->items[0] = new PyRepString("LoginAuthFailed");

		//throw(PyException(e));
	}*/

	// this is needed so if we exit before selecting a character, the account 'online' flag would switch back to 0
	//m_char.charid = 0;
	//m_services->GetServiceDB()->SetAccountOnlineStatus(m_accountID, true);

	mUserName =  mRequest->user_name;
	mUserid = 42;


	//send this before session change
	CryptoHandshakeAck ack;
	ack.connectionLogID = 1;	//TODO: what is this??
	ack.jit = mRequest->user_languageid;
	ack.userid = mUserid;//m_accountID;
	ack.maxSessionTime = new PyRepNone;
	ack.userType = 1;	//TODO: what is this??

	/*
	1 = I think normal user
	2 = rooky
	23 = I think trial account (unable to verify atm)
	*/

	ack.role = 1;//m_role;
	ack.address = GetRemoteIP();
	ack.inDetention = new PyRepNone;
	ack.user_clientid = 42;//m_accountID;
	
	PyRep * ectpack = ack.Encode();
	OutPacket(ectpack);
	delete ectpack;

	//m_net._QueueRep(ack.Encode());

	//session.Set_userType(1);	//TODO: what is this??
	//session.Set_userid(m_accountID);
	//session.Set_address(m_net.GetRemoteIP().c_str());
	//session.Set_role(m_role);
	//session.Set_languageID(pack->user_languageid.c_str());

	//_CheckSessionChange();

	/* end of the evil hacking */

	delete mRequest;
	mRequest = NULL;

	/* assumed that authorization is completed */
	if (mSession == NULL)
	{
		mSession = new EveClientSession(mUserid, mUserName, this);
	}

	sSpace.AddSession(mSession);
	sSpace.AddGlobalSession(mSession);

	Log.Debug("AuthStateMachine","State changed into StateDone");
	mCurrentStateMachine = &EveClientSocket::_authStateDone;
	mAuthed = true;
}

void EveClientSocket::_authStateDone(PyRep* packet)
{
	Log.Debug("ClientSocket","received packet 'whooo' we passed authorization");

	// and of course is this not the correct way todo this
	if ( packet->CheckType(PyRep::PackedObject1) == true )
	{
		//Log.Debug("AuthStateMachine","State changed into StateException");
		//mCurrentStateMachine = &EveClientSocket::_authStateException;
		//(this->*mCurrentStateMachine)(recvPyPacket);

		Log.Debug("AuthStateMachine","Exception noticed");
		//mCurrentStateMachine(packet);
		_authStateException(packet);
		return;		
	}

	mSession->QueuePacket((PyPacket*)packet);
}

void EveClientSocket::_authStateException(PyRep* packet)
{
	Log.Debug("AuthStateMachine","Processing Exception");

	PyRepPackedObject1* obj = (PyRepPackedObject1*)packet;
	//obj->

	//ByteBuffer data;
	//data.append(packet->)

	// whoo delete if for now
	delete packet;
}