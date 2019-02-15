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

#include "definitions.h"

Kernel *kernel = &Kernel::getInstance();  // create Kernel object

void setup() {
  //ESP.eraseConfig();
  //ESP.reset();

  Serial.begin(115200);
  Serial.println();
  Serial.println(F("ESPMultiDriver..."));
  Serial.setDebugOutput(true);

  // Register all known device drivers
  KernelDriver::Allocator *kernelAlloc = new KernelDriver::Allocator;
  kernel->registerDriverAllocator(kernelAlloc);  // kernel event responder
  WiFiAPDriver::Allocator *wifiapAlloc = new WiFiAPDriver::Allocator;
  kernel->registerDriverAllocator(wifiapAlloc);  // WiFi AP
  HttpdDriver::Allocator *httpdAlloc = new HttpdDriver::Allocator;
  kernel->registerDriverAllocator(httpdAlloc);  // Web Server

  // Not mandatory drivers
  TcpdDriver::Allocator *tcpdAlloc = new TcpdDriver::Allocator;
  kernel->registerDriverAllocator(tcpdAlloc);  // TCP Server
  DCMotor::Allocator *dcmotorAlloc = new DCMotor::Allocator;
  kernel->registerDriverAllocator(dcmotorAlloc);

  // initialize Kernel
  if ( !kernel->begin() ) {
    Serial.println(F("Kernel initialization problem!"));
  }

  // create mandatory devices in case someone disabled them
  kernel->createDevice(kernelAlloc->name(), kernelAlloc->name());
  kernel->createDevice(wifiapAlloc->name(), wifiapAlloc->name());
  kernel->createDevice(httpdAlloc->name(), httpdAlloc->name());

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
