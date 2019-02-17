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

#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__

#include "DriverEvent.h"

#define DCMOTOR_SPEED_CHANGE_FREQ 50   // how often we can change motor speed? time in ms

#define DRIVER_DCMOTOR_ALOCNAME "dcmotor"

class DCMotorDriver : public Driver {
  friend class DriverAllocator;

  uint8_t pin0;
  uint8_t pin1;
  int currentSpeed;   // negative number means rotation in opposite direction
  unsigned long lastCmdTime;

  DCMotorDriver(); // only DriverAllocator can create driver object
  void stopMotor();
  void setSpeed(int speed);  // change current speed

public:
  class Allocator : public DriverAllocator {
    Driver* allocate() { return new DCMotorDriver; }
  public:
    const char* name() override final { static const char* fname = DRIVER_DCMOTOR_ALOCNAME; return fname; }
  };
  ~DCMotorDriver() {};

  bool begin();
  bool run();
  void end();

  bool handleEvent(DriverEventString *event);
  bool handleEvent(DriverEventJson *event);
};

#endif
