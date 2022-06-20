## ArduGSM - Arduino GSM Kontrol Karti
```
Version:    v1.0
Tarih  :    06.06.2022
```
## ArduGSM Nedir?
ArduGSM, Arduino UNO, SIM800C, buzzer ve iki adet röle barındıran tümleşik bir kontrol kartıdır. 
Dahili 3A regülatör ile kartın gerekli güç bağlantısı sağlanır, Type-C USB konnektörü üzerinden Arduino kolayca programlanabilir.

*Kart hakkında daha detaylı bilgi ve kolay kullanım için 5 dakika ayırarak okumaya devam ediniz.*  

**Ön görünüm**

![ArduGSM2](https://user-images.githubusercontent.com/58399702/174570445-8dfc70df-92fc-401e-99d6-86f883eb7e69.jpg)

## Kart Özellikleri

### Donanım Özellikleri
- Arduino Uno (Atmega328P)
- SIM800C
- 2 Röle
- 1 Buzzer
- Mikro SIM kart konnektörü
- Dahili mikrofon
- Hoparlör bağlantı konnektörü
- 3dBi 90C SMA Anten
- Dahili 3A voltaj regülatörü
- SIM kart konnektöründe TVS koruması
- Besleme hattında ters polarite koruması

### Tasarım ve Kutu Özellikleri
- Kart 97mmx85mm (EnxBoy) boyutlarında özel kutuya yerleşecek şekilde tasarlanmıştır.
- Kutu üzerinde hem pano hem de duvar montaj aparatı mevcuttur.


**Soldaki alt kapak: opsiyonel olarak pano rayına ve duvara montaj kulakçıkları mevcuttur**

![ArduGSM Kutu](https://user-images.githubusercontent.com/58399702/171063530-c1530e02-cee1-432a-b798-4fea779e2fbf.jpg) 


**Kutu - Montajlı**

![Kutu kutu montajlı](https://user-images.githubusercontent.com/58399702/171063774-f5ca42cf-f03c-4323-a74d-35c914cc97f1.jpg)
------------------------

## Pin Bağlantıları

### SIM800C
Arduino UNO  |   SIM800C      
-------------|------------
10  	 |     TX	    
11  	 |     RX	   


### Röle ve Buzzer
Arduino UNO  |   Modül      
-------------|---------
7  	 |     Buzzer
8  	 |     Röle 1	    
9  	 |     Röle 2	

Atmega328P üzerindeki diğer tüm pinler geliştirme kartı üzerinde mevcuttur.

------------------------
## Nasıl Kullanılır
Kart üzerindeki Atmega328P, SIM800C, röleler ve buzzer için 12V 1A adaptör kullanmak yeterlidir. 

Şebekeye bağlanma, arama ve SMS gönderme durumlardında yüksek akım çeken SIM800C modulünün güç gereksinimi karşılamak için güçlü bir besleme hattı oluşturulmuştur. Bu besleme hattı sayesinde, harici pinler aracılığıyla bağlanabilecek sensör vb. donanımlar rahatlıkla kullanılabilir
 
SIM800C modulü sadece dahili regülatör üzerinden beslenmektedir. Kart adaptör girişinden beslendiğinde tüm bileşenler  regülatör üzerinden enerjilenmektedir.

--------------------------
--------------------------
### Güç bağlantı seçenekleri

- USB bağlantısı
```
Tüm çevre birimleri enerjilenir, Atmega328P programlanabilir.
USB'nin sağlayacağından daha fazla akıma ihtiyaç duyan SIM800C enerjilenmez!!!
```
- Voltaj regülatör bağlantısı
```
SIM800C ve tüm çevre birimleri enerjilenir.
```
- USB ve voltaj regülatörü (Her iki giriş enerjilendiğinde)
```
SIM800C ve  tüm çevre birimleri enerjilenir. USB üzerinden kart sorunsuzca programlanabilir.
Kart voltaj kaynağı seçme devresi ile otomatik olarak voltaj regülatörü üzerinden enerjilenir. 
```

**Besleme için 12V 1A adaptör kullanınız!!!**

--------------------------
--------------------------

## Kütüphane kullanımı
Kütüphaneyi yükledikten sonra Dosya -> Örnekler -> ArduGSM  uzantısı üzerindeki örnek uygulamalar ile kolaylıkla test yapabilirsiniz.

**ArduGSM kütüphanesi Arduino UNO tabanlı, ArduGSM kontrol kartı için düzenlenmiş bir kütüphanedir. Bununla birlikte SIM800C, SIM800L serisi SIM modulleri için de sorunsuzca kullanılabilir.**


**Mesaj ile Röle Kontrol Örneği**
```c++
#include <ArduGSM.h>
ArduGSM GSM(10, 11); //SIM800C pinleri tanımlanıyor

String gelenMesaj;  
String numara;       //Mesajı gönderen numara

#define telefonNo "+90xxxxxxxxxx"

int buzzer = 7;
int role1 = 8;
int role2 = 9;

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
}

void loop()
{
  gelenMesaj = GSM.readSms(1);     // ilk mesajı oku

  if (gelenMesaj.indexOf("OK") != -1) // Mesaj var mı?
  {
    if (gelenMesaj.length() > 7)   // Gelen sms boş değil ise
    {
      numara = GSM.getNumberSms(1);// Mesaj gönderenin numarasını al
      Serial.println(numara);   
      gelenMesaj.toUpperCase();    // mesajı BÜYÜK harflere çevir

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
      GSM.delAllSms();  //her zaman ilk mesaj okunduğu için, gerekli işlemlerden sonra tüm mesajları sil
    }
  }
}
```


**Arama Örneği**
```c++
#include <ArduGSM.h>
ArduGSM GSM(10, 11); //Arduino ile SIM800C haberleşme pinleri tanımlanıyor

#define telefonNo "+90xxxxxxxxxx"
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
  delay(5000);    		  //GSM operatörüne bağlanmayı bekle
  GSM.DTMFAktive();	  	//Arama özelliğini aktif yap.	
  GSM.sendSms(telefonNo, "ArduGSM DTMF Aktif");
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
```
## Fonksiyonlar
ArduGSM kütüphanesi içerisindeki fonksiyonlar ve bu fonksiyonların işlevleri aşağıda gösterilmektedir. 
Metot Adı|Return|Açıklama
:----------------------|:---------------:|:-----------------------------------------------:|
begin()       	       |None     		 |  GSM aktif olur. (Varsayılan baudrate 9600)
begin(baudrate)		   |None			 |  GSM aktif olur. Parametre olarak baudrate alır.
DTMFAktive()		   |None			 |  Arama özelliği (DTMF) aktif olur. (Tüm mesajlar silinir)
sendSms(numara,mesaj)  |true veya false  |	SMS gönderir. Parametreler String olmalıdır.
readSms(index)		   |String		     |  SMS oku
getNumberSms(index)	   |String  		 |  Mesaj gönderenin numarasını al
delAllSms()			   |true veya false  |  Tüm mesajları sil
answerCall()		   |true veya false  |  Arama cevaplama
callNumber(numara)	   |None     		 |  Parametre olarak verilen numarayı ara
hangoffCall()		   |true veya false  |  Arama sonlandır
getCallStatus()		   |uint8_t		     |  Arama durumunu gösterir, 0=hazır, 2=Bilinmiyor(),3=Çalıyor,4=Aramada
setPhoneFunctionality()|None		     |  Tüm Fonksiyonlar ayarlanır 
RTCtime(int *day,int *month, int *year,int *hour,int *minute, int *second)|None| Parametreler & referansı ile verlmelidir
dateNet()|String|GSM tarih ve zaman bilgisi döndürür
updateRtc(utc)|true or false|
activateBearerProfile()|None			 |
deactivateBearerProfile()|None			 |
____________________________________________________________________________________


*NOT: Testleri gerçekleştirilen ve herhangi bir sorun yaşanmayan geliştirme kartını kullanırken kütüphane veya donanım kaynaklı bir bir sorun yaşadığınızda bu sorunu bize bildirirseniz gelişmemize katkı sağlamış olursunuz.*

İyi çalışmalar.
