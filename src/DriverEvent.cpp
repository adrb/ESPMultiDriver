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

DriverEvent::DriverEvent(String dstDevice) {

  dstDeviceName = dstDevice;
  setTime();

  status = INITIALIZED;
}

DriverEvent::~DriverEvent() {
  status = INVALID;
}

// handle event in Kernel
DriverEvent::EventStatus DriverEvent::handle() {
  Kernel::getInstance().handleEvent(this);
  return status;
}

void DriverEvent::setTime(unsigned long _handleTime, unsigned long _expireTime) {

  unsigned long curTime = millis();

  if ( _handleTime != 0 ) {
    handleTime = curTime + _handleTime;
    expireTime = handleTime + (_expireTime==0?DRIVEREVENT_EXPIRETIME:_expireTime);
  } else {
    handleTime = 0;
    expireTime = (unsigned long)-1;  // "infinite" expiration time
  }
}

DriverEventGeneric::DriverEventGeneric(String dstDevice, String eName) :
  DriverEvent(dstDevice) {

  eventName = eName;
}

DriverEventInt::DriverEventInt(String dstDevice, String eName, int _intval) :
  DriverEventGeneric(dstDevice, eName) {

  setInt(_intval);
}

bool DriverEventGeneric::handleDevice(Driver *device) {
  return device->handleEvent(this);
}

DriverEventString::DriverEventString(String dstDevice, String eName, String _strval) :
  DriverEventGeneric(dstDevice, eName) {

  setString(_strval);
}

bool DriverEventString::handleDevice(Driver *device) {
  return device->handleEvent(this);
}

bool DriverEventInt::handleDevice(Driver *device) {
  return device->handleEvent(this);
}

bool DriverEventStop::handleDevice(Driver *device) {
  return device->handleEvent(this);
}

bool DriverEventStart::handleDevice(Driver *device) {
  return device->handleEvent(this);
}

bool DriverEventDisable::handleDevice(Driver *device) {
  if ( !device->handleEvent(this) ) return false;
  Kernel::getInstance().deleteDevice(getDstDeviceName());
return true;
}

DriverEventJson::DriverEventJson(String dstDevice, String eName) :
  DriverEventGeneric(dstDevice, eName) {

  jsonObj = jsonBuffer.createObject();
}

JsonObject& DriverEventJson::getJsonObj() {
  return jsonObj.as<JsonObject>();
}

void DriverEventJson::setJsonObj(JsonVariant srcJsonV) {

  jsonObj = json_clone(jsonBuffer, srcJsonV);
/*
  srcJsonV.as<JsonObject>().printTo(Serial);
  jsonObj.printTo(Serial);
  Serial.println();
*/
}

bool DriverEventJson::handleDevice(Driver *device) {
  return device->handleEvent(this);
}

bool DriverEventExportConfig::handleDevice(Driver *device) {
  return device->handleEvent(this);
}

bool DriverEventUpdateConfig::handleDevice(Driver *device) {
  return device->handleEvent(this);
}
