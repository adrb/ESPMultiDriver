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

#include "definitions.h"

StringMap::~StringMap() {
  erase();
}

void StringMap::erase() {
  std::map<const char*, String>::iterator it;
  for ( it = map.begin() ; it != map.end() ; it++ ) {
    map.erase(it);
  }
}

std::map<const char*, String>::iterator StringMap::find(const char* name) {

  std::map<const char*, String>::iterator it;
  for ( it = map.begin() ; it != map.end() ; it++ ) {
    if ( !strcmp(it->first, name) ) break;
  }

  return it;
}

bool StringMap::set(const char* name, String value) {

  //DEBUG_SERIAL("%s: \"%s\" = \"%s\"\n", __FUNCTION__, name, value.c_str());

  // update existing value
  std::map<const char*, String>::iterator it = find(name);
  if ( it != map.end() ) {
    it->second = value;
    return true;
  }

  map[name] = value;
  return true;
}

bool StringMap::set(const char* name, int value) {
  return set(name, String(value));
}

String StringMap::get(const char* name, String defaultValue) {

  std::map<const char*, String>::iterator it = find(name);
  if ( it != map.end() ) {
    //DEBUG_SERIAL("%s: \"%s\" = \"%s\"\n", __FUNCTION__, name, value.c_str());
    return it->second;
  }

//  DEBUG_SERIAL("%s: No assosiation for \"%s\"!\n", __FUNCTION__, name);

return defaultValue;
}

int StringMap::getInt(const char* name, int defaultValue) {

  String svalue = get(name);

  if ( svalue != "" ) {
    return svalue.toInt();
  }

  return defaultValue;
}

void StringMap::exportMap(JsonObject& json) {
  std::map<const char*, String>::iterator it;
  for ( it = map.begin() ; it != map.end() ; it++ ) {
    json[it->first] = it->second.c_str();
  }
}

bool StringMap::exportMap(fs::FS &fs, const char* filename) {

  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  exportMap(json);
  if ( !json.success() ) return false;

  File configFile = fs.open(filename, "w");
  if (!configFile) {
    DEBUG_SERIAL("Failed to write config file: %s\n", filename);
    return false;
  }

  json.printTo(configFile);
  return true;
}

void StringMap::updateMap(JsonObject& json) {
  std::map<const char*, String>::iterator it;
  for ( it = map.begin() ; it != map.end() ; it++ ) {

    // continue if no new value in json
    if ( !json.containsKey(it->first) ) continue;

    it->second = (const char*)json[it->first];
  }
}

bool StringMap::updateMap(fs::FS &fs, const char* filename) {

  File configFile = SPIFFS.open(filename, "r");
  if (!configFile) {
    DEBUG_SERIAL("Failed to read config file: %s\n", filename);
    return false;
  }

  size_t size = configFile.size();
  if (size > PARAMS_FILE_SIZE_MAX) {
    DEBUG_SERIAL("Config file \"%s\" size over %d bytes\n", filename, size);
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    DEBUG_SERIAL("Failed to parse config file: %s\n", filename);
    return false;
  }

  updateMap(json);
  return true;
}
