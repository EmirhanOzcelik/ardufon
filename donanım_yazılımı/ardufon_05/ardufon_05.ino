/*
 Name:		ardufon_05.ino
 Created:	23.12.2023 18:11:00
 Author:	emirhan
*/
#include <EEPROM.h>
#include <OLED_I2C.h>

OLED ekran(SDA, SCL);
 extern uint8_t SmallFont[];
 extern uint8_t MediumNumbers[];	

#define joy_x A0
#define joy_y A1
#define salter 2
#define buzzer 11
#define joy_but 4
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 unsigned long gecen_zaman = 0;
 unsigned long aralik = 250; // ayarlanabilir makine yenileme sıklığı
 byte aralik_say = 0; //yenileme aralığı sayacı
 uint16_t epr_kay_say = 0; //tarih eeprom kaydı DK gelişimi
 byte mesaj_kutu_say = 0; //mesaj kutusu sn sayacı
 byte salter_islem_say = 0; //acma kapama dugmesi islem tekrarı engelleyici
  byte sn = 0;
  byte dk = 0;
  byte saat = 0;
  byte gun = 0;
  byte ay_gun = 1;
  byte ay = 1; 
  uint8_t yil = 23; 
  uint16_t gercek_yil = 2000 + yil; // kullanıcıya yazılacak yil
 const byte ay_gun_max[] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 }; //null ,o, ş, m, n, m, h, t, a, e, e, k, a
 const String hafta_gunler[] = { "pazartesi","sali","carsamba","persembe","cuma","cumartesi","pazar" }; //max krktr 9 --> 6*9=53 (boşluk = 60)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 const byte zaman_son_kayit_adr[] = { 1,2,3,4,5,6,7 }; // eeprom tarih tutucu bellek adresleri
 const byte kayit_deg_adr = 8; // eeprom zaman kaydı aktivitesi tutucu bellek adresi
 const byte kayit_var = 50; // eeprom zaman kaydı var ise bu değer döndürülür
 const byte kayit_yok = 100; // eeprom zaman kaydı yok ise bu deger döndürülür
 const byte epr_kay_aralik = 3; //kaç saat aralıkla kayıt yapılacak?
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 bool bot_mod = false;
 bool mesaj_kutu = false;
 bool tel_acik = true;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////       setup     ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{
	Serial.begin(9600);
	if (!ekran.begin(SSD1306_128X64))
		while (1);
	pinMode(joy_but, INPUT_PULLUP);
  pinMode(salter,INPUT_PULLUP);
	pinMode(buzzer, OUTPUT);
  ekran.setFont(SmallFont);
  if(EEPROM.read(kayit_deg_adr) == kayit_var)
  {
    zaman_geri_yukle();
  }
	else if (digitalRead(joy_but) == 0 || bot_mod)
	{
		bot_mod = true;
		while (bot_mod)
		{
			test_ekran();
			if (digitalRead(joy_but) == 0 && analogRead(joy_y) <= 300)
			{
				bot_mod = false; ekran.clrScr(); ekran.update(); delay(1);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////       loop      ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
	if (millis() - aralik > gecen_zaman)
	{
    gecen_zaman = millis();
		aralik_say++;
		if (aralik_say * aralik == 1000)
		{
      aralik_say = 0;
			zaman_kontrol2();
      if(tel_acik)
      {
			  ana_ust_panel();
        if(mesaj_kutu == true && mesaj_kutu_say*aralik >= 2000){mesaj_kutu = false; mesaj_kutu_say=0;} //mesaj kutusu bildirisi sonlandır        
      }
		}
    else if(digitalRead(salter) == 0 || digitalRead(salter) == 1){salter_islem_say = 0;}
    else if(tel_acik && mesaj_kutu)
    {
     mesaj_kutusu();    mesaj_kutu_say++; //mesaj kutusu bildirisi başlat
    }

	}
  else if(digitalRead(salter) == 0 && salter_islem_say < 1)
  {
    tel_acik = false;
    ekran.clrScr();
    ekran.update();
    ekran.sleepMode(SLEEP_ON);
    salter_islem_say++;
    mesaj_kutu_say = 0;
  }  
  else if(digitalRead(salter) == 1 && salter_islem_say < 1)
  {
    ekran.sleepMode(SLEEP_OFF);
    tel_acik = true;
    salter_islem_say++;
  }
  if(tel_acik && Serial.available() > 0)
  {
   zaman_update_serial();
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////    fonksiyonlar   //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void zaman_kontrol()
{
	if (sn < 59)
	{
		sn++;
	}
	else
	{
		sn = 0; 
		if (dk < 59)
		{
			dk++;
		}
		else
		{
			dk = 0;
			epr_kay_say++;
			if (epr_kay_say == epr_kay_aralik)
			{
				epr_kay_say = 0;
				epr_zaman_upd();
			}
			if (saat < 23)
			{
				saat++;
			}
			else
			{
				saat = 0;
				if (ay_gun < ay_gun_max[ay])
				{
					ay_gun++;
				}
				else
				{
					ay_gun = 1;
					if (ay < 11)
					{
						ay++;
					}
					else
					{
						ay = 1;
						yil++;
					}
				}
				if (gun < 6)
				{
					gun++;
				}
				else
				{
					gun = 0;
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void zaman_kontrol2()
{
  sn++;
  if(sn>=60)
  {
    epr_kay_say++;
    dk++;
    sn = 0;
    if(epr_kay_say == epr_kay_aralik*60){ epr_kay_say = 0; epr_zaman_upd(); }
    if(dk>=60)
    {
     saat++;
     dk = 0;
    }
    else if(saat>=24)
    {
     gun++;
     saat = 0;
     ay_gun++;
    }
    else if(gun>=6)
    {
     gun = 0;
    }
    else if(ay_gun == ay_gun_max[ay])
    {
     ay_gun = 1;
     ay++;
    }
    else if(ay >= 11)
    {
     ay = 1;
     yil++;
    }
 }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ana_ust_panel()
{
  ekran.clrScr();
	ekran.printNumF(sn, 0, 0, 12); //sn
	ekran.printNumF(dk, 0, 21, 12); //dk
	ekran.printNumF(saat, 0, 41, 12); //saat
	ekran.print(":", 14, 12);
	ekran.print(":", 34, 12);
	ekran.printNumF(ay_gun, 0, 0, 2); //ay gun
	ekran.printNumF(ay, 0, 21, 2); //ay
	ekran.printNumF(gercek_yil, 0, 41, 2); //yıl
	ekran.print(".", 14, 2);
	ekran.print(".", 34, 2);
	ekran.print(hafta_gunler[gun], 66, 12);
	//ekran.drawBitmap(80, 2, bit_panel_pil, 17, 10);
  ekran.update();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void test_ekran()
{
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void epr_zaman_upd()
{
  if(EEPROM.read(kayit_deg_adr) != kayit_var)
  {
    EEPROM.write(kayit_deg_adr, kayit_var);
  }
		EEPROM.write(zaman_son_kayit_adr[0],sn);
    EEPROM.write(zaman_son_kayit_adr[1],dk);
    EEPROM.write(zaman_son_kayit_adr[2],saat);
    EEPROM.write(zaman_son_kayit_adr[3],gun);
    EEPROM.write(zaman_son_kayit_adr[4],ay_gun);
    EEPROM.write(zaman_son_kayit_adr[5],ay);
    EEPROM.write(zaman_son_kayit_adr[6],yil);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void zaman_geri_yukle()
{
    sn = EEPROM.read(zaman_son_kayit_adr[0]);
    dk = EEPROM.read(zaman_son_kayit_adr[1]);
    saat = EEPROM.read(zaman_son_kayit_adr[2]);
    gun = EEPROM.read(zaman_son_kayit_adr[3]);
    ay_gun = EEPROM.read(zaman_son_kayit_adr[4]);
    ay = EEPROM.read(zaman_son_kayit_adr[5]);
    yil = EEPROM.read(zaman_son_kayit_adr[6]);  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mesaj_kutusu()
{
  ekran.clrScr();
  ekran.drawRect(5, 3, 123, 61);
  ekran.update();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void zaman_update_serial()
{
  if(Serial.read() == 'T')
  {
    int d = Serial.parseInt();
    if(d < 100)
    {
      sn = d;
    }
    else if(d < 200)
    {
      dk = d - 100;
    }
    else if(d < 300)
    {
      saat = d - 200;
    }
    else if(d < 400)
    {
      ay_gun = d - 300;
    }
  }
  else if(Serial.read() == 'Y')
  {
    int z = Serial.parseInt();
    if(z < 10)
    {
      gun = z;
    }
    else if(z > 2000)
    {
      if(z - 2000 < 250)
      {
        yil = z-2000;        
      }
    }
  }
  else if(Serial.read() == '*')
  {
    epr_zaman_upd();
    zaman_geri_yukle();
    Serial.println("epr & upd ok");
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
String ekrana_metin_yaz(String str,uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t kaydirma_cubuk)
{
  int str_uzunluk = str.length();
  uint8_t max_satir = y2-y1;
  uint8_t max_kelime = x2-x1;
}