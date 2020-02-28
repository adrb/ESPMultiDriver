/*
  Copyright (C) 2019  Adrian Brzeziński

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ESPMultiDriver.h"

JsonConfig::JsonConfig() {
    _jsonRoot = _jsonBuffer.createObject();
}

JsonConfig::~JsonConfig() {
  _jsonBuffer.clear();
}

JsonConfig::ExitCode JsonConfig::set(const char* name, String value) {

  JsonObject &jc = jsonConfig();

  if ( jc.containsKey(name) && !jc[name].is<String>() ) {
    //DEBUG_SERIAL("%s: INCOMPATIBLE_TYPE \"%s\" = \"%s\"\n", __FUNCTION__, name, value.c_str());
    return INCOMPATIBLE_TYPE;
  }

  jc[name] = value;

//  DEBUG_SERIAL("%s: \"%s\" = \"%s\"\n", __FUNCTION__, name, jc.get<String>(name).c_str());
//  jc.printTo(Serial); DEBUG_SERIAL("\n");

return SUCCESS;
}

JsonConfig::ExitCode JsonConfig::set(const char* name, int value) {
  JsonObject &jc = jsonConfig();

  if ( jc.containsKey(name) && !jc[name].is<int>() ) {
    return INCOMPATIBLE_TYPE;
  }

  jc[name] = value;

return SUCCESS;
}

JsonConfig::ExitCode JsonConfig::set(const char* name, bool value) {
  JsonObject &jc = jsonConfig();

  if ( jc.containsKey(name) && !jc[name].is<bool>() ) {
    return INCOMPATIBLE_TYPE;
  }

  jc[name] = value;

return SUCCESS;
}
/*
JsonConfig::ExitCode JsonConfig::set(const char* name, JsonVariant &value) {
  JsonObject &jc = jsonConfig();

  jc[name] = value;

return SUCCESS;
}
*/
JsonVariant JsonConfig::get(const char* name) {

//  DEBUG_SERIAL("%s: \"%s\" = \"%s\"\n", __FUNCTION__, name, jsonConfig().get<String>(name).c_str());

  return jsonConfig().get<JsonVariant>(name);
}

bool JsonConfig::save(JsonVariant jdst) {

  JsonObject& dstObj = jdst.as<JsonObject>();

  for (JsonPair& p : jsonConfig()) {
    dstObj[p.key] = p.value;
  }

//  DEBUG_SERIAL("\nsave from json: ");jsonConfig().printTo(Serial); DEBUG_SERIAL("\n");
//  DEBUG_SERIAL("save to json: ");dstObj.printTo(Serial); DEBUG_SERIAL("\n");

return true;
}

// update existing configuration
bool JsonConfig::load(JsonVariant jsrc) {

  JsonObject& srcObj = jsrc.as<JsonObject>();

  for (JsonPair& p : jsonConfig()) {
    if ( srcObj.containsKey(p.key) ) {
      jsonConfig()[p.key] = srcObj[p.key];
    }
  }

return true;
}

bool JsonConfig::save(fs::FS &fs, const char* filename) {

  JsonObject &jc = jsonConfig();

  if ( !jc.success() ) return false;

  File configFile = fs.open(filename, "w");
  if (!configFile) {
    DEBUG_SERIAL("Failed to write config file: %s\n", filename);
    return false;
  }

  jc.printTo(configFile);
  return true;
}

bool JsonConfig::load(fs::FS &fs, const char* filename) {

  File configFile = SPIFFS.open(filename, "r");
  if (!configFile) {
    DEBUG_SERIAL("Failed to read config file: %s\n", filename);
    return false;
  }

  size_t size = configFile.size();
  if (size > CONFIG_FILE_SIZE_MAX) {
    DEBUG_SERIAL("Config file \"%s\" size over %d bytes\n", filename, size);
    return false;
  }

  // Check if it's json file
  DynamicJsonBuffer jsonTmpBuf;
  JsonObject& json = jsonTmpBuf.parseObject(configFile);

  if (!json.success()) {
    DEBUG_SERIAL("Failed to parse config file: %s\n", filename);
    return false;
  }

  jsonTmpBuf.clear();
  configFile.seek(0, SeekSet);

  _jsonBuffer.clear();
  _jsonRoot = _jsonBuffer.parseObject(configFile);

//  DEBUG_SERIAL("%s: ", __FUNCTION__); _jsonRoot.printTo(Serial); DEBUG_SERIAL("\n");

return jsonConfig().success();
}
