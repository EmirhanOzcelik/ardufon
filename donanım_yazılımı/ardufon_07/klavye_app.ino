int8_t sut = 0;                 // Dizi sütun gezdirici
int8_t sat = 0;                // Dizi satır gezdirici
int8_t secilen = sat*sut;     // Imleç kodu
int8_t kl_imlec = 0;         // OLED ekran yazı takipçi
int8_t kl_prg = 0;          // OLED ekran satır takipçi
const uint8_t max_alan = 2; //maksimum 10 satir yazdirilacak
bool klavye_uyari = false;

const byte x_pix = 6;           // Font1 pixel
const byte y_pix = 8;           // Font1 pixel
const uint8_t kl_satir = 3;     // Klavyeler için genel tanım
const uint8_t kl_sutun = 12;    // Klavyeler için genel tanım
bool klavye_harf = true;
bool klavye_buyuk = false;
uint8_t kl_mod = 0; // 0 = kücük harf , 1 = büyük harf , 2 = sayilar

uint8_t klavye_imlec_sayac = 0; //imlec yanıp sönme sayac
uint16_t klavye_imlec_ms = 800; //imlec yanıp sönme ms
bool imlec_anim_kontrolcu = false; //animasyon kontrolu
const String imlec_anim[] = {"|","!"}; //imlec animasyon

const String kl_kucuk[kl_satir][kl_sutun] = {  // bosluk = 14, buyult = 16, sayilar = 18, sil = 20, full sil = 22
  { "a", "b", "c", "d", "e", "f", "g", "h", "i", "i", "j", "k" },
  { "l", "m", "n", "o", "p", "r", "s", "t", "u", "v", "y", "z" },
  { "x", "w", "q", ".", ",", "!", "?", "bosluk", "buyult", "sayilar", "sil", "full sil" }
};

const String kl_buyuk[kl_satir][kl_sutun] = {  // bosluk = 14, kucult = 16, sayilar = 18, sil = 20, full sil = 22
  { "A", "B", "C", "D", "E", "F", "G", "H", "I", "I", "J", "K" },
  { "L", "M", "N", "O", "P", "R", "S", "T", "U", "V", "Y", "Z" },
  { "X", "W", "Q", ".", ",", "!", "?", "bosluk", "kucult", "sayilar", "sil", "full sil" }
};

const String kl_sayi[kl_satir][kl_sutun] = {  // bosluk = 14, kucult = 16, sayilar = 18, sil = 20, full sil = 22
  { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "*", "+" },
  { "=", "(", ")", "<", ">", ":", ";", "''", "'", "%", "/" },
  { ":>", ":()", ":)", ":(", ":|", ":#", "<3", "bosluk", "buy.-kucuk", "harfler", "sil", "full sil" }
};

void klavye2()
{
  if(!klavye_uyari){kl_sifirla();}
  while(klavye_izin)
  {
      sil();
     ekran.drawRect(sut*10+1, sat*10+34, 10, 10, WHITE);
     yaz("imlec ->",1,25);
    if(kl_mod == 2)
    {
      yaz(kl_sayi[sat][sut],54,25);
      yazB(1,34,klavye2,120,30); 
    }//klavye1 bitmap
    else
    {
      if(kl_mod == 0){yaz(kl_kucuk[sat][sut],54,25);}
      else if(kl_mod == 1){yaz(kl_buyuk[sat][sut],54,25);}
      yazB(1,34,klavye1,120,30); 
    }//klavye2 bitmap
    paragraf_yaz(klavye_verisi,max_alan);
    update();
    seri_buton();
    if (yuk == 1) {if (sat == 0) {sat = kl_satir - 1;} else {sat--;}
    } else if (asa == 1) {if (sat == kl_satir - 1) {sat = 0;} else {sat++;}
    } else if (sag == 1) {if (sut == kl_sutun) {sut = 0;} else {sut++;}
    } else if (sol == 1) {if (sut == 0) {sut = kl_sutun - 1;} else {sut--;}
    } else if (ok == 1) {ok_tus();}
      else if (yes == 1) {kl_bitir();}
      else if (no == 1) {sill();}
  }

}

void ok_tus()
{
  imlec_ayar();
  if(secilen == 14 || secilen == 16 || secilen == 18 || secilen == 20 || secilen == 22)
  {
    if(kl_mod == 0)//kucuk harf
     {
       if(secilen == 16){klavye_buyuk = !klavye_buyuk; kl_mod = (uint8_t)klavye_buyuk;}
      else if(secilen == 18){kl_mod = 2;}
     }
    else if(kl_mod == 1)//buyuk harf
     {
       if(secilen == 16){klavye_buyuk = !klavye_buyuk; kl_mod = (uint8_t)klavye_buyuk;}
      else if(secilen == 18){kl_mod = 2;}
     }
    else if(kl_mod == 2)//sayi isaret
     {
       if(secilen == 16){klavye_buyuk = !klavye_buyuk;}
      else if(secilen == 18){kl_mod = (uint8_t)klavye_buyuk;}
     }
      if(secilen == 14){klavye_verisi += " ";}
      else if(secilen == 20){sill();}
      else if(secilen == 22){full_sill();}
  }
  else{
  if(kl_mod == 0)//kucuk harf
  {
    klavye_verisi += kl_kucuk[sat][sut];
  }
  else if(kl_mod == 1)//buyuk harf
  {
    klavye_verisi += kl_buyuk[sat][sut];
  }
  else if(kl_mod == 2)//sayi isaret
  {
    klavye_verisi += kl_sayi[sat][sut];
  }
 }
}
void imlec_ayar()
{
  if(kl_imlec >= satir_max*x_pix)
  {
    if(kl_prg >= oled_max_satir){kl_uyari_ekran();}
    else{kl_prg++; yuk = 2;}
  }
  else
  {
    kl_imlec = kl_imlec+x_pix;
  }
}
void kl_uyari_ekran()
{
  klavye_izin = false;
  while(!klavye_izin && klavye_uyari)
  {
    sil();
    ekran.drawRect(0,30,63,34,WHITE);
    ekran.drawRect(64,30,64,34,WHITE);
    yaz("->",96,2);
    yaz("<-",32,42);
    yaz("maximum",ORTA,2);
    yaz("bellege",ORTA,12);
    yaz("eristiniz !!!",ORTA,22);
    if(sag == 1){kl_bitir(); klavye_uyari = false;}
    else if(sol == 1){klavye_izin = true; klavye_uyari = false;}
  }
}
void kl_bitir()
{
  kl_imlec = 0;
  kl_prg = 0;
  klavye_izin = false;
  klavye_uyari = false;
  kl_mod = 0;
  sat = 0;
  sut = 0;
  klavye_harf = true;
  klavye_buyuk = false;
  sil(); update();
  for(int u = 0; u<oled_max_satir;u++)
  {
    oled_satir[u] = "";
  }
}
void kl_sifirla()
{
  klavye_verisi = 0;
  klavye_uyari = false;
}
void sill()
{

}
void full_sill()
{

}