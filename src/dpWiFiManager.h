#include "dpWebserver.h"
#include "dpProjectFunctions.h"

#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <TelnetStream.h>
#include <Update.h>
#include <b64.h>
#include <HttpClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiAP.h>
#include <esp_wifi.h>




#define EEPROMConfigSize 2000

//EEPROM Memory addresses
//Config Validation Memory Address
#define initMemAddress                     10

//Network Specifig Parameters
#define espHostnameMemAddress             100
#define ssidMemAddress                    150
#define passMemAddress                    200
#define staticIpMemAddress                240
#define staticGatewayMemAddress           260
#define staticSubnetMemAddress            280

//Firmware Update Parameters
#define firmwareURLMemAddress             300
#define firmwarePathMemAddress            400
#define firmwareFilenameMemAddress        500
#define firmwareLastModifiedMemAddress    600
#define firmwareUpdateRequestMemAddress   699

//Project Specific Parameters
#define deviceIDMemAddress                700
#define apiURLMemAddress                  730
#define apiPathMemAddress                 800
#define apiPortMemAddress                 850



//ESP WebServer initial values
String init_ssidString = "GyapiNet_Service";
String init_passwordString = "IoT_t3st@GyN";
String init_espHostname = "DP_ESP32_Default";
String init_espWiFiStaticIpAddress = "";
String init_espWiFiGateway = "";
String init_espWiFiSubnet = "";

String init_deviceID = "-1";
String init_apiURL = "127.0.0.0";
String init_apiPort = "8080";
String init_apiPath = "/";

String init_firmwareURL = "www.orlang.hu";
String init_firmwarePath = "/firmwares/";
String init_firmwareFilename = "projectBinfile";
String init_firmwareLastModified = "Last modified: Firmware has not yet been modified!";


String ssidString;
String passwordString;
String espHostname = "";
String espWiFiStaticIpAddress = "";
String espWiFiStaticGateway = "";
String espWiFiStaticSubnet = "";
String espWiFiMacAddress = "";
String deviceID = "";
String apiURL = "";
String apiPort = "";
String apiPath = "";
String firmwareURL = "";
String firmwarePath = "";
String firmwareFilename = "";
String firmwareLastModified = "";


String AP_HOST; 
String AP_SSID;
const char* ap_password = "12345678";
int wifiCount = 0;

IPAddress staticIP;
IPAddress staticGateway;
IPAddress staticSubnet;
IPAddress wifiIP;
IPAddress hotSpotIP(192,168,33,1);
IPAddress lastIP(0,0,0,0);
WebServer wifiWebServer(80);
WiFiClient wifiClient;

//Adafruit_NeoPixel pixels;
// Define the array of leds
#define statusLedPin      2
const uint16_t PixelCount = 4; // this example assumes 4 pixels, making it smaller will cause a failure
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> wsLeds(PixelCount, statusLedPin);


//WebServer Related Functions
void handleMain();
void handleProjectConfigPage();
void savingProjectConfig();
void handleNetworkConfigPage(bool _isAdvanced);
void savingNetworkConfig();
void handleFirmwareConfigPage();
void savingFirmwareConfig();
void onHardReset();
void onSoftReset();
void onScanWifis();
void handleNotFound();
void feedBackWebPage(String feedBackMessage);


String IpAddressToString(const IPAddress& ipAddress);
String splitString(String data, char separator, int index);
void setPixel(uint8_t r, uint8_t g, uint8_t b);

RGBColor hsl_to_rgb(double h, double s, double l);
void calibMag();
void calibMPU();


//Firmware Related Parameters
int totalLength;       //total size of firmware
int currentLength = 0; //current size of written firmware
double lastPercent = 0;
void updateFirmware(uint8_t *data, size_t len, String NewFWDateTime);
void searchForUpdate();


//Status Led Pin
int wsLedPin = -1;

bool isDegreeMatching = false;
int degreeMatchingActionID = 0;
unsigned long degreeMatchingTimer = 0;

bool devDelay = false;
bool blinkBool = false;


int setupNetwork(int _wsLedPin = -1)
{      
  
    
    if(devDelay){delay(1000);}
    //disableCore0WDT();
    //disableCore1WDT();
    int retVal = 0; 
    
    
    if(_wsLedPin != -1)
    {
      
      wsLeds.Begin();
      wsLeds.Show();
      wsLedPin = _wsLedPin;    

           

      Serial.print("Setting up Status Led. wsLedPin: "); Serial.println(wsLedPin);
      setPixel(0,0,0);
    }
        
    Serial.println("Setting up Network");  

    esp_wifi_set_ps(WIFI_PS_NONE);
    

    if(EEPROM.read(initMemAddress) == 123)
    {            
      ssidString = EEPROM.readString(ssidMemAddress);          
      passwordString = EEPROM.readString(passMemAddress);  
      //espHostname = EEPROM.readString(espHostnameMemAddress);  
      espWiFiStaticIpAddress = EEPROM.readString(staticIpMemAddress);
      espWiFiStaticGateway = EEPROM.readString(staticGatewayMemAddress);
      espWiFiStaticSubnet = EEPROM.readString(staticSubnetMemAddress);
    }
    else
    {
      //for(int i = 0; i<EEPROMConfigSize;i++){EEPROM.write(i,0);} //Clearing Config EEPROM!
      //EEPROM.commit();      

      ssidString = init_ssidString;   
      passwordString = init_passwordString;       
      //espHostname = init_espHostname;
      espWiFiStaticIpAddress = init_espWiFiStaticIpAddress;
      espWiFiStaticGateway = init_espWiFiGateway;
      espWiFiStaticSubnet = init_espWiFiSubnet;
    }
    if(devDelay){delay(1000);}

    ssidString      = "GyapiNet_Service";   
    passwordString  = "IoT_t3st@GyN";

    //WiFi.setHostname(espHostname.c_str());

    //IF STATIC IP NEEDED    

    if(espWiFiStaticIpAddress!="")
    {
      Serial.println("Static IP Configuration is desired!");
      staticIP = IPAddress(splitString(espWiFiStaticIpAddress,'.',0).toInt(),splitString(espWiFiStaticIpAddress,'.',1).toInt(),splitString(espWiFiStaticIpAddress,'.',2).toInt(),splitString(espWiFiStaticIpAddress,'.',3).toInt());
      staticGateway =  IPAddress(splitString(espWiFiStaticGateway,'.',0).toInt(),splitString(espWiFiStaticGateway,'.',1).toInt(),splitString(espWiFiStaticGateway,'.',2).toInt(),splitString(espWiFiStaticGateway,'.',3).toInt());
      staticSubnet =   IPAddress(splitString(espWiFiStaticSubnet,'.',0).toInt(),splitString(espWiFiStaticSubnet,'.',1).toInt(),splitString(espWiFiStaticSubnet,'.',2).toInt(),splitString(espWiFiStaticSubnet,'.',3).toInt());
      WiFi.config(staticIP, staticGateway, staticSubnet);
    }

    Serial.println("Wifi connecting to "+ssidString+" with Password: "+passwordString);        
    WiFi.mode(WIFI_STA);    
    Serial.println("Wifi mode set to WiFi_STA");

    if(devDelay){delay(1000);}
    WiFi.begin(ssidString.c_str(), passwordString.c_str());
    Serial.println("Wifi connection began...");

    int connCntr = 0;
    while(WiFi.status() != WL_CONNECTED && connCntr<20) 
    {            
      setPixel(5,0,0);delay(250);
      setPixel(0,0,0);delay(250);
      Serial.print(".");
      connCntr++;
    }
    if(devDelay){delay(1000);}
    Serial.println();

    if(WiFi.status() == WL_CONNECTED)
    {           
      Serial.println("Wifi connected!");      
      retVal = 0;                
      lastIP = WiFi.localIP(); 
      wifiIP = lastIP;
      Serial.println("IP address: ");
      Serial.println(lastIP); 
      setPixel(0,5,0); 
      delay(1000);      
      
    }
    else
    {                       
      Serial.println("Wifi not connected!");          
      retVal = -1;        
      
      WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
      delay(1000);     
      WiFi.mode(WIFI_AP);

      if(EEPROM.read(initMemAddress) == 123)
        {espHostname = EEPROM.readString(espHostnameMemAddress);}
      else
        {espHostname = init_espHostname;}
      

      WiFi.softAP(espHostname.c_str(),"12345678");      
      delay(100);
      
      WiFi.softAPConfig(hotSpotIP, hotSpotIP, IPAddress(255, 255, 255, 0));
            
      wifiIP = WiFi.softAPIP();
      setPixel(5,0,0); 

      

                  

      debugln("ESP WiFi AP SSID: "+String(espHostname));
      debugln("ESP WiFi AP PASS: "+String(ap_password));
      debug("AP IP address: ");debugln(wifiIP);
      
    }

    //Boot operation signal Led
    setPixel(7,5,0);

    TelnetStream.begin();


    Serial.println("Scanning for WiFi networks...");
    wifiCount = WiFi.scanNetworks();         
    
    
    if(false)
    {
      //Waiting for debug availability!!!
      int waitForDebugValue = 5000;             //Wait this much if connected
      if(retVal!=0){waitForDebugValue = 15000;} //Wait this much if not connected
      
      unsigned long waitForDebug = millis();
      unsigned long dotTimer = millis();
      while(millis() - waitForDebug < waitForDebugValue)
      {      
        
        ArduinoOTA.handle(); 
        if(millis() - dotTimer >250)
        {
          blinkBool = !blinkBool;   
          if(blinkBool)
          {
            setPixel(5,5,0);
          }
          else
          {
            setPixel(0,0,0);
          }
          dotTimer = millis();
          debug(".");
        } 
                   
      }
      debugln();    
    }

    
    if(EEPROM.read(initMemAddress) == 123)
    {        
      espHostname = EEPROM.readString(espHostnameMemAddress);  
      deviceID = EEPROM.readString(deviceIDMemAddress);          
      apiURL = EEPROM.readString(apiURLMemAddress);
      apiPort = EEPROM.readString(apiPortMemAddress);
      apiPath = EEPROM.readString(apiPathMemAddress);
      firmwareURL = EEPROM.readString(firmwareURLMemAddress);
      firmwarePath = EEPROM.readString(firmwarePathMemAddress);
      firmwareFilename = EEPROM.readString(firmwareFilenameMemAddress);   
      firmwareLastModified = EEPROM.readString(firmwareLastModifiedMemAddress);   

      debug("SSID:");debugln(ssidString);
      debug("PASSWORD:");debugln(passwordString);
      debug("espHostname:");debugln(espHostname);
      
    }
    else
    {             
      debugln("EEPROM is EMPTY!!!");
      debugln("Loading init values!");
             
      espHostname = init_espHostname;
      deviceID = init_deviceID;
      apiURL = init_apiURL;
      apiPort = init_apiPort;
      apiPath = init_apiPath;
      firmwareURL = init_firmwareURL;
      firmwarePath = init_firmwarePath;            
      firmwareFilename = init_firmwareFilename;
      firmwareLastModified = init_firmwareLastModified;

      EEPROM.writeString(ssidMemAddress,ssidString);
      EEPROM.writeString(passMemAddress,passwordString);       
      EEPROM.writeString(staticIpMemAddress,espWiFiStaticIpAddress);
      EEPROM.writeString(staticGatewayMemAddress,espWiFiStaticGateway);
      EEPROM.writeString(staticSubnetMemAddress,espWiFiStaticSubnet);
      EEPROM.writeString(espHostnameMemAddress,espHostname);

      EEPROM.writeString(deviceIDMemAddress, deviceID);
      EEPROM.writeString(apiURLMemAddress,apiURL);
      EEPROM.writeString(apiPortMemAddress,apiPort);
      EEPROM.writeString(apiPathMemAddress,apiPath);
      EEPROM.writeString(firmwareURLMemAddress,firmwareURL);
      EEPROM.writeString(firmwarePathMemAddress,firmwarePath);
      EEPROM.writeString(firmwareFilenameMemAddress,firmwareFilename);
      EEPROM.writeString(firmwareLastModifiedMemAddress,firmwareLastModified);

      EEPROM.write(initMemAddress,123);
      EEPROM.commit();
    }



    char projectWorkTitle_char[espHostname.length()+1];
    espHostname.toCharArray(projectWorkTitle_char,espHostname.length()+1);

    searchForUpdate();

    wifiWebServer.on("/", [](){ handleMain();});
    wifiWebServer.on("/projectConfig", [](){handleProjectConfigPage();});
    wifiWebServer.on("/saveProjectConfig", [](){savingProjectConfig();});
    wifiWebServer.on("/networkConfig", [](){handleNetworkConfigPage(false);});
    wifiWebServer.on("/advancedNetworkConfig", [](){handleNetworkConfigPage(true);});
    wifiWebServer.on("/saveNetworkConfig", [](){savingNetworkConfig();});
    wifiWebServer.on("/firmwareConfig", [](){handleFirmwareConfigPage();});
    wifiWebServer.on("/saveFirmwareConfig", [](){savingFirmwareConfig();});              
    wifiWebServer.on("/hardReset", onHardReset);
    wifiWebServer.on("/softReset", onSoftReset);   
    wifiWebServer.on("/scanWifi", onScanWifis);

    wifiWebServer.onNotFound(handleNotFound); 
    wifiWebServer.begin();    

    espWiFiMacAddress = String(WiFi.macAddress());
    debug("WiFi Mac Address: ");debugln(espWiFiMacAddress);
    debug("ESP Hostname: ");debugln(espHostname);
    Serial.print("ESP Hostname: ");Serial.println(espHostname);
    
    resetReason = esp_reset_reason();

    debugln("###################");
    debug("Cause of Reset: ");    
    switch(resetReason)
    {
      case ESP_RST_BROWNOUT: debug("BROWNOUT");break;
      case ESP_RST_EXT: debug("EXT");break;
      case ESP_RST_INT_WDT: debug("INT_WDT");break;
      case ESP_RST_PANIC: debug("PANIC");break;
      case ESP_RST_POWERON: debug("POWER ON");break;
      case ESP_RST_SDIO: debug("SD IO");break;
      case ESP_RST_SW: debug("SW");break;
      case ESP_RST_TASK_WDT: debug("TASK WDT");break;
      case ESP_RST_WDT: debug("WDT");break;
      case ESP_RST_UNKNOWN: debug("UNKOWN");break;
      default:break;
    }    
    debugln();
    debugln("###################");
    debugln();
    
    Serial.print("Cause of Reset: ");Serial.println(resetReason);
    
    if(retVal == 0)
    {
      setPixel(0,0,5);
    }
    else
    {
      setPixel(5,0,0);
    }
    
    return retVal;   
}

void onSoftReset()
{
  String pageString = FEEDBACK_PAGE;

  pageString.replace("|refreshTimeParameter|","30");
  pageString.replace("|refreshPathParameter|","./");
  pageString.replace("|feedBackMessageParameter|","Ujrainditas...");
  wifiWebServer.send(200, "text/html", pageString);
  delay(1000);
  ESP.restart();
}

void onHardReset()
{  

  for(int i = 0; i<EEPROMConfigSize;i++){EEPROM.write(i,0);} //Clearing EEPROM!
  EEPROM.commit();
  EEPROM.end();

  
  String pageString = FEEDBACK_PAGE;

  pageString.replace("|refreshTimeParameter|","30");
  pageString.replace("|refreshPathParameter|","./");
  pageString.replace("|feedBackMessageParameter|","Beallitasok torolve! Ujrainditas...");
  
  wifiWebServer.send(200, "text/html", pageString);
  delay(1000);
  ESP.restart();
}


void onScanWifis()
{
  String pageString = FEEDBACK_PAGE;

  pageString.replace("|refreshTimeParameter|","6");
  pageString.replace("|refreshPathParameter|","./");
  pageString.replace("|feedBackMessageParameter|","WiFi halozatok keresese...");
  wifiWebServer.send(200, "text/html", pageString);
  wifiCount = WiFi.scanNetworks();

}


  
void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += wifiWebServer.uri();
  message += "\nMethod: ";
  message += (wifiWebServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += wifiWebServer.args();
  message += "\n";
  for (uint8_t i=0; i<wifiWebServer.args(); i++){
    message += " " + wifiWebServer.argName(i) + ": " + wifiWebServer.arg(i) + "\n";
  }
  wifiWebServer.send(404, "text/plain", message);
}


void handleMain()
{
  String pageString = MAIN_PAGE; 
  String menuString = MENU_ROW;
  pageString.replace("|menuRow|",menuString);

  
  if(wifiWebServer.arg("btnSetTestMode")!=""){
    //
  } 
  
  if(wifiWebServer.arg("btnSetDigPotValue")!=""){
    //
  }

  
  String projectTitleParameter = "<h2>" + String(espHostname) + "</h2>"; 
  String defaultPageTitleParameter = "<h3>Default Page Title</h3>\r\n";
  String defaultMainPageParameter = "<a style=\"margin-left: 15px;\">" + String("defaultParameter") + "</a>";
    
    
  pageString.replace("|panelID|",espHostname);    
  pageString.replace("|projectTitle|",projectTitleParameter);       
  pageString.replace("|defaultMainPageParameter|",defaultPageTitleParameter+defaultMainPageParameter); 

  
  pageString.replace("|testLabelValue|","TestLabel");
  
  pageString.replace("|testVariableValueParameter|","TestVarPar");

  
    


  wifiWebServer.send(200, "text/html", pageString);    
  
}


void handleProjectConfigPage()
{
  String projectTitleParameter = "<h2>" + String(espHostname) + " panel</h2>";
  String hostnameParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"hostName\" value=\"" + espHostname + "\" required/>"; 
  String deviceIDParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"deviceID\" value=\"" + deviceID + "\" required/>";                
  String apiURLParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"apiURL\" value=\"" + apiURL + "\" required/>";          
  String apiPathParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"apiPath\" value=\"" + apiPath + "\" required/>";     
  String apiPortParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"apiPort\" value=\"" + apiPort + "\" required/>";                           
  
  String pageString = PROJECT_CONFIG_PAGE;
  String menuString = MENU_ROW;
  pageString.replace("|menuRow|",menuString);

  pageString.replace("|panelID|",espHostname);
  pageString.replace("|projectTitleParameter|",projectTitleParameter);
  pageString.replace("|deviceID|",deviceIDParameter); 
  pageString.replace("|apiURL|",apiURLParameter);  
  pageString.replace("|apiPath|",apiPathParameter);
  pageString.replace("|apiPort|",apiPortParameter);
    

  wifiWebServer.send(200, "text/html", pageString);
  debugln("Project Config Page Loaded!");  
}

void savingProjectConfig()
{
  debugln("In Saving Project Config!"); 


  debug("Web Arg DeviceID: ");debugln(wifiWebServer.arg("deviceID"));

  deviceID = wifiWebServer.arg("deviceID");
  apiURL = wifiWebServer.arg("apiURL");
  apiPath = wifiWebServer.arg("apiPath");
  apiPort = wifiWebServer.arg("apiPort");
  

  debug("DeviceID: ");debugln(deviceID);

  EEPROM.writeString(deviceIDMemAddress,deviceID); 
  EEPROM.writeString(apiURLMemAddress,apiURL); 
  EEPROM.writeString(apiPathMemAddress,apiPath); 
  EEPROM.writeString(apiPortMemAddress,apiPort); 
  EEPROM.commit(); 

  
  String pageString = FEEDBACK_PAGE;

  pageString.replace("|refreshTimeParameter|","3");
  pageString.replace("|refreshPathParameter|","./");
  pageString.replace("|feedBackMessageParameter|","Projekt beallitasok mentve!");

  wifiWebServer.send(200, "text/html", pageString);

}

void handleNetworkConfigPage(bool _isAdvanced)
{       
  String projectTitleParameter = "<h2>" + String(espHostname) + " panel</h2>";
  String hostnameParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"hostName\" value=\"" + espHostname + "\" required/>";  
  String macParameter = "<a style=\"margin-left: 15px;\">" + espWiFiMacAddress + "</a>";
  String staticIpAddressParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"static_ip\" value=\"" + IpAddressToString(staticIP) + "\" required/>";  
  String staticGatewayAddressParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"static_gateway\" value=\"" + IpAddressToString(staticGateway) + "\" required/>";  
  String staticSubnetMaskParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"static_subnet\" value=\"" + IpAddressToString(staticSubnet) + "\" required/>";  
  String wifiRSSIParameter = "<a style=\"margin-left: 15px;\">" + String(WiFi.RSSI()) + "</a>";
  String currentSSIDParameter = "<a style=\"margin-left: 15px;\">" + String(WiFi.SSID()) + "</a>";
  String connectedWifiNetworkInfoParameter = "<br><h3>Az alabbi halozathoz kapcsolodik jelenleg az eszkoz:</h3>"+
                                              currentSSIDParameter +
                                              "<br><h3>WiFi jel erossege</h3>" +
                                              wifiRSSIParameter;
  String btnUpdateParameter = "<INPUT type=\"submit\" name=\"btnUpdate\" value=\"FRISSITES\">";

  
  
  //String passwordParameter = "<INPUT style=\"margin-left: 15px;\" type=\"password\" name=\"password\" value=\"" + passwordString + "\">";
  String passwordParameter = "<INPUT style=\"margin-left: 15px;\" type=\"password\" name=\"password\" value=\"\">";
  String manualSSIDParameter = "<INPUT style=\"margin-left: 15px;\" type=\"text\" name=\"manualSSID\" value=\"\">";
  String availableSSIDs = "";
  for(int i =0; i< wifiCount; i++)
  {
    if(String(WiFi.SSID(i))==ssidString)
    {availableSSIDs += "  <option value='"+String(WiFi.SSID(i))+"' selected='selected'>"+String(WiFi.SSID(i))+"</option>";}
    else
    {availableSSIDs += "  <option value='"+String(WiFi.SSID(i))+"'>"+String(WiFi.SSID(i))+"</option>";}
  } 
  
  String pageString = NETWORK_CONFIG_PAGE;
  String menuString = MENU_ROW;
  pageString.replace("|menuRow|",menuString);

  if(_isAdvanced){pageString = ADVANCED_NETWORK_CONFIG_PAGE;}

  pageString.replace("|panelID|",espHostname);
  pageString.replace("|hostname|",hostnameParameter);
  pageString.replace("|projectTitleParameter|",projectTitleParameter);
  pageString.replace("|hostnameParameter|",hostnameParameter); 
  pageString.replace("|macParameter|",macParameter);
  pageString.replace("|staticIpAddressParameter|",staticIpAddressParameter);
  pageString.replace("|staticGatewayAddressParameter|",staticGatewayAddressParameter);
  pageString.replace("|staticSubnetMask|",staticSubnetMaskParameter);
  pageString.replace("|currentSSIDParameter|",currentSSIDParameter);
  pageString.replace("|wifiRSSIDParameter|",wifiRSSIParameter);

  if(WiFi.getMode() == WIFI_MODE_STA)
  {
    pageString.replace("|connectedWifiNetworkInfoParameter|",connectedWifiNetworkInfoParameter);
    pageString.replace("|btnUpdateParameter|",btnUpdateParameter);
  }
  else
  {
    pageString.replace("|connectedWifiNetworkInfoParameter|","");
    pageString.replace("|btnUpdateParameter|","");
  }
  


  pageString.replace("|manualSSIDParameter|",manualSSIDParameter);
  pageString.replace("|passwordParameter|",passwordParameter);
  pageString.replace("|wifis|",availableSSIDs);

  wifiWebServer.send(200, "text/html", pageString);
  debugln("Network Config Page Loaded!");
}

void savingNetworkConfig()
{
  debugln("In saving Network Config!");     
  
  if(wifiWebServer.arg("btnSave")!="")
  {

    if(String(wifiWebServer.arg("manualSSID"))=="")
      {
        Serial.println("Selected wifi: "+String(wifiWebServer.arg("wifis")));
        debugln("Selected wifi: |"+String(wifiWebServer.arg("wifis"))+"|");
      }
    else
      {
        Serial.println("Selected wifi: "+String(wifiWebServer.arg("manualSSID")));
        debugln("Selected wifi: |"+String(wifiWebServer.arg("manualSSID"))+"|");
      }

    if(wifiWebServer.hasArg("wifis"))
    {

      if(String(wifiWebServer.arg("manualSSID"))=="")
        {EEPROM.writeString(ssidMemAddress,wifiWebServer.arg("wifis"));}
      else
        {EEPROM.writeString(ssidMemAddress,wifiWebServer.arg("manualSSID"));}
      EEPROM.writeString(passMemAddress,wifiWebServer.arg("password")); 

      EEPROM.writeString(espHostnameMemAddress,wifiWebServer.arg("hostName"));        
    }

    if(wifiWebServer.arg("static_ip")!="" && wifiWebServer.arg("static_ip")!="0.0.0.0")
    {
      Serial.println("Static IP Configuration is given...");
      EEPROM.writeString(staticIpMemAddress,wifiWebServer.arg("static_ip"));
      EEPROM.writeString(staticGatewayMemAddress,wifiWebServer.arg("static_gateway"));
      EEPROM.writeString(staticSubnetMemAddress,wifiWebServer.arg("static_subnet"));
    }
    else
    {
      Serial.println("Static IP is not desired. DHCP will be active!");
      EEPROM.writeString(staticIpMemAddress,"");
      EEPROM.writeString(staticGatewayMemAddress,"");
      EEPROM.writeString(staticSubnetMemAddress,"");

    }
    

    
    String pageString = FEEDBACK_PAGE;

    pageString.replace("|refreshTimeParameter|","30");
    pageString.replace("|refreshPathParameter|","./");
    pageString.replace("|feedBackMessageParameter|","Halozati bellitasok mentve. Ujrainditas...");

    wifiWebServer.send(200, "text/html", pageString);
    
      
    EEPROM.write(initMemAddress,123);
    EEPROM.commit();
    
    Serial.println("REBOOTING DEVICE...");
    debugln("REBOOTING DEVICE...");  
    EEPROM.end();
    delay(1000);
    ESP.restart();

  }

  if(wifiWebServer.arg("btnUpdate")!="")
  {
    String pageString = FEEDBACK_PAGE;

    pageString.replace("|refreshTimeParameter|","0");
    pageString.replace("|refreshPathParameter|","./firmwareConfig");
    pageString.replace("|feedBackMessageParameter|","");

    wifiWebServer.send(200, "text/html", pageString);
    

  }

    
}


void handleFirmwareConfigPage()
{
  debugln();
  debugln("In FirmWareConfigPage!");    

  String projectTitleParameter = "<h2>" + String(espHostname) + " panel</h2>";
  String hostnameParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"hostName\" value=\"" + espHostname + "\" required/>";  
  String lastFWFileNameParameter = "<a style=\"margin-left: 15px;\">" + firmwareFilename + "</a>";  
  String lastFWDateTimeParameter = "<a style=\"margin-left: 15px;\">" + firmwareLastModified.substring(14) + "</a>";  
  String firmwareURLParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"firmwareURL\" value=\"" + firmwareURL + "\" required/>"; 
  String firmwarePathParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"firmwarePath\" value=\"" + firmwarePath + "\" required/>"; 
  String firmwareFilenameParameter = "<input style=\"margin-left: 15px;\" type=\"text\" class=\"form-control\" name=\"firmwareFilename\" value=\"" + firmwareFilename + "\" required/>"; 

  String pageString = FIRMWARE_CONFIG_PAGE;
  String menuString = MENU_ROW;
  pageString.replace("|menuRow|",menuString);

  pageString.replace("|panelID|",espHostname);
  pageString.replace("|projectTitleParameter|",projectTitleParameter);
  pageString.replace("|lastFWFileName|",lastFWFileNameParameter);
  pageString.replace("|lastFWDateTime|",lastFWDateTimeParameter);
  pageString.replace("|firmwareURL|",firmwareURLParameter);
  pageString.replace("|firmwarePath|",firmwarePathParameter);
  pageString.replace("|firmwareFilename|",firmwareFilenameParameter);

  wifiWebServer.send(200, "text/html", pageString);
  debugln("Firmware Config Page Loaded!");
}

void savingFirmwareConfig()
{

  
  debugln("Firmware Config Save!");
  Serial.println("Firmware Config Save");

  if(wifiWebServer.arg("btnSave")!="")
  {

    debugln("In btnSave...");

    firmwareURL = wifiWebServer.arg("firmwareURL");
    firmwarePath = wifiWebServer.arg("firmwarePath");
    firmwareFilename = wifiWebServer.arg("firmwareFilename");

    EEPROM.writeString(firmwareURLMemAddress,firmwareURL); 
    EEPROM.writeString(firmwarePathMemAddress,firmwarePath); 
    EEPROM.writeString(firmwareFilenameMemAddress,firmwareFilename); 
    EEPROM.commit();

    
    String pageString = FEEDBACK_PAGE;

    pageString.replace("|refreshTimeParameter|","3");
    pageString.replace("|refreshPathParameter|","./networkConfig");
    pageString.replace("|feedBackMessageParameter|","Firmware beallitasok mentve!");

    wifiWebServer.send(200, "text/html", pageString);
  }
  
  if(wifiWebServer.arg("btnUpdate")!="")
  {
    debugln("In btnUpdate...");
    setPixel(20,20,20);            
    
    String pageString = FEEDBACK_PAGE;
    String feedBackMessage = "";
    feedBackMessage = "Az alabbi cimen keressuk az ujabb Firmware-t: ";
    feedBackMessage += firmwareURL;
    feedBackMessage += firmwarePath;
    feedBackMessage += firmwareFilename;

    pageString.replace("|refreshTimeParameter|","30");
    pageString.replace("|refreshPathParameter|","./firmwareConfig");
    pageString.replace("|feedBackMessageParameter|",feedBackMessage);

    wifiWebServer.send(200, "text/html", pageString);



    if(WiFi.getMode()==WIFI_MODE_APSTA)
    {
        ssidString = wifiWebServer.arg("wifis");
        passwordString = wifiWebServer.arg("password");

        debugln("Wifi connecting to "+ssidString+" with Password: "+passwordString);
        WiFi.mode(WIFI_STA);    
        if(devDelay){delay(1000);}
        WiFi.begin(ssidString.c_str(), passwordString.c_str());

        int connCntr = 0;
        while(WiFi.status() != WL_CONNECTED && connCntr<20) 
        {            
          setPixel(5,0,0);delay(250);
          setPixel(0,0,0);delay(250);
          debug(".");
          connCntr++;
        }
        delay(1000);

    }

    if(WiFi.getMode()==WIFI_MODE_STA)
    {
      searchForUpdate();
      setPixel(0,0,0);delay(1000);
      setPixel(10,0,0);delay(1000);
      setPixel(0,0,0);delay(1000);
      setPixel(0,0,10);delay(1000);
      ESP.restart();
    }
    else
    {
      setPixel(0,0,0);delay(1000);
      setPixel(10,0,0);delay(1000);
      ESP.restart();
    }
  

  }
  
  
    
}


void setPixel(uint8_t r, uint8_t g, uint8_t b)
{    
    
    if(wsLedPin != -1)
    { 
      if(wsLedPin == 99) //Normal led output on pin2 @nodemcu
      {
        pinMode(2,OUTPUT);
        if(r == 0 && g == 0 && b == 0)
        {          
          digitalWrite(2,LOW);
        }
        else
        {          
          digitalWrite(2,HIGH);
        }
      }
      else
      {
        
        int szorzo = 5;  
        RgbColor _setColor(g*szorzo,r*szorzo,b*szorzo);      
        wsLeds.SetPixelColor(0, _setColor);
        wsLeds.Show();

      }

    }
  }



void searchForUpdate()
{  
  
  //Firmware Update Routine
  char FWHostURL_char[firmwareURL.length()+1];
  firmwareURL.toCharArray(FWHostURL_char,firmwareURL.length()+1);

  String FirmWareFullPath = firmwarePath + firmwareFilename;
  char FWFullPath_char[FirmWareFullPath.length()+1];
  FirmWareFullPath.toCharArray(FWFullPath_char,FirmWareFullPath.length()+1);

  debug("Search For Firmware update at: ");debug(firmwareURL);debugln(FirmWareFullPath);
  debugln();
  Serial.print("Search For Firmware update at: ");Serial.print(firmwareURL);Serial.println(FirmWareFullPath);
  Serial.println();

  HttpClient firmwareUpdateClient(wifiClient);  
  int resp = firmwareUpdateClient.get(FWHostURL_char,FWFullPath_char);
  
  if(resp == 0)
  {
    resp = firmwareUpdateClient.responseStatusCode();    
  
    if(resp == 200)
    {      
      debugln("File at given URL is available!");
            
        String headerResp = "";
        String headerLine = "";
        String dateTimeOfNewFirmware = "";
        String dateTimeOfCurrentFirmware = EEPROM.readString(firmwareLastModifiedMemAddress);
        while(!firmwareUpdateClient.endOfHeadersReached())
        {
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

      //EEPROM.writeString(firmwareLastModifiedMemAddress,firmwareLastModified);
      
      resp = firmwareUpdateClient.skipResponseHeaders();
      //debug("SkipResponseHeaders: "); debugln(resp);
      if(resp >= 0 && (dateTimeOfCurrentFirmware != dateTimeOfNewFirmware))
      {
        debugln("Downloading FirmWare!");
        setPixel(10,10,0);
        totalLength = firmwareUpdateClient.contentLength();
        int remainingLength = totalLength;
        debug("FW size is: ");debugln(totalLength);
        
        Update.begin(UPDATE_SIZE_UNKNOWN);  
        int buffSize = 500;      
        uint8_t buff[buffSize];

        int kNetworkTimeout = 60*1000;
        int kNetworkDelay = 1000;
        unsigned long timeoutStart = millis();
        setPixel(0,0,0);
        debugln("Updating firmware...");
        Serial.println("Updating firmware...");
        while ( (firmwareUpdateClient.connected() || firmwareUpdateClient.available()) && ((millis() - timeoutStart) < kNetworkTimeout) )
        {                    
          if(firmwareUpdateClient.available())
          {   

            int s = firmwareUpdateClient.readBytes(buff,buffSize);            
            updateFirmware(buff, s, dateTimeOfNewFirmware);

            if(remainingLength > 0)
            {
              remainingLength -= s;
              //Serial.print("Remaining content: ");Serial.println(len);
            }
            
          }          
        }

        Serial.println("\r\nUpdate got Interrupted!");
        debugln("\r\nUpdate got Interrupted!");
        setPixel(20,0,0);        
        delay(5000);  
        ESP.restart();

      }
      else
      {
        if(resp<0)
        {
          debug("Cannot download firmware file. HTTP Response code: ");debugln(resp);
        }

        if(dateTimeOfCurrentFirmware == dateTimeOfNewFirmware)
        {
          debugln("Firmware is Up To Date!");
          debugln(dateTimeOfCurrentFirmware);
        }
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
  
  Update.write(data, len);
  currentLength += len;
  double currentPercent = currentLength / (totalLength / 100);
  int roundedCurrentPercent = round(currentPercent);

  blinkBool = !blinkBool;            

  if(lastPercent != currentPercent && (roundedCurrentPercent %  10) == 0)
  {
    debug("\r                 ");
    debug("\r");debug(currentPercent);debug(" %");
    Serial.print(currentPercent);Serial.println(" %");
    RGBColor hslConvertedToRGB = hsl_to_rgb(currentPercent*1.2,1.0,0.5);
    
    setPixel(hslConvertedToRGB.r / 25.0, hslConvertedToRGB.g / 25.0, hslConvertedToRGB.b / 25.0);
    
    
    
  }
  lastPercent = currentPercent;
  // if current length of written firmware is not equal to total firmware size, repeat
  
  //debug("\r                                                              ");
  //debug("\r"+String(currentLength)+" / "+String(totalLength));
  
  if(currentLength != totalLength) return;

  delay(500);
  debugln("\r\nUpdate Success, Total Size: "+String(currentLength)+" Byte");
  Serial.println("\r\nUpdate Success, Total Size: "+String(currentLength)+" Byte");
  delay(500);
  Update.end(true);
  
  
  /*
  String latestFWFilaName = EEPROM.readString(firmwareFilenameMemAddress);    
  latestFWFilaName = latestFWFilaName.substring(0,latestFWFilaName.lastIndexOf("_")) + "_" + String(latestFWFilaName.substring(latestFWFilaName.lastIndexOf("_")+1).toInt()+1);  
  EEPROM.writeString(firmwareFilenameMemAddress,latestFWFilaName);
  debug("New FirmWare FileName: ");debugln(latestFWFilaName);
  */
 
  debug("New Firmware DateTime: ");debugln(NewFWDateTime);
  Serial.print("New Firmware DateTime: ");Serial.println(NewFWDateTime);
  EEPROM.writeString(firmwareLastModifiedMemAddress,NewFWDateTime);
  EEPROM.commit();
  
  
  debugln("Rebooting...\r\n");
  Serial.println("Rebooting...\r\n");
        setPixel(0,15,0);delay(500);setPixel(0,0,0);delay(500);
        setPixel(0,15,0);delay(500);setPixel(0,0,0);delay(500);
        setPixel(0,15,0);delay(1500);

  ESP.restart();
}

String IpAddressToString(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3]);
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

RGBColor hsl_to_rgb(double h, double s, double l) {
    double c, x, m;
    RGBColor rgb;
    h /= 360.0;
    s = std::max(0.0, std::min(1.0, s));
    l = std::max(0.0, std::min(1.0, l));
    c = (1 - std::abs(2 * l - 1)) * s;
    x = c * (1 - std::abs(std::fmod(h * 6, 2) - 1));
    m = l - c / 2;
    if (h < 1.0 / 6.0) {
        rgb.r = c;
        rgb.g = x;
        rgb.b = 0;
    } else if (h < 1.0 / 3.0) {
        rgb.r = x;
        rgb.g = c;
        rgb.b = 0;
    } else if (h < 1.0 / 2.0) {
        rgb.r = 0;
        rgb.g = c;
        rgb.b = x;
    } else if (h < 2.0 / 3.0) {
        rgb.r = 0;
        rgb.g = x;
        rgb.b = c;
    } else if (h < 5.0 / 6.0) {
        rgb.r = x;
        rgb.g = 0;
        rgb.b = c;
    } else {
        rgb.r = c;
        rgb.g = 0;
        rgb.b = x;
    }
    rgb.r = (rgb.r + m) * 255.0;
    rgb.g = (rgb.g + m) * 255.0;
    rgb.b = (rgb.b + m) * 255.0;
    return rgb;
}

void feedBackWebPage(String feedBackMessage)
{
  String pageString = FEEDBACK_PAGE;
  pageString.replace("|refreshTimeParameter|","5");
  pageString.replace("|refreshPathParameter|","./");
  pageString.replace("|feedBackMessageParameter|",feedBackMessage);
  
  wifiWebServer.send(200, "text/html", pageString);  
}


unsigned long updateCheckTimer = 0;
