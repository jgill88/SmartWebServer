/*
 * Title       OnStep Smart Web Server
 * by          Howard Dutton
 *
 * Copyright (C) 2016 to 2021 Howard Dutton
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 
 *
 * Revision History, see GitHub
 *
 *
 * Author: Howard Dutton
 * http://www.stellarjourney.com
 * hjd1964@gmail.com
 *
 * Description
 *
 * Web and IP Servers for OnStep and OnStepX
 *
 */

// Use Config.h to configure the SWS to your requirements

// See Constants.h for version information

#include "Constants.h"

#include "Config.h"
#include "ConfigX.h"
#include "src/HAL/HAL.h"
NVS nv;
#include "src/debug/Debug.h"
#include "src/tasks/OnTask.h"
Tasks tasks;

#include "src/pinmaps/Models.h"
#include "src/commands/Commands.h"
#include "src/ethernetServers/EthernetServers.h"
#include "src/wifiServers/WifiServers.h"
#include "src/pages/Pages.h"

void setup(void) {

  HAL_INIT();

  #if OPERATIONAL_MODE == WIFI
    wifiInit();
  #endif

  #if LED_STATUS != OFF
    pinMode(LED_STATUS_PIN, OUTPUT);
  #endif

  // EEPROM Init
  #if ENCODERS == ON
    if (nv.readI(EE_KEY_HIGH) != 8266 || nv.readI(EE_KEY_LOW) != 2) {
      nv.writeI(EE_ENC_AUTO_SYNC, ENC_AUTO_SYNC_DEFAULT);
      nv.writeL(EE_ENC_A1_DIFF_TO, AXIS1_ENC_DIFF_LIMIT_TO);
      nv.writeL(EE_ENC_A2_DIFF_TO, AXIS2_ENC_DIFF_LIMIT_TO);
      nv.writeL(EE_ENC_RC_STA, 20);     // enc short term average samples
      nv.writeL(EE_ENC_RC_LTA, 200);    // enc long term average samples
      nv.writeL(EE_ENC_RC_RCOMP, 0);    // enc rate comp
      nv.writeL(EE_ENC_RC_INTP_P, 1);   // intpol phase
      nv.writeL(EE_ENC_RC_INTP_M, 0);   // intpol mag
      nv.writeL(EE_ENC_RC_PROP, 10);    // prop
      nv.writeL(EE_ENC_MIN_GUIDE, 100); // minimum guide duration
      nv.writeL(EE_ENC_A1_ZERO, 0);     // absolute Encoder Axis1 zero
      nv.writeL(EE_ENC_A2_ZERO, 0);     // absolute Encoder Axis2 zero
      nv.writeD(EE_ENC_A1_TICKS, AXIS1_ENC_TICKS_DEG);
      nv.writeD(EE_ENC_A2_TICKS, AXIS2_ENC_TICKS_DEG);
      nv.writeI(EE_ENC_A1_REV, AXIS1_ENC_REVERSE);
      nv.writeI(EE_ENC_A2_REV, AXIS2_ENC_REVERSE);
    }

    if (ENC_AUTO_SYNC_MEMORY == ON) encAutoSync = nv.readI(EE_ENC_AUTO_SYNC);
    Axis1EncDiffTo = nv.readL(EE_ENC_A1_DIFF_TO);
    Axis2EncDiffTo = nv.readL(EE_ENC_A2_DIFF_TO);

    #if AXIS1_ENC_RATE_CONTROL == ON
      Axis1EncStaSamples = nv.readL(EE_ENC_RC_STA);
      Axis1EncLtaSamples = nv.readL(EE_ENC_RC_LTA);
      long l = nv.readLong(EE_ENC_RC_RCOMP);
      axis1EncRateComp = (float)l/1000000.0;
      #if AXIS1_ENC_INTPOL_COS == ON
        Axis1EncIntPolPhase = nv.readL(EE_ENC_RC_INTP_P);
        Axis1EncIntPolMag = nv.readL(EE_ENC_RC_INTP_M);
      #endif
      Axis1EncProp = nv.readL(EE_ENC_RC_PROP);
      Axis1EncMinGuide = nv.readL(EE_ENC_MIN_GUIDE);
    #endif

    Axis1EncTicksPerDeg = nv.readD(EE_ENC_A1_TICKS);
    Axis2EncTicksPerDeg = nv.readD(EE_ENC_A2_TICKS);
    Axis1EncRev = nv.readI(EE_ENC_A1_REV);
    Axis2EncRev = nv.readI(EE_ENC_A2_REV);
  #endif

  // attempt to connect to OnStep
  int serialSwap = OFF;
  if (OPERATIONAL_MODE == WIFI) serialSwap = SERIAL_SWAP;
  if (serialSwap == AUTO) serialSwap = AUTO_OFF;

  long serial_baud = SERIAL_BAUD;
  serialBegin(SERIAL_BAUD_DEFAULT, serialSwap);
  uint8_t tb = 1;

Again:
  clearSerialChannel();

  // look for OnStep
  Ser.print(":GVP#"); delay(100);
  String s = Ser.readString();
  if (s == "On-Step#" || s == "OnStepX#") {
    // check for fastest baud rate
    Ser.print(":GB#"); delay(100);
    if (Ser.available() != 1) { serialRecvFlush(); goto Again; }
    // Mega2560 returns '4' for 19200 baud recommended
    if (Ser.read() == '4' && serial_baud > 19200) serial_baud = 19200;

    // set fastest baud rate
    Ser.print(highSpeedCommsStr(serial_baud)); delay(100);
    if (Ser.available() != 1) { serialRecvFlush(); goto Again; }
    if (Ser.read() != '1') goto Again;
    
    // we're all set, just change the baud rate to match OnStep
    serialBegin(serial_baud, serialSwap);
    VLF("WEM: WiFi Connection established");
  } else {
    #if LED_STATUS == ON
      digitalWrite(LED_STATUS_PIN, LED_STATUS_OFF_STATE);
    #endif
    // got nothing back, toggle baud rate and/or swap ports
    serialRecvFlush();
    tb++;
    if (tb == 16) { tb = 1; if (serialSwap == AUTO_OFF) serialSwap = AUTO_ON; else if (serialSwap == AUTO_ON) serialSwap = AUTO_OFF; }
    if (tb == 1) serialBegin(SERIAL_BAUD_DEFAULT, serialSwap);
    if (tb == 6) serialBegin(serial_baud, serialSwap);
    if (tb == 11) { if (SERIAL_BAUD_DEFAULT == 9600) serialBegin(19200, serialSwap); else tb = 15; }
    goto Again;
  }

  // bring servers up
  clearSerialChannel();

  VLF("WEM: Starting port 80 web svr");
  #if OPERATIONAL_MODE == WIFI
    wifiStart();
  #else
    server.init();
  #endif

  VLF("WEM: Connecting web-page handlers");
  server.on("/", handleRoot);
  server.on("/index.htm", handleRoot);
  server.on("/configuration.htm", handleConfiguration);
  server.on("/configurationA.txt", configurationAjaxGet);
  server.on("/settings.htm", handleSettings);
  server.on("/settingsA.txt", settingsAjaxGet);
  server.on("/settings.txt", settingsAjax);
  #if ENCODERS == ON
    server.on("/enc.htm", handleEncoders);
    server.on("/encA.txt", encAjaxGet);
    server.on("/enc.txt", encAjax);
  #endif
  server.on("/library.htm", handleLibrary);
  server.on("/libraryA.txt", libraryAjaxGet);
  server.on("/library.txt", libraryAjax);
  server.on("/control.htm", handleControl);
  server.on("/controlA.txt", controlAjaxGet);
  server.on("/control.txt", controlAjax);
  server.on("/auxiliary.htm", handleAux);
  server.on("/auxiliaryA.txt", auxAjaxGet);
  server.on("/auxiliary.txt", auxAjax);
  server.on("/pec.htm", handlePec);
  server.on("/pec.txt", pecAjax);
  #if OPERATIONAL_MODE == WIFI
    server.on("/wifi.htm", handleWifi);
  #endif
  
  server.onNotFound(handleNotFound);

  #if STANDARD_COMMAND_CHANNEL == ON
    VLF("WEM: Starting port 9999 cmd svr");
    #if OPERATIONAL_MODE == WIFI
      cmdSvr.begin();
      cmdSvr.setNoDelay(true);
    #else
      cmdSvr.init(9999, 500);
    #endif
  #endif

  #if PERSISTENT_COMMAND_CHANNEL == ON
    VLF("WEM: Starting port 9998 persistant cmd svr");
    #if OPERATIONAL_MODE == WIFI
      persistentCmdSvr.begin();
      persistentCmdSvr.setNoDelay(true);
    #endif
  #endif

  #if OPERATIONAL_MODE == WIFI
    VLF("WEM: Starting port 80 web svr");
    server.begin();
  #endif

  // allow time for the background servers to come up
  delay(2000);

  // clear the serial channel one last time
  clearSerialChannel();

  #if ENCODERS == ON
    VLF("WEM: Starting Encoders");
    encoders.init();
  #endif
    
  VLF("WEM: SmartWebServer is ready");
}

void loop(void) {
  server.handleClient();

  #if ENCODERS == ON
    encoders.poll();
  #endif

  #if OPERATIONAL_MODE == WIFI
    wifiCommandChannel();
    wifiPersistantCommandChannel();
  #else
    ethernetCommandChannel();
  #endif
}
