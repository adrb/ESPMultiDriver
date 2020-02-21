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

#ifndef __DEFS_H__
#define __DEFS_H__

static const char EMD_VERSION[] = "1.0.2";

#define DEBUG_ESP_PORT Serial
#define DEBUG_ESP_WIFI
//#undef DEBUG_WIFI

extern "C" {
  #include "user_interface.h"
}

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#else
#error Platform not supported
#endif

#include <FS.h>
#include <ArduinoJson.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define DEBUG_SERIAL(fmt, ...) Serial.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )

#define FSTR(_pmem) ( String(F(_pmem)) )
#define FCSTR(_pmem) ( String(F(_pmem)).c_str() )

#include "Utils.h"
#include "StringMap.h"
#include "DriverEvent.h"
#include "Driver.h"
#include "Kernel.h"
#include "Drivers/KernelDriver.h"
#include "Drivers/WiFiAP.h"
#include "Drivers/Httpd.h"
#include "Drivers/Tcpd.h"
#include "Drivers/DCMotor.h"
#include "Drivers/Pin.h"

#endif
