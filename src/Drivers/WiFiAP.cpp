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

WiFiAPDriver::WiFiAPDriver() {

  //WiFi.persistent(false);  // don't store wifi configuration
  //ESP.eraseConfig();
  //WiFi.setAutoConnect(0);  // it seems that it's causing problems
  WiFi.disconnect(true);   // sdk can setup wifi before running sketch, so disable it

  // set default params
  jconfig.set(DRIVER_PARAM_PASS, FSTR(""));
  jconfig.set("ip", FSTR("192.168.0.1"));
  jconfig.set("netmask", FSTR("255.255.255.0"));

/*
  String ssid("EMD-");
  String macaddr(WiFi.softAPmacAddress());
  macaddr.replace(":","");
  macaddr.remove(0,6);
  ssid += macaddr;
  */
  jconfig.set("ssid", FSTR("EMD-") + String(ESP.getChipId()) );

  ledTimer = millis();
}

bool WiFiAPDriver::begin() {

//  DEBUG_SERIAL("APconfig::begin --> %s/%s\n", jconfig.get("ip").as<String>().c_str(), jconfig.get("netmask").as<String>().c_str());

  IPAddress ip;
  ip.fromString(jconfig.get(FCSTR("ip")).as<String>());

  IPAddress netmask;
  netmask.fromString(jconfig.get(FCSTR("netmask")).as<String>());

  String pass = jconfig.get(DRIVER_PARAM_PASS).as<String>();

  DEBUG_SERIAL("APconfig: %s/%s, ssid: %s, pass: %s\n",
    ip.toString().c_str(), netmask.toString().c_str(),
    jconfig.get(FCSTR("ssid")).as<String>().c_str(), pass.c_str() );

  wifi_set_phy_mode(PHY_MODE_11G);
  WiFi.mode(WIFI_AP);
//WiFi.setOutputPower(10.0); // between 0 and 20.5 dbm
  WiFi.softAPConfig(ip, ip, netmask);

  WiFi.softAP(jconfig.get(FCSTR("ssid")).as<String>().c_str(), (pass != "" ? pass.c_str() : NULL), 1, false, 1);
  WiFi.printDiag(Serial);

  return true;
}

void WiFiAPDriver::end() {
  setStatus(STOPPED);
  WiFi.softAPdisconnect(true);
}

bool WiFiAPDriver::run() {

  // blink every 5sec, once if waiting for association, otherwise twice
  if ((millis() - ledTimer) > 5000) {
    ledTimer = millis();

    // send "led_on" event to "kernel" device
    DriverEventInt *e = new DriverEventInt(KERNEL_NAME, "led_on", WiFi.softAPgetStationNum() == 0 ? 1 : 2);
    e->setTime(100);  // event will wait 100ms in queue
    if ( e->handle() != DriverEvent::QUEUED ) {
      DEBUG_SERIAL("Can't queue led_on event, status: %d\n", e->getStatus());
      delete e;
    }
  }

return true;
}
