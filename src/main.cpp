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
unsigned long readingFrameTimer = 0;
bool isInit = true;

int kPinLepCS = 5;
int kPinLepSck = 18;
int kPinLepMiso = 19;
int kPinLepRst = 26;
int kPinLepPWDown = 27;

//LeptonFLiR flirController(Wire,5);

FlirLepton lepton(Wire, SPI, kPinLepCS, kPinLepRst, kPinLepRst);
uint8_t vospiBuf[160*120*3] = {0};  // up to RGB888, double-buffered

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

  

  // wait for post-flash reset
  
    

  SPI.begin();
  Wire.begin();
    
  networkConnStatus = setupNetwork(99);

  
  //flirController.init(LeptonFLiR_ImageStorageMode_80x60_8bpp, LeptonFLiR_TemperatureMode_Celsius);
  //flirController.init(LeptonFLiR_ImageStorageMode_40x30_8bpp);  
  

  // Setting use of AGC for histogram equalization (since we only have 8-bit per pixel data anyways)
  flirController.agc_setAGCEnabled(ENABLED);
  // Ensure telemetry is enabled
  flirController.sys_setTelemetryEnabled(ENABLED); 

<<<<<<< HEAD
  debugln();debugln();debugln();
  debug("Flir controller chip select pin: ");debugln(flirController.getChipSelectPin());
  debugln();debugln();debugln();
=======
  
  

  Serial.println("Lepton start");  
  assert(lepton.begin());

  while (!lepton.isReady()) {
    delay(1);
  }
  Serial.println("Lepton ready");

  Serial.print("Lepton Serial = ");
  Serial.print(lepton.getFlirSerial());
  Serial.println("");

  Serial.print("Lepton Part Number = ");
  Serial.print(lepton.getFlirPartNum());
  Serial.println("");


  //Serial.print("LeptonSoftwareVersion: "); Serial.println(lepton.getFlirSoftwareVerison()[0]);

  

  assert(lepton.enableVsync());

  delay(5000);

  

  //flirController.printModuleInfo();
>>>>>>> 3c7d6c32c43421b90eb5c38f94ab4deda49f5b1c

  flirController.printModuleInfo();
  debugln();
  debugln("Begin LOOP");
  debugln();
  
    
}


uint32_t lastFrameNumber = -1;          // Tracks for when a new frame is available

unsigned long readingFrameTimer = millis();

int okFrameCntr = 0;

void loop()
{

  //wifiWebServer.handleClient();

<<<<<<< HEAD
  if(false && millis() - checkUpdateTimer > 10000)
=======
  if(millis() - checkUpdateTimer > 60000)
>>>>>>> 3c7d6c32c43421b90eb5c38f94ab4deda49f5b1c
  {
    checkUpdateTimer = millis();
    searchForUpdate();     
  }
  
  if(lepton.readVoSpi(sizeof(vospiBuf), vospiBuf))    
  {
    debug("Frame read OK! - ");debugln(okFrameCntr++);
  }
  

<<<<<<< HEAD
  if(millis() - readingFrameTimer > 10000)
  {
=======

   
  
  /*
  if (flirController.readNextFrame()) { // Read next frame and store result into internal imageData
>>>>>>> 3c7d6c32c43421b90eb5c38f94ab4deda49f5b1c
    debugln("Reading next frame...");
  
    if (flirController.readNextFrame()) { // Read next frame and store result into internal imageData
      

          
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

<<<<<<< HEAD
    } 

    readingFrameTimer = millis();
  }  
=======
  }  
        */ 
>>>>>>> 3c7d6c32c43421b90eb5c38f94ab4deda49f5b1c
}

