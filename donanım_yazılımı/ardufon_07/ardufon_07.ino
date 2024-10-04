/*
 Name:		ardufon_07.ino
 Created:	29.08.2024 16:41:00
 Author:	emirhan
*/
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <Wire.h>                   // I2C iletişimi için
#include <Adafruit_GFX.h>           // Grafik kütüphanesi
#include <Adafruit_SSD1306.h>       // SSD1306 OLED ekran kütüphanesi
#define SCREEN_WIDTH 128            // OLED  genişliği
#define SCREEN_HEIGHT 64            // OLED  yüksekliği
#define OLED_RESET    -1            // Reset pini (-1 kullanılırsa donanım reset pini kullanılmaz)
#define SCREEN_ADDRESS 0x3C         // Ekranın I2C adresi (genellikle 0x3C)
const uint16_t ORTA = 2506;
Adafruit_SSD1306 ekran(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //ekran setup1
extern uint8_t pil_20x10[];
extern uint8_t yok_10x10[];
extern uint8_t dusuk_10x10[];
extern uint8_t orta_10x10[];
extern uint8_t full_10x10[];
extern uint8_t ekran_32x32[];
extern uint8_t gelen_32x32[];
extern uint8_t off_32x32[];
extern uint8_t wifi_32x32[];
extern uint8_t tarih_32x32[];
extern uint8_t saat_32x32[];
extern uint8_t gonder_32x32[];
extern uint8_t ayarlar_32x32[];
extern uint8_t klavye1[];
extern uint8_t klavye2[];
extern uint8_t _mutlu[];
extern uint8_t _donuk[];
extern uint8_t _klp[];
extern uint8_t _uzgun[];
extern uint8_t _saskin[];
extern uint8_t _kedi[];
extern uint8_t _iskelet[];
const String uyg_ad [] = {"temel ayarlar","zaman ayarla","internete baglan","mesaj gonder","mesaj oku"}; 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t buzzer = 11;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  unsigned long gecen_zaman = 0;
  const unsigned long g_ms = 10;
  byte g_ms_sayac = 0; //aktiflik zaman için sayac
uint8_t zaman1[] = {21,21,21,21}; //sn - dk - saat - gun (makine calısma zamanını tutar)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool wifi_baglantisi = false;
  const byte max_ag = 25;
  char bulunan_aglar[max_ag];
  int8_t aglar_dbm[max_ag];
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte pil_deger = 99;
String pil_voltaj = "???";
  byte sag = 0;
  byte sol = 0;
  byte yuk = 0;
  byte asa = 0;
  byte ok = 0;
  byte yes = 0;
  byte no = 0;
  byte ctr = 0;
  byte but_sayac = 0;
byte sayfa_no = 0; //sayfa degisim imleci
byte sayfa_max = 5;
    bool klavye_izin = true;
    String klavye_verisi = "";
    const String emoji_kod[] = {"#","$","^","{","}","|","&"};//devre disi
    const byte oled_max_satir = 20;  // OLED ekran için max kaydırılacak satır (0,1,2,3,...)
    String oled_satir[oled_max_satir];  // OLED ekrana yazdırmak için dizi
    uint8_t kaydirici = 0; //birden çok satir yazdirmak icin fonksiyon degiskeni
    const uint8_t satir_max = 20; //oled ekran için satır başı max karakter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////       setup     ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
  Serial.begin(115200);
  ekran_kur();
  kilit_ekran();
}

void loop() 
{

  if(sayfa_no == 0) //uyg seçimi yapılan ekran
  {
    klavye();
  }
  else if(sayfa_no == 1)
  {
    panel_app();
  }
  else if(sayfa_no == 2)
  {

  }
  else if(sayfa_no == 3)
  {

  }
  else if(sayfa_no == 4)
  {

  }
}
