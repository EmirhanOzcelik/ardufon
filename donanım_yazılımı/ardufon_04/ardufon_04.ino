/*
 Name:		Sketch1.ino
 Created:	11.11.2023 18:07:28
 Author:	emirh
 *bitmap oluştururken dikey okuma işaretlenmelidir (OLED_I2C kütüphanesinde geçerli)
*/

#include <OLED_I2C.h>

OLED ekran(SDA, SCL);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t TinyFont[];
extern uint8_t pil_bit[];
extern uint8_t markabit[];
String kullanici = "Emirhan Ozcelik";
String anim1_goz[] = { "*  *","^  ^" };

unsigned long gecen_zaman = 0;
uint8_t snn = 0;
uint8_t sn = 0;
uint8_t dk = 0;
uint8_t saat = 0;

int buton_takip = 0;
const String alfabe[]={"_","a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","r","s","t","u","v","y","z",",","."};
bool invmod = false;

int menu_imlec = 0;
int satir_max = 8;

int ekran_num = 0; // 0=ana ekran , 1=saatekran , 2=pilekran ,3=rehber ...
const int sayfamax = 3; // 0 1 2 �� ekran i�in maksimum kontrol say�s�

void setup() {
	Serial.begin(9600);
	if (!ekran.begin(SSD1306_128X64))
		while (1);
	pinMode(2, INPUT_PULLUP);
	pinMode(4, INPUT_PULLUP);
	pinMode(7, INPUT_PULLUP);
	pinMode(8, INPUT_PULLUP);
	//yukleniyor("Hosgeldiniz", kullanici);
}

void loop()
{
	if (millis() - 500 > gecen_zaman)
	{
		snn++;
		sn = sn + snn % 2;
		buton_takip = 0;
		gecen_zaman = millis();
		Saat();
  	ekran_guncelle(ekran_num);
	}
		uint16_t x = map(analogRead(A1), 0, 1023, 0, 5); // min=0,orta=2,max=4
		uint16_t y = map(analogRead(A0), 0, 1023, 0, 5); //tu�lar aktifken de�er =
		/*uint8_t tsl = digitalRead(2); //sol
		uint8_t tsg = digitalRead(4); //sa�
		uint8_t tas = digitalRead(7); //a�a��
		uint8_t tyk = digitalRead(8); //yukar�*/
		if (x == 0 && buton_takip == 0)
		{
			buton_takip++;
			ekran_num++;
			if (ekran_num > sayfamax) { ekran_num = 0; }
      menu_imlec=0;
			ekran.clrScr(); 
		}
		else if (x == 4 && buton_takip == 0)
		{
			buton_takip++;
			ekran_num--;
			if (ekran_num < 0) { ekran_num = sayfamax; }
      menu_imlec=0;
			ekran.clrScr(); 
		}

}
int sarj_hesap(int b)
{
  int deger = map(analogRead(A0),0,1023,0,b);
  return deger;
}

void ekran_guncelle(byte a)
{
	if (a == 0) { anaekran(); }
	else if (a == 1) { saatekran(); }
	else if (a == 2) { menu_ekran(); }
  else if (a == 3) { pil_ekran();  }
	ekran.update();
}
void yukleniyor(String s, String user)
{
	ekran.setFont(SmallFont);
	ekran.clrScr();
	ekran.print(user, CENTER, 2);
	ekran.print(s, CENTER, 18);
	for (int a = 0; a < 4; a++)
	{
		for (int i = 0; i < 2; i++)
		{
			ekran.print(anim1_goz[i], CENTER, 40);
			ekran.print("-", CENTER, 54);
			delay(500);
			ekran.update();
		}
	}
	ekran.clrScr();
	ekran.update();
  if(digitalRead(7)==0 && digitalRead(2)==0)
  {invmod=true;while(invmod){kontrol_ekran(); if(digitalRead(8)==0&&digitalRead(4)==0){invmod=false; yukleniyor("Rmode exit",kullanici);}}}
}
void Saat()
{
	if (sn >= 60)
	{
		sn = 0; dk++; ekran.clrScr();
		if (dk >= 60)
		{
			sn = 0; dk = 0; saat++;
			if (saat >= 24) { sn = 0; dk = 0; saat = 0; }
		}
	}
}
void anaekran()
{
	ekran.setFont(SmallFont);
	ekran.print(kullanici, LEFT, 11);
	ekran.print("ardufon_02", LEFT, 1);
	ekran.print("gelen arama = ", LEFT, 31);
	ekran.print("gelen mesaj = ", LEFT, 51);
	ekran.printNumF(0, 0, 100, 31);
	ekran.printNumF(0, 0, 100, 51);
}
void saatekran()
{
	ekran.setFont(SmallFont);
	ekran.print("SAAT", CENTER, 10);
	ekran.setFont(MediumNumbers);
	ekran.printNumF(saat, 0, 10, 40);
	ekran.printNumF(dk, 0, 50, 40);
	ekran.printNumF(sn, 0, 90, 40);
}
void kontrol_ekran()
{
  	uint8_t tsl = digitalRead(2); //sol
		uint8_t tsg = digitalRead(4); //sag
		uint8_t tas = digitalRead(7); //asagi
		uint8_t tyk = digitalRead(8); //yukari
    uint16_t klavye = map(analogRead(A1), 0, 1023, 0, 26); // klavye kullanımı için pot.
		uint16_t numpad = map(analogRead(A0), 0, 1023, 1, 9); // numpad için kullanılan pot
    ekran.setFont(SmallFont);
    ekran.print("Rmod",RIGHT,0);
    ekran.print("klavye1: ",LEFT,0);
    ekran.print("klavye2: ",LEFT,12);
    ekran.print(alfabe[klavye],LEFT +60,0);
    ekran.printNumF(numpad,0,LEFT+60,12);
    ekran.printNumF(tsl,0,LEFT+40,40);
    ekran.printNumF(tsg,0,LEFT+80,40);
    ekran.printNumF(tas,0,CENTER,50);
    ekran.printNumF(tyk,0,CENTER,30);
    ekran.update();
    delay(50);
    ekran.clrScr();
}
void menu_ekran()
{
    uint8_t tsl = digitalRead(2); //sol --> off anahtar
		uint8_t tsg = digitalRead(4); //sag --> on anahtar
		uint8_t tas = digitalRead(7); //asagi
		uint8_t tyk = digitalRead(8); //yukari
    if(tas == 0 && buton_takip==0){ekran.clrScr(); buton_takip++; menu_imlec++; if(menu_imlec>satir_max){menu_imlec=0;}}
    else if (tyk == 0 && buton_takip==0){ekran.clrScr(); buton_takip++; menu_imlec--; if(menu_imlec<0){menu_imlec=satir_max;}}
    ekran.setFont(SmallFont);
    ekran.print("->",0,menu_imlec*8);
}
void pil_ekran()
{   
   int a;
  a = sarj_hesap(10);
  ekran.clrScr();
  ekran.setFont(MediumNumbers);
  ekran.printNumF(a, 0, 18, 20);  
  ekran.drawBitmap(6, 16, pil_bit, 30, 48);
}