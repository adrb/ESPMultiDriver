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

#ifndef __TCPD_H__
#define __TCPD_H__

#include <vector>
#include <string.h>
#include <ESPAsyncTCP.h>
#include "DriverEvent.h"

#define DRIVER_TCPD_ALOCNAME "tcpd"

class TcpdDriver : public Driver {
  friend class DriverAllocator;

  AsyncServer tcpdServer;
  std::vector<AsyncClient*> clients;

  TcpdDriver() : tcpdServer(23) {}; // only DriverAllocator can create driver object
  static void handleClientData(void* arg, AsyncClient* client, void *data, size_t len);

public:
  class Allocator : public DriverAllocator {
    Driver* allocate() { return new TcpdDriver; }
  public:
    const char* name() override final { static const char* fname = DRIVER_TCPD_ALOCNAME; return fname; }
  };
  ~TcpdDriver() {};

  bool begin();
  bool run();
  void end();
};

#endif
