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

DCMotorDriver::DCMotorDriver() {

  // initialize driver parameters
  params.set("pin0", "0");
  params.set("pin1", "0");
}

bool DCMotorDriver::begin() {

  pin0 = params.getInt("pin0");
  pin1 = params.getInt("pin1");

  if (pin0 == pin1) return false;

  currentSpeed = 0;
  lastCmdTime = millis();

  pinMode(pin0, OUTPUT);
  pinMode(pin1, OUTPUT);
  stopMotor();

  return true;
}

void DCMotorDriver::stopMotor() {
  analogWrite(pin0, PWMRANGE);
  analogWrite(pin1, PWMRANGE);
  currentSpeed = 0;
}

void DCMotorDriver::setSpeed(int speed) {

  // don't change motor speed too often
  if ( (millis() - lastCmdTime) < DCMOTOR_SPEED_CHANGE_FREQ ) return;
  lastCmdTime = millis();

  // stop motor in case of spin direction change
  if ((speed > 0 && currentSpeed < 0) || (currentSpeed > 0 && speed < 0)) {
    stopMotor();
    return;
  }

  // negative number means rotation in opposite direction
  analogWrite(pin0, speed > 0 ? speed : 0 );
  analogWrite(pin1, speed > 0 ? 0 : abs(speed) );
  currentSpeed = speed;
}

void DCMotorDriver::end() {
  setStatus(STOPPED);
  stopMotor();
}

bool DCMotorDriver::run() {

  // lost connection? stop the motor
  if ( (millis() - lastCmdTime) > (DCMOTOR_SPEED_CHANGE_FREQ * 10) ) {
    stopMotor();
  }

return true;
}

bool DCMotorDriver::handleEvent(DriverEventString *event) {

  if ( event->getEventName() == F("speed") ) {
    int newSpeed = event->getString().toInt();
    setSpeed(newSpeed);
    event->setString(String(currentSpeed));

    return true;
  }

return false;
}

bool DCMotorDriver::handleEvent(DriverEventJson *event) {

  JsonObject& json = event->getJsonObj();

  // curl -H "Content-Type: application/json" -X POST --data '{"event":"speed","device":"dcmotor","params":{"speed":"1000"}}' "http://192.168.0.1/api" -si
  if ( event->getEventName() == F("speed") ) {
    int newSpeed = json.get<signed int>(F("speed"));
    //DEBUG_SERIAL("dcmotor: newSpeed=%d\n", newSpeed);

    setSpeed( newSpeed );
    json[F("speed")] = (signed int)currentSpeed;

    return true;
  }

return false;
}
