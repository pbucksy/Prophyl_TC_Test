#include "dpWiFiManager.h"
#include <SD.h>



//Some stuff so that it shows it changed...

static const BaseType_t core_0 = 0;
static const BaseType_t core_1 = 1;

int networkConnStatus = 0;

WiFiClient tcpClient;

unsigned long tcpClientTimeoutTimer = 0;
bool toggleBool = false;


String telnetMsg = "";
unsigned long battInfoTimer = 0;
unsigned long statusTimer = 0;
unsigned long checkUpdateTimer = 0;
bool isInit = true;



LeptonFLiR flirController(Wire,5);

byte leptonChipSelectPin = 0;
void setup() {  

  Serial.begin(115200);  
  delay(1000);

  Serial.println();
  Serial.println("In da beginning!");
  
  EEPROM.begin(EEPROMConfigSize); 
  
  
  Wire.begin();
  Wire.setClock(400000);

  SPI.begin();
    
  networkConnStatus = setupNetwork(99);

  
  //flirController.init(LeptonFLiR_ImageStorageMode_80x60_8bpp, LeptonFLiR_TemperatureMode_Fahrenheit);
  flirController.init(LeptonFLiR_ImageStorageMode_40x30_8bpp);  
  

  // Setting use of AGC for histogram equalization (since we only have 8-bit per pixel data anyways)
  //flirController.agc_setAGCEnabled(ENABLED);
  // Ensure telemetry is enabled
  //flirController.sys_setTelemetryEnabled(ENABLED); 

  leptonChipSelectPin = flirController.getChipSelectPin();

  //flirController.printModuleInfo();

  debugln("Begin LOOP");
  
    
}


uint32_t lastFrameNumber = -1;          // Tracks for when a new frame is available


void loop()
{

  wifiWebServer.handleClient();

  if(millis() - checkUpdateTimer > 10000)
  {
    checkUpdateTimer = millis();
    searchForUpdate(); 
    debug("Lepton Chip Select pin: ");debugln(leptonChipSelectPin);
  }

  if (flirController.readNextFrame()) { // Read next frame and store result into internal imageData
    debugln("Reading next frame...");

        
    // Find the hottest spot on the frame
    int hotVal = 0; 
    int hotX   = 0; 
    int hotY   = 0;
    

    for (int y = 0; y < flirController.getImageHeight(); ++y) {
        for (int x = 0; x < flirController.getImageWidth(); ++x) {
            int val = flirController.getImageDataRowCol(y, x);

            if (val > hotVal) {
                hotVal = val;
                hotX = x; hotY = y;
            }
        }
    }

    debug("Millis: ");
    debug(millis());
    debug(" | ");
    debug("Hottest point: [");
    debug(hotX);
    debug(",");
    debug(hotY);
    debugln("]");

    
    // Occasionally flat field correction normalization needs ran
    if (flirController.getShouldRunFFCNormalization())
        flirController.sys_runFFCNormalization();

  }   
}

