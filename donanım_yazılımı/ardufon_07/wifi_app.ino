char kayitli_aglar[] = {"TurkTelekom_ZJP7M"};
char sifreler[] = {"4Ff5e364718FF"};

int8_t wifi_guc() {
  if (WiFi.status() != WL_CONNECTED) {
    return 0;
  } else {
    return WiFi.RSSI();
  }
}
void panel_wifi()
{

}
void internete_baglan()
{
  
}
int8_t ag_sayi()
{
  uint8_t n = WiFi.scanNetworks();
  if(n>0){return n;}else{return -1;}
}
void aglari_grupla()
{
  

}