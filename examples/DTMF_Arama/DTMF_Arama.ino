
/*
*  NOT1:ArduGSM kartındaki SIM800C modulü harici adaptör girişinden beslenmektedir. 
*  Atmega328P, röleler ve buzzer ise hem adaptör girişinden hem de USB üzerinden çalışabilmektedir.

*  NOT2: ArduGSM kartınızda kodlama işlemi yaparken ilk olarak güç bağlantısını ardından
*  Type-C USB bağlantısını sağlayınız. Aksi halde Arduino çalışıp SIM800C enerjilenmeyeceği için
*  setup dosyası içerisindeki GSM başlatma komutları çalışmayacaktır. Ya da güç bağlantısı sonrası kartı resetleyiniz.
*
*/
#include <ArduGSM.h>
ArduGSM GSM(10, 11); //Arduino ile SIM800C haberleşme pinleri tanımlanıyor

#define telefonNo "+90xxxxxxxxxx" //Telefon numaranızı giriniz
int buzzer = 7;
int role1 = 8;
int role2 = 9;

void setup()
{
  pinMode(buzzer, OUTPUT);
  pinMode(role1, OUTPUT);
  pinMode(role2, OUTPUT);

  Serial.begin(9600);
  GSM.begin();          //Arduino ile GSM haberleşmesini başlat
  GSM.DTMFAktive();     //Arama özelliğini aktif yap.
  delay(5000);    		//GSM operatörüne bağlanmayı bekle
  GSM.sendSms(telefonNo, "ArduGSM DTMF Aktif");
  Serial.println("ArduGSM DTMF Aktif");
}

void loop()
{
  String veri = "";
  if (GSM.available())
  {
    while (GSM.available())
    {
      delay(13);
      char c = GSM.read();
      veri += c;
    }
    Serial.print("veri:");
    Serial.println(veri);
    if (veri.indexOf("DTMF: 1") != -1)
    {
      digitalWrite(role1, HIGH);
      Serial.println("ROLE1 AKTIF");
      GSM.sendSms(telefonNo, "ROLE1 AKTIF");
    }
    else if (veri.indexOf("DTMF: 3") != -1)
    {
      digitalWrite(role1, LOW);
      Serial.println("ROLE1 PASIF");
      GSM.sendSms(telefonNo, "ROLE1 PASIF");
    }
    else if (veri.indexOf("DTMF: 2") != -1)
    {
      digitalWrite(role2, HIGH);
      Serial.println("ROLE2 AKTIF");
      GSM.sendSms(telefonNo, "ROLE2 AKTIF");
    }
    else if (veri.indexOf("DTMF: 4") != -1)
    {
      digitalWrite(role2, LOW);
      Serial.println("ROLE2 PASIF");
      GSM.sendSms(telefonNo, "ROLE2 PASIF");
    }
  }
}
