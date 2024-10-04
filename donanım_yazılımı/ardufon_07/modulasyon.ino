void devre_buton()
{

}
void seri_buton()
{
  if(Serial.available())
  {
    bt_sill();
    char veri = Serial.read();
    if(but_sayac < 1)
    {
    //if(*veri == 'CW'){ctr = 1; but_sayac++;}
     if(veri=='W'){yuk = 1; but_sayac++;}
    else if(veri=='S'){ asa = 1;  but_sayac++;}
    else if(veri=='D'){ sag = 1;  but_sayac++;}
    else if(veri=='A'){ sol = 1; but_sayac++;}
    else if(veri=='Y'){ yes = 1; but_sayac++;}
    else if(veri=='N'){ no = 1;  but_sayac++;}
    else if(veri=='O'){ ok = 1; but_sayac++;}
    }
  }
  
}
void bt_sill(){yuk = 0; asa = 0; sag = 0; sol = 0; yes = 0; no = 0; ok = 0; ctr = 1; but_sayac=0;}
void pil_olcum()
{
  int adc = analogRead(A0);  // A0 pininden ADC değerini oku

  // Voltajı hesapla (referans voltajı 1V kabul ediliyor)
  float voltaj = adc * (0.336 / 105.0)*11;

  // Voltajı stringe çevir
  pil_voltaj = String(voltaj, 3);  // 3 ondalık basamakla stringe çevir
  pil_deger = map(voltaj,0,4.200,0,100);

}
