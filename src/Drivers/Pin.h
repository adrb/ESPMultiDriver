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

#ifndef __PIN_H__
#define __PIN_H__

#include "DriverEvent.h"

static const char PIN_NAME[] = "pin";

class PinDriver : public Driver {
  friend class DriverAllocator;
  PinDriver(); // only DriverAllocator can create driver object

  uint8_t pin;
public:
  class Allocator : public DriverAllocator {
    Driver* allocate() { return new PinDriver; }
  public:
    const char* driverName() override final { return PIN_NAME; }
  };
  ~PinDriver() {};

  bool begin();
  bool handleEvent(DriverEventString *event);
  bool handleEvent(DriverEventJson *event);
};

#endif
