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

#ifndef __DRIVER_H__
#define __DRIVER_H__

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "DriverEvent.h"

class StringMap;
class Driver;

class DriverAllocator {
  friend class Kernel;
  virtual Driver* allocate() = 0;
public:
  virtual const char* driverName() = 0;
};

// device mandatory parameters
static const char DRIVER_PARAM_NAME[] = "name";
static const char DRIVER_PARAM_TYPE[] = "type";
static const char DRIVER_PARAM_STATE[] = "state"; // 0 - stopped, 1 - running

// shared parameters names
// you can use it in multiple drivers to reduce memory overhead
static const char DRIVER_PARAM_USER[] = "user";
static const char DRIVER_PARAM_PASS[] = "pass";

// No delay() or yield() in classed derived from Driver.
// Instead generate DriverEvent with handleTime set
class Driver {
  friend class Kernel;

public:
  enum DriverStatus { RUNNING, STOPPED, DISABLED };

protected:
  StringMap params;
  DriverStatus status;

  Driver();
  void setStatus( DriverStatus _status );

  String configFileName();
  bool loadConfig();

  virtual bool begin();  // initialize driver
  virtual bool run();    // executed in loop
  virtual void end();    // stop device
public:
  virtual ~Driver();    // virtual - you should also call destructors for derived classes
  DriverStatus getStatus() { return status; };
  String getParam(String paramName);
  bool saveConfig();
  bool deleteConfig();

  // multi-purpouse events
  virtual bool handleEvent(DriverEventGeneric *event __attribute__((unused)) )  { return false; };
  virtual bool handleEvent(DriverEventInt *event __attribute__((unused)) )  { return false; };
  virtual bool handleEvent(DriverEventString *event __attribute__((unused)) )  { return false; };
  virtual bool handleEvent(DriverEventJson *event __attribute__((unused)) )  { return false; };

  // events designated for specific actions
  virtual bool handleEvent(DriverEventStop *event);
  virtual bool handleEvent(DriverEventStart *event);
  virtual bool handleEvent(DriverEventDisable *event);
  bool handleEvent(DriverEventExportParams *event); // export parameters to Json
  bool handleEvent(DriverEventUpdateParams *event); // update parameters from Json
};

#endif
