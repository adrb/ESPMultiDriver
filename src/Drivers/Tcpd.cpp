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

void TcpdDriver::handleClientData(void* arg, AsyncClient* client, void *data, size_t len) {

  static char cmd[128];        // static command buffer :/
  static size_t cmdIndex = 0;

  //Serial.write((uint8_t*)data, len);

  for ( size_t i = 0 ; i < len ; i++ ) {
    char c = *((char*)data + i);

    //Serial.print(c);

    if ( c == '\n' || c == '\r' || c == '\0' ) {
      cmd[cmdIndex] = '\0';

      // parse event command
      const char tokendelim = ' ';
      size_t tokenlen = cmdIndex;
      char *strtoken = (char*)cmd;
      cmdIndex = 0;

      char *dstDevice = xstrtok(&strtoken, &tokenlen, tokendelim);
      char *eventName = xstrtok(&strtoken, &tokenlen, tokendelim);
      char *clientMsg = strtoken; // remaining string

      if (!dstDevice || !eventName || !clientMsg) return;
      //DEBUG_SERIAL("tcpd: %s, %s, %s\n", dstDevice, eventName, clientMsg);

      DriverEventString strEvent(dstDevice, eventName, clientMsg);
      if ( strEvent.handle() != DriverEvent::SUCCESS ) return;

      // send back response
      String deviceReplyMsg = strEvent.getString();
      if (client->space() > deviceReplyMsg.length() && client->canSend()) {
        client->add(deviceReplyMsg.c_str(), deviceReplyMsg.length());
        client->send();
      }

    } else {
      if( cmdIndex < sizeof(cmd) ) {
        cmd[cmdIndex++] = c;
      } else {
        // command too long
        cmd[cmdIndex] = 0;
        cmdIndex = 0;
      }
    }
  }
}

bool TcpdDriver::begin() {

  tcpdServer.onClient([&](void* arg, AsyncClient* client) {
    DEBUG_SERIAL("New client has been connected to server, ip: %s\n", client->remoteIP().toString().c_str());

    clients.push_back(client);  // add to list

    // register events
    client->onData(&handleClientData, NULL);
    //client->onError(&handleError, NULL);
    //client->onDisconnect(&handleDisconnect, NULL);
    //client->onTimeout(&handleTimeOut, NULL);

  }, &tcpdServer);

  tcpdServer.begin();

  return true;
}

void TcpdDriver::end() {
  setStatus(STOPPED);
  tcpdServer.end();
}

bool TcpdDriver::run() {

return true;
}
