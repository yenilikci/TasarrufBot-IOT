/* Muhammed Melih Çelik ~ G181210023
 * muhammed.celik7@ogr.sakarya.edu.tr
 * BSM313 - 2/B
 */

//Json kütüphanesi
#include <ArduinoJson.h>

//Zaman İçin
#include <NTPClient.h>
#include <WiFiUdp.h>

//CTBot kütüphanesi
#include "CTBot.h" 
CTBot myBot;

//Wifi ssid
String ssid = "BU_KISIM_GITHUB_ICIN_MEVCUT_DEGIL";   
//Wifi şifrem
String pass = "BU_KISIM_GITHUB_ICIN_MEVCUT_DEGIL"; 

//Telegram bot tokenim
String token = "BU_KISIM_GITHUB_ICIN_MEVCUT_DEGIL";

//Firebase host bilgisi
#define FIREBASE_HOST "BU_KISIM_GITHUB_ICIN_MEVCUT_DEGIL" 
//Firebase auth bilgisi
#define FIREBASE_AUTH "BU_KISIM_GITHUB_ICIN_MEVCUT_DEGIL"

//Firebase kütüphanesinin dahil edilmesi
#include "FirebaseESP8266.h"

// Zaman işlemleri için kütüphanem
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//FirebaseESP8266 veri objem
FirebaseData firebaseData;
FirebaseJson json;
void printResult(FirebaseData &data);

//Odaları temsil eden pinler
uint8_t oda1 = D0;
uint8_t oda2 = D2;
uint8_t oda3 = D4;
uint8_t oda4 = D6;            


//Odaların dakika bilgileri için
int oda1AcilmaDakika = 0;
int oda1KapanmaDakika = 0;
int oda2AcilmaDakika = 0;
int oda2KapanmaDakika = 0;
int oda3AcilmaDakika = 0;
int oda3KapanmaDakika = 0;
int oda4AcilmaDakika = 0;
int oda4KapanmaDakika = 0;
  
//Odaların saniye bilgileri için
int oda1AcilmaSaniye = 0;
int oda1KapanmaSaniye = 0;
int oda2AcilmaSaniye = 0;
int oda2KapanmaSaniye = 0;
int oda3AcilmaSaniye = 0;
int oda3KapanmaSaniye = 0;
int oda4AcilmaSaniye = 0;
int oda4KapanmaSaniye = 0;

//Dakika saniye fark bilgileri için
int oda1SaniyeFark = 0;
int oda1DakikaFark = 0;
int oda2SaniyeFark = 0;
int oda2DakikaFark = 0;
int oda3SaniyeFark = 0;
int oda3DakikaFark = 0;
int oda4SaniyeFark = 0;
int oda4DakikaFark = 0;
  

void setup() {
  
  // Serial ayarı
  Serial.begin(115200);
  Serial.println("TasarrufBot Başlatılıyor...");

  // Bot ile ilgili tanımlamalar
  myBot.wifiConnect(ssid, pass);
  myBot.setTelegramToken(token);
  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");

  // Odaların bağlantıları
  pinMode(oda1, OUTPUT);
  pinMode(oda2, OUTPUT);
  pinMode(oda3, OUTPUT);
  pinMode(oda4, OUTPUT);

  //Odaların ışıkları otomatik kapalı başlatılıyor
  digitalWrite(oda1, LOW); 
  digitalWrite(oda2, LOW); 
  digitalWrite(oda3, LOW); 
  digitalWrite(oda4, LOW); 

  // Varsayılan firebase ayarları
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  Firebase.setFloatDigits(2);
  Firebase.setDoubleDigits(6);

  // Zaman bilgileri almak için zaman kütüphanesinin başlatılması
    timeClient.begin();
    timeClient.setTimeOffset(0);


}

void loop() {
  // Telegram mesaj objesi
  TBMessage msg;
  timeClient.update();
  
  // Eğer telegrama bir mesaj(komut) gelirse
  if (myBot.getNewMessage(msg)) {

    //Eğer "ODA1 IŞIK AÇ" komutu gelirse 
    if (msg.text.equalsIgnoreCase("ODA1 IŞIK AÇ"))
    {   
        //Zaman atamaları
        oda1AcilmaSaniye = timeClient.getSeconds();
        oda1AcilmaDakika = timeClient.getMinutes();
      
        //Eğer "ODA1 IŞIK AÇ" mesajı gelirse oda1'e bağlı ışık açılır.
        digitalWrite(oda1, HIGH);

        //Firebase oda1 durumunun yazımı
        String oda1path = "/oda1";
        Firebase.setString(firebaseData,oda1path,"oda1 ışık açık");
        
        //Işık açıldıktan sonra telegramda durum bildirilir.
        myBot.sendMessage(msg.sender.id, "BİLDİRİM: ODA1 IŞIĞI AÇILDI!"); 
    }
    //Eğer "ODA1 IŞIK KAPAT" komutu gelirse
    else if (msg.text.equalsIgnoreCase("ODA1 IŞIK KAPAT")) 
    { 
        //Zaman atamaları
        oda1KapanmaSaniye = timeClient.getSeconds();
        oda1KapanmaDakika = timeClient.getMinutes();

        //Eğer oda1KapanmaSaniye > oda1AcilmaSaniye ise fark için çıkarma işlemi yapılır
        if(oda1KapanmaSaniye > oda1AcilmaSaniye)
        {
          oda1SaniyeFark = oda1KapanmaSaniye - oda1AcilmaSaniye;
        }
        //eğer büyük değil ise çıkarma işlemi yapılıp değerin - 'lisi alınır
        else if(oda1KapanmaSaniye < oda1AcilmaSaniye)
        {
          oda1SaniyeFark = -(oda1KapanmaSaniye-oda1AcilmaSaniye);  
        }

        //eğer oda1KapanmaDakika > oda1AcilmaDakika ise fark için çıkarma işlemi yapılır
        if(oda1KapanmaDakika > oda1AcilmaDakika)
        {
          oda1DakikaFark = oda1KapanmaDakika - oda1AcilmaDakika;
        }
        //eğer büyük değil ise çıkarma işlemi yapılıp değerin -'lisi alınır
        else if(oda1KapanmaDakika > oda1AcilmaDakika)
        {
          oda1DakikaFark = -(oda1KapanmaDakika-oda1AcilmaDakika);
        }
      
        //Eğer "ODA1 IŞIK KAPAT" mesajı gelirse oda1'e bağlı ışık kapatılır.
        digitalWrite(oda1, LOW);
        
        //Firebase oda1 durumunun , açık kalma dakikasının ve saniyesinin yazımı
        String oda1path = "/oda1";
        Firebase.setString(firebaseData,oda1path,"oda1 ışık kapalı");
        String oda1dakika = "/oda1dakika";
        Firebase.setInt(firebaseData,oda1dakika,oda1DakikaFark);
        String oda1saniye = "/oda1saniye";
        Firebase.setInt(firebaseData,oda1dakika,oda1SaniyeFark);
        
        //Işık kapandıktan sonra telegramda durum bildirilir
        myBot.sendMessage(msg.sender.id, "BİLDİRİM: ODA1 IŞIĞI KAPANDI!"); 
        myBot.sendMessage(msg.sender.id, "Açık Kalma Süresi (Dakika): " + String(oda1DakikaFark));
        myBot.sendMessage(msg.sender.id, "Açık Kalma Süresi (Saniye): " + String(oda1SaniyeFark));
        
    }
    //Eğer "ODA2 IŞIK AÇ" komutu gelirse
        if (msg.text.equalsIgnoreCase("ODA2 IŞIK AÇ"))
    {   
         //Zaman atamaları
        oda2AcilmaSaniye = timeClient.getSeconds();
        oda2AcilmaDakika = timeClient.getMinutes();
      
        //Eğer "ODA2 IŞIK AÇ" mesajı gelirse oda2'e bağlı ışık açılır.
        digitalWrite(oda2, HIGH);

        //Firebase oda2 durumunun yazımı
        String oda2path = "/oda2";
        Firebase.setString(firebaseData,oda2path,"oda2 ışık açık");
        
        //Işık açıldıktan sonra telegramda durum bildirilir.
        myBot.sendMessage(msg.sender.id, "BİLDİRİM: ODA2 IŞIĞI AÇILDI!"); 
    }
    //Eğer "ODA2 IŞIK KAPAT" komutu gelirse
    else if (msg.text.equalsIgnoreCase("ODA2 IŞIK KAPAT")) 
    { 
        //Zaman atamaları
        oda2KapanmaSaniye = timeClient.getSeconds();
        oda2KapanmaDakika = timeClient.getMinutes();

        //Eğer oda2KapanmaSaniye > oda2AcilmaSaniye ise fark için çıkarma işlemi yapılır
        if(oda2KapanmaSaniye > oda2AcilmaSaniye)
        {
          oda2SaniyeFark = oda2KapanmaSaniye - oda2AcilmaSaniye;
        }
        //eğer büyük değil ise çıkarma işlemi yapılıp değerin - 'lisi alınır
        else if(oda2KapanmaSaniye < oda2AcilmaSaniye)
        {
          oda2SaniyeFark = -(oda2KapanmaSaniye-oda2AcilmaSaniye);  
        }

        //eğer oda2KapanmaDakika > oda2AcilmaDakika ise fark için çıkarma işlemi yapılır
        if(oda2KapanmaDakika > oda2AcilmaDakika)
        {
          oda2DakikaFark = oda2KapanmaDakika - oda2AcilmaDakika;
        }
        //eğer büyük değil ise çıkarma işlemi yapılıp değerin -'lisi alınır
        else if(oda2KapanmaDakika > oda2AcilmaDakika)
        {
          oda2DakikaFark = -(oda2KapanmaDakika-oda2AcilmaDakika);
        }
      
        //Eğer "ODA2 IŞIK KAPAT" mesajı gelirse oda2'e bağlı ışık kapatılır.
        digitalWrite(oda2, LOW);
        
        //Firebase oda2 durumunun , açık kalma dakikasının ve saniyesinin yazımı
        String oda2path = "/oda2";
        Firebase.setString(firebaseData,oda2path,"oda2 ışık kapalı");
        String oda2dakika = "/oda2dakika";
        Firebase.setInt(firebaseData,oda2dakika,oda2DakikaFark);
        String oda2saniye = "/oda2saniye";
        Firebase.setInt(firebaseData,oda2dakika,oda2SaniyeFark);
        
        //Işık kapandıktan sonra telegramda durum bildirilir
        myBot.sendMessage(msg.sender.id, "BİLDİRİM: ODA2 IŞIĞI KAPANDI!"); 
        myBot.sendMessage(msg.sender.id, "Açık Kalma Süresi (Dakika): " + String(oda2DakikaFark));
        myBot.sendMessage(msg.sender.id, "Açık Kalma Süresi (Saniye): " + String(oda2SaniyeFark));
    }
    //Eğer "ODA3 IŞIK AÇ" komutu gelirse
    if (msg.text.equalsIgnoreCase("ODA3 IŞIK AÇ"))
    {   
        //Zaman atamaları
        oda3AcilmaSaniye = timeClient.getSeconds();
        oda3AcilmaDakika = timeClient.getMinutes();
      
        //Eğer "ODA3 IŞIK AÇ" mesajı gelirse oda3'e bağlı ışık açılır.
        digitalWrite(oda3, HIGH);

        //Firebase oda3 durumunun yazımı
        String oda3path = "/oda3";
        Firebase.setString(firebaseData,oda3path,"oda3 ışık açık");
        
        //Işık açıldıktan sonra telegramda durum bildirilir.
        myBot.sendMessage(msg.sender.id, "BİLDİRİM: ODA3 IŞIĞI AÇILDI!"); 
    }
    //Eğer "ODA3 IŞIK KAPAT" komutu gelirse
    else if (msg.text.equalsIgnoreCase("ODA3 IŞIK KAPAT")) 
    { 
        //Zaman atamaları
        oda3KapanmaSaniye = timeClient.getSeconds();
        oda3KapanmaDakika = timeClient.getMinutes();

        //Eğer oda3KapanmaSaniye > oda3AcilmaSaniye ise fark için çıkarma işlemi yapılır
        if(oda3KapanmaSaniye > oda3AcilmaSaniye)
        {
          oda3SaniyeFark = oda3KapanmaSaniye - oda3AcilmaSaniye;
        }
        //eğer büyük değil ise çıkarma işlemi yapılıp değerin - 'lisi alınır
        else if(oda2KapanmaSaniye < oda2AcilmaSaniye)
        {
          oda3SaniyeFark = -(oda3KapanmaSaniye-oda3AcilmaSaniye);  
        }

        //eğer oda3KapanmaDakika > oda3AcilmaDakika ise fark için çıkarma işlemi yapılır
        if(oda3KapanmaDakika > oda3AcilmaDakika)
        {
          oda3DakikaFark = oda3KapanmaDakika - oda3AcilmaDakika;
        }
        //eğer büyük değil ise çıkarma işlemi yapılıp değerin -'lisi alınır
        else if(oda3KapanmaDakika > oda3AcilmaDakika)
        {
          oda3DakikaFark = -(oda3KapanmaDakika-oda3AcilmaDakika);
        }
      
        //Eğer "ODA3 IŞIK KAPAT" mesajı gelirse oda3'e bağlı ışık kapatılır.
        digitalWrite(oda3, LOW);
        
        //Firebase oda3 durumunun , açık kalma dakikasının ve saniyesinin yazımı
        String oda3path = "/oda3";
        Firebase.setString(firebaseData,oda3path,"oda3 ışık kapalı");
        String oda3dakika = "/oda3dakika";
        Firebase.setInt(firebaseData,oda3dakika,oda3DakikaFark);
        String oda3saniye = "/oda3saniye";
        Firebase.setInt(firebaseData,oda3dakika,oda3SaniyeFark);
        
        //Işık kapandıktan sonra telegramda durum bildirilir
        myBot.sendMessage(msg.sender.id, "BİLDİRİM: ODA3 IŞIĞI KAPANDI!"); 
        myBot.sendMessage(msg.sender.id, "Açık Kalma Süresi (Dakika): " + String(oda3DakikaFark));
        myBot.sendMessage(msg.sender.id, "Açık Kalma Süresi (Saniye): " + String(oda3SaniyeFark));
    }
    //Eğer "ODA4 IŞIK AÇ" komutu gelirse
       if (msg.text.equalsIgnoreCase("ODA4 IŞIK AÇ"))
    {   
        //Zaman atamaları
        oda3AcilmaSaniye = timeClient.getSeconds();
        oda3AcilmaDakika = timeClient.getMinutes();
      
        //Eğer "ODA4 IŞIK AÇ" mesajı gelirse oda4'e bağlı ışık açılır.
        digitalWrite(oda4, HIGH);

        //Firebase oda4 durumunun yazımı
        String oda4path = "/oda4";
        Firebase.setString(firebaseData,oda4path,"oda4 ışık açık");
        
        //Işık açıldıktan sonra telegramda durum bildirilir.
        myBot.sendMessage(msg.sender.id, "BİLDİRİM: ODA4 IŞIĞI AÇILDI!"); 
    }
    else if (msg.text.equalsIgnoreCase("ODA4 IŞIK KAPAT")) 
    { 
       //Zaman atamaları
        oda4KapanmaSaniye = timeClient.getSeconds();
        oda4KapanmaDakika = timeClient.getMinutes();

        //Eğer oda4KapanmaSaniye > oda4AcilmaSaniye ise fark için çıkarma işlemi yapılır
        if(oda4KapanmaSaniye > oda4AcilmaSaniye)
        {
          oda4SaniyeFark = oda4KapanmaSaniye - oda4AcilmaSaniye;
        }
        //eğer büyük değil ise çıkarma işlemi yapılıp değerin - 'lisi alınır
        else if(oda4KapanmaSaniye < oda4AcilmaSaniye)
        {
          oda4SaniyeFark = -(oda4KapanmaSaniye-oda4AcilmaSaniye);  
        }

        //eğer oda4KapanmaDakika > oda4AcilmaDakika ise fark için çıkarma işlemi yapılır
        if(oda4KapanmaDakika > oda4AcilmaDakika)
        {
          oda4DakikaFark = oda4KapanmaDakika - oda4AcilmaDakika;
        }
        //eğer büyük değil ise çıkarma işlemi yapılıp değerin -'lisi alınır
        else if(oda4KapanmaDakika > oda4AcilmaDakika)
        {
          oda4DakikaFark = -(oda4KapanmaDakika-oda4AcilmaDakika);
        }
      
        //Eğer "ODA4 IŞIK KAPAT" mesajı gelirse oda4'e bağlı ışık kapatılır.
        digitalWrite(oda4, LOW);
        
        //Firebase oda4 durumunun , açık kalma dakikasının ve saniyesinin yazımı
        String oda4path = "/oda4";
        Firebase.setString(firebaseData,oda4path,"oda3 ışık kapalı");
        String oda4dakika = "/oda4dakika";
        Firebase.setInt(firebaseData,oda4dakika,oda4DakikaFark);
        String oda4saniye = "/oda4saniye";
        Firebase.setInt(firebaseData,oda4dakika,oda4SaniyeFark);
        
        //Işık kapandıktan sonra telegramda durum bildirilir
        myBot.sendMessage(msg.sender.id, "BİLDİRİM: ODA4 IŞIĞI KAPANDI!"); 
        myBot.sendMessage(msg.sender.id, "Açık Kalma Süresi (Dakika): " + String(oda4DakikaFark));
        myBot.sendMessage(msg.sender.id, "Açık Kalma Süresi (Saniye): " + String(oda4SaniyeFark));
    }
    //Eğer bu komutlardan herhangi birisi gelmez ise
    else {                                                    
      String reply;
      reply = (String)"Sayın, " + msg.sender.username + (String)"! 1'den 4'e kadar odaların ışığını kontrol edebilirsin. ";
      myBot.sendMessage(msg.sender.id, reply);            
    }
  }
  // 100 milisaniye bekle
  delay(100);
}
