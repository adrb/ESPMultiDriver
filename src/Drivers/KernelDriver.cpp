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

bool KernelDriver::begin() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // led off
  return true;
}

bool KernelDriver::listAllocators(JsonObject& json) {

  Kernel *kernel = &Kernel::getInstance();
  JsonArray& jsonArray = json.createNestedArray(F("drivers"));


  // enumerate registered device allocators
  std::list<DriverAllocator*>::iterator it;
  for ( it = kernel->allocators.begin() ; it != kernel->allocators.end() ; it++ ) {
    jsonArray.add((*it)->name());
  }

return true;
}

bool KernelDriver::listDevices(JsonObject& json) {

  Kernel *kernel = &Kernel::getInstance();

  // enumerate loaded devices
  std::map<String, Driver*>::iterator it;
  for ( it = kernel->devices.begin() ; it != kernel->devices.end() ; it++ ) {

    JsonObject& jsonObj = json.createNestedObject(it->first);
    jsonObj[DRIVER_PARAM_TYPE] = it->second->getParam(DRIVER_PARAM_TYPE);
    jsonObj[DRIVER_PARAM_STATE] = (int)it->second->getStatus();
  }

  // enumerate disabled devices
  Dir dir = SPIFFS.openDir(F("/dev/"));
  while (dir.next()) {
    String filename(dir.fileName());
    String deviceName = filename.substring(5, filename.lastIndexOf('.')); // strlen("/dev/") = 5

    //DEBUG_SERIAL("Found device \"%s\" config file: %s\n", deviceName.c_str(), filename.c_str());
    if (json.containsKey(deviceName)) continue;   // skip already enumerated devices

    JsonObject& jsonObj = json.createNestedObject(deviceName);
    jsonObj[DRIVER_PARAM_STATE] = (int)Driver::DISABLED;

    // load device type parameter from config file
    StringMap deviceParams;
    deviceParams.set(DRIVER_PARAM_TYPE,"");

    if ( !deviceParams.updateMap(SPIFFS, dir.fileName().c_str()) ) {
      DEBUG_SERIAL("Failed to load: %s\n", dir.fileName().c_str());
      continue;
    }

    String deviceType = deviceParams.get(DRIVER_PARAM_TYPE);
    if ( deviceType == "" ) continue;

    jsonObj[DRIVER_PARAM_TYPE] = deviceType;
  }

return true;
}

bool KernelDriver::handleEvent(DriverEventJson *event) {

  JsonObject& json = event->getJsonObj();

  // curl -H "Content-Type: application/json" -X POST --data '{"event":"info","device":"kernel","params":{}}' "http://192.168.0.1/api" -si
  if ( event->getEventName() == F("info") ) {
    json[F("version")] = EMD_VERSION;
    return true;
  }

  // curl -H "Content-Type: application/json" -X POST --data '{"event":"lsdrivers","device":"kernel","params":{}}' "http://192.168.0.1/api" -si
  if ( event->getEventName() == F("lsdrivers") ) {
    return listAllocators(json);
  }

  // curl -H "Content-Type: application/json" -X POST --data '{"event":"lsdev","device":"kernel","params":{}}' "http://192.168.0.1/api" -si
  if ( event->getEventName() == F("lsdev") ) {
    return listDevices(json);
  }

  // curl -H "Content-Type: application/json" -X POST --data '{"event":"mkdev","device":"kernel","params":{"type":"dcmotor","name":"motor1"}}' "http://192.168.0.1/api" -si
  if ( event->getEventName() == F("mkdev") ) {

    //json.printTo(Serial);

    String allocatorName = json.get<String>(F("type"));
    String devName = json.get<String>(F("name"));

    return Kernel::getInstance().createDevice(allocatorName, devName, true); // force starting disabled devices
  }

  // curl -H "Content-Type: application/json" -X POST --data '{"event":"rmdev","device":"kernel","params":{"name":"motor1"}}' "http://192.168.0.1/api" -si
  if ( event->getEventName() == F("rmdev") ) {

    String devName = json.get<String>(F("name"));
    Driver *dev = Kernel::getInstance().getDevice(devName);

    if ( dev ) {
      if ( !dev->deleteConfig() ) return false;
      Kernel::getInstance().deleteDevice(devName);
    } else {
      // wiping out config file even for disabled devices
      if ( !SPIFFS.remove(FSTR("/dev/") + devName + FSTR(".json")) ) return false;
    }

    return true;
  }

  return true;
}

bool KernelDriver::handleEvent(DriverEventDisable *event) {
  return false; // you can't disable that device
}

bool KernelDriver::handleEvent(DriverEventInt *event)  {

  if ( event->getEventName() == F("led_on") ) {
    digitalWrite(LED_BUILTIN, LOW);

    //DEBUG_SERIAL("led_off: %d\n", event->getInt() );

    DriverEventInt *e = new DriverEventInt(params.get(DRIVER_PARAM_NAME), F("led_off"), event->getInt());
    e->setTime(100);  // event will wait 100ms in queue

    if ( e->handle() != DriverEvent::QUEUED ) {
      DEBUG_SERIAL("Can't queue led_off event, status: %d\n", e->getStatus());
      delete e;
    }
  }

  if ( event->getEventName() == F("led_off") ) {
    digitalWrite(LED_BUILTIN, HIGH);

    //DEBUG_SERIAL("led_off: %d\n", event->getInt() );

    int blinkNum = event->getInt() - 1;
    if ( blinkNum <= 0 ) return true;

    DriverEventInt *e = new DriverEventInt(params.get(DRIVER_PARAM_NAME), F("led_on"), blinkNum);
    e->setTime(100);  // event will wait 100ms in queue
    if ( e->handle() != DriverEvent::QUEUED ) {
      DEBUG_SERIAL("Can't queue led_on event, status: %d\n", e->getStatus());
      delete e;
    }
  }

  return true;
};
