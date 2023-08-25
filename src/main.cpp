

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <uri/UriBraces.h>

// Defining the WiFi channel speeds up the connection:
#define WIFI_CHANNEL 6

String password_Wifi = "";
String ssid_Wifi = "";


WebServer server(80);

const int LED1 = 26;
const int LED2 = 27;

bool led1State = false;
bool led2State = false;

void sendWiFiScanHtml() {
  int n = WiFi.scanNetworks();

  String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>ESP32 Web Server Demo</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          html { font-family: sans-serif; text-align: center; }
          body { display: inline-flex; flex-direction: column; }
        </style>

      </head>
            
      <body>
        <h1>ESP32 Web Server</h1>

        <table>
            <tr>
              <th>Index</th>
              <th>SSID</th>
              <th>RSSI</th>
            </tr>
            ROWS
        </table> 

        <form id="wifiForm">
          <label>Which Wifi network would you like to choose?</label><br>
          <input id="wifiChoice"><br>

          <label>Input the wifi password</label><br>
          <input id="wifiPassword"><br>

          <button id="changeAction" type="submit" onclick="beforeSubmit();">Choose WiFi</button>
        </form>



        <script defer >
            function beforeSubmit() {
                var form = document.getElementById('wifiForm');
                var wifiChoice = document.getElementById('wifiChoice').value;
                var wifiPassword = document.getElementById('wifiPassword').value;
                form.action="/wifi/" + wifiChoice + '/pass/' + wifiPassword;
                console.log("called")
              }
        </script>
      </body>
    </html>
    )";



  String rows = "";
  if (n == 0) {
      Serial.println("no networks found");
  } else {
    for (int i = 0; i < n; ++i) {
      String rowTemplate = R"(
      <tr>
        <td>ROW_1</td>
        <td>ROW_2</td>
        <td>ROW_3</td>
        <td>ROW_4</td>
      </tr>)";

      // Print SSID and RSSI for each network found
      rowTemplate.replace("ROW_1", String(i+1));
      rowTemplate.replace("ROW_2", WiFi.SSID(i));
      rowTemplate.replace("ROW_3", String(WiFi.RSSI(i)));
      rowTemplate.replace("ROW_4", (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?"None":"Secure");

      rows = rows + rowTemplate;
    }
  }

  response.replace("ROWS", rows);
  server.send(200, "text/html", response);
}
void sendHtml() {
  String response = R"(
    <!DOCTYPE html><html>
      <head>
        <title>ESP32 Web Server Demo</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          html { font-family: sans-serif; text-align: center; }
          body { display: inline-flex; flex-direction: column; }
          h1 { margin-bottom: 1.2em; } 
          h2 { margin: 0; }
          div { display: grid; grid-template-columns: 1fr 1fr; grid-template-rows: auto auto; grid-auto-flow: column; grid-gap: 1em; }
          .btn { background-color: #5B5; border: none; color: #fff; padding: 0.5em 1em;
                 font-size: 2em; text-decoration: none }
          .btn.OFF { background-color: #333; }
        </style>
      </head>
            
      <body>
        <h1>ESP32 Web Server</h1>

        <div>
          Connection successful
        </div>
      </body>
    </html>
  )";
  server.send(200, "text/html", response);
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  String password_AP = "12345678";
  String ssid_AP = "ESP32" ;
  WiFi.softAP(ssid_AP,password_AP);
  Serial.println("Created AP");
  Serial.print("ESP AP IP: ");
  Serial.println(WiFi.softAPIP());
  
  sendWiFiScanHtml();

  server.on("/", sendWiFiScanHtml);

  server.on(UriBraces("/wifi/{}/pass/{}"), []() {
    String wifiIndex = server.pathArg(0);
    String pass = server.pathArg(1);

    Serial.println(wifiIndex);
    Serial.println(pass);

    WiFi.begin(WiFi.SSID(wifiIndex.toInt()-1),pass,6);
    
    Serial.println("WiFi Connected");

    sendHtml();
  });
  server.begin();
  Serial.println("HTTP server started");

 
}

void loop(void) {
  server.handleClient();
  delay(2);
}
