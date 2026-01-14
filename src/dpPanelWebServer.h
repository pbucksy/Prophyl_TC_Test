//----------------------
const char MENU_ROW[] = R"rawliteral(
    
      <div class="menuDiv" align="center"> 
        <form action="./"><input type="submit" value="Main"/></form> &nbsp;
        <form action="./projectConfig"><input type="submit" value="Project"/></form> &nbsp;
        <form action="./ethConfig"><input type="submit" value="Network"/></form> &nbsp;
        <form action="./firmwareConfig"><input type="submit" value="FW"/></form> &nbsp;      
      </div>
    </FORM>
)rawliteral";


const char DATA_PAGE[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
  <title>
    |panelID| Data Page
  </title>
  <style>
  FORM { display: inline; }
  body { background-color: #303030; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
  .outerDiv {
    background-color: #404040;
    //border: 1px outset red;  
    padding-top:      5px;
    margin-top:       25px;
    margin-left:      auto;
    margin-right:     auto;
    margin-bottom:    25px;
    padding-bottom:   5px;
    width:            500px;  
    text-align:       center;    
  }
  .centerDiv {	  
    //border: 1px outset black;  
    margin-top:       25px;
    margin-left:      auto;
    margin-right:     auto;
    margin-bottom:    50px;
    width:            425px;
    text-align:       left;
  }
  .menuDiv {	  
    //border: 1px outset black;  
    margin-top:       25px;
    margin-left:      auto;
    margin-right:     auto;
    margin-bottom:    50px;
    width:            275px;
    text-align:       center;
  }
  .listGrid {
    display: grid;
    grid-template-columns: repeat(5, 1fr);
    grid-template-rows: repeat(2, 1fr);
    grid-column-gap: 0px;
    grid-row-gap: 0px;
  }

  .insertGrid {
    display: grid;
    grid-template-columns: repeat(5, 1fr);
    grid-template-rows: repeat(2, 1fr);
    grid-column-gap: 0px;
    grid-row-gap: 0px;
  }
  </style>
</head>
  <body>    
    <div class="outerDiv">      
      <h2>|panelID| Data Page</h2>
        
          <div class="centerDiv">
            <h3>Device ID</h3>
              |deviceID|            
            <br>
            <h3>Sensor list</h3>
            
            <FORM action="./deleteSensor" method="post">
              <div class="listGrid">
                <div>Type</div>
                <div>ID</div>
                <div>Pin</div>
                <div>Value</div>
                <div></div>
                |sensorList|
              </div>
            </FORM>
            <br>
            <div style="text-align:center;">__________________________________________</div>
            <br><br>
            <FORM action="./addSensor" method="post">
              <div class="insertGrid">
                <div>
                <select id='sensorTypes' style="width:75px;" name='setNewSensorType'>
                  |types|
                </select>                
                </div>
                <div><INPUT style="width:65px;"         type="text" name="setNewSensorID" value=""/></div>
                <div><INPUT style="width:65px;"         type="text" name="setNewSensorPin" value=""/></div>
                <div></div>
                <div><INPUT type="submit" name="btnAddSensor" value="ADD"></div>
              </div>
            </FORM>

          </div>
        
      |menuRow|
      <br>
    </div>    
  </body>
</html>

)rawliteral";

const char PROJECT_CONFIG_PAGE[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>
  |panelID| Project Config
</title>
<style>
  FORM { display: inline; }
body { background-color: #303030; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
.outerDiv {
  background-color: #404040;
  //border: 1px outset red;  
  padding-top:      5px;
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    25px;
  padding-bottom:   5px;
  width:            325px;  
  text-align:       center;    
}
.centerDiv {	  
  //border: 1px outset black;  
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    50px;
  width:            275px;
	text-align:       left;
}
.menuDiv {	  
  //border: 1px outset black;  
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    50px;
  width:            275px;
  text-align:       center;
}
</style>
</head>
  <body>
    <FORM action="./saveProjectConfig" method="post">
      <div class="outerDiv">
        |projectTitleParameter|
        <h2>Project Konfiguracios felulet!</h2>
        <div class="centerDiv">
          <h3>Project Title</h3>
            |hostnameParameter|
          <h3>Device ID</h3>
            |deviceID|
          <br><h3>API Endpoint IP / URL</h3>
            |apiURL|
          <br><h3>API Path</h3>
            |apiPath|
          <br><h3>API Endpoint PORT</h3>
            |apiPort|
          <br><br><INPUT style="margin-left: 15px;" type="submit" value="OK">
          </p>
        </div>        
      </FORM>    
      |menuRow|
      </div>
  </body>
</html>


)rawliteral";

const char WIFI_CONFIG_PAGE[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>
  |panelID| WiFi Config
</title>
<style>
  FORM { display: inline; }
body { background-color: #303030; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
.outerDiv {
  background-color: #404040;
  //border: 1px outset red;  
  padding-top:      5px;
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    25px;
  padding-bottom:   5px;
  width:            325px;  
  text-align:       center;    
}
.centerDiv {	  
  //border: 1px outset black;  
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    50px;
  width:            275px;
	text-align:       left;
}
.menuDiv {	  
  //border: 1px outset black;  
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    50px;
  width:            275px;
  text-align:       center;
}
</style>
</head>
  <body>
    <FORM action="./saveWifiConfig" method="post">
      <div class="outerDiv">
        |projectTitleParameter|
        <h2>WiFi Konfiguracios felulet!</h2>
        <div class="centerDiv">
          <br><h3>WiFi MAC Address</h3>
            |wifiMacParameter|
          <br><h3>Connected to:</h3>
            |currentSSIDParameter|
          <br><h3>WiFi Signal Strength</h3>
            |wifiRSSIDParameter|
            <br><h3>Elerheto WiFi halozatok</h1>
            <select id='wifiNetworks' style="margin-left: 15px;" name='wifis' style='width: 200px'>
              |wifis|
            </select><br>
          <h3>SSID</h3>
          <p style="margin-left: 15px;">(Csak akkor toltsd ki, ha manualisan akarod megadni a WiFi halozatod nevet! Egyebkent hagyd uresen!)</p>
            |manualSSIDParameter|
          <h3>Jelszo</h3>
            |passwordParameter|
          |submitParameter|
          
          </p>
        </div>
      </FORM>
      |menuRow|
      </div>
  </body>
</html>


)rawliteral";

const char ETH_CONFIG_PAGE[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>
  |panelID| Eth Config
</title>
<style>
  FORM { display: inline; }
body { background-color: #303030; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
.outerDiv {
  background-color: #404040;
  //border: 1px outset red;  
  padding-top:      5px;
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    25px;
  padding-bottom:   5px;
  width:            325px;  
  text-align:       center;    
}
.centerDiv {	  
  //border: 1px outset black;  
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    50px;
  width:            275px;
	text-align:       left;
}
.menuDiv {	  
  //border: 1px outset black;  
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    50px;
  width:            275px;
  text-align:       center;
}
</style>
</head>
  <body>
    <FORM action="./saveEthConfig" method="post">
      <div class="outerDiv">
        |projectTitleParameter|
        <h2>Ethernet Konfiguracios felulet!</h2>
        <div class="centerDiv">
          <br><h3>Eth IP Address</h3>
            |ethIpAddressParameter|
          <br><h3>Eth MAC Address</h3>
            |ethMacParameter|
          |submitParameter|
          </p>
        </div>
      </FORM>
      |menuRow|
      </div>
  </body>
</html>


)rawliteral";

const char FIRMWARE_CONFIG_PAGE[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>
  |panelID| FW Config
</title>
<style>
  FORM { display: inline; }
body { background-color: #303030; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
.outerDiv {
  background-color: #404040;
  //border: 1px outset red;  
  padding-top:      5px;
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    25px;
  padding-bottom:   5px;
  width:            325px;  
  text-align:       center;    
}
.centerDiv {	  
  //border: 1px outset black;  
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    50px;
  width:            275px;
	text-align:       left;
}
.menuDiv {	  
  //border: 1px outset black;  
  margin-top:       25px;
  margin-left:      auto;
  margin-right:     auto;
  margin-bottom:    50px;
  width:            275px;
  text-align:       center;
}
</style>
</head>
  <body>
    <FORM action="./saveFirmwareConfig" method="post">
      <div class="outerDiv">
        |projectTitleParameter|
        <h2>FW Konfiguracios felulet!</h2>
        <div class="centerDiv">
          <h3>Jelenlegi Firmware</h3>
            |lastFWFileName|
          <h3>Legutobbi Firmware Frissites Datuma</h3>
            |lastFWDateTime|
          <h3>FirmWare URL</h3>
            |firmwareURL|
          <br><h3>FirmWare Path</h3>
            |firmwarePath|
          <br><h3>FirmWare FileName</h3>
            |firmwareFilename|
          <br><br><INPUT style="margin-left: 15px;" type="submit" value="OK">
          </p>
        </div>    
      </FORM>
      |menuRow|
      </div>
  </body>
</html>


)rawliteral";


const char WIFI_SAVED_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<style>
body { background-color: #404040; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
</style>
</head>
<body>
<h3>Wifi Beallitasok mentve! Ujrainditas...</h3>
</body>
</html>
)rawliteral";


const char ETH_SAVED_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<style>
body { background-color: #404040; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
</style>
</head>
<body>
<h3>Ethernet Beallitasok mentve! Ujrainditas...</h3>
</body>
</html>
)rawliteral";


const char SENSOR_ADDED_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
  <html>
    <head>
    <meta http-equiv="refresh" content="8; url='./'"/>
    <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
      <style>
        body { background-color: #404040; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
      </style>
    </head>
    <body>
      <h3>Szenzor hozzaadva! Ujrainditas...</h3>
    </body>
  </html>
)rawliteral";


const char SENSOR_DELETED_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
  <html>
    <head>
    <meta http-equiv="refresh" content="8; url='./'"/>
    <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
      <style>
        body { background-color: #404040; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
      </style>
    </head>
    <body>
      <h3>Szenzor torolve! Ujrainditas...</h3>
    </body>
  </html>
)rawliteral";

const char softReset_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<style>
body { background-color: #404040; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
</style>
</head>
<body>
<h3>Ujrainditas...</h3>
</body>
</html>
)rawliteral";



const char hardReset_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<style>
body { background-color: #404040; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
</style>
</head>
<body>
<h3>Beallitasok torolve! Ujrainditas...</h3>
</body>
</html>
)rawliteral";

const char redirecToWifiConfig_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
  <html>
    <head>
      <meta http-equiv="refresh" content="2; url='/wifiConfig'"/>
      <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
      <title>
        Redirect...
      </title>
      <style>
      body { background-color: #303030; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
      .outerDiv {
        background-color: #404040;
        //border: 1px outset red;  
        padding-top:      5px;
        margin-top:       25px;
        margin-left:      auto;
        margin-right:     auto;
        margin-bottom:    25px;
        padding-bottom:   5px;
        width:            325px;  
        text-align:       center;    
      }
      .centerDiv {	  
        //border: 1px outset black;  
        margin-top:       25px;
        margin-left:      auto;
        margin-right:     auto;
        margin-bottom:    50px;
        width:            275px;
        text-align:       left;
      }
      </style>
    </head>
    <body>    
      <div class="outerDiv">      
        <h2>Redirecting to Wifi Config Page</h2>
        <div class="centerDiv">
        </div>
      </div>    
    </body>
  </html>
)rawliteral";

const char redirectToHome_HTML[] = R"rawliteral(
<!DOCTYPE HTML>
  <html>
    <head>
      <meta http-equiv="refresh" content="2; url='./'"/>
      <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
      <title>
        Redirect...
      </title>
      <style>
      body { background-color: #303030; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
      .outerDiv {
        background-color: #404040;
        //border: 1px outset red;  
        padding-top:      5px;
        margin-top:       25px;
        margin-left:      auto;
        margin-right:     auto;
        margin-bottom:    25px;
        padding-bottom:   5px;
        width:            325px;  
        text-align:       center;    
      }
      .centerDiv {	  
        //border: 1px outset black;  
        margin-top:       25px;
        margin-left:      auto;
        margin-right:     auto;
        margin-bottom:    50px;
        width:            275px;
        text-align:       left;
      }
      </style>
    </head>
    <body>    
      <div class="outerDiv">      
        <h2>Redirecting to Data Page</h2>
        <div class="centerDiv">
        </div>
      </div>    
    </body>
  </html>
)rawliteral";

//<meta http-equiv="Refresh" content="0; url='/config'"/>
