#include<WiFi.h>
#include<PubSubClient.h>
#include <Ethernet.h>
#include <SPI.h>

const char* ssid = "Rely";
const char*password = "12345678";
const char*mqttServer = "broker.mqtt-dashboard.com";
const int mqttPort = 1883;
const char* clientID = "";

const uint8_t A = 16;
const uint8_t B = 17;
const uint8_t C = 5;
const uint8_t D = 18;
const uint8_t G = 4;
const uint8_t DI = 0;
const uint8_t CLK = 2;
const uint8_t LAT = 15;

const String b[16] = {"000100000110000000000100010000000000000000000000", "000100000010000001100100010000000000000110000000",
                      "000100111111111001100100111111100000000110000000", "011111001000100001100100100011000000000110000000",
                      "000100001000100001100100100011000011111111111100", "000100001000100001100101000010000010000110000100",
                      "000100111111111001100101010010000010000110000100", "000111000000000001100100010010000010000110000100",
                      "011100000100100001100100010110000010000110000100", "000100111111111001101100011100000011111111111100",
                      "000100001000100001110100001100000000000110000000", "000100011001000001100100001100000000000110000000",
                      "000100000111000000000100010110000000000110000000", "000100011101110000000101100011000000000110000000",
                      "011101110000011000000111000001100000000110000000", "000000000000000000000000000000000000000000000000"
                     };
String a[16];
int lcdx = 16;
int lcdy = 16;
int clinex = 16;//字的寬度
int cliney = 16;
String go = "b";
unsigned long atime = millis();
unsigned long btime;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting...");
  }
  Serial.println("");
  Serial.println("WiFi connected with IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(clientID)) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  // 等5秒之後再重試
    }
  }
}

int ix = 0; // a[ix]陣列初始
void callback(char*topic, byte*payload , unsigned int length) {
  String messenger = "";
  int math = length;
  Serial.print("Messagearrived in PC: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messenger += (char)payload[i];
  }
  Serial.println();
  Serial.println("-----------------------");

  if (messenger == "b") {
    go = "b";
    ix = 0;
  }
  if (messenger == "a") {
    go = "a";
  }
  if (math % 16 == 0) {
    clinex = math;
    a[ix] = messenger;
    //Serial.println(a[i]);
    ix++;
  }
  for (int i = 0; i < ix; i++) {
    Serial.println(a[i]);
  }
}

void showclstr(int i, int j, int lcdx, int clinex) {
  if (go == "b") {//執行接收中
    for (int k = 0; k < lcdx; k++) {
      if (b[j].charAt(((i + k) % 48)) == '1')
        digitalWrite(DI , LOW);
      else {
        digitalWrite(DI , HIGH);
      }
      digitalWrite(CLK , HIGH);
      digitalWrite(CLK , LOW);
    }
  }
  if (go == "a") {
    for (int k = 0; k < lcdx; k++) {
      if (a[j].charAt(((i + k) % clinex)) == '1')
        digitalWrite(DI , LOW);
      else {
        digitalWrite(DI , HIGH);
      }
      digitalWrite(CLK , HIGH);
      digitalWrite(CLK , LOW);
    }
  }
  digitalWrite(G, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(LAT, HIGH);
  digitalWrite(LAT, LOW);
  row(15 - j);
  digitalWrite(G, LOW);
}

void  setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
  reconnect();
  client.subscribe("esp/test");
  client.setCallback(callback);
  pinMode(2, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(DI, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(LAT, OUTPUT);

  //pinMode(2, INPUT_PULLUP);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(G, HIGH);
  digitalWrite(DI , LOW);
  digitalWrite(CLK, LOW);
  digitalWrite(LAT, LOW);
}

int n = 0;
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  btime = millis();
  if (go == "b") {//執行接收中
    if ((btime - atime) >= 100) {
      n = n + 1;
      atime = btime;
    }
    if (n > 48) {
      n = 0;
    }
    for (int j = 0; j < cliney; j++) {
      showclstr(n, j, lcdx, clinex);
    }
  }

  if (go == "a") {
    if ((btime - atime) >= 100) {
      n = n + 1;
      atime = btime;
    }
    if (n > clinex) {
      n = 0;
    }
    for (int j = 0; j < cliney; j++) {
      showclstr(n, j, lcdx, clinex);
    }
  }
}

void row(int k) {
  switch (k) {
    case 0:
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      break;
    case 1:
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      break;
    case 2:
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      break;
    case 3:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      break;
    case 4:
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      break;
    case 5:
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      break;
    case 6:
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      break;
    case 7:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, LOW);
      break;
    case 8:
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      break;
    case 9:
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      break;
    case 10:
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      break;
    case 11:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, LOW);
      digitalWrite(D, HIGH);
      break;
    case 12:
      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      break;
    case 13:
      digitalWrite(A, HIGH);
      digitalWrite(B, LOW);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      break;
    case 14:
      digitalWrite(A, LOW);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      break;
    case 15:
      digitalWrite(A, HIGH);
      digitalWrite(B, HIGH);
      digitalWrite(C, HIGH);
      digitalWrite(D, HIGH);
      break;
  }
}
