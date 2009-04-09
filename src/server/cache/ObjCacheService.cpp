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
	Author:		Zhur
*/

#include "EvemuPCH.h"

const char *const ObjCacheService::LoginCachableObjects[] = {
	"config.BulkData.ramactivities",
	"config.BulkData.billtypes",
	"config.Bloodlines",
	"config.Units",
	"config.BulkData.tickernames",
	"config.BulkData.ramtyperequirements",
	"config.BulkData.ramaltypesdetailpergroup",
	"config.BulkData.ramaltypes",
	"config.BulkData.allianceshortnames",
	"config.BulkData.ramcompletedstatuses",
	"config.BulkData.categories",
	"config.BulkData.invtypereactions",
	"config.BulkData.dgmtypeeffects",
	"config.BulkData.metagroups",
	"config.BulkData.ramtypematerials",
	"config.BulkData.ramaltypesdetailpercategory",
	"config.BulkData.owners",
	"config.StaticOwners",
	"config.Races",
	"config.Attributes",
	"config.BulkData.dgmtypeattribs",
	"config.BulkData.locations",
	"config.BulkData.locationwormholeclasses",
	"config.BulkData.groups",
	"config.BulkData.shiptypes",
	"config.BulkData.dgmattribs",
	"config.Flags",
	"config.BulkData.bptypes",
	"config.BulkData.graphics",
	"config.BulkData.mapcelestialdescriptions",
	"config.BulkData.certificates",
	"config.StaticLocations",
	"config.InvContrabandTypes",
	"config.BulkData.certificaterelationships",
	"config.BulkData.units",
	"config.BulkData.dgmeffects",
	"config.BulkData.types",
	"config.BulkData.invmetatypes"
};
const uint32 ObjCacheService::LoginCachableObjectCount = sizeof(ObjCacheService::LoginCachableObjects) / sizeof(const char *);

const char *const ObjCacheService::CharCreateCachableObjects[] = {
	"charCreationInfo.bl_eyebrows",
	"charCreationInfo.bl_eyes",
	"charCreationInfo.bl_decos",
	"charCreationInfo.bloodlines",
	"charCreationInfo.bl_hairs",
	"charCreationInfo.bl_backgrounds",
	"charCreationInfo.bl_accessories",
	"charCreationInfo.bl_costumes",
	"charCreationInfo.bl_lights",
	"charCreationInfo.races",
	"charCreationInfo.ancestries",
	"charCreationInfo.schools",
	"charCreationInfo.attributes",
	"charCreationInfo.bl_beards",
	"charCreationInfo.bl_skins",
	"charCreationInfo.bl_lipsticks",
	"charCreationInfo.bl_makeups"
};
const uint32 ObjCacheService::CharCreateCachableObjectCount = sizeof(ObjCacheService::CharCreateCachableObjects) / sizeof(const char *);

const char *const ObjCacheService::CharNewExtraCreateCachableObjects[] = {
	"charNewExtraCreationInfo.raceskills",
	"charNewExtraCreationInfo.careerskills",
	"charNewExtraCreationInfo.specialityskills",
	"charNewExtraCreationInfo.careers",
	"charNewExtraCreationInfo.specialities"
};
const uint32 ObjCacheService::CharNewExtraCreateCachableObjectCount = sizeof(ObjCacheService::CharNewExtraCreateCachableObjects) / sizeof(const char *);

const char *const ObjCacheService::AppearanceCachableObjects[] = {
	"charCreationInfo.eyebrows",
	"charCreationInfo.eyes",
	"charCreationInfo.decos",
	"charCreationInfo.hairs",
	"charCreationInfo.backgrounds",
	"charCreationInfo.accessories",
	"charCreationInfo.costumes",
	"charCreationInfo.lights",
	"charCreationInfo.makeups",
	"charCreationInfo.beards",
	"charCreationInfo.skins",
	"charCreationInfo.lipsticks"
};
const uint32 ObjCacheService::AppearanceCachableObjectCount = sizeof(ObjCacheService::AppearanceCachableObjects) / sizeof(const char *);

PyCallable_Make_InnerDispatcher(ObjCacheService)

ObjCacheService::ObjCacheService(PyServiceMgr *mgr, DBcore *dbc, const char *cacheDir)
: PyService(mgr, "objectCaching"),
  m_dispatch(new Dispatcher(this)),
  m_db(dbc),
  m_cacheDir(cacheDir)
{
	_SetCallDispatcher(m_dispatch);

	PyCallable_REG_CALL(ObjCacheService, GetCachableObject)

	//register full name -> short key in m_cacheKeys
	m_cacheKeys["config.BulkData.billtypes"] = "config.BulkData.billtypes";
	m_cacheKeys["config.BulkData.allianceshortnames"] = "config.BulkData.allianceshortnames";
	m_cacheKeys["config.BulkData.categories"] = "config.BulkData.categories";
	m_cacheKeys["config.BulkData.invtypereactions"] = "config.BulkData.invtypereactions";
	m_cacheKeys["config.BulkData.dgmtypeattribs"] = "config.BulkData.dgmtypeattribs";
	m_cacheKeys["config.BulkData.dgmtypeeffects"] = "config.BulkData.dgmtypeeffects";
	m_cacheKeys["config.BulkData.dgmeffects"] = "config.BulkData.dgmeffects";
	m_cacheKeys["config.BulkData.dgmattribs"] = "config.BulkData.dgmattribs";
	m_cacheKeys["config.BulkData.metagroups"] = "config.BulkData.metagroups";
	m_cacheKeys["config.BulkData.ramactivities"] = "config.BulkData.ramactivities";
	m_cacheKeys["config.BulkData.ramaltypesdetailpergroup"] = "config.BulkData.ramaltypesdetailpergroup";
	m_cacheKeys["config.BulkData.ramaltypesdetailpercategory"] = "config.BulkData.ramaltypesdetailpercategory";
	m_cacheKeys["config.BulkData.ramaltypes"] = "config.BulkData.ramaltypes";
	m_cacheKeys["config.BulkData.ramcompletedstatuses"] = "config.BulkData.ramcompletedstatuses";
	m_cacheKeys["config.BulkData.ramtypematerials"] = "config.BulkData.ramtypematerials";

	m_cacheKeys["config.BulkData.ramtyperequirements"] = "config.BulkData.ramtyperequirements";
	m_cacheKeys["config.BulkData.mapcelestialdescriptions"] = "config.BulkData.mapcelestialdescriptions";
	m_cacheKeys["config.BulkData.tickernames"] = "config.BulkData.tickernames";
	m_cacheKeys["config.BulkData.groups"] = "config.BulkData.groups";
	m_cacheKeys["config.BulkData.certificates"] = "config.BulkData.certificates";
	m_cacheKeys["config.BulkData.certificaterelationships"] = "config.BulkData.certificaterelationships";
	m_cacheKeys["config.BulkData.shiptypes"] = "config.BulkData.shiptypes";
	m_cacheKeys["config.BulkData.locations"] = "config.BulkData.locations";
	m_cacheKeys["config.BulkData.locationwormholeclasses"] = "config.BulkData.locationwormholeclasses";
	m_cacheKeys["config.BulkData.bptypes"] = "config.BulkData.bptypes";
	m_cacheKeys["config.BulkData.graphics"] = "config.BulkData.graphics";
	m_cacheKeys["config.BulkData.types"] = "config.BulkData.types";
	m_cacheKeys["config.BulkData.invmetatypes"] = "config.BulkData.invmetatypes";
	m_cacheKeys["config.Bloodlines"] = "config.Bloodlines";
	m_cacheKeys["config.Units"] = "config.Units";
	m_cacheKeys["config.BulkData.units"] = "config.BulkData.units";
	m_cacheKeys["config.BulkData.owners"] = "config.BulkData.owners";
	m_cacheKeys["config.StaticOwners"] = "config.StaticOwners";
	m_cacheKeys["config.Races"] = "config.Races";
	m_cacheKeys["config.Attributes"] = "config.Attributes";
	m_cacheKeys["config.Flags"] = "config.Flags";
	m_cacheKeys["config.StaticLocations"] = "config.StaticLocations";
	m_cacheKeys["config.InvContrabandTypes"] = "config.InvContrabandTypes";

	m_cacheKeys["charCreationInfo.bloodlines"] = "bloodlines";
	m_cacheKeys["charCreationInfo.races"] = "races";
	m_cacheKeys["charCreationInfo.ancestries"] = "ancestries";
	m_cacheKeys["charCreationInfo.schools"] = "schools";
	m_cacheKeys["charCreationInfo.attributes"] = "attributes";
	m_cacheKeys["charCreationInfo.bl_accessories"] = "accessories";
	m_cacheKeys["charCreationInfo.bl_lights"] = "lights";
	m_cacheKeys["charCreationInfo.bl_skins"] = "skins";
	m_cacheKeys["charCreationInfo.bl_beards"] = "beards";
	m_cacheKeys["charCreationInfo.bl_eyes"] = "eyes";
	m_cacheKeys["charCreationInfo.bl_lipsticks"] = "lipsticks";
	m_cacheKeys["charCreationInfo.bl_makeups"] = "makeups";
	m_cacheKeys["charCreationInfo.bl_hairs"] = "hairs";
	m_cacheKeys["charCreationInfo.bl_backgrounds"] = "backgrounds";
	m_cacheKeys["charCreationInfo.bl_decos"] = "decos";
	m_cacheKeys["charCreationInfo.bl_eyebrows"] = "eyebrows";
	m_cacheKeys["charCreationInfo.bl_costumes"] = "costumes";

	m_cacheKeys["charNewExtraCreationInfo.raceskills"] = "raceskills";
	m_cacheKeys["charNewExtraCreationInfo.careerskills"] = "careerskills";
	m_cacheKeys["charNewExtraCreationInfo.specialityskills"] = "specialityskills";
	m_cacheKeys["charNewExtraCreationInfo.careers"] = "careers";
	m_cacheKeys["charNewExtraCreationInfo.specialities"] = "specialities";

	m_cacheKeys["charCreationInfo.eyebrows"] = "eyebrows";
	m_cacheKeys["charCreationInfo.eyes"] = "eyes";
	m_cacheKeys["charCreationInfo.decos"] = "decos";
	m_cacheKeys["charCreationInfo.hairs"] = "hairs";
	m_cacheKeys["charCreationInfo.backgrounds"] = "backgrounds";
	m_cacheKeys["charCreationInfo.accessories"] = "accessories";
	m_cacheKeys["charCreationInfo.lights"] = "lights";
	m_cacheKeys["charCreationInfo.costumes"] = "costumes";
	m_cacheKeys["charCreationInfo.makeups"] = "makeups";
	m_cacheKeys["charCreationInfo.beards"] = "beards";
	m_cacheKeys["charCreationInfo.skins"] = "skins";
	m_cacheKeys["charCreationInfo.lipsticks"] = "lipsticks";

	m_cacheKeys["dogmaIM.attributesByName"] = "dogmaIM.attributesByName";
}

ObjCacheService::~ObjCacheService() {
	delete m_dispatch;
}

PyResult ObjCacheService::Handle_GetCachableObject(PyCallArgs &call) {
	CallGetCachableObject args;
	if(!args.Decode(&call.tuple)) {
		codelog(SERVICE__ERROR, "%s: Unable to decode arguments", call.client->GetName());
		return NULL;
	}
	
	if(!_LoadCachableObject(args.objectID))
		return NULL;	//print done already
	
	//should we check their version? I am pretty sure they check it and only request what they want.
	//well, we want to do something like this, but this doesn't seem to be it. taken
	// out until we have time to figure out how to properly throw the CacheOK exception.
	/*if(m_cache.IsCacheUpToDate(args.objectID, args.version, args.timestamp)) {
		//they throw an exception for "its up to date", lets give it a try...
		objectCaching_CacheOK except;
		return(PyException(except.FastEncode()));
	}*/
	
	PyRepObject *result = m_cache.GetCachedObject(args.objectID);
	
	return(result);
}

void ObjCacheService::PrimeCache() {
	std::map<std::string, std::string>::const_iterator cur, end;
	cur = m_cacheKeys.begin();
	end = m_cacheKeys.end();
	printf("Loading cache");
	for(; cur != end; cur++) {
		_log(SERVICE__CACHE, "Priming cache object '%s'", cur->first.c_str());
		PyRepString str(cur->first);
		_LoadCachableObject(&str);
		
		putchar('.'); // print a dot so we have a indication of loading. I know this sucks.
	}
	printf("\nLoading done\n");
}

bool ObjCacheService::LoadCachedFile(const char *filename, const char *oname, PyRepSubStream *into) {
	//temp hack...
	return(m_cache.LoadCachedFile(filename, oname, into));
}


bool ObjCacheService::_LoadCachableObject(const PyRep *objectID) {
	if(m_cache.HaveCached(objectID))
		return true;
	
	const std::string objectID_string = CachedObjectMgr::OIDToString(objectID);

	if(!m_cacheDir.empty()) {
		if(m_cache.LoadCachedFromFile(m_cacheDir, objectID)) {
			_log(SERVICE__CACHE, "Loaded cached object '%s' from file.", objectID_string.c_str());
			return true;
		}
	}
	
	PyRepSubStream *ss;
	
	//first try to generate it from the database...
	//we go to the DB with a string, not a rep
	PyRep *cache = m_db.GetCachableObject(objectID_string);
	if(cache != NULL) {
		//we have generated the cache file in question, remember it
		m_cache.UpdateCache(objectID, &cache);
	} else {
		//failed to query from the database... fall back to old
		//hackish file loading.
		ss = new PyRepSubStream();
		if(!m_cache.LoadCachedObject(objectID_string.c_str(), ss)) {
			_log(SERVICE__ERROR, "Failed to create or load cache file for '%s'", objectID_string.c_str());
			return false;
		}
		
		//we have generated the cache file in question, remember it
		m_cache.UpdateCacheFromSS(objectID_string, &ss);
	}
	
	//if we have a cache dir, write out the cache entry:
	if(!m_cacheDir.empty()) {
		if(!m_cache.SaveCachedToFile(m_cacheDir, objectID)) {
			_log(SERVICE__ERROR, "Failed to save cache file for '%s' in '%s'", objectID_string.c_str(), m_cacheDir.c_str());
		} else {
			_log(SERVICE__CACHE, "Saved cached object '%s' to file.", objectID_string.c_str());
		}
	}

	return true;
}

PyRep *ObjCacheService::GetCacheHint(const char *objectID) {
	PyRepString str(objectID);
	
	if(!_LoadCachableObject(&str))
		return NULL;	//print done already

	PyRepObject *cache_hint = m_cache.MakeCacheHint(&str);
	if(cache_hint == NULL) {
		_log(SERVICE__ERROR, "Unable to build cache hint for object ID '%s' (h), skipping.", objectID);
		return NULL;
	}

	return(cache_hint);
}

void ObjCacheService::InsertCacheHints(hintSet hset, PyRepDict *into) {
	const char *const *objects = NULL;
	uint32 object_count = 0;
	switch(hset) {
	case hLoginCachables:
		objects = LoginCachableObjects;
		object_count = LoginCachableObjectCount;
		break;
	case hCharCreateCachables:
		objects = CharCreateCachableObjects;
		object_count = CharCreateCachableObjectCount;
		break;
	case hCharNewExtraCreateCachables:
		objects = CharNewExtraCreateCachableObjects;
		object_count = CharNewExtraCreateCachableObjectCount;
		break;
	case hAppearanceCachables:
		objects = AppearanceCachableObjects;
		object_count = AppearanceCachableObjectCount;
		break;
	}
	if(objects == NULL)
		return;
	uint32 r;
	std::map<std::string, std::string>::const_iterator res;
	for(r = 0; r < object_count; r++) {
		//find the dict key to use for this object
		res = m_cacheKeys.find(objects[r]);
		if(res == m_cacheKeys.end()) {
			_log(SERVICE__ERROR, "Unable to find cache key for object ID '%s', skipping.", objects[r]);
			continue;
		}

		//get the hint
		PyRep *cache_hint = GetCacheHint(objects[r]);
		if(cache_hint == NULL)
			continue;	//print already done.

		into->add(res->second.c_str(), cache_hint);
	}
}

bool ObjCacheService::IsCacheLoaded(const PyRep *objectID) const {
	return(m_cache.HaveCached(objectID));
}

void ObjCacheService::InvalidateCache(const PyRep *objectID) {
	m_cache.InvalidateCache(objectID);
}

void ObjCacheService::GiveCache(const PyRep *objectID, PyRep **contents) {
	//contents is consumed.
	m_cache.UpdateCache(objectID, contents);
}

PyRepObject *ObjCacheService::MakeObjectCachedMethodCallResult(const PyRep *objectID, const char *versionCheck) {
	if(!IsCacheLoaded(objectID))
		return NULL;
	objectCaching_CachedMethodCallResult_object c;
	c.versionCheck = versionCheck;
	c.object = m_cache.MakeCacheHint(objectID);
	return(c.Encode());
}

ObjectCachedMethodID::ObjectCachedMethodID(const char *service, const char *method) {
	SimpleMethodCallID c;
	c.service = service;
	c.method = method;
	objectID = c.Encode();
}

ObjectCachedMethodID::~ObjectCachedMethodID() {
	delete objectID;
}