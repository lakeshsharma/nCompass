#include <stdio.h>   // standard input / output functions
#include <stdlib.h>  // general standard library functions
#include <unistd.h>  // UNIX standard function definitions
#include <string.h>
#include <stdint.h>
//#include "rbcom/rbcom_serial_api.h"

char errmsg[255];


const char *serial_success_to_str(uint8_t code) {
  if (code == 0x01) {
    return "DFU Reboot";
  }
  ;
  if (code == 0x02) {
    return "Factory reset";
  }
  ;
  if (code == 0x10) {
    return "Serial port response is hex";
  }
  ;
  if (code == 0x11) {
    return "Serial port response is string";
  }
  ;
  if (code == 0x20) {
    return "Advertisement data updated";
  }
  ;
  if (code == 0x30) {
    return "Operation data updated";
  }
  ;
  if (code == 0x31) {
    return "Beacon turned off";
  }
  ;
  if (code == 0x32) {
    return "Beacon soft reboot";
  }
  ;
  if (code == 0x40) {
    return "Informational data updated";
  }
  ;
  if (code == 0x50) {
    return "PIN changed";
  }
  ;
  if (code == 0x51) {
    return "Locked";
  }
  ;
  if (code == 0x52) {
    return "Unlocked";
  }
  ;
  if (code == 0x60) {
    return "Telemetry interval updated";
  }
  ;
  // else
  return "Unknown";
}

const char *serial_error_to_str(uint8_t code) {
  if (code == 0x01) {
    return "Data overflow going into serial port";
  }
  ;
  if (code == 0x12) {
    return "The device did not recognize the command, Check version for API mismatch?";
  }
  ;
  if (code == 0x20) {
    return "Serial port tried to send too much data";
  }
  ;
  if (code == 0x30) {
    return "Invalid PIN";
  }
  ;
  if (code == 0x31) {
    return "New PIN invalid";
  }
  ;
  if (code == 0x32) {
    return "Invalid data";
  }
  if (code == 0x33) {
    return "Attached device does not support 4 character PIN";
  }
  ;
  if (code == 0x40) {
    return "Transmit power set to high";
  }
  ;
  if (code == 0x50) {
    return "Advertisement interval set too low";
  }
  ;
  if (code == 0x51) {
    return "Advertisement interval set too high";
  }
  ;
  if (code == 0x60) {
    return "Name too long";
  }
  ;
  if (code == 0x70) {
    return "Invalid beacon type identifier";
  }
  ;
  if (code == 0x80) {
    return "Connection timeout setting too low";
  }
  ;
  if (code == 0x81) {
    return "Connection timeout setting too high";
  }
  ;
  //else
  snprintf(errmsg, 255, "Unknown error code: %u (decimal)", code);
  return errmsg;
}
