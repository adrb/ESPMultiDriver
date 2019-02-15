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

#ifndef __WIFIAP_H__
#define __WIFIAP_H__

#include "DriverEvent.h"

#define DRIVER_WIFIAP_ALOCNAME "wifiap"

class WiFiAPDriver : public Driver {

  unsigned long ledTimer;

  friend class DriverAllocator;
  WiFiAPDriver(); // only DriverAllocator can create driver object
public:
  class Allocator : public DriverAllocator {
    Driver* allocate() { return new WiFiAPDriver; }
  public:
    const char* name() override final { static const char* fname = DRIVER_WIFIAP_ALOCNAME; return fname; }
  };
  ~WiFiAPDriver() {};

  bool begin();
  bool run();
  void end();

};

#endif
