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

bool HttpdDriver::begin() {

  httpServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect(F("/index.htm"));
  });

  AsyncCallbackJsonWebHandler* jsonHandler = new AsyncCallbackJsonWebHandler(HTTPD_API_URI, [](AsyncWebServerRequest *request, JsonVariant &json) {

    JsonObject& jsonObj = json.as<JsonObject>();
    String eventName;
    String deviceName;

    if ( jsonObj.containsKey(F("event")) ) {
      eventName = jsonObj.get<String>(F("event"));
    }
    if ( jsonObj.containsKey(F("device")) ) {
      deviceName = jsonObj.get<String>(F("device"));
    }

    if ( eventName == "" || deviceName == "" || !jsonObj.containsKey(F("params"))) {
      request->send(400);
      return;
    }
    //DEBUG_SERIAL("event: %s, device: %s\n", eventName.c_str(), deviceName.c_str());

    AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
    JsonObject& jsonParams = jsonObj.get<JsonVariant>(F("params"));

    // curl -H "Content-Type: application/json" -X POST --data '{"event":"get_params","device":"wifiap","params":{}}' "http://192.168.0.1/api" -si
    if ( eventName == F("get_params") ) {
      DriverEventExportParams exportParams(deviceName);
      if ( exportParams.handle() != DriverEvent::SUCCESS )
        response->setCode(500);
      else
        exportParams.getJsonObj().printTo(*response);
      request->send(response);
      return;
    }

    // curl -H "Content-Type: application/json" -X POST --data '{"event":"set_params","device":"wifiap","params":{"pass":"12345678"}}' "http://192.168.0.1/api" -si
    if ( eventName == F("set_params") ) {
      DriverEventUpdateParams updateParams(deviceName);
      updateParams.setJsonObj(jsonParams);

      if ( updateParams.handle() != DriverEvent::SUCCESS ) {
        response->setCode(500);
      } else {
        response->setCode(200);
      }
      request->send(response);
      return;
    }

    if ( eventName == F("stop") ) {
      DriverEventStop stopDev(deviceName);
      if ( stopDev.handle() != DriverEvent::SUCCESS ) {
        response->setCode(500);
      } else {
        response->setCode(200);
      }
      request->send(response);
      return;
    }

    // curl -H "Content-Type: application/json" -X POST --data '{"event":"start","device":"motor1","params":{}}' "http://192.168.0.1/api" -si
    if ( eventName == F("start") ) {
      DriverEventStart startDev(deviceName);
      if ( startDev.handle() != DriverEvent::SUCCESS ) {
        response->setCode(500);
      } else {
        response->setCode(200);
      }
      request->send(response);
      return;
    }

    if ( eventName == F("disable") ) {
      DriverEventDisable disableDev(deviceName);
      if ( disableDev.handle() != DriverEvent::SUCCESS ) {
        response->setCode(500);
      } else {
        response->setCode(200);
      }
      request->send(response);
      return;
    }

    // Generic json event
    DriverEventJson jsonEvent(deviceName, eventName);
    jsonEvent.setJsonObj( jsonParams );
    if ( jsonEvent.handle() != DriverEvent::SUCCESS ) {
      response->setCode(500);
    } else {
      jsonEvent.getJsonObj().printTo(*response);
      response->setCode(200);
    }
    request->send(response);

  });
  httpServer.addHandler(jsonHandler);

  httpServer.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404);
  });

  httpServer.serveStatic("/", SPIFFS, "/www");
  httpServer.begin();

  return true;
}

void HttpdDriver::end() {
  setStatus(STOPPED);
  httpServer.reset();
}

bool HttpdDriver::run() {

return true;
}
