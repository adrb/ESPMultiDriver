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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <Arduino.h>
#include <ArduinoJson.h>

bool json_containsNestedKey(const JsonObject& obj, const char* key);
void json_merge(JsonObject& dest, const JsonObject& src);
JsonVariant json_clone(JsonBuffer& jb, JsonVariant prototype);

char *xstrtok(char **strptr, size_t *len, const char delim);

#endif
