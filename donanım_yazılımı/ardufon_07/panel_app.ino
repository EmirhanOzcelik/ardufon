void panel_app()
{
sil();
  if(wifi_baglantisi)
  {
    int16_t wifi_dbm = wifi_guc(); 
    if(wifi_dbm == 0){yaz("?",85,1,1);}//wifi baglanmadı
    else if(wifi_dbm < 0 && wifi_dbm > -40){yazB(80,0,dusuk_10x10,10,10);}//düsük lvl
    else if(wifi_dbm < -40 && wifi_dbm > -65){yazB(80,0,orta_10x10,10,10);}//orta lvl
    else if(wifi_dbm < -65 && wifi_dbm > -100){yazB(80,0,full_10x10,10,10);}//max lvl
  }
  else{yazB(80,0,yok_10x10,10,10);}
panel_aktiflik();
seri_buton();
update();
}
