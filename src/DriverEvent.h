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

#ifndef __DRIVER_EVENT_H__
#define __DRIVER_EVENT_H__

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

class Kernel;
class Driver;

// deliver events within 60sec (time in milisenconds)
#define DRIVEREVENT_EXPIRETIME 60*1000

class DriverEvent {
  friend class Kernel;

  String dstDeviceName;    // device which should handle event

  unsigned long handleTime;    // deliver event after specified time, 0 - deliver immediately
  unsigned long expireTime;    // drop event if it wasn't handled til that time

  virtual bool handleDevice(Driver *device __attribute__((unused))) { return true; };

public:
  enum EventStatus { INVALID, INITIALIZED, QUEUED, RUNNING, NOTDELIVERED, SUCCESS, FAILED, EXPIRED };
  EventStatus getStatus() { return status; }

  void setTime(unsigned long _handleTime = 0, unsigned long _expireTime = 0);
  String getDstDeviceName() { return dstDeviceName; }

  DriverEvent(String dstDevice);
  virtual ~DriverEvent();

  EventStatus handle();

protected:
  EventStatus status;
};

// -----------
// base class for multi-purpouse events
class DriverEventGeneric : public DriverEvent {

  String eventName;       // name of the event, it should be recognized by driver

  bool handleDevice(Driver *device) override;
public:

  DriverEventGeneric(String dstDevice, String eName);
  String getEventName() { return eventName; }
};

// exchange integer value
class DriverEventInt : public DriverEventGeneric {

  int intval;

  bool handleDevice(Driver *device) override;
public:

  DriverEventInt(String dstDevice, String eName, int _intval);

  int getInt() { return intval; }
  void setInt(int _intval) { intval = _intval; }
};

// exchange string value
class DriverEventString : public DriverEventGeneric {

  String strval;

  bool handleDevice(Driver *device) override;
public:

  DriverEventString(String dstDevice, String eName, String _strval);

  String getString() { return strval; }
  void setString(String _strval) { strval = _strval; }
};

// -----------
// standard events supported by all drivers

class DriverEventStop : public DriverEvent {
  bool handleDevice(Driver *device) override;
public:
  DriverEventStop(String dstDevice) : DriverEvent(dstDevice) {}
};

class DriverEventStart : public DriverEvent {
  bool handleDevice(Driver *device) override;
public:
  DriverEventStart(String dstDevice) : DriverEvent(dstDevice) {}
};

class DriverEventDisable : public DriverEvent {
  bool handleDevice(Driver *device) override;
public:
  DriverEventDisable(String dstDevice) : DriverEvent(dstDevice) {}
};

// Base class for json events
class DriverEventJson : public DriverEventGeneric {

  StaticJsonBuffer<1024> jsonBuffer; // TODO: reconsider, static or maybe dynamic buffer?
  JsonVariant jsonObj;

  bool handleDevice(Driver *device) override;
public:

  DriverEventJson(String dstDevice, String eName = "");

  JsonObject& getJsonObj();
  void setJsonObj(JsonVariant srcJsonV);
};

class DriverEventExportParams : public DriverEventJson {
  bool handleDevice(Driver *device) override;
public:
  DriverEventExportParams(String dstDevice) : DriverEventJson(dstDevice) {}
};

class DriverEventUpdateParams : public DriverEventJson {
  bool handleDevice(Driver *device) override;
public:
  DriverEventUpdateParams(String dstDevice) : DriverEventJson(dstDevice) {}
};

#endif
