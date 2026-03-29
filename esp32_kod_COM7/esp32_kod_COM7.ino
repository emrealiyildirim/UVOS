#include <WiFi.h>
#include <esp_now.h>

// Gelen veri yapısı (Gönderici ile aynı olmalı)
typedef struct struct_message {
  float sicaklik;
  int irtifa;
  bool hataVarMi;
} struct_message;

struct_message gelenVeri;
int sonSaglamIrtifa = 1200; // İlk güvenli veri başlangıcı

// Veri geldiğinde çalışan fonksiyon
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&gelenVeri, incomingData, sizeof(gelenVeri));
  
  int hamVeri = gelenVeri.irtifa; // Ekran için bozuk hali tutuyoruz

  if (gelenVeri.hataVarMi) {
    // --- UVOS FILTRELEME MOTORU ---
    // Gelen absürt veriyi reddet, son sağlam veriyi yazdır
    gelenVeri.irtifa = sonSaglamIrtifa; 
    
    Serial.println("--------------------------------------------------");
    Serial.print("[!] KRITIK HATA ALGILANDI! Ham Veri: "); Serial.println(hamVeri);
    Serial.print("[v] UVOS ONARIMI AKTIF: "); Serial.print(gelenVeri.irtifa); 
    Serial.println(" m [SISTEM KORUMADA]");
    Serial.println("--------------------------------------------------");
  } 
  else {
    // Veri temizse bir sonraki olası hata için yedekle
    sonSaglamIrtifa = gelenVeri.irtifa; 
    Serial.print("[+] SINYAL STABIL: "); Serial.print(gelenVeri.irtifa);
    Serial.print(" m | Sicaklik: "); Serial.print(gelenVeri.sicaklik);
    Serial.println(" C");
  }
}

void setup() {
  Serial.begin(115200);
  
  // --- SERI PORT AÇILIŞ LOGOSU (ASCII ART) ---
  Serial.println("");
  Serial.println("########################################################");
  Serial.println("#                                                      #");
  Serial.println("#   _    _  __      __  ____    _____                  #");
  Serial.println("#  | |  | | \\ \\    / / / __ \\  / ____|                 #");
  Serial.println("#  | |  | |  \\ \\  / / | |  | | (___                    #");
  Serial.println("#  | |  | |   \\ \\/ /  | |  | |  \\___ \\                   #");
  Serial.println("#  | |__| |    \\  /   | |__| |  ____) |                  #");
  Serial.println("#   \\____/      \\/     \\____/  |_____/                   #");
  Serial.println("#                                                      #");
  Serial.println("#       UYDU VERILERI ONARIM SISTEMI - 2026            #");
  Serial.println("#           PROJE EKIBI: MUSTAFA & UVOS                #");
  Serial.println("########################################################");
  Serial.println("");

  // Wi-Fi Modu
  WiFi.mode(WIFI_STA);

  // ESP-NOW Başlat
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Baslatilamadi!");
    return;
  }
  
  // Veri alma callback'ini kaydet
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  
  Serial.println(">>> YER ISTASYONU DINLEMEDE. VERI BEKLENIYOR... <<<");
  Serial.println("");
}

void loop() {
  // Tüm işlemler veri geldiğinde OnDataRecv içinde döner.
}
