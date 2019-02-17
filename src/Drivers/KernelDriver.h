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

#ifndef __KERNEL_DRIVER_H__
#define __KERNEL_DRIVER_H__

#include <FS.h>
#include <AsyncJson.h>
#include <ESPAsyncTCP.h>

#include "DriverEvent.h"

static const char KERNEL_NAME[] = "kernel";

// Handles events for Kernel class
class KernelDriver : public Driver {
  friend class DriverAllocator;

  KernelDriver() {}; // only DriverAllocator can create driver object

  bool listAllocators(JsonObject& json);
  bool listDevices(JsonObject& json);

public:
  class Allocator : public DriverAllocator {
    Driver* allocate() { return new KernelDriver; }
  public:
    const char* driverName() override final { return KERNEL_NAME; }
  };
  ~KernelDriver() {};

  bool begin();

  bool handleEvent(DriverEventJson *event) override final;
  bool handleEvent(DriverEventDisable *event) override final;
  bool handleEvent(DriverEventInt *event) override final;
};

#endif
