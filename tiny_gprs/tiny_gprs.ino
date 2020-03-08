//---------------------------- GSM SETTINGS ---------------------------------------------
#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 650
#define SerialMon Serial
#define SerialAT Serial2
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI true
//---------------------------- /GSM SETTINGS --------------------------------------------

//---------------------------- GSM - WIFI ----------------------------------------------------
#include <WiFi.h>
WiFiClient Wifi;
const char* WSSID="hasan";
const char* WPW="hasan123456r";
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
TinyGsm modem(SerialAT);
TinyGsmClient client2(modem);

//---------------------------- /GSM - WIFI ----------------------------------------------------

//---------------------------- SERVERS ----------------------------------------------------
const char server[] = "sp.xxxx.com";
String resource = "/getfinger/?printkey=";
const int  port = 8756;
String contentType = "application/json";
//---------------------------- /SERVERS ----------------------------------------------------


WebSocketClient gpssocket = WebSocketClient(Wifi, "sp.xxxx.com", 80);
bool conncheck=false;bool wificheck = false;
void setup() {
  
  Serial.begin(115200); 
  SerialAT.begin(115200);
   while (!conncheck) {
        WiFi.disconnect();
        WiFi.begin(WSSID, WPW);
        int count = 0; bool wififail = false;
        while (!(WiFi.status() == WL_CONNECTED)) {
          count++; Serial.print(".."); if (count > 30) {
            wififail = true;
            break;
          }  delay(250);
        } 
        //if Wifi fail,Start Gprs
        if (wififail) {
          if (!modem.waitForNetwork()) {
          delay(1000);
          conncheck=false;
        }
        modem.isNetworkConnected();
        if (!modem.gprsConnect("", "", "")) {
          delay(3000);
          conncheck=false;
        }//Gprs company,User,pw - (null)
            gpssocket = WebSocketClient(client2, "sp.xxxx.com", 80); //Change ArduinoHttpClient Source to Gprs (client2-Gprs)
            gpssocket.begin("/api/SocketClient/FingerChangeListenerSocketServer?userID=4&companyNo=asd&vehicleID=32");// Start GPSSocket
        }
        else {// Wifi Success
            wificheck = true;
            gpssocket.begin("/api/SocketClient/FingerChangeListenerSocketServer?userID=4&companyNo=asd&vehicleID=32");// Start GPSSocket
        }
        int err2; const char pingserver[] = "www.google.com";//Internet Check
        if (wificheck) {
          HttpClient httpcontrol(Wifi, pingserver, 80);
          err2 = httpcontrol.get("/");
        }
        else {
          HttpClient httpcontrol(client2, pingserver, 80);
          err2 = httpcontrol.get("/");
        }
        delay(1000);
        if (err2 != 0) {
          Serial.println(F("Connection Failed")); 
          conncheck=false;
        }
        Serial.println(F("Connection Success"));
        conncheck=true;
       }
}

void loop() {

  
  // --------------------------------------------------------- Socket Using
       Serial.println(gpssocket.ping());//Check Socket   -- if sometimes failed,connect wifi&gprs and start socket again
       gpssocket.beginMessage(TYPE_TEXT); 
       gpssocket.print("Send data to Socket");
       gpssocket.endMessage();

       
        int messageSize = client2.parseMessage();//Read Data from Socket
        if (messageSize > 0) {
          Serial.println("Received a message:");
          Serial.println(client2.readString());
        }            
   \\ --------------------------------------------------------- Socket Using

  
  // --------------------------------------------------------- Http Using
  
      int status; String body;
      if (wificheck) {
        HttpClient http(Wifi, server, port);
        err = http.get(resource + data); status = http.responseStatusCode(); body = http.responseBody();
      }
      else {
        HttpClient http(client2, server, port);
        err = http.get(resource + data); status = http.responseStatusCode(); body = http.responseBody();
      }
    
      if (err != 0) {
        Serial.println(F("Failed")); 
        delay(1000);
        return 0;
      }
    
      Serial.println(status);
       Serial.println(body);
  \\ --------------------------------------------------------- Httpt Using

   //--------------------------------------------------------- Http Using Json
  
   // https://github.com/hasansahinnn/Esp32_Online_Fingerprint_Scanner_Cloud_Server---Django
  
   \\ --------------------------------------------------------- Http Using Json
  
   delay(5000);

}
