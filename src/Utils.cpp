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

// https://arduinojson.org/v5/doc/tricks/
bool json_containsNestedKey(const JsonObject& obj, const char* key) {
    for (const JsonPair& pair : obj) {
        if (!strcmp(pair.key, key))
            return true;

        if (json_containsNestedKey(pair.value.as<JsonObject>(), key))
            return true;
    }

    return false;
}

void json_merge(JsonObject& dest, const JsonObject& src) {
   for (auto kvp : src) {
     dest[kvp.key] = kvp.value;
   }
}

JsonVariant json_clone(JsonBuffer& jb, JsonVariant prototype)
{
  if (prototype.is<JsonObject>()) {
    const JsonObject& protoObj = prototype;
    JsonObject& newObj = jb.createObject();
    for (const auto& kvp : protoObj) {
      newObj[kvp.key] = json_clone(jb, kvp.value);
    }
    return newObj;
  }

  if (prototype.is<JsonArray>()) {
    const JsonArray& protoArr = prototype;
    JsonArray& newArr = jb.createArray();
    for (const auto& elem : protoArr) {
      newArr.add(json_clone(jb, elem));
    }
    return newArr;
  }

  if (prototype.is<char*>()) {
    return prototype.as<const char*>();
  }

  return prototype;
}

// subsequent request returns next token
char *xstrtok(char **strptr, size_t *len, const char delim) {

  char *strstart = *strptr;
  char *s = *strptr;

  //DEBUG_SERIAL("strtok: '%c', %d, %s\n", delim, *len, s);

  while (*len > 0 && *s && *s != delim) {
    s++;
    (*len)--;
  }

  // replace delimiter with zeros
  while (*len > 0 && *s && *s == delim) {
    *s++ = '\0';
    (*len)--;
  }

  *strptr = s;
  if (*strptr == strstart) return NULL;

  return strstart;
}
