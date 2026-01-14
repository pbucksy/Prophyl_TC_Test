#include "dpPanelManager.h"


#define isInit        false
#define isOutputs     1


bool isEthernet = true;

bool checkHost();
bool post_data(String sensorID, String sensorValue);

void setup() {
  
  Serial.begin(115200);
  EEPROM.begin(4000);  
  debugln();debugln();
  debugln("In da beginning!!!!!!!!!!!!!!!!!!!!");

 
  delay(1000);



  
  myStatusLed.red = 0;
  myStatusLed.green = 0;
  myStatusLed.blue = 0;




  initPanel(); 
  
  if(isEthernetPresent)
  {
    
    debugln(millis());
    EthernetClient testClient;
    Serial.print("Checking reachability___: ");
    Serial.println(firmwareURL);

    unsigned long reachabilityTimoutTimer = millis();
    

    while(!testClient.connect(firmwareURL.c_str(), 80) && millis() - reachabilityTimoutTimer < 30000) {

      delay(1000);
      Serial.print(firmwareURL); Serial.println(" is OFF");
          
    }

    if(millis() - reachabilityTimoutTimer > 30000)
    {
      Serial.println("Reachability Timeout Occured. Restart...");
      delay(1000);
      ESP.restart();
    }

    testClient.stop();
    Serial.print(firmwareURL); Serial.println(" is reachable!");    
    
   
    searchForUpdate();


  }
    
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  debugln("Begin Loop!");
  Serial.println("Begin loop!");


  //Testing mods

  /*
  



*/


}







void loop() 
{   
   esp_task_wdt_reset();
  
  if(isEthernetPresent)
  {  
    if(millis() - ExternalOTATimer > 20000)
    {
      debugln("Search for Ethernet Update!");
      ExternalOTATimer = millis();
      searchForUpdate();
    }

  }
    
  
 

 
}


//#define needDebug  
String sendRequest(String post_function, String sensorID, String sensorValue,String settingType = "-", String ioStates = "-", String message = "-")
{      

    unsigned long ethernetClientConnectTimer = millis();
    unsigned long ethernetClientConnectTimoutTimer = millis();
    int tryCounter = 1;
    IPAddress restApiIP = StringToIPAddress(apiURL);
    while(!ethPostClient.connected() && millis() - ethernetClientConnectTimer < 4000)
    {
      //debug("Connecting to :");debug(restApiIP);
      ethernetClientConnectTimoutTimer = millis();
      ethPostClient.connect(restApiIP,apiPort.toInt());
      
/*
      

      char projectURL_char[projectUrl.length()+1];
      projectUrl.toCharArray(projectURL_char,projectUrl.length()+1); 
      ethClient.connect(projectURL_char,80);
      */
      delay(900);
      if(!ethPostClient.connected())
      {
        debug(post_function);debug(" "); debug(tryCounter);debug(": Http connect status: ");debugln(ethPostClient.connected());tryCounter++;
      }
    }

    if(!ethPostClient.connected()){return "NOPE";}
    else
    {
      //debug(post_function);debug(": "); debug(sensorID); debug("; ");debug(tryCounter);debug(": Http connect status: ");debugln(ethernetClient.connected());
    }
  
 
    String chkInData = "";
    chkInData += "post_function=";    chkInData += post_function;    
    chkInData += "&ip_address=";      chkInData += IpAddressToString(ethIP);
    if(sensorID !="-"){ chkInData += "&sensor_id=";       chkInData += sensorID;}
    if(sensorValue !="-"){chkInData += "&sensor_value=";    chkInData += sensorValue;}
    if(settingType !="-"){chkInData += "&setting_type=";    chkInData += settingType;}
    if(ioStates !="-"){chkInData += "&io_states=";       chkInData += ioStates;}
    if(message !="-"){chkInData += "&message=";         chkInData += message;}
    
    
    if(true)
    {
      //ethClient.println("POST /rest/api.php HTTP/1.1");
      ethPostClient.println("POST " + apiPath +" HTTP/1.1");
      ethPostClient.println("Host: " + apiURL);
      ethPostClient.println("Content-Type: application/x-www-form-urlencoded");
      ethPostClient.println("Content-Length: "+String(chkInData.length()));
      ethPostClient.println();
      ethPostClient.println(chkInData);
    }
    if(false)
    {
      
      //debugln("POST /rest/api.php HTTP/1.1");
      debugln("POST " + apiPath +" HTTP/1.1");
      debugln("Host: " + apiURL);
      debugln("Content-Type: application/x-www-form-urlencoded");
      debugln("Content-Length: "+String(chkInData.length()));
      debugln();
      debugln(chkInData);
      debugln();
            
    }
    
    
    //debug("\r\nPosting data: ");debugln(chkInData);
        
    int timeOutCntr = 0;
    unsigned long ethernetClientTimeoutTimer = millis();  
    
    int headersLineCntr = 0;     
          bool ethernetClientTimeout = false;
          #ifdef needDebug
            debug("Http connect status: ");debugln(ethernetClient.connected());
          #endif
          if(!ethPostClient.connected()){return "NOPE";}
          #ifdef needDebug
            debugln("Receiving headers...");
          #endif
          while (!ethernetClientTimeout) {
            if((millis() - ethernetClientTimeoutTimer > 3000 && timeOutCntr<3))
            {    
              #ifdef needDebug
                debugln("Timeout! Retry!");
              #endif
              delay(1000);
              
              timeOutCntr++;ethernetClientTimeoutTimer=millis();
            }
            else if(millis() - ethernetClientTimeoutTimer > 3000)
            {
              #ifdef needDebug
                debugln("\r\n\r\nPOST REQUEST TIMEOUT!!!\r\n\r\n");
              #endif
              delay(1000);
              
              ethernetClientTimeout = true;ethPostClient.stop();              
            }
            String line = ethPostClient.readStringUntil('\n');            
            #ifdef needDebug
              if(headersLineCntr == 0){debugln(line); headersLineCntr++;}
            #endif
            if (line == "\r") {
              
              #ifdef needDebug
                debugln("Headers received!");
              #endif
              break;
            }
          }
    String postResponse = "";
    while(ethPostClient.available()) 
          {
            char c = ethPostClient.read();
            //debug(c);
            postResponse += c;            
          }
    #ifdef needDebug
      debugln("retVal: ");debugln(postResponse);debugln("\r\nPOSTING ENDED!\r\n####\r\n\r\n");
    #endif

    ethPostClient.stop();     
    return postResponse;
}

bool checkHost()
{
  
  String postResponse = sendRequest("CHECK_HOST","","");
  if(postResponse.indexOf("CHECK_HOST_OK")>=0)
  {
    debug(postResponse);
    debugln(" : HOST IS AVAILABLE");
    checkHostCntr = 0;
    return true;
  }

  
  checkHostCntr++;
  debug("HOST IS UNAVAILABLE - ");debug(checkHostCntr);debugln();
  return false;

}

bool post_data(String sensorID, String sensorValue)
{
   
  //debug("Posting SensorID : ");debug(sensorID);debug(" | ");debug("SensorValue : ");debug(sensorValue);debugln();
  String postResponse = sendRequest("POSTING", sensorID, sensorValue);
  if(postResponse.indexOf("NOPE")>=0 || postResponse.indexOf("Bad")>=0 || postResponse.indexOf("error")>=0)
  {    
    debug("Error! ");debugln(postResponse);debugln();
    return false;
  }    
  
  debugln(postResponse);
  return true;
  
}

void getPanelInfo()
{
  

        debugln("Panel Info");
        debug("STATUD LED: (");debug(myStatusLed.red);debug(", ");debug(myStatusLed.green);debug(", ");debug(myStatusLed.blue);debug(")");debugln();
        debug("SBID: ");debugln(sbid);
        debug("Ethernet Chip: ");debugln(Ethernet.getChip());        
        debug("Assigned Ethernet IP Address: ");debugln(Ethernet.localIP());         
        debug("Ethernet PHY State: ");debugln(Ethernet.phyState());
        debug("Ethernet Link Report: ");debugln(Ethernet.linkReport());
        debug("Ethernet Speed Report: ");debugln(Ethernet.speedReport());
        debug("Ethernet Duplex Report: ");debugln(Ethernet.duplexReport());            
        debug("Ethernet MAC Address: ");debugln(espEthMacAddress);
        debug("Saved Ethernet IP Address: ");debugln(espEthIP);
        debug("Api IP/URL: ");debugln(apiURL);
        debug("Api port: ");debugln(apiPort);
        debug("FW URL: ");debugln(firmwareURL);
        debug("FW Path: ");debugln(firmwarePath);
        debug("FW FileName: ");debugln(firmwareFilename);

}

