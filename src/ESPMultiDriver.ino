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

Kernel *kernel = &Kernel::getInstance();  // create Kernel object

void setup() {
  //ESP.eraseConfig();
  //ESP.reset();

  Serial.begin(115200);
  Serial.println();
  Serial.println(F("ESPMultiDriver..."));
  Serial.setDebugOutput(true);

  // Register all known device drivers
  kernel->registerDriverAllocator(new KernelDriver::Allocator);  // kernel event responder
  kernel->registerDriverAllocator(new WiFiAPDriver::Allocator);  // WiFi AP
  kernel->registerDriverAllocator(new HttpdDriver::Allocator);  // Web Server

  // Register not mandatory drivers
  kernel->registerDriverAllocator(new TcpdDriver::Allocator);
  kernel->registerDriverAllocator(new DCMotorDriver::Allocator);
  kernel->registerDriverAllocator(new PinDriver::Allocator);

  // initialize Kernel
  if ( !kernel->begin() ) {
    Serial.println(F("Kernel initialization problem!"));
  }

  // create mandatory devices in case someone disabled them
  kernel->createDevice(KERNEL_NAME, KERNEL_NAME);
  kernel->createDevice(WIFIAP_NAME, WIFIAP_NAME);
  kernel->createDevice(HTTPD_NAME, HTTPD_NAME);

  kernel->autoloadDevices();
}

void loop() {
  if ( !kernel->run() ) {
    kernel->end();
    delay(2000);
    //ESP.restart();
    ESP.reset();
  }
}
