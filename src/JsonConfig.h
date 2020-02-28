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

#ifndef __JSON_CONFIG_H__
#define __JSON_CONFIG_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>

#define CONFIG_FILE_SIZE_MAX 1024 // stack size connot go beyond 4k

class JsonConfig {

  DynamicJsonBuffer _jsonBuffer;
  JsonVariant _jsonRoot;

  JsonObject & jsonConfig() { return _jsonRoot.as<JsonObject>(); };

public:

  enum ExitCode { SUCCESS, ERROR, NOT_FOUND, INCOMPATIBLE_TYPE };

  JsonConfig();
  virtual ~JsonConfig();  // virtual destructor also calls destructors for derived classes

  ExitCode set(const char* name, String value);
  ExitCode set(const char* name, int value);
  ExitCode set(const char* name, bool value);
//  ExitCode set(const char* name, JsonVariant &value);

  JsonVariant get(const char* name);

  bool save(JsonVariant jdst); // export config to json object
  bool load(JsonVariant jsrc); // update config from json object

  bool save(fs::FS &fs, const char* filename);
  bool load(fs::FS &fs, const char* filename);
};

#endif
