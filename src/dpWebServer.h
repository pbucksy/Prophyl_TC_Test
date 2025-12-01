//----------------------

const char MENU_ROW[] = R"rawliteral(
      <div class="menuDiv" align="center">         
        <button onclick="document.location='./'">Main</button> &nbsp;
        <button onclick="document.location='./projectConfig'">Project</button> &nbsp;
        <button onclick="document.location='./networkConfig'">Network</button> &nbsp;
        <button onclick="document.location='./firmwareConfig'">FW</button> &nbsp;
      </div>
)rawliteral";

const char TEST_ROW[] = "testLineForMe";

const char MAIN_PAGE[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">  
  <title>
    |panelID| Data
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
    <div class="outerDiv">
        |projectTitle|
      <h2>Main Page</h2>
      <FORM action="./" method="post">    
        <div class="centerDiv" align="center">             

            <br><br><h3>TestLabel Value:  |testLabelValue|
              <INPUT style="margin-left: 15px;" type="submit" name="btnSetTestLabel" value="SET">

            <br><br><h3>Set TestVariable Value: |testVariableValueParameter|</h3>
              <INPUT style="margin-left: 15px;" type="text" class="form-control" name="setTestVariableValue" value=""/>         
              <INPUT style="margin-left: 15px;" type="submit" name="btnSetTestVariableValue" value="SET">      

        </div>
      </FORM>
      |menuRow|
      </div>
    </div>        
  </body>
</html>

)rawliteral";

const char NETWORK_CONFIG_PAGE[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>
  |panelID| Config
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
    <div class="outerDiv">
      |projectTitleParameter|
      <h2>Halozat beallitasa</h2>
      <FORM action="./saveNetworkConfig" method="post">
        <div class="centerDiv">
          <br><h3>Eszkoz neve</h3>
            |hostname|
          |connectedWifiNetworkInfoParameter|          
          <br><h3>Elerheto WiFi halozatok</h3>
            <select id='wifiNetworks' style="margin-left: 15px;" name='wifis' style='width: 200px'>
              |wifis|
            </select>
          <h3>Jelszo</h3>
            |passwordParameter|
          <br>
          <br>
            <p style="text-align:center;">
              <INPUT type="submit" name="btnSave" value="MENTES"> 
                &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;     
              |btnUpdateParameter|
            </p>
          </p>
        </div>
      </FORM>
      |menuRow|
    </div>    
  </body>
</html>


)rawliteral";

const char ADVANCED_NETWORK_CONFIG_PAGE[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<title>
  |panelID| Config
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
    <div class="outerDiv">
      |projectTitleParameter|
      <h2>Network Configuration</h2>
      <FORM action="./saveNetworkConfig" method="post">
        <div class="centerDiv">
          <br><h3>ESP Hostname</h3>
            |hostname|
          <br><h3>WiFi MAC Address</h3>
            |macParameter|
          <br><h3>Desired Static IP Address</h3>
            |staticIpAddressParameter|
          <br><h3>Desired Static GateWay IP Address</h3>
            |staticGatewayAddressParameter|
          <br><h3>Desired Static Subnet Mask</h3>
            |staticSubnetMask|
          <br><h3>Connected to:</h3>
            |currentSSIDParameter|
          <br><h3>WiFi Signal Strength</h3>
            |wifiRSSIDParameter|
          <br><h3>Elerheto WiFi halozatok</h3>
            <select id='wifiNetworks' style="margin-left: 15px;" name='wifis' style='width: 200px'>
              |wifis|
            </select>
          <br><h3>SSID</h3>
          <p style="margin-left: 15px;">(Csak akkor toltsd ki, ha manualisan akarod megadni a WiFi halozatod nevet! Egyebkent hagyd uresen!)</p>
            |manualSSIDParameter|
          <h3>Jelszo</h3>
            |passwordParameter|
          <br><br><INPUT style="margin-left: 15px;" type="submit" value="SAVE">
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
    <div class="outerDiv">
      |projectTitleParameter|
      <h2>FW Konfiguracios felulet!</h2>
      <FORM action="/saveFirmwareConfig" method="post">
        <div class="centerDiv">
          <h3>Jelenlegi Firmware</h3>
            |lastFWFileName|
          <h3>Legutobbi Firmware Frissites Datuma</h3>
            |lastFWDateTime|
          <h3>FirmWare eleresi URL cime</h3>
            |firmwareURL|
          <br><h3>FirmWare eleresi utja</h3>
            |firmwarePath|
          <br><h3>FirmWare fajlneve</h3>
            |firmwareFilename|
          <br><br>
          <p style="text-align:center;">
            <INPUT type="submit" name="btnSave" value="MENTES"> 
              &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
              &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
              &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;                
            <INPUT type="submit" name="btnUpdate" value="FRISSITES">        
          </p>          
        </div>
      </FORM>
      |menuRow|
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
  |panelID| Config
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
    <div class="outerDiv">
      |projectTitleParameter|
      <h2>Konfiguracios felulet!</h2>
      <FORM action="./saveProjectConfig" method="post">
        <div class="centerDiv">
          <br><h3>API Endpoint IP / URL</h3>
            |apiURL|
          <br><h3>API Path</h3>
            |apiPath|
          <br><h3>API Endpoint PORT</h3>
            |apiPort|
          <br><br><INPUT style="margin-left: 15px;" type="submit" value="SAVE">
        </div>
      </FORM>
      |menuRow|
    </div>    
  </body>
</html>


)rawliteral";




const char FEEDBACK_PAGE[] = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name = "viewport" content = "width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0">
<meta http-equiv="refresh" content="|refreshTimeParameter|; url='|refreshPathParameter|'">
<style>
body { background-color: #404040; font-size: 12px; font-family: Arial, Helvetica, Sans-Serif; Color: #F6F6F6; }
</style>
</head>
<body>
<h3>|feedBackMessageParameter|</h3>
</body>
</html>
)rawliteral";
