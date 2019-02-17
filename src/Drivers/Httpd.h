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

#ifndef __HTTPD_H__
#define __HTTPD_H__

#include <FS.h>
#include <AsyncJson.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "DriverEvent.h"

static const char HTTPD_NAME[] = "httpd";
static const char HTTPD_API_URI[] = "/api";

class HttpdDriver : public Driver {
  friend class DriverAllocator;

  AsyncWebServer httpServer;
  HttpdDriver() : httpServer(80) {}; // only DriverAllocator can create driver object

public:
  class Allocator : public DriverAllocator {
    Driver* allocate() { return new HttpdDriver; }
  public:
    const char* driverName() override final { return HTTPD_NAME; }
  };
  ~HttpdDriver() {};

  bool begin();
  bool run();
  void end();

};

#endif
