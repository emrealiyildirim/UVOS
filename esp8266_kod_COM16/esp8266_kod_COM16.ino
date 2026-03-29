#include <ESP8266WiFi.h>
#include <espnow.h>

const int trigPin = 14; 
const int echoPin = 12; 

// ALICI (ESP32) MAC ADRESI (Kendi kartınınkini buraya yaz)
uint8_t broadcastAddress[] = {0xD4, 0xE9, 0xF4, 0xE3, 0x2C, 0x00}; 

typedef struct struct_message {
  float sicaklik;
  int irtifa;
  bool hataVarMi; 
} struct_message;

struct_message uvostest;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) return;
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  randomSeed(analogRead(0)); 
}

void loop() {
  // Mesafe Ölçümü
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  // SADECE ELINI YAKLASTIRINCA CALIS
  if (distance > 0 && distance < 20) { 
    
    // %50 IHTIMALLE ABSÜRT/HATALI VERI
    if (random(0, 2) == 1) { 
      uvostest.irtifa = random(80000, 200000); // Absürt fırlama
      uvostest.sicaklik = random(150, 500);    // Absürt sıcaklık
      uvostest.hataVarMi = true;
    } 
    else {
      // %50 IHTIMALLE TEMIZ VERI
      uvostest.irtifa = 1200 + random(-5, 5);
      uvostest.sicaklik = 24.5 + (random(-10, 10) / 10.0);
      uvostest.hataVarMi = false;
    }

    esp_now_send(broadcastAddress, (uint8_t *) &uvostest, sizeof(uvostest));
    delay(250); // Seri portta akışı izlemek için ideal hız
  } 
  delay(50); // Sensör okuma kararlılığı için kısa bekleme
}
