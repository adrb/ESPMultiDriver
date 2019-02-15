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

WiFiAPDriver::WiFiAPDriver() {

  //WiFi.persistent(false);  // don't store wifi configuration
  ESP.eraseConfig();
  WiFi.setAutoConnect(0);  // it seems that it's causing problems
  WiFi.disconnect(true);   // sdk can setup wifi before running sketch, so disable it

  // set default params
  params.set(DRIVER_PARAM_PASS, "");
  params.set("ip", "192.168.0.1");
  params.set("netmask", "255.255.255.0");

/*
  String ssid("EMD-");
  String macaddr(WiFi.softAPmacAddress());
  macaddr.replace(":","");
  macaddr.remove(0,6);
  ssid += macaddr;
  */
  String ssid("EMD-");
  params.set("ssid", ssid  + String(ESP.getChipId()) );

  ledTimer = millis();
}

bool WiFiAPDriver::begin() {

  IPAddress ip;
  ip.fromString(params.get("ip"));

  IPAddress netmask;
  netmask.fromString(params.get("netmask"));

  String pass = params.get(DRIVER_PARAM_PASS);

  DEBUG_SERIAL("APconfig: %s/%s, ssid: %s, pass: %s\n",
    ip.toString().c_str(), netmask.toString().c_str(),
    params.get(FCSTR("ssid")).c_str(), pass.c_str() );

  wifi_set_phy_mode(PHY_MODE_11G);
  WiFi.mode(WIFI_AP);
//WiFi.setOutputPower(10.0); // between 0 and 20.5 dbm
  WiFi.softAPConfig(ip, ip, netmask);

  WiFi.softAP(params.get("ssid").c_str(), (pass != "" ? pass.c_str() : NULL), 1, false, 1);
  WiFi.printDiag(Serial);

  return true;
}

void WiFiAPDriver::end() {
  setStatus(STOPPED);
  WiFi.softAPdisconnect(true);
}

bool WiFiAPDriver::run() {

  // blink twice, every 5sec if waiting for association
  if (WiFi.softAPgetStationNum() == 0 && (millis() - ledTimer) > 5000) {
    ledTimer = millis();

    // send "led_on" event to "kernel" device
    DriverEventInt *e = new DriverEventInt(DRIVER_KERNEL_ALOCNAME, F("led_on"), 2);
    e->setTime(100);  // event will wait 100ms in queue
    if ( e->handle() != DriverEvent::QUEUED ) {
      DEBUG_SERIAL("Can't queue led_on event, status: %d\n", e->getStatus());
      delete e;
    }
  }

return true;
}
