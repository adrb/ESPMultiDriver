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

PinDriver::PinDriver() {

  // initialize driver parameters
  jconfig.set("pin", 255);
  jconfig.set("output", 1);
}

bool PinDriver::begin() {

  pin = jconfig.get(FCSTR("pin")).as<int>();
  if ( pin < 0 || pin > 16 ) return false;

  if ( (bool)jconfig.get(FCSTR("output")).as<int>() ) {
    pinMode(pin, OUTPUT);
  } else {
    pinMode(pin, INPUT);
  }

  return true;
}

bool PinDriver::handleEvent(DriverEventString *event) {

  if ( event->getEventName() == F("digitalWrite") ) {
    digitalWrite(pin, event->getString().toInt());
    return true;
  }

  if ( event->getEventName() == F("digitalRead") ) {
    event->setString(String(digitalRead(pin)));
    return true;
  }

  if ( event->getEventName() == F("analogWrite") ) {
    analogWrite(pin, event->getString().toInt());
    return true;
  }

  if ( event->getEventName() == F("analogRead") ) {
    event->setString(String(analogRead(pin)));
    return true;
  }

return false;
}

bool PinDriver::handleEvent(DriverEventJson *event) {

  JsonObject& json = event->getJsonObj();

  // turn on LED on pin 2
  // curl -H "Content-Type: application/json" -X POST --data '{"event":"digitalWrite","device":"led","params":{"value":"0"}}' "http://192.168.0.1/api" -si
  if ( event->getEventName() == F("digitalWrite") ) {
    digitalWrite(pin, json.get<signed int>(F("value")));
    return true;
  }

  // curl -H "Content-Type: application/json" -X POST --data '{"event":"digitalRead","device":"led","params":{}}' "http://192.168.0.1/api" -si
  if ( event->getEventName() == F("digitalRead") ) {
    json[F("value")] = (signed int)digitalRead(pin);
    return true;
  }

  if ( event->getEventName() == F("analogWrite") ) {
    analogWrite(pin, json.get<signed int>(F("value")));
    return true;
  }

  if ( event->getEventName() == F("analogRead") ) {
    json["value"] = (signed int)analogRead(pin);
    return true;
  }

return false;
}
