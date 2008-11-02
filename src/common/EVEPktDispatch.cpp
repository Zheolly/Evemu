/*  EVEmu: EVE Online Server Emulator

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY except by those people which sell it, which
  are required to give you total support for your newly bought product;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
	
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "EVEPktDispatch.h"
#include "../common/logsys.h"
#include "../common/PyPacket.h"
#include "../common/PyRep.h"
#include "../packets/AccountPkts.h"
#include "../packets/General.h"

EVEPacketDispatcher::EVEPacketDispatcher() {
}

EVEPacketDispatcher::~EVEPacketDispatcher() {
}

void EVEPacketDispatcher::DispatchPacket(PyPacket **in_p) {
	PyPacket *packet = *in_p;
	*in_p = NULL;
	
	switch(packet->type) {
	case MACHONETMSG_TYPE_AUTHENTICATION_REQ: {
		//check the string part, just for good measure
		if(packet->type_string != "macho.AuthenticationReq") {
			_log(NET__DISPATCH_ERROR, "Received MACHONETMSG_TYPE_AUTHENTICATION_RSP with invalid type string '%s'", packet->type_string.c_str());
			break;
		}
		
		AuthenticationReq *obj = new AuthenticationReq();
		if(!obj->Decode(&packet->payload)) {
			_log(NET__DISPATCH_ERROR, "Failed to decode AuthenticationReq");
			SafeDelete(obj);
			break;
		}
		Handle_AuthenticationReq(packet, &obj);
		SafeDelete(obj);	//in case they didn't
	} break;
	
	case MACHONETMSG_TYPE_AUTHENTICATION_RSP: {
		//check the string part, just for good measure
		if(packet->type_string != "macho.AuthenticationRsp") {
			_log(NET__DISPATCH_ERROR, "Received MACHONETMSG_TYPE_AUTHENTICATION_RSP with invalid type string '%s'", packet->type_string.c_str());
			break;
		}
		
		AuthenticationRsp *obj = new AuthenticationRsp();
		if(!obj->Decode(&packet->payload)) {
			_log(NET__DISPATCH_ERROR, "Failed to decode AuthenticationRsp");
			SafeDelete(obj);
			break;
		}
		Handle_AuthenticationRsp(packet, &obj);
		SafeDelete(obj);	//in case they didnt
	} break;

	case MACHONETMSG_TYPE_CALL_REQ: {
		//check the string part, just for good measure
		if(packet->type_string != "macho.CallReq") {
			_log(NET__DISPATCH_ERROR, "Received MACHONETMSG_TYPE_CALL_REQ with invalid type string '%s'", packet->type_string.c_str());
			break;
		}
		
		PyCallStream *call = new PyCallStream();
		if(!call->Decode(packet->type_string, packet->payload)) {	//payload is consumed
			_log(NET__DISPATCH_ERROR, "Failed to convert packet into a call stream");
			delete call;
			break;
		}
		Handle_CallReq(packet, &call);
		delete call;	//in case they didnt
	} break;
	
	case MACHONETMSG_TYPE_CALL_RSP: {
		//check the string part, just for good measure
		if(packet->type_string != "macho.CallRsp") {
			_log(NET__DISPATCH_ERROR, "Received MACHONETMSG_TYPE_CALL_RSP with invalid type string '%s'", packet->type_string.c_str());
			break;
		}
		
		Handle_CallRsp(packet, &packet->payload);
	} break;
	
	case MACHONETMSG_TYPE_NOTIFICATION: {
		//check the string part, just for good measure
		if(packet->type_string != "macho.Notification") {
			_log(NET__DISPATCH_ERROR, "Received MACHONETMSG_TYPE_CALL_RSP with invalid type string '%s'", packet->type_string.c_str());
			break;
		}
		
		if(packet->dest.type != PyAddress::Broadcast) {
			_log(NET__DISPATCH_ERROR, "Notification received with non-broadcast destination:");
			packet->dest.Dump(NET__DISPATCH_ERROR, "  Dest: ");
			packet->source.Dump(NET__DISPATCH_ERROR, "  Src:  ");
			break;
		}
		EVENotificationStream *obj = new EVENotificationStream;
		if(!obj->Decode(packet->type_string, packet->dest.service, packet->payload)) {
			_log(NET__DISPATCH_ERROR, "Failed to decode notification of type '%s'", packet->dest.service.c_str());
			SafeDelete(obj);
			break;
		}
		Handle_Notify(packet, &obj);
		SafeDelete(obj);	//in case they didnt
	} break;
	
	case MACHONETMSG_TYPE_ERRORRESPONSE: {
		//check the string part, just for good measure
		if(packet->type_string != "macho.ErrorResponse") {
			_log(NET__DISPATCH_ERROR, "Received MACHONETMSG_TYPE_ERRORRESPONSE with invalid type string '%s'", packet->type_string.c_str());
			break;
		}
		
		ErrorResponseBody *obj = new ErrorResponseBody;
		if(!obj->Decode(&packet->payload)) {
			_log(NET__DISPATCH_ERROR, "Failed to decode Error Response");
			SafeDelete(obj);
			break;
		}
		Handle_ErrorResponse(packet, &obj);
		SafeDelete(obj);	//in case they didnt
	} break;
	
	case MACHONETMSG_TYPE_SESSIONCHANGENOTIFICATION: {
		//check the string part, just for good measure
		if(packet->type_string != "macho.SessionChangeNotification") {
			_log(NET__DISPATCH_ERROR, "Received MACHONETMSG_TYPE_SESSIONCHANGENOTIFICATION with invalid type string '%s'", packet->type_string.c_str());
			break;
		}
		
		SessionChangeNotification *obj = new SessionChangeNotification();
		if(!obj->Decode(&packet->payload)) {
			_log(NET__DISPATCH_ERROR, "Failed to decode session change notification");
			SafeDelete(obj);
			break;
		}
		Handle_SessionChange(packet, &obj);
		SafeDelete(obj);	//in case they didnt
	} break;
	
	default: {
		Handle_Other(&packet);
	} break;
	
	}
	SafeDelete(packet);
}

/* default handlers do nothing but print */
void EVEPacketDispatcher::Handle_AuthenticationReq(const PyPacket *packet, AuthenticationReq **call) {
	_log(NET__DISPATCH_ERROR, "Unhandled Authentication Request");
}

void EVEPacketDispatcher::Handle_AuthenticationRsp(const PyPacket *packet, AuthenticationRsp **call) {
	_log(NET__DISPATCH_ERROR, "Unhandled Authentication Response");
}

void EVEPacketDispatcher::Handle_CallReq(const PyPacket *packet, PyCallStream **call) {
	_log(NET__DISPATCH_ERROR, "Unhandled Call Request");
}

void EVEPacketDispatcher::Handle_CallRsp(const PyPacket *packet, PyRepTuple **res) {
	_log(NET__DISPATCH_ERROR, "Unhandled Call Response");
}

void EVEPacketDispatcher::Handle_Notify(const PyPacket *packet, EVENotificationStream **notify) {
	_log(NET__DISPATCH_ERROR, "Unhandled Notification");
}

void EVEPacketDispatcher::Handle_SessionChange(const PyPacket *packet, SessionChangeNotification **notify) {
	_log(NET__DISPATCH_ERROR, "Unhandled SessionChange");
}

void EVEPacketDispatcher::Handle_ErrorResponse(const PyPacket *packet, ErrorResponseBody **body) {
	_log(NET__DISPATCH_ERROR, "Unhandled Error Response");
}

void EVEPacketDispatcher::Handle_Other(PyPacket **packet) {
	_log(NET__DISPATCH_ERROR, "Unhandled Packet of type %d", (*packet)->type);
}
















