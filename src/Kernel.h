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

#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <ESPAsyncWebServer.h>
#include <map>
#include <list>
#include "Driver.h"

// Do not deliver more than 10 events at time.
// It's protection against event loops
#define KERNEL_MAX_DELIVERY_NUM 10

// How long (in ms) we should block on events processing
#define KERNEL_PROCESS_EVENTS_TIMEOUT 100

class DriverEvent;
class KernelDriver;

// The kernel singleton
class Kernel {
  Kernel() {};
  Kernel(Kernel const&);              // Don't implement
  Kernel& operator=(Kernel const&);   // Don't implement

  // factory used to allocate new device of given driver class
  std::list<DriverAllocator*> allocators;
  std::list<DriverAllocator*>::iterator findAllocator(const char* name);

  // Map of initialized device driver objects.
  // It's map instead of list, because its faster to go through it than query "name" parameter
  // for every device on a list. Memory overhead should be minimal as it's impossible
  // to have many devices at the same time, due to GPIO number limits
  std::map<String, Driver*> devices;
  std::map<String, Driver*>::iterator findDevice(String name);
  Driver* getDevice(String deviceName);
  void deleteDevice(std::map<String, Driver*>::iterator dit);

  std::list<DriverEvent*> events;
  bool deliverEvent(Driver *dev, DriverEvent *event);
  void queueEvent(DriverEvent *event);
  void deleteEvent(std::list<DriverEvent*>::iterator eit);
  void processEvents();
  friend class KernelDriver; // handles events for Kernel class

public:
  static Kernel& getInstance() {
    static Kernel kernel;
    return kernel;
  }

  bool registerDriverAllocator(DriverAllocator* allocator);
  bool createDevice(String allocatorName, String deviceName, bool force = true);
  void deleteDevice(String deviceName);

  bool handleEvent(DriverEvent *event);
  bool begin();
  bool run(); // keep running if true
  void end();

  void autoloadDevices();
};

#endif
