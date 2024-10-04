void ekran_kur() {
if (!ekran.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  Serial.println(F("SSD1306 ekran bulunamadı"));}
}  //ekranı kur
void update() {
  ekran.display();
}  // Ekranı güncelle
void sil() {
  ekran.clearDisplay();
}  // Ekranı temizle
void yaz(String a, uint8_t x, uint8_t y, uint8_t boyut)  //string yaz
{
  ekran.setTextSize(boyut);
  ekran.setTextColor(SSD1306_WHITE);
  if(x == ORTA){uint8_t xs = a.length(); ekran.setCursor((128-(xs+1)*6)/2, y);}
  else{ekran.setCursor(x, y);}
  ekran.print(a);
}
void yaz(String a, uint16_t x, uint8_t y)  //string yaz
{
  ekran.setTextSize(1);
  ekran.setTextColor(SSD1306_WHITE);
  if(x == ORTA){uint16_t xs = a.length(); ekran.setCursor((128-(xs+1)*6)/2, y);}
  else{ekran.setCursor(x, y);}
  ekran.print(a);
}
void yazF(uint16_t a, uint8_t x, uint8_t y, uint8_t boyut2)  //sayı yaz
{
  ekran.setTextSize(boyut2);
  ekran.setTextColor(SSD1306_WHITE);
  ekran.setCursor(x, y);
  ekran.print(a);
}
void yazB(uint8_t x, uint8_t y, const uint8_t c[], uint8_t bx, uint8_t by)  //bitmap yaz
{
  ekran.drawBitmap(x, y, c, bx, by, WHITE);
}
void yazE(const uint8_t ec[], uint8_t ex, uint8_t ey)  //emoji yaz
{
  ekran.drawBitmap(ex, ey, ec, 6, 8, WHITE);
}
void paragraf_yaz(String sveri,uint8_t kapsam)// paragraf yaz
{
  if(kapsam <= 5)
  {
  uint16_t boyut = sveri.length();
  uint8_t satir; //satir sayisi
  if(boyut%satir_max > 0){satir = boyut/satir_max + 1;}else{satir = boyut/satir_max;}
  const uint8_t kaydirici_sutun = 122;
  uint8_t kaydirici_alan = kapsam*8;
  uint8_t kaydirici_bolme = kaydirici_alan/satir;
  for(int y=0; y<oled_max_satir;y++)//veri dizelere bolundu
  {
    if(boyut >= satir_max*y && boyut < satir_max*(y+1))
    {oled_satir[y] = sveri.substring(satir_max*y,satir_max*(y+1));}
  }
  if(satir > kapsam)
  {
    if(yuk == 2){if(kaydirici == satir){kaydirici=0;}else{kaydirici++;}}
    else if(asa == 2){if(kaydirici == 0){kaydirici=satir;}else{kaydirici--;}}
    for(uint8_t h=0; h<kapsam;h++)
    {
      yaz(oled_satir[h+kaydirici],0,h*8);
    }  
    yaz("#",kaydirici_sutun,kaydirici*kaydirici_bolme);
  }
  else
  {
    for(int v=0; v<satir; v++)
    {
      yaz(oled_satir[v],0,v*8);
    }
  }
  }
  else{Serial.println("kapsam 5 i asti");while(1);}
}
void paragraf_yaz_sifirla()
{
  kaydirici = 0;
  sil();
  update();
}