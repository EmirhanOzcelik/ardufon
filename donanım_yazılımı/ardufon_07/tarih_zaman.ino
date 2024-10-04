void aktif_zaman() //millis degerine baglı zaman hesabı
{
  g_ms_sayac++;
  if(g_ms_sayac*g_ms>=1000)
  {
    g_ms_sayac = 0;
    zaman1[0]++;
    if(zaman1[0] >= 60)
    {
      zaman1[1]++; zaman1[0] = 0;
      if(zaman1[1]>=60)
      {
        zaman1[2]++; zaman1[1] = 0;
        if(zaman1[2]>=24)
        {
          zaman1[3]++; zaman1[2] = 0;
        }
      }
    }
  }
}
void panel_aktiflik()
{
yazF(zaman1[1],0,1,1);
yaz(F(":"),12,1,1);
yazF(zaman1[0],19,1,1);
yazB(92,0,pil_20x10,20,10);
}