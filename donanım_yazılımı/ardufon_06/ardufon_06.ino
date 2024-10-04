/*
 Name:		ardufon_06.ino
 Created:	19.3.2024 18:11:00
 Author:	emirhan
*/
#include <EEPROM.h>
#include <OLED_I2C.h>

OLED ekran(SDA, SCL);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t ana_ekran_bitmap[];
extern uint8_t saat_bitmap[];
extern uint8_t ayarlar_bitmap[];
extern uint8_t duvar_kagit1[];
extern uint8_t duvar_kagit2[];
extern uint8_t duvar_kagit3[];

#define joy_x A0  // sag icin '< 2' sol icin '> 2'
#define joy_y A1  // yukari icin '>8 ' asagi icin '<8'
#define buzzer 11
#define joy_but 4  //on icin '5' off icin '0' \
                   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long gecen_zaman = 0;
unsigned long aralik = 125;                                                                                   // ayarlanabilir makine yenileme sıklığı
byte aralik_say = 0;                                                                                          //yenileme aralığı sayacı
uint16_t epr_kay_say = 0;                                                                                     //tarih eeprom kaydı DK gelişimi
byte mesaj_kutu_say = 0;                                                                                      //mesaj kutusu sn sayacı
byte mesaj_kutu_say_max = 10000;                                                                              //milisaniye cinsinden bekleme süresi
byte zaman[6];                                                                                                //sn dk saat gun ay_gun ay yil
const byte ay_gun_max[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };                              //null ,o, ş, m, n, m, h, t, a, e, e, k, a
const String hafta_gunler[] = { "pazartesi", "sali", "carsamba", "persembe", "cuma", "cumartesi", "pazar" };  //max krktr 9 --> 6*9=53 (boşluk = 60)
const byte zaman_son_kayit_adr[] = { 1, 2, 3, 4, 5, 6, 7 };                                                   // eeprom tarih tutucu bellek adresleri
const byte kayit_deg_adr = 8;                                                                                 // eeprom zaman kaydı aktivitesi tutucu bellek adresi
const byte kayit_var = 50;                                                                                    // eeprom zaman kaydı var ise bu değer döndürülür
const byte kayit_yok = 100;                                                                                   // eeprom zaman kaydı yok ise bu deger döndürülür
const byte epr_kay_aralik = 3;                                                                                //kaç saat aralıkla kayıt yapılacak?
const byte duvar_kagit_kayit_adr = 9;                                                                         // duvar kagidi secimi kayit adresi
byte duvar_kagit = 0;                                                                                         // 0 1 2 3 ... resim arttıkca boyle gidebilir
                                                                                                              ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool bot_mod = false;                                                                                         //sistem aksaganlari kontrolu ekrani izni
bool mesaj_bildiri = false;                                                                                   // mesaj kutusu baslatma izni
bool seri_yaz = false;                                                                                        //seri ekrandan oled ekrana mesaj yazma izni
bool acilis_bekletme = true;                                                                                  // acilista bekletme döngüsü icin tutucu
bool uyku = false;                                                                                            // uyku modu izni
bool zaman_guncelleme = false;                                                                                //eeprom seri port zaman güncellestirme izni
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte islem = 0;              // dügmelerden gelen verileri kapsar
uint16_t uyku_sayac = 0;     // islem yapılmadıgı zaman artar (her sn başı)
byte uyku_dk = 1;            //kac dk hareketsiz kalınca uyku moduna gecilecek ?
byte islem_sayac = 0;        // dügme komut spawnı icin engel (0 ise okunur 1 ise okunmaz)
int8_t imlec = 0;            // genel kaydırma cubukları icin
int8_t sayfa_secici = 0;     //joyistik ile sayfa ikonu gezme islemi icin degisken
byte sayfa_no = 0;           // sayfa degisimi icin son onay (en son sayfa_seciciye eslenir)
int8_t doluyor_anim_sn = 1;  //animasyon karakter sayısı (sizeof(doluyor_anim))
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
String mesaj_kutu_not = "";  // mesaj kutusunda gösterilecek mesaj
byte mesaj_kutu_boyut = 18;  // satır başına 18 karakter görüntülemeye izin verir
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const byte sayfa_max = 2;  //ekran_isim dizisi eleman sayısı
String ekran_isim[] = { "ana ekran", "ayarlar", "saat" };
String doluyor_anim[] = { "<", ">" };
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const byte satir1 = 6;  //satir sayısı (ayar_secenek)
const byte sayfa1 = 1;  //sütun sayısı (ayar_secenek)
const String secici_imlec = "->";
String ayar_secenek[satir1][sayfa1] = {
  { "uyku =" },
  { "pp = " },
  { "zaman epr" },
  { "bos yuva" },
  { "bos yuva" },
  { "bos yuva" }
};                                           //ayarlar menüsü için yazılacak yazılar liste halinde
uint8_t uyku_dk_sc[] = { 1, 2, 5, 10, 30 };  //secim icin secenekler (eeproma kaydedilecek ham veriler)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////       setup     ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  if (!ekran.begin(SSD1306_128X64))
    while (1)
      ;

  pinMode(joy_but, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  ekran.setFont(SmallFont);
  acilis_ekran();
  if (EEPROM.read(kayit_deg_adr) == kayit_var) {
    zaman_geri_yukle();

    while (bot_mod) {
      test_ekran();
      if (digitalRead(joy_but) == 0 && analogRead(joy_y) <= 300) {
        bot_mod = false;
        ekran.clrScr();
        ekran.update();
        delay(1);
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////       loop      ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  if (millis() - aralik >= gecen_zaman) {
    if (islem_sayac > 0) {
      islem_sayac = 0;
      uyku_sayac = 0;
      if (uyku) {
        uyku = false;
        uyku_mod();
      }
    } else {
      uyku_sayac++;
      if (uyku_sayac * aralik >= uyku_dk * 60000) {
        uyku = true;
        uyku_mod();
      }
    }
    ana_ekran_panel();
    if (aralik_say * aralik >= 1000) {
      aralik_say = 0;
      zaman_kontrol();
    } else if (mesaj_bildiri) {
      mesaj_kutu_say++;
      if (mesaj_kutu_say * aralik == mesaj_kutu_say_max || islem == 5) {
        mesaj_kutu_say = 0;
        mesaj_bildiri = false;
        mesaj_kutu_not = "";
      }
      ekran.clrScr();
      mesaj_kutusu(mesaj_kutu_not, mesaj_kutu_boyut);
      ekran.update();
    }
    islem = 0;
    gecen_zaman = millis();
    aralik_say++;
  }
  if (Serial.available()) {
  } else if (islem_sayac < 1) {
    volatile byte x = analogRead(joy_x);
    volatile byte y = analogRead(joy_y);
    if (x <= 3) {
      islem = 1;
      islem_sayac++;
    }
    if (x >= 1010) {
      islem = 2;
      islem_sayac++;
    }
    if (y <= 3) {
      islem = 3;
      islem_sayac++;
    }
    if (y >= 1020) {
      islem = 4;
      islem_sayac++;
    }
    if (digitalRead(joy_but) == 0) {
      islem_sayac++;
      islem = 5;
    }
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////    fonksiyonlar   /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void zaman_kontrol() {
  zaman[0]++;
  if (zaman[0] >= 60) {
    epr_kay_say++;
    zaman[1]++;
    zaman[0] = 0;
    if (epr_kay_say == epr_kay_aralik * 60) {
      epr_kay_say = 0;
      epr_zaman_upd();
    }
    if (zaman[1] >= 60) {
      zaman[2]++;
      zaman[1] = 0;
    } else if (zaman[2] >= 24) {
      zaman[3]++;
      zaman[2] = 0;
      zaman[4]++;
    } else if (zaman[3] >= 6) {
      zaman[3] = 0;
    } else if (zaman[4] == ay_gun_max[zaman[5]]) {
      zaman[4] = 1;
      zaman[5]++;
    } else if (zaman[5] >= 11) {
      zaman[5] = 1;
      zaman[6]++;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ust_panel() {
  ekran.printNumF(zaman[1], 0, 116, 2);  //dk
  ekran.printNumF(zaman[2], 0, 98, 2);   //saat
  ekran.print(":", 110, 2);
  ekran.printNumF(zaman[4], 0, 0, 2);          //ay gun
  ekran.printNumF(zaman[5], 0, 21, 2);         //ay
  ekran.printNumF(zaman[6] + 2000, 0, 41, 2);  //yıl
  ekran.print(".", 14, 2);
  ekran.print(".", 34, 2);
  ekran.print(hafta_gunler[zaman[3]], 1, 10);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void saat_panel() {
  ekran.print("saat", 0, 0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ayar_panel() {
  if (islem == 3) { imlec = artan_tekrar(imlec, 0, 4, 1, 1); }
  if (islem == 4) { imlec = artan_tekrar(imlec, 0, 4, 1, 0); }
  if (imlec <= 6) {
    ekran.print(secici_imlec, 0, imlec * 16);
    for (int a = 0; a < 6; a++) {
      ekran.print(ayar_secenek[a][0], 14, a * 16);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void alt_panel() {
  if (islem == 1) {
    sayfa_secici = artan_tekrar(sayfa_secici, 0, sayfa_max, 1, 1);
  } else if (islem == 2) {
    sayfa_secici = artan_tekrar(sayfa_secici, 0, sayfa_max, 1, 0);
  } else if (islem == 5) {
    sayfa_no = sayfa_secici;
  }
  if (sayfa_secici == 0) {
    ekran.drawBitmap(96, 32, ana_ekran_bitmap, 32, 32);
  } else if (sayfa_secici == 1) {
    ekran.drawBitmap(96, 32, ayarlar_bitmap, 32, 32);
  } else if (sayfa_secici == 2) {
    ekran.drawBitmap(96, 32, saat_bitmap, 32, 32);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ana_ekran_panel() {
  ekran.clrScr();
  alt_panel();
  if (sayfa_no == 0) {
    ust_panel();
  } else if (sayfa_no == 1) {
    ayar_panel();
  } else if (sayfa_no == 2) {
    saat_panel();
  }
  ekran.update();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void test_ekran() {
  ekran.setFont(SmallFont);
  ekran.clrScr();
  ekran.print("A0 = ", 0, 0);
  ekran.print("A1 = ", 0, 8);
  ekran.print("joy_but = ", 0, 16);
  ekran.printNumF(analogRead(joy_x), 0, 30, 0);
  ekran.printNumF(analogRead(joy_y), 0, 30, 8);
  ekran.printNumF(digitalRead(joy_but), 0, 60, 16);
  ekran.update();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void epr_zaman_upd() {
  if (EEPROM.read(kayit_deg_adr) != kayit_var) {
    EEPROM.write(kayit_deg_adr, kayit_var);
  }
  for (int a = 0; a < 6; a++) {
    EEPROM.write(zaman[a], zaman_son_kayit_adr[a]);
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void zaman_geri_yukle() {
  for (int a = 0; a < 6; a++) {
    zaman[a] = EEPROM.read(zaman_son_kayit_adr[a]);
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void seri_port_zaman_ayar() {
  char Veri[250];
  uint16_t Boyut = Serial.readBytesUntil('*', Veri, sizeof(Veri));
  for (int p = 0; p < Boyut; p++) {
    if (Veri[p] == '<') { zaman_guncelleme = true; }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mesaj_box(String st[], byte size_s, byte islem) {
  ekran.drawLine(4, 0, 4, 64);
  ekran.drawLine(124, 0, 124, 64);
  if (size_s > 6) {
    ekran.print("*", 126, imlec);
    byte imlec_atlama_oran = 64 / (size_s - 6);
    if (islem == 4) {
    }                        //yarım kaldı
    else if (islem == 3) {}  //yarım kaldı
  } else {
    for (int e = 0; e <= size_s; e++) { ekran.print(st[e], 10, e * 8); }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
String mesaj_kutusu(String str, byte boyut) {
  uint16_t index0 = 0;
  uint16_t str_uzunluk = str.length();
  uint16_t satir_max = (str_uzunluk - (str_uzunluk % boyut)) / boyut + 1;
  String bildiri[satir_max];
  for (int a = 0; a < satir_max; a++) {
    if (a == satir_max - 1) {
      bildiri[a] = str.substring(index0, index0 + (str_uzunluk % boyut));
      mesaj_bildiri = true;
      mesaj_box(bildiri, satir_max, islem);
    } else {
      bildiri[a] = str.substring(index0, index0 + boyut);
      index0 = index0 + boyut;
    }
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void acilis_ekran() {
  int16_t sss = 0;
  const byte duvar_kagidi_max_sn = 60;
  duvar_kagit = EEPROM.read(duvar_kagit_kayit_adr);
  ekran.clrScr();
  if (bot_mod == true) {
    ekran.print("b", 122, 8);
    ekran.print("o", 122, 16);
    ekran.print("t", 122, 24);
  } else {
    ekran.print("o", 122, 8);
    ekran.print("t", 122, 16);
    ekran.print("o", 122, 24);
  }
  if (duvar_kagit == 250) {
    duvar_kagit = random(1, 3);
    if (duvar_kagit == 1) {
      ekran.drawBitmap(0, 0, duvar_kagit1, 110, 60);
    } else if (duvar_kagit == 2) {
      ekran.drawBitmap(0, 0, duvar_kagit2, 110, 60);
    } else if (duvar_kagit == 3) {
      ekran.drawBitmap(0, 0, duvar_kagit3, 110, 60);
    }
  } else {
    if (duvar_kagit == 1) {
      ekran.drawBitmap(0, 0, duvar_kagit1, 110, 60);
    } else if (duvar_kagit == 0) {
      ekran.drawBitmap(0, 0, duvar_kagit1, 110, 60);
    } else if (duvar_kagit == 3) {
      ekran.drawBitmap(0, 0, duvar_kagit3, 110, 60);
    }
  }
  while (acilis_bekletme) {
    if (millis() - aralik >= gecen_zaman) {
      aralik_say++;
      islem_sayac = 0;
      ekran.clrLine(120, 40, 128, 64);
      ekran.update();
      if (aralik_say * aralik >= 1000) {
        aralik_say = 0;
        zaman_kontrol();
        sss++;
        if (sss >= duvar_kagidi_max_sn) {
          acilis_bekletme = false;
        } else if (doluyor_anim_sn >= 0) {
          ekran.print(doluyor_anim[doluyor_anim_sn], 120, 50);
          doluyor_anim_sn--;
        } else {
          doluyor_anim_sn = 1;
        }
      }
    }
    if (digitalRead(joy_but) == 0 && analogRead(joy_y) >= 600) {
      islem_sayac++;
      ekran.clrLine(120, 0, 128, 30);
      bot_mod = !bot_mod;
      if (bot_mod) {
        ekran.print("b", 122, 8);
        ekran.print("o", 122, 16);
        ekran.print("t", 122, 24);
      } else {
        ekran.print("o", 122, 8);
        ekran.print("t", 122, 16);
        ekran.print("o", 122, 24);
      }
      ekran.update();
    } else if (analogRead(A0) > 900 || analogRead(A0) < 23) {
      acilis_bekletme = false;
      islem_sayac++;
    }
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void uyku_mod() {
  if (uyku) {
    ekran.sleepMode(SLEEP_ON);
  } else {
    ekran.sleepMode(SLEEP_OFF);
    ekran.update();
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int8_t artan_tekrar(byte artan, byte min, byte max, byte aralik, byte artis) {
  if (artis == 0) {
    if (artan <= min) {
      artan = max;
    } else {
      artan = artan - aralik;
    }
  } else if (artis == 1) {
    if (artan >= max) {
      artan = min;
    } else {
      artan = artan + aralik;
    }
  }
  return artan;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
