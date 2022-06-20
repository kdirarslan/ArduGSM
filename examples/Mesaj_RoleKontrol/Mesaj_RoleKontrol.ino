#include <ArduGSM.h>
ArduGSM GSM(10, 11); //SIM800C pinleri tanımlanıyor

String gelenMesaj;  
String numara;       //Mesajı gönderen numara

#define telefonNo "+905468522225"

int buzzer = 7;
int role1 = 8;
int role2 =9;

void setup()
{
  pinMode(buzzer, OUTPUT);
  pinMode(role1, OUTPUT);
  pinMode(role2, OUTPUT);

  Serial.begin(9600);
  GSM.begin();     //Arduino ile GSM haberleşmesini başlat
  delay(5000);     //GSM operatörüne bağlanmayı bekle
  GSM.delAllSms(); //Tüm mesajları sil
  GSM.sendSms(telefonNo, "ArduGSM Mesaj Mod Aktif");
  Serial.println("ArduGSM Mesaj Mod Aktif");
}

void loop()
{
  gelenMesaj = GSM.readSms(1); // ilk mesajı oku

  if (gelenMesaj.indexOf("OK") != -1) // Mesaj var mı?
  {
    if (gelenMesaj.length() > 7) // Gelen sms boş değil ise
    {
      numara = GSM.getNumberSms(1);// Mesaj gönderenin numarasını al
      Serial.println(numara);   
      gelenMesaj.toUpperCase(); // mesajı BÜYÜK harflere çevir

      if (gelenMesaj.indexOf("ROLE1AKTIF") != -1)
      {
        digitalWrite(role1, HIGH);
        Serial.println("ROLE1 AKTIF");
        GSM.sendSms(telefonNo, "ROLE1 AKTIF OLDU");
      }
      else if (gelenMesaj.indexOf("ROLE1PASIF") != -1)
      {
        digitalWrite(role1, LOW);
        Serial.println("ROLE1 PASIF");
        GSM.sendSms(telefonNo, "ROLE1 PASIF OLDU");
      }
      else if (gelenMesaj.indexOf("ROLE2AKTIF") != -1)
      {
        digitalWrite(role2, HIGH);
        Serial.println("ROLE2 AKTIF");
        GSM.sendSms(telefonNo, "ROLE2 AKTIF OLDU");
      }
      else if (gelenMesaj.indexOf("ROLE2PASIF") != -1)
      {
        digitalWrite(role2, LOW);
        Serial.println("ROLE2 PASIF");
        GSM.sendSms(telefonNo, "ROLE2 PASIF OLDU");
      }
      else
      {
        Serial.println("Gecersiz Mesaj");
      }
      GSM.delAllSms(); // her zaman ilk mesaj okunduğu için, gerekli işlemlerden sonra tüm mesajları sil
    }
  }
}
