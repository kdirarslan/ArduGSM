/* 
 *        _____________________________
 *       | ARDUINO UNO  ---   SIM800C |
 *       | ---------------------------|
 *       |   (RX) 10    ---   TX      |
 *       |   (TX) 11    ---   RX	  |
 *       |    GND       ---   GND     |
 *		 |____________________________|
*/
#ifndef ArduGSM_h
#define ArduGSM_h
#include <SoftwareSerial.h>
#include "Arduino.h"


class ArduGSM : public SoftwareSerial	
{									
  private:
	int _timeout;
	String _buffer;
	String _readSerial();
	uint32_t _baud;
  	
  public:
	uint8_t	RX_PIN;
    uint8_t TX_PIN;
	ArduGSM(uint8_t rx, uint8_t tx);
	void begin();
 	void begin(uint32_t);	
	void DTMFAktive();
 	void reset(); 
	
 	// GSM Arama metotları
 	bool answerCall(); 	
 	void callNumber(char* number);
 	bool hangoffCall();
 	uint8_t getCallStatus();   
 	//GSM SMS Metotları
	bool sendSms(char* number,char* text);	 
	String readSms(uint8_t index); //sms içeriğini döndürür
	String getNumberSms(uint8_t index); //sms gönderen numarayı döndürür
	bool delAllSms();     // tüm mesajlar silinir

	void signalQuality();
	void setPhoneFunctionality();
	void activateBearerProfile();
	void deactivateBearerProfile();
	//zaman verilerini getirir
	void RTCtime(int *day,int *month, int *year,int *hour,int *minute, int *second);  
	String dateNet(); //Ağ'daki tarih ve zaman bilgisini getirir.
	bool updateRtc(int utc);  //RTC güncelleme yapar.
};

#endif 