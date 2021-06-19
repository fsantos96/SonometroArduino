#include <WiFi.h> 
#include <WiFiClientSecure.h> 
#include <HTTPClient.h> 
#include <ArduinoJson.h> 
#include <UniversalTelegramBot.h>
#include "config.h"


WiFiClientSecure client; 
UniversalTelegramBot bot(BOTtoken, client); 
String chat_id = "";
unsigned int senialMax = 0;
unsigned int senialMin = 1024;
int amplitude = 0;
int decibels = 0;
long miliseconds = 0;
int delaySample = 50;

void setup() {
  Serial.begin(115200);
  client.setInsecure();
  delay(100);
//conecta al wifi
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(10);
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = "Federico";
    if (from_name == "") from_name = "Guest";

    if (text == "/start") {
      String welcome = "Bienvenido, " + from_name + "\n";
      welcome += "Su disposivo ha sido agregado\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void loop() {
   // escucha los mensajes de telegrm
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  // se recorre de a un mensaje
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    lastTimeBotRan = millis();
  }

  miliseconds = millis();
  
  while (millis() - miliseconds < delaySample)
  {
    int micValue = analogRead(MicPin);
    Serial.println(micValue);
    if (micValue < 1024)
    {
      if (micValue > senialMax)
      {
        senialMax = micValue;
      }
      else if (micValue < senialMin)
      {
        senialMin = micValue;
      }
    }
  }
  
  amplitude = senialMax - senialMin;
  decibels = map(amplitude, 0, 1024, 49, 120);
 
  if (decibels <= 50)
  {
    Serial.println("bajo");
  }
  else if (decibels > 50 && decibels < 75)
  {
    Serial.println("Nivel: Moderado");
  }
  else
  {
    Serial.println("Nivel: Alto");       
  }
  delay (500);
}
