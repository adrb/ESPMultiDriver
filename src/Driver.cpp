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

Driver::Driver() {
  setStatus(DISABLED);
}

Driver::~Driver() {
  setStatus(DISABLED);
}

void Driver::setStatus( Driver::DriverStatus _status ) {
  status = _status;
  params.set(DRIVER_PARAM_STATE, (int)_status);
}

String Driver::getParam(String paramName) {
  return params.get(paramName.c_str());
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
  return FSTR("/dev/") + params.get(DRIVER_PARAM_NAME) + FSTR(".json");
}

bool Driver::loadConfig() {
  bool ret = params.updateMap(SPIFFS, configFileName().c_str());
  status = (DriverStatus)params.getInt(DRIVER_PARAM_STATE); // update device status
  return ret;
}

bool Driver::saveConfig() {
  return params.exportMap(SPIFFS, configFileName().c_str());
}

bool Driver::deleteConfig() {
  return SPIFFS.remove(configFileName());
}

bool Driver::handleEvent(DriverEventStop *event)  {
  if ( getStatus() != Driver::RUNNING ) return false;
  end();
return true;
}

bool Driver::handleEvent(DriverEventStart *event)  {
  if ( getStatus() != Driver::RUNNING && begin() ) {
    setStatus(Driver::RUNNING);
    saveConfig();
    return true;
  }
return false;
}

bool Driver::handleEvent(DriverEventDisable *event) {
  if ( getStatus() == Driver::RUNNING ) end();
  setStatus(Driver::DISABLED);
  if ( !saveConfig() ) return false;
return true;
}

bool Driver::handleEvent(DriverEventExportParams *event) {
  params.exportMap(event->getJsonObj());
  return true;
}

bool Driver::handleEvent(DriverEventUpdateParams *event) {

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

  params.updateMap(json);
  if ( !saveConfig() ) return false;
  return true;
}
