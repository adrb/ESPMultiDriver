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

#ifndef __STRING_MAP_H__
#define __STRING_MAP_H__

#include <map>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>

#define PARAMS_FILE_SIZE_MAX 2048

// TODO: reconsider, map or maybe JsonObject?
class StringMap {
  std::map<const char*, String> map;

  void erase();
  std::map<const char*, String>::iterator find(const char* name);
public:

  virtual ~StringMap();  // virtual destructor also calls destructors for derived classes

  bool set(const char* name, String value);
  bool set(const char* name, int value);
  String get(const char* name, String defaultvalue = "");
  int getInt(const char* name, int defaultvalue = 0);

  void exportMap(JsonObject& json);    // export to json
  bool exportMap(fs::FS &fs, const char* filename);  // export to json file

  void updateMap(JsonObject& json);  // load existing params values from json
  bool updateMap(fs::FS &fs, const char* filename);  // load existing params values from json file
};

#endif
