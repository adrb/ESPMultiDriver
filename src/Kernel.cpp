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

std::list<DriverAllocator*>::iterator Kernel::findAllocator(const char* name) {

  std::list<DriverAllocator*>::iterator it;
  for ( it = allocators.begin() ; it != allocators.end() ; it++ ) {
    if ( !strcmp((*it)->name(), name) ) break;
  }

  return it;
}

std::map<String, Driver*>::iterator Kernel::findDevice(String name) {

  std::map<String, Driver*>::iterator it;
  for ( it = devices.begin() ; it != devices.end() ; it++ ) {
    if ( it->first == name ) break;
  }

  return it;
}

bool Kernel::registerDriverAllocator(DriverAllocator* allocator) {

  const char* allocName = allocator->name();

  if ( findAllocator(allocName) != allocators.end() ) {
    DEBUG_SERIAL("Driver allocator \"%s\" already registered!\n", allocName);
    return false;
  }

  allocators.push_back(allocator);

  DEBUG_SERIAL("New allocator registered: \"%s\"\n", allocName);
return true;
}

// force - load device even if it's disabled
bool Kernel::createDevice(String allocatorName, String deviceName, bool force) {

  std::list<DriverAllocator*>::iterator ait = findAllocator(allocatorName.c_str());
  Driver *dev;

  if ( deviceName.length() == 0 ) {
    DEBUG_SERIAL("Can't create device without name!\n");
    return false;
  }

  if ( ait == allocators.end() ) {
    DEBUG_SERIAL("Can't create device \"\", no registered driver allocator: \"%s\"!\n",
      deviceName.c_str(), allocatorName.c_str());

    return false;
  }

  if ( getDevice(deviceName) ) {
    DEBUG_SERIAL("Device \"%s\" already exists!\n", deviceName.c_str());
    return false;
  }

  if ( !(dev = (*ait)->allocate()) ) {
    DEBUG_SERIAL("Can't allocate new device \"%s\"!\n", deviceName.c_str());
    return false;
  }

  // device mandatory parameters
  dev->params.set(DRIVER_PARAM_NAME, deviceName);
  dev->params.set(DRIVER_PARAM_TYPE, allocatorName);
  dev->setStatus(Driver::STOPPED);

  // update parameters from config file
  dev->loadConfig();

  if ( force ) dev->setStatus(Driver::RUNNING);

  if ( dev->getStatus() == Driver::DISABLED ) {
    DEBUG_SERIAL("Use 'force' to initialize disabled device: %s (%d)\n", deviceName.c_str(), (int)dev->getStatus());
    delete dev;
    return false;
  }

  // adding device to map before invoking begin(),
  // allows the device to generate events
  devices[deviceName] = dev;

  // try to start only devices in RUNNING state
  if ( dev->getStatus() == Driver::RUNNING && !dev->begin() ) {
    dev->setStatus(Driver::STOPPED);
  }

return true;
}

void Kernel::deleteDevice(std::map<String, Driver*>::iterator dit) {

  dit->second->setStatus(Driver::STOPPED);
  dit->second->end();
//  dit->second->deleteConfig();

  delete dit->second;
  devices.erase(dit);
}

void Kernel::deleteDevice(String deviceName) {

  std::map<String, Driver*>::iterator dit = findDevice(deviceName);

  if ( dit == devices.end() ) {
    DEBUG_SERIAL("No such device \"%s\"\n", deviceName.c_str());
    return;
  }

  deleteDevice(dit);
}

Driver* Kernel::getDevice(String deviceName) {

  std::map<String, Driver*>::iterator dit = findDevice(deviceName);
  if ( dit == devices.end() ) return (Driver*)0;

  return dit->second;
}

// deliver immediately, returns true when handled
bool Kernel::deliverEvent(Driver *dev, DriverEvent *event) {

  static int entryNum = 0;

  if ( event->status != DriverEvent::INITIALIZED && event->status != DriverEvent::QUEUED ) {
    // there is nothing to do, mark handled
    return true;
  }

  if ( !dev ) {
    event->status = DriverEvent::NOTDELIVERED;
    return true;
  }

  unsigned long curTime = millis();

  // expired event mark as handled
  if (curTime > event->expireTime) {
    event->status = DriverEvent::EXPIRED;
    return true;
  }

  // not yet
  if (curTime < event->handleTime ) {

    // not queued event with handleTime set? something went seriously wrong
    if ( event->status != DriverEvent::QUEUED ) {
      event->status = DriverEvent::NOTDELIVERED;
      return true; // handled
    }
    return false; // not handled
  }

  // event loop protection
  if ( entryNum > KERNEL_MAX_DELIVERY_NUM ) {
    event->status = DriverEvent::NOTDELIVERED;
    return true;
  }
  entryNum++;

  event->status = DriverEvent::RUNNING;
  if ( event->handleDevice(dev) )
    event->status = DriverEvent::SUCCESS;
  else
    event->status = DriverEvent::FAILED;

  entryNum--;
  return true;
}

// add to events queue
void Kernel::queueEvent(DriverEvent *event) {

  event->status = DriverEvent::QUEUED;
  events.push_back(event);
}

void Kernel::deleteEvent(std::list<DriverEvent*>::iterator eit) {

  (*eit)->status = DriverEvent::INVALID;
  delete *eit;
  events.erase(eit);
}

bool Kernel::handleEvent(DriverEvent *event) {

  Driver *dev;

  if ( !event || event->status == DriverEvent::INVALID ) {
    DEBUG_SERIAL("Invalid event!\n");
    return true;
  }

  dev = getDevice(event->dstDeviceName);
  if ( !dev ) {
    DEBUG_SERIAL("No such device \"%s\"\n", event->dstDeviceName.c_str());
    event->status = DriverEvent::NOTDELIVERED;
    return true;
  }

  if ( event->handleTime == 0 ) {
    return deliverEvent(dev, event);
  }

  queueEvent(event);
return false;
}

void Kernel::processEvents() {

  DriverEvent *event;
  Driver *dev;
  std::list<DriverEvent*>::iterator eit;

  unsigned long startTime = millis();
  while ( (millis() - startTime) < KERNEL_PROCESS_EVENTS_TIMEOUT ) {

    for ( eit = events.begin() ; eit != events.end() ; eit++ ) {
      event = *eit;

      if ( event->status != DriverEvent::QUEUED ) {
        // drop processed events
        deleteEvent(eit);
        break;
      }

      dev = getDevice(event->dstDeviceName);
      if ( dev ) {
        if ( deliverEvent(dev, event) ) break;
      } else {
        DEBUG_SERIAL("No such device \"%s\"\n", event->dstDeviceName.c_str());
        deleteEvent(eit);
        break;
      }
    }

    // all events processed
    if ( eit == events.end() ) break;
  }
}

bool Kernel::run() {

  // initiate restart when KernelDriver isn't running
  Driver *kernelDriver = getDevice(DRIVER_KERNEL_ALOCNAME);
  if ( !kernelDriver || kernelDriver->getStatus() != Driver::RUNNING ) {
    DEBUG_SERIAL("KernelDriver not running!\n");
    return false;
  }

  // loop through all devices
  std::map<String, Driver*>::iterator dit;
  for ( dit = devices.begin() ; dit != devices.end() ; dit++ ) {
    if ( dit->second->getStatus() == Driver::RUNNING && !dit->second->run() ) {

      // remove failed device
      deleteDevice(dit);
    }
  }

  processEvents();
return true;
}

bool Kernel::begin() {

  if ( !SPIFFS.begin() ) {
    DEBUG_SERIAL("Can't mount SPIFFS!\nSPIFFS update needed!\n");
    return false;
  }

return true;
}

void Kernel::end() {
  // TODO: delete all devices, events and allocators
  SPIFFS.end();
}

// autoload devices from config files
void Kernel::autoloadDevices() {

  Dir dir = SPIFFS.openDir(F("/dev/"));
  while (dir.next()) {
    StringMap deviceParams;

    // initialize parameters to be updated from config file
    deviceParams.set(DRIVER_PARAM_NAME,"");
    deviceParams.set(DRIVER_PARAM_TYPE,"");

    if ( !deviceParams.updateMap(SPIFFS, dir.fileName().c_str()) ) {
      DEBUG_SERIAL("Failed to load: %s\n", dir.fileName().c_str());
      continue;
    }

    String deviceName = deviceParams.get(DRIVER_PARAM_NAME);
    String deviceType = deviceParams.get(DRIVER_PARAM_TYPE);

    if (deviceName == "" || deviceType == "") continue;

    DEBUG_SERIAL("Autoloading device \"%s\" (type: \"%s\") from file: %s\n",
      deviceName.c_str(), deviceType.c_str(), dir.fileName().c_str());

    createDevice(deviceType.c_str(), deviceName, false);
  }
}
