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

Driver::Driver() {
  setStatus(DISABLED);
}

Driver::~Driver() {
  setStatus(DISABLED);
}

void Driver::setStatus( Driver::DriverStatus _status ) {
  status = _status;
  jconfig.set(DRIVER_PARAM_STATE, (int)_status);
}

JsonVariant Driver::getParam(const char* name) {
  return jconfig.get(name);
}

bool Driver::begin() {

  setStatus(RUNNING);
  return true;
};

bool Driver::run() {

  return true;
};

void Driver::end() {
  setStatus(STOPPED);
}

String Driver::configFileName() {
  return FSTR("/dev/") + jconfig.get(DRIVER_PARAM_NAME).as<String>() + FSTR(".json");
}

bool Driver::loadConfig() {
  bool ret = jconfig.load(SPIFFS, configFileName().c_str());

  // update device status
  status = (DriverStatus) jconfig.get(DRIVER_PARAM_STATE).as<int>();

return ret;
}

bool Driver::saveConfig() {
  return jconfig.save(SPIFFS, configFileName().c_str());
}

bool Driver::deleteConfig() {
  return SPIFFS.remove(configFileName());
}

bool Driver::handleEvent(DriverEventStop *event)  {
  if ( getStatus() != RUNNING ) return false;
  end();
return true;
}

bool Driver::handleEvent(DriverEventStart *event)  {
  if ( getStatus() != RUNNING && begin() ) {
    setStatus(RUNNING);
    saveConfig();
    return true;
  }
return false;
}

bool Driver::handleEvent(DriverEventDisable *event) {
  if ( getStatus() == RUNNING ) end();
  setStatus(DISABLED);
  if ( !saveConfig() ) return false;
return true;
}

bool Driver::handleEvent(DriverEventExportConfig *event) {
  jconfig.save(event->getJsonObj());
  return true;
}

bool Driver::handleEvent(DriverEventUpdateConfig *event) {

  JsonObject& json = event->getJsonObj();

  // You can't change device type
  if ( json.containsKey(DRIVER_PARAM_TYPE) ) {
    json.remove(DRIVER_PARAM_TYPE);
  }

  // You can't change device name
  if ( json.containsKey(DRIVER_PARAM_NAME) ) {
    json.remove(DRIVER_PARAM_NAME);
  }

  // state can be changed only with proper events
  if ( json.containsKey(DRIVER_PARAM_STATE) ) {
    json.remove(DRIVER_PARAM_STATE);
  }

  jconfig.load(json);

return saveConfig();
}
