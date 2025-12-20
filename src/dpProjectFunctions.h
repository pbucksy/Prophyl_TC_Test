/* Include FreeRTOS APIs and defines */
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include <esp_task_wdt.h>
#include <SPI.h>
#include <Wire.h>

//#include <LeptonFLiR.h>

#include "lepton.h"



#define DEBUG

#ifdef DEBUG
  //#define debug(x) TelnetStream.print(x)
  //#define debugln(x) TelnetStream.println(x)
  #define debug(x) TelnetStream.print(x); Serial.print(x)
  #define debugln(x) TelnetStream.println(x); Serial.println(x)
#endif

#ifndef DEBUG
  #define debug(x)
  #define debugln(x)  
#endif

int resetReason = -1;

struct RGBColor {
    double r;
    double g;
    double b;
};