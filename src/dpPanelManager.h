
#include <Arduino.h>
#include "dpPanelWebserver.h"
#include <EEPROM.h>
#include <Ethernet_Generic.h>
#include <EthernetWebServer.h>
//#include <ArduinoHttpClient.h>
#include <HttpClient.h>
#include <TelnetStream.h>
#include "esp_partition.h"
#include "SPIFFS.h"


//PlatformIO Libraries
#include <NeoPixelBus.h>
#include <Update.h>
#include <esp_task_wdt.h>



//External libraries


//10 seconds WDT
#define WDT_TIMEOUT 10

#define DEBUG

#ifdef DEBUG
  //#define debug(x)   {if(isEthLinkOn){TelnetStream.print(x);if(ethTelnetConnected){ethTelnetClient.print(x);}}else{Serial.print(x);}}
  //#define debugln(x) {if(isEthLinkOn){TelnetStream.println(x);if(ethTelnetConnected){ethTelnetClient.println(x);}}else{Serial.println(x);}}                                                 
  //#define debug(x)   {if(isEthLinkOn){TelnetPrint.print(x);if(ethTelnetConnected){ethTelnetClient.print(x);}}else{Serial.print(x);}}
  //#define debugln(x) {if(isEthLinkOn){TelnetPrint.println(x);if(ethTelnetConnected){ethTelnetClient.println(x);}}else{Serial.println(x);}}
  #define debug(x) {Serial.print(x);}  
  #define debugln(x) {Serial.println(x);}                                                 
#endif

#ifndef DEBUG
  #define debug(x)
  #define debugln(x)  
#endif

// location of firmware file on external web server
// change to your actual .bin location
//#define firmwareRootPath "/firmwares/"
//#define firmwarePath "/"
// Global variables

int fwTotalLength = 0; //total size of firmware
int fwCurrentLength = 0; //current size of written firmware
double lastPercent = 0;

 
//EEPROM Memory addresses
#define ssidMemAddress 0
#define passMemAddress 40
#define hostMemAddress 80
#define gatewayMemAddress 140
#define subnetMemAddress 160
#define initMemAddress 180
#define urlMemAddress 200
#define deviceIDMemAddress 300
#define apiURLMemAddress 310
#define apiPortMemAddress 390
#define apiPathMemAddress 400
#define ethIpMemAddress 500



//Firmware Update Parameters
#define firmwareFilenameMemAddress        260
#define firmwareURLMemAddress             550
#define firmwarePathMemAddress            600
#define firmwareLastModifiedMemAddress    700
#define firmwareUpdateRequestMemAddress   799


//Sensor RelatedMemAddresses
#define sensorsCntrMemAddress            1000
#define sensorsDataStartMemAddress       1001

//Sensor Data sizes
#define sensorBlockAlocatedSize                30
#define sensorValueAllocatedSize                10


//wsLedPin
#define wsLedPin  12
#define wsLedBrightness 100
const uint16_t PixelCount = 1;
const uint8_t PixelPin = wsLedPin;
NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt0800KbpsMethod> strip(PixelCount, PixelPin);

//RTOD Task related stuff
static const BaseType_t core_0 = 0;
static const BaseType_t core_1 = 1;
//Task declarations
void telnetTask( void * parameter );

TaskHandle_t telnetTask_Handle;



struct rgbStatusLed
{
  int red;
  int green;
  int blue;    
};

int checkHostCntr = 0;

rgbStatusLed myStatusLed;

//Panel Specific
  byte sbid = 0x99;


//Ethernet Parameters
  
  EthernetClient    ethFWClient;
  EthernetClient    ethFSClient;
  EthernetClient    ethPostClient;
  
  //NetClient    ethTelnetClient;  
  EthernetServer    ethTelnetServer(23);
  EthernetClient    ethTelnetClient;

  
  String telnetMsg = "";
  String ethTelnetMsg = "";
  bool shouldPost = false;
  unsigned long ExternalOTATimer = millis();

  #define _ethWebServer
  #ifdef _ethWebServer
  EthernetWebServer ethWebServer(80);
  #endif

  IPAddress         ethIP(0,0,0,0);
  byte              ethMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  bool isEthernetPresent = false;
  bool ethTelnetConnected = false;
  bool isEthLinkOn = false;  
  int ethReset = 15;
  int ethSS = 2;

  


String ssidString;
String passwordString;

//ESP WebServer initial values
String init_panelID = "Prophyl_SensorPanel";
String init_espGateway = "192.168.41.99";
String init_espSubnet = "255.255.255.0";
String init_projectUrl = "projectURL";
String init_deviceID = "-1";
String init_apiURL = "192.168.41.33";
String init_apiPort = "8080";
String init_apiPath = "/";
String init_espEthIP = "192.168.41.99";
String init_firmwareURL = "192.168.41.33";
String init_firmwarePath = "/firmwares/";
String init_firmwareFilename = "prophyl_Sensor_51_ESP32.ino.bin";
String init_firmwareLastModified = "Last modified: Firmware has not yet been modified!";

String init_interval_min = "1";


String panelID = "";
String espWiFiMacAddress = "";
String espEthMacAddress = "";
String espGateway = "";
String espSubnet = "";
String projectUrl = "";
String deviceID = "";
String apiURL = "";
String apiPort = "";
String apiPath = "";
String espEthIP = "";
String firmwareURL = "";
String firmwarePath = "";
String firmwareFilename = "";
String firmwareLastModified = "";

String server_interval_min = "";



IPAddress gateway;
IPAddress subnet;
IPAddress apIP(192,168,33,1);
IPAddress lastIP(0,0,0,0);

//IPAddress projectServerIP = IPAddress(0,0,0,0);




bool blinkBool = false;
String latestFWFilaName = "";
String IpAddressToString(const IPAddress& ipAddress);
IPAddress StringToIPAddress(String ipString);
String splitString(String data, char separator, int index);
String readStringFromEEPROM(int memAddress);
void saveStringToEEPROM(String _string, int memAddress);
void searchForUpdate();
void updateFirmware(uint8_t *data, size_t len, String NewFWDateTime);
void clearEEPROM();
void getPanelInfo();

//WebPage Functions
void handleDataPage();
void projectConfigPage_html();
void saveProjectConfig_html();
void ethConfigPage_html();
void saveEthConfig_html();
void firmwareConfigPage_html();
void saveFirmwareConfig_html();

void handleNotFound();
void onHardReset();
void onSoftReset();
void returnFail(String msg);
void returnOK();


void setPixel(int r, int g, int b);






int initPanel()
{   
    

    Serial.println("Setting up Network");   
    //onHardReset();
    //disableCore0WDT();
    //disableCore1WDT();
    int retVal = 0;      
    if(wsLedPin != -1)
    {

          
      strip.Begin();
      strip.Show();

      //FastLED.addLeds<WS2812B,wsLedPin,GRB>(leds,1,0);
      //FastLED.setBrightness(254);
      
      Serial.println("NeoPixel initialization...");
      
      setPixel(5,5,5);
      delay(500);
      setPixel(0,0,0);
    }
    
    

    //for(int i = 0; i<999;i++){EEPROM.write(i,255);} //Clearing Config EEPROM!
    //EEPROM.commit();
    
    
    if(EEPROM.read(initMemAddress) == 123)
    {      
      
      ssidString = "DPWLan";
      passwordString = "dopeedwifi";
      //ssidString = readStringFromEEPROM(ssidMemAddress);    
      //passwordString = readStringFromEEPROM(passMemAddress);      
      espGateway = readStringFromEEPROM(gatewayMemAddress);
      espSubnet = readStringFromEEPROM(subnetMemAddress); 
    }
    else
    {
                  
      //saveStringToEEPROM("-", ssidMemAddress);
      //saveStringToEEPROM("-", ssidMemAddress);

      ssidString = "DPWLan";   
      passwordString = "dopeedwifi";      
      espGateway = init_espGateway;
      espSubnet = init_espSubnet;     
    }

    
    
    gateway =  IPAddress(splitString(espGateway,'.',0).toInt(),splitString(espGateway,'.',1).toInt(),splitString(espGateway,'.',2).toInt(),splitString(espGateway,'.',3).toInt());
    subnet =   IPAddress(splitString(espSubnet,'.',0).toInt(),splitString(espSubnet,'.',1).toInt(),splitString(espSubnet,'.',2).toInt(),splitString(espSubnet,'.',3).toInt());

    
    char ssid[ssidString.length()+1];
    ssidString.toCharArray(ssid,ssidString.length()+1);

    char password[passwordString.length()+1];
    passwordString.toCharArray(password,passwordString.length()+1);    

    char charHostname[panelID.length()+1];
    panelID.toCharArray(charHostname,panelID.length()+1);
    
    

    if(EEPROM.read(initMemAddress) == 123)
    {      
      
      
      //ssidString = readStringFromEEPROM(ssidMemAddress);    
      //passwordString = readStringFromEEPROM(passMemAddress);
      ssidString = "DPWLan";
      passwordString = "dopeedwifi";
      panelID = readStringFromEEPROM(hostMemAddress);
      panelID = readStringFromEEPROM(hostMemAddress);      
      espGateway = readStringFromEEPROM(gatewayMemAddress);
      espSubnet = readStringFromEEPROM(subnetMemAddress);
      projectUrl = readStringFromEEPROM(urlMemAddress);        
      deviceID = readStringFromEEPROM(deviceIDMemAddress);  
      apiURL = readStringFromEEPROM(apiURLMemAddress);  
      apiPort = readStringFromEEPROM(apiPortMemAddress);  
      apiPath = readStringFromEEPROM(apiPathMemAddress);
      espEthIP = readStringFromEEPROM(ethIpMemAddress);      
      firmwareURL = readStringFromEEPROM(firmwareURLMemAddress);
      firmwarePath = readStringFromEEPROM(firmwarePathMemAddress);
      firmwareFilename = readStringFromEEPROM(firmwareFilenameMemAddress); 
      firmwareLastModified = EEPROM.readString(firmwareLastModifiedMemAddress);    

      
    }
    else
    {
      ssidString = "DPWLan";   
      passwordString = "dopeedwifi";
      panelID = init_panelID;      
      espGateway = init_espGateway;
      espSubnet = init_espSubnet;
      projectUrl = init_projectUrl;      
      deviceID = init_deviceID;
      apiURL = init_apiURL;
      apiPort = init_apiPort;
      apiPath = init_apiPath; 
      espEthIP = init_espEthIP; 
      server_interval_min = init_interval_min;   
      firmwareURL = init_firmwareURL; 
      firmwarePath = init_firmwarePath;
      firmwareFilename = init_firmwareFilename;
      firmwareLastModified = init_firmwareLastModified;

      saveStringToEEPROM(ssidString,ssidMemAddress);
      saveStringToEEPROM(passwordString,passMemAddress);
      saveStringToEEPROM(panelID,hostMemAddress);      
      saveStringToEEPROM(espGateway,gatewayMemAddress);
      saveStringToEEPROM(espSubnet,subnetMemAddress);
      saveStringToEEPROM(projectUrl,urlMemAddress);
      saveStringToEEPROM(deviceID,deviceIDMemAddress);
      saveStringToEEPROM(apiURL,apiURLMemAddress);
      saveStringToEEPROM(apiPort,apiPortMemAddress);
      saveStringToEEPROM(apiPath,apiPathMemAddress);
      saveStringToEEPROM(espEthIP,ethIpMemAddress);
      saveStringToEEPROM(firmwareURL,firmwareURLMemAddress);
      saveStringToEEPROM(firmwarePath,firmwarePathMemAddress);
      saveStringToEEPROM(firmwareFilename,firmwareFilenameMemAddress);
      EEPROM.writeString(firmwareLastModifiedMemAddress,firmwareLastModified);



    }

    //Test ip
    //cespEthIP = "192.168.1.90";


    setPixel(5,5,0);

    char projectWorkTitle_char[panelID.length()+1];
    panelID.toCharArray(projectWorkTitle_char,panelID.length()+1);
    

    if(ethSS != -1)
    {
      //ETHERNET INIT
      debugln("ETHERNET RESET!");

      /*
      if(true)
      {
        //Test values!!! 
        
        String manualEspEthIp = "192.168.41.61";
        Serial.print("Setting ip manually to -> ");
        Serial.println(manualEspEthIp);
          
        ethIP = StringToIPAddress(manualEspEthIp);
        espEthIP = manualEspEthIp;
        saveStringToEEPROM(manualEspEthIp,ethIpMemAddress);      
        EEPROM.commit();
        
        
        //Test Pin Values...
        

      }
      */

      
      

      pinMode(ethReset,OUTPUT);
      pinMode(ethSS, OUTPUT);

      digitalWrite(ethReset,LOW);
      delay(500);
      digitalWrite(ethReset,HIGH);   
      delay(1000);
      
      ethIP = StringToIPAddress(espEthIP);
      sbid = ethIP[3];
      debug("SBID: ");debugln(sbid);
      
      
      Ethernet.setCsPin(ethSS);
      Ethernet.setRstPin(ethReset);
      Ethernet.setGatewayIP(IPAddress(192,168,41,1));
      Ethernet.setDnsServerIP(IPAddress(192,168,41,1));
      Ethernet.setSubnetMask(IPAddress(255,255,255,0));

      
      
      debugln("ETHERNET INIT!");
      Ethernet.init(ethSS);         
      
      delay(500);

      
      ethMac[5] = ((ethIP[3] / 10) * 16) + (ethIP[3] % 10);
      debug("ethMAC[5]: ");debugln(ethMac[5]);
      
      if(sbid != 0xED)
      {
        //Static IP Setup
        //int IP_last_octett = ((_sbid / 16)*10) + (_sbid % 16);      
        //eth_ip = IPAddress(192,168,1,IP_last_octett);
        //projectServerIP = IPAddress(192,168,1,94);
        debugln("Static IP Ethernet Init");
        
        Ethernet.begin(ethMac, ethIP);
        //Ethernet.begin(ethMac, _ethIP,_ethIP,IPAddress(192,168,1,2),IPAddress(255,255,255,0));
      }
      else
      {
        //DHCP Steup
        debugln("DHCP Ethernet Init");
        if(Ethernet.begin(ethMac) == 0)
        {
          debugln("Static IP Ethernet Init");
          Ethernet.begin(ethMac, ethIP);
        }        
      }

      debug("Ethernet got IP: ");debugln(Ethernet.localIP().toString());
      

      if(Ethernet.localIP() == ethIP)
      {
        debugln("W5500 is present!");
        isEthernetPresent = true;
        debug("Ethernet Chip: ");debugln(Ethernet.getChip());              
        unsigned long linkTimeoutTimer = millis();
        
          
        espEthMacAddress  = String(ethMac[0],HEX) + ":";
        espEthMacAddress += String(ethMac[1],HEX) + ":";
        espEthMacAddress += String(ethMac[2],HEX) + ":";
        espEthMacAddress += String(ethMac[3],HEX) + ":";
        espEthMacAddress += String(ethMac[4],HEX) + ":";
        espEthMacAddress += String(ethMac[5],HEX);

        debug("Ethernet MAC Address: ");debugln(espEthMacAddress);

        while(Ethernet.linkStatus() != LinkON && millis() - linkTimeoutTimer <3000){}    
        if(Ethernet.linkStatus() == LinkON)
        { 
          
          Serial.println("Link status: On");
          isEthLinkOn = true;
          Serial.print("Assigned Ethernet IP Address: ");Serial.println(Ethernet.localIP()); 

          Serial.print("Ethernet PHY State: ");Serial.println(Ethernet.phyState());
          Serial.print("Ethernet Link Report: ");Serial.println(Ethernet.linkReport());
          Serial.print("Ethernet Speed Report: ");Serial.println(Ethernet.speedReport());
          Serial.print("Ethernet Duplex Report: ");Serial.println(Ethernet.duplexReport());
          
          

          
          
          #ifdef _ethWebServer
          ethWebServer.on("/", [](){handleDataPage();});      
          ethWebServer.on("/projectConfig", [](){projectConfigPage_html();});
          ethWebServer.on("/saveProjectConfig", [](){saveProjectConfig_html();});
          ethWebServer.on("/ethConfig", [](){ethConfigPage_html();});
          ethWebServer.on("/saveEthConfig", [](){saveEthConfig_html();});
          ethWebServer.on("/firmwareConfig", [](){firmwareConfigPage_html();});
          ethWebServer.on("/saveFirmwareConfig", [](){saveFirmwareConfig_html();});
          ethWebServer.on("/updateFirmware", [](){searchForUpdate();});          
          ethWebServer.on("/hardReset", onHardReset);
          ethWebServer.on("/softReset", onSoftReset);
          ethWebServer.on("/clearEEPROM", clearEEPROM);             
          ethWebServer.on("/inline", []() {ethWebServer.send(200, "text/plain", "this works as well");});    
          ethWebServer.onNotFound(handleNotFound);   
          ethWebServer.begin(); 
          #endif  

          retVal = 1;
          
          
          Serial.println("Starting telnetTask on Core1");
          xTaskCreatePinnedToCore(telnetTask, "Telnet_Task", 3000, NULL, 10, &telnetTask_Handle, core_1); 

          
          
        }        
        else
        {
          Serial.println("Link status: Off / Not detected!");
          debugln("Link status: Off / Not detected!");
          isEthernetPresent = false;

        }

      }
      else
      {
        Serial.println("Eth Config error!");
        debugln("Eth Config error!");
      }
    }
        

    
    if(retVal == 0)
      {setPixel(0,0,5);}
    else if(retVal == 1)
      {setPixel(0,5,0);}
    else
      {setPixel(5,0,0);}

      
    Serial.println("InitPanel Done!");
    Serial.print("Ethernet IP: ");Serial.println(Ethernet.localIP().toString());
      
    return retVal;   
}


void onSoftReset()
{    
  #ifdef _ethWebServer
  ethWebServer.send(200, "text/html", softReset_HTML);
  #endif
  delay(1000);
  ESP.restart();
}

void clearEEPROM()
{
  for(int i = 0; i<3999;i++){EEPROM.write(i,255);} //Clearing EEPROM!  
  EEPROM.commit();
  EEPROM.end();
  
  #ifdef _ethWebServer
  ethWebServer.send(200, "text/html", hardReset_HTML);
  #endif
  delay(1000);
  ESP.restart();
}

void onHardReset()
{  
  for(int i = 0; i<3999;i++){EEPROM.write(i,255);} //Clearing Config EEPROM!

  EEPROM.commit();
  EEPROM.end();
  
  #ifdef _ethWebServer
  ethWebServer.send(200, "text/html", hardReset_HTML);
  #endif
  delay(1000);
  ESP.restart();
}

void returnFail(String msg)
{
  #ifdef _ethWebServer
  ethWebServer.sendHeader("Connection", "close");
  ethWebServer.sendHeader("Access-Control-Allow-Origin", "*");
  ethWebServer.send(500, "text/plain", msg + "\r\n");
  #endif
}


void returnOK()
{
  #ifdef _ethWebServer
  ethWebServer.sendHeader("Connection", "close");
  ethWebServer.sendHeader("Access-Control-Allow-Origin", "*");
  ethWebServer.send(200, "text/plain", "OK\r\n");
  #endif
}
  
void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  //message += wifiWebServer.uri();
  message += "\nMethod: ";
  //message += (wifiWebServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  //message += wifiWebServer.args();
  message += "\n";
  /*
  for (uint8_t i=0; i<wifiWebServer.args(); i++){
    message += " " + wifiWebServer.argName(i) + ": " + wifiWebServer.arg(i) + "\n";
  }
  wifiWebServer.send(404, "text/plain", message);
  */
}


void handleDataPage()
{
  
  debugln();
  debug("Millis:");debug(millis());debug(" ");debugln("In handleRoot!");    

  String pageString = DATA_PAGE;

  pageString.replace("|menuRow|",MENU_ROW);
  pageString.replace("|panelID|",panelID);
  pageString.replace("|deviceID|","<a style=\"margin-left: 15px;\">" + deviceID + "</a>");

  //{"sensorID","sensorValue","type"}



  #ifdef _ethWebServer    
  ethWebServer.send(200, "text/html", pageString);
  #endif
  debugln("Eth Data Page Loaded!");          

}



void projectConfigPage_html()
{  
  
  String projectTitleParameter = "<h2>" + String(panelID) + " panel</h2>";
  String hostnameParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"hostName\" value=\"" + panelID + "\" required/>";  
  String deviceIDParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"deviceID\" value=\"" + deviceID + "\" required/>";        
  String apiURLParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"apiURL\" value=\"" + apiURL + "\" required/>";        
  String apiPortParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"apiPort\" value=\"" + apiPort + "\" required/>";                
  String apiPathParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"apiPath\" value=\"" + apiPath + "\" required/>";                
  String urlParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"url\" value=\"" + projectUrl + "\" required/>";   
  
  String projectConfigPage = PROJECT_CONFIG_PAGE;
  projectConfigPage.replace("|menuRow|",MENU_ROW);
  projectConfigPage.replace("|panelID|",panelID);
  projectConfigPage.replace("|projectTitleParameter|",projectTitleParameter);
  projectConfigPage.replace("|hostnameParameter|",hostnameParameter);
  projectConfigPage.replace("|deviceID|",deviceIDParameter);
  projectConfigPage.replace("|apiURL|",apiURLParameter);
  projectConfigPage.replace("|apiPort|",apiPortParameter);
  projectConfigPage.replace("|apiPath|",apiPathParameter);
  projectConfigPage.replace("|urlParameter|",urlParameter);  
    

  #ifdef _ethWebServer
  ethWebServer.send(200, "text/html", projectConfigPage);
  #endif
  debugln("Eth Project Config Page Loaded!");        

  

}

void saveProjectConfig_html()
{
  debugln("Project Config Save!");
  Serial.println("Project Config Save");

  #ifdef _ethWebServer
  panelID = ethWebServer.arg("hostName");
  deviceID = ethWebServer.arg("deviceID");
  apiURL = ethWebServer.arg("apiURL");
  apiPort = ethWebServer.arg("apiPort");
  apiPath = ethWebServer.arg("apiPath");
  projectUrl = ethWebServer.arg("projectUrl");
  ethWebServer.send(200, "text/html", redirectToHome_HTML);debugln("(ETH) Project Config Save...");
  #endif
  

  saveStringToEEPROM(panelID,hostMemAddress);    
  saveStringToEEPROM(deviceID,deviceIDMemAddress);
  saveStringToEEPROM(apiURL,apiURLMemAddress);
  saveStringToEEPROM(apiPort,apiPortMemAddress);
  saveStringToEEPROM(apiPath,apiPathMemAddress);
  saveStringToEEPROM(projectUrl,urlMemAddress);
  
  
  debugln("Project Config Saved! Redirecting to HomePage!"); 
}



void ethConfigPage_html()
{
  debugln();
  debug("Millis:");debug(millis());debug(" ");debugln("In Eth Config Page!");    
  
  String projectTitleParameter = "<h2>" + String(panelID) + " panel</h2>";
  String ethIPParameter = "<INPUT style=\"margin-left: 15px;\" type=\"text\" name=\"ethIP\" value=\"" + ethIP.toString() + "\">";
  String ethMacParameter = "<a style=\"margin-left: 15px;\">" + espEthMacAddress + "</a>";
  String submitParameter = "<br><br><INPUT style=\"margin-left: 15px;\" type=\"submit\" name=\"ethSubmit\" value=\"OK\">";
  
  

  
  String ethConfigPage = ETH_CONFIG_PAGE;

  ethConfigPage.replace("|menuRow|",MENU_ROW);
  ethConfigPage.replace("|panelID|",panelID);
  ethConfigPage.replace("|projectTitleParameter|",projectTitleParameter); 
  ethConfigPage.replace("|ethIpAddressParameter|",ethIPParameter);
  ethConfigPage.replace("|ethMacParameter|",ethMacParameter);
  ethConfigPage.replace("|submitParameter|",submitParameter);       

  #ifdef _ethWebServer 
  ethWebServer.send(200, "text/html", ethConfigPage);
  #endif
  debugln("EthConfig Page Loaded of Ethernet Adapter!");        
  
}

void saveEthConfig_html()
{
  debugln("Eth Config Save!");
  Serial.println("Eth Config Save");
  
  #ifdef _ethWebServer
  if(ethWebServer.hasArg("ethSubmit"))
  {
    espEthIP = ethWebServer.arg("ethIP");    
    ethWebServer.send(200, "text/html", ETH_SAVED_HTML);
  }
  #endif
  ethIP = StringToIPAddress(espEthIP);
  saveStringToEEPROM(espEthIP,ethIpMemAddress);    
    

  
  EEPROM.write(initMemAddress,123);
  EEPROM.commit();

  Serial.println("REBOOTING DEVICE...");
  debugln("REBOOTING DEVICE...");  
  EEPROM.end();
  delay(1000);
  ESP.restart();  
  
}


void firmwareConfigPage_html()
{
  debugln();
  debugln("In FirmWareConfigPage!");    

  String projectTitleParameter = "<h2>" + String(panelID) + " panel</h2>";
  String hostnameParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"hostName\" value=\"" + panelID + "\" required/>";  

  String lastFWFileNameParameter = "<a style=\"margin-left: 15px;\">" + firmwareFilename + "</a>";  
  String lastFWDateTimeParameter = "<a style=\"margin-left: 15px;\">" + firmwareLastModified.substring(14) + "</a>";  

  String firmwareURLParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"firmwareURL\" value=\"" + firmwareURL + "\" required/>"; 
  String firmwarePathParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"firmwarePath\" value=\"" + firmwarePath + "\" required/>"; 
  String firmwareFilenameParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"firmwareFilename\" value=\"" + firmwareFilename + "\" required/>"; 

  String fwConfigPage = FIRMWARE_CONFIG_PAGE;
  
  fwConfigPage.replace("|menuRow|",MENU_ROW);
  fwConfigPage.replace("|panelID|",panelID);
  fwConfigPage.replace("|projectTitleParameter|",projectTitleParameter);
  fwConfigPage.replace("|lastFWFileName|",lastFWFileNameParameter);
  fwConfigPage.replace("|lastFWDateTime|",lastFWDateTimeParameter);
  fwConfigPage.replace("|firmwareURL|",firmwareURLParameter);
  fwConfigPage.replace("|firmwarePath|",firmwarePathParameter);
  fwConfigPage.replace("|firmwareFilename|",firmwareFilenameParameter);

  #ifdef _ethWebServer
    ethWebServer.send(200, "text/html", fwConfigPage);debugln("Eth FirmWareConfigPage Loaded");  
  #endif
}

void saveFirmwareConfig_html()
{
  debugln("Firmware Config Save!");
  Serial.println("Firmware Config Save");

    #ifdef _ethWebServer
    firmwareURL = ethWebServer.arg("firmwareURL");
    firmwarePath = ethWebServer.arg("firmwarePath");
    firmwareFilename = ethWebServer.arg("firmwareFilename");
    #endif

  

  saveStringToEEPROM(firmwareURL,firmwareURLMemAddress);    
  saveStringToEEPROM(firmwarePath,firmwarePathMemAddress);    
  saveStringToEEPROM(firmwareFilename,firmwareFilenameMemAddress);
  
  debugln("FirmWare Config Saved! Redirecting to HomePage!"); 
  #ifdef _ethWebServer
    ethWebServer.send(200, "text/html", redirectToHome_HTML);debugln("(Eth) Firmware Config Save...");  
  #endif


}


void setPixel(int r, int g, int b)
{ 
  
  int szorzo = 1;    
  RgbColor color(szorzo * r, szorzo * g, szorzo * b);
  strip.SetPixelColor(0, color);
  strip.Show();  

}




String splitString(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void searchForUpdate()
{  
  esp_task_wdt_reset();
  //Firmware Update Routine
   
  char FWHostIP_char[firmwareURL.length()+1];
  firmwareURL.toCharArray(FWHostIP_char,firmwareURL.length()+1);

  String FirmWareFullPath = String(firmwarePath)+firmwareFilename;
  char FWFilaNem_char[FirmWareFullPath.length()+1];
  FirmWareFullPath.toCharArray(FWFilaNem_char,FirmWareFullPath.length()+1);
  
  debug("Firmware Update Attempt from: ");debug(firmwareURL);debug(firmwarePath);debug(firmwareFilename);debugln();
  Serial.print("Firmware Update Attempt from: ");Serial.print(firmwareURL);Serial.print(firmwarePath);Serial.print(firmwareFilename);Serial.println();
  String fwFilePath = firmwarePath+firmwareFilename;
  
  HttpClient firmwareUpdateClient(ethFWClient);
  esp_task_wdt_reset();
  //if(isEthernetPresent){firmwareUpdateClient = HttpClient(ethFWClient);} //Firmware Update through Ethernet port
    
  int resp = firmwareUpdateClient.get(FWHostIP_char, FWFilaNem_char);
  debug("Firmware Request Response: ");debugln(resp);
  Serial.print("Firmware Request Response: ");Serial.println(resp);
  
   
  // If file is reachable, start downloading
  if(resp == 0)
  {
    esp_task_wdt_reset();
    debugln("Started request OK!");

    resp = firmwareUpdateClient.responseStatusCode();
    esp_task_wdt_reset();
    if(resp == 200)
    {                  
      debugln("File at given URL is available!");

      esp_task_wdt_reset();  
            
      String headerResp = "";
      String headerLine = "";
      String dateTimeOfNewFirmware = "";
      String dateTimeOfCurrentFirmware = EEPROM.readString(firmwareLastModifiedMemAddress);
      while(!firmwareUpdateClient.endOfHeadersReached())
      {
        esp_task_wdt_reset();
        char h = firmwareUpdateClient.readHeader();        
        headerLine += String(h);
        if(h == '\n')
        {          
          if(headerLine.indexOf("Last-Modified")>=0){dateTimeOfNewFirmware = headerLine;}
          headerLine = "";
        }
        headerResp += String(h);
      }          
      debugln("Current Firmware: ");debugln(dateTimeOfCurrentFirmware);
      debugln("Online File: ");debugln(dateTimeOfNewFirmware);


      resp = firmwareUpdateClient.skipResponseHeaders();
      debug("respValue: ");debugln(resp);
      if(resp >= 0 && (dateTimeOfCurrentFirmware != dateTimeOfNewFirmware))
      {
        esp_task_wdt_reset();
        debugln("Getting FirmWare!");
        setPixel(10,10,0);
        fwTotalLength = firmwareUpdateClient.contentLength();
        int len = fwTotalLength;
        debug("FW size is: ");debugln(fwTotalLength);
        
        Update.begin(UPDATE_SIZE_UNKNOWN);  
        int buffSize = 250;      
        uint8_t buff[buffSize] = { 0 };

        int kNetworkTimeout = 60*1000;
        int kNetworkDelay = 1000;
        unsigned long timeoutStart = millis();
        fwCurrentLength = 0;
        byte c;
        setPixel(0,0,0);
        debugln("Updating firmware...");
        while ( (firmwareUpdateClient.connected() || firmwareUpdateClient.available()) && ((millis() - timeoutStart) < kNetworkTimeout) )
        {     
          esp_task_wdt_reset();          
          if(firmwareUpdateClient.available())
          {   
             
            blinkBool = !blinkBool;                     
            if(blinkBool){setPixel(0,0,0);}
            else{setPixel(10,10,0);}
            
            int s = firmwareUpdateClient.readBytes(buff,buffSize);            
            updateFirmware(buff, s, dateTimeOfNewFirmware);

            if(len > 0)
            {
              len -= s;
            }
            
          }          
        }
      }
      else
      {
         
        //debug("Cannot download firmware file. HTTP Response code: ");debugln(resp);
      }
    }    
    else
    {
       
      //debug("Response Code Error! HTTP Response code: ");debugln(resp);
    }
  }  
}


// Function to update firmware incrementally
// Buffer is declared to be 128 so chunks of 128 bytes
// from firmware is written to device until server closes
void updateFirmware(uint8_t *data, size_t len, String NewFWDateTime){

  esp_task_wdt_reset();
  Update.write(data, len);
  fwCurrentLength += len;
  double currentPercent = fwCurrentLength / (fwTotalLength / 100);
  int roundedCurrentPercent = round(currentPercent);

  if(lastPercent != currentPercent && (roundedCurrentPercent %  10) == 0)
  {
    debug("\r                 ");
    debug("\r");debug(currentPercent);debug(" %");
  }
  lastPercent = currentPercent;
  // if current length of written firmware is not equal to total firmware size, repeat
  
  //debug("\r                                                              ");
  //debug("\r"+String(currentLength)+" / "+String(totalLength));
  
  if(fwCurrentLength != fwTotalLength) return;
  Update.end(true);
   
  
  /*
  String latestFWFilaName = readStringFromEEPROM(FWFilenameMemAddress);    
  latestFWFilaName = latestFWFilaName.substring(0,latestFWFilaName.lastIndexOf("_")) + "_" + String(latestFWFilaName.substring(latestFWFilaName.lastIndexOf("_")+1).toInt()+1);  
  saveStringToEEPROM(latestFWFilaName,FWFilenameMemAddress);
  debug("New FirmWare FileName: ");debugln(latestFWFilaName);
  */

  
  debug("New Firmware DateTime: ");debugln(NewFWDateTime);
  Serial.print("New Firmware DateTime: ");Serial.println(NewFWDateTime);
  EEPROM.writeString(firmwareLastModifiedMemAddress,NewFWDateTime);
  EEPROM.commit();

  String latestFWFilaName = readStringFromEEPROM(firmwareFilenameMemAddress);    
  debug("Current FirmWare FileName: ");debugln(latestFWFilaName);

  debug("\r\nUpdate Success, Total Size: "+String(fwCurrentLength)+" Byte\r\nRebooting...\r\n");  
  delay(1000);
  // Restart ESP32 to see changes 
  ESP.restart();

}



void saveStringToEEPROM(String _stringToEEPROM, int memAddress)
{  
  EEPROM.writeString(memAddress,_stringToEEPROM);
  EEPROM.commit();
}


String readStringFromEEPROM(int memAddress)
{
  String retValString = EEPROM.readString(memAddress);
  //Serial.print("Read from: ");Serial.print(memAddress);Serial.print(" -> ");Serial.print(retValString);Serial.println();
  return retValString;
}

String IpAddressToString(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3]);
}


IPAddress StringToIPAddress(String ipString)
{
  String choppedIPString = ipString;
  uint8_t octett[4] = {0,0,0,0};
  for(int i = 3; i>=0;i--)
  {    
    octett[i] = choppedIPString.substring(choppedIPString.lastIndexOf('.')+1).toInt();
    choppedIPString = choppedIPString.substring(0,choppedIPString.lastIndexOf('.'));
    
  }
  /*
  debug("Oktett 1 : ");debugln(octett[0]);
  debug("Oktett 2 : ");debugln(octett[1]);
  debug("Oktett 3 : ");debugln(octett[2]);
  debug("Oktett 4 : ");debugln(octett[3]);
  */

  return IPAddress(octett[0],octett[1],octett[2],octett[3]);  
}



void telnetTask( void * parameter ) {
  esp_task_wdt_reset();
    Serial.println();
    Serial.println();
    Serial.print("Core ID Of telnetTask(): ");Serial.println(xPortGetCoreID());

          
    Serial.println("Starting Ethernet Telnet Server on port 23!");
    ethTelnetServer.begin();
    //TelnetPrint = NetServer(23);

    unsigned long coreActivityTimer = 0;

    Serial.print("tellnetTask uxTaskGetStack: "); Serial.println(uxTaskGetStackHighWaterMark(telnetTask_Handle));
    Serial.println();
    Serial.println();

    while(1)
    {
      esp_task_wdt_reset();
      vTaskDelay(1);
      #ifdef _ethWebServer  
        ethWebServer.handleClient();
        //Serial.print(".");
      #endif

      if(false && millis() - coreActivityTimer > 5000)
      {
        coreActivityTimer = millis();

        Serial.println();
        Serial.print("tellnetTask uxTaskGetStack: "); Serial.println(uxTaskGetStackHighWaterMark(telnetTask_Handle));
        Serial.println();

      }
      

      //NetClient tempEthClient = TelnetPrint.available();
      EthernetClient tempEthClient = ethTelnetServer.available();
      if(tempEthClient)
      {
        //Serial.println("TempEthClien got True");
        if(tempEthClient.remoteIP()!= ethTelnetClient.remoteIP() || tempEthClient.remotePort() != ethTelnetClient.remotePort())
        {
          ethTelnetClient = tempEthClient;
          ethTelnetConnected = true;
          
          ethTelnetClient.flush();        
          ethTelnetClient.println("Welcome dear Client!");

          debugln();
          debug(" New Telnet Client Connected: ");debug(ethTelnetClient.remoteIP());debug(" ");debugln(ethTelnetClient.remotePort());
          
          ethTelnetClient.setTimeout(120000);        
        }
        
      }
      
      
      if(ethTelnetClient.connected())
      {
        if(ethTelnetClient.available() > 0)
        {        
          char ethTelnetChar =  ethTelnetClient.read();
          ethTelnetMsg += String(ethTelnetChar);
          if(ethTelnetChar =='\n')
          {
            debug("GOT ETH TELNET MESSAGE!: ");debugln(ethTelnetMsg);
            if(ethTelnetMsg.indexOf("reboot") >=0 ){debugln("REBOOTING...");delay(1000);ESP.restart();}    
            else if(ethTelnetMsg.indexOf("getInfo") >=0 ){getPanelInfo();}
            else if(ethTelnetMsg.indexOf("fwUpdate") >=0 ){ExternalOTATimer = 0;}
            else if(ethTelnetMsg.indexOf("post_go") >=0 ){shouldPost = true; debugln("Posting to HOST: YES!"); }
            else if(ethTelnetMsg.indexOf("post_no") >=0 ){shouldPost = false;debugln("Posting to HOST: NO !"); }
            ethTelnetMsg = "";

          }
        }
      }
      else if(!ethTelnetClient.connected() && ethTelnetConnected){

        debug(millis());debug(" ");debug("Eth Telnet Client ");debug(ethTelnetClient.remoteIP());debug(" ");debug(ethTelnetClient.remotePort());debugln(" disconnected!");
        //ethTelnetClient = ethTelnetServer.available();
        ethTelnetClient.stop();
        ethTelnetConnected = false;

      }



    }

    Serial.println("Core1 got OUT of LOOP!");
    


}
  