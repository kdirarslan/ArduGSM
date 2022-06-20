#include "Arduino.h"
#include "ArduGSM.h"
#include <SoftwareSerial.h>

//SoftwareSerial SIM(RX_PIN,TX_PIN);
//String _buffer;

ArduGSM::ArduGSM(uint8_t rx, uint8_t tx) : SoftwareSerial(rx, tx)
{
    RX_PIN 		= rx;
    TX_PIN 		= tx;
}

void ArduGSM::begin(){
  delay(2000);
    this->SoftwareSerial::begin(9600);
  _buffer.reserve(255); //reserve memory to prevent intern fragmention
  delay(1000);
}

void ArduGSM::begin(uint32_t baud){
  _baud = baud;
    this->SoftwareSerial::begin(_baud);
  _buffer.reserve(255); //reserve memory to prevent intern fragmention
  delay(1000);
}

void ArduGSM::DTMFAktive(){
  delay(1500);
  delAllSms();
  delay(1500);
  this->SoftwareSerial::print(F("ATS0=1\r\n"));
  delay(1500);
  this->SoftwareSerial::print(F("AT+DDET=1\r\n"));
  delay(1500);
  //Serial.println("Arama Modu(DTMF) Aktif");
}

//
//PRIVATE METHODS
//
String ArduGSM::_readSerial(){
  _timeout=0;
  while  (!this->SoftwareSerial::available() && _timeout < 12000  ) 
  {
    delay(13);
    _timeout++;
  }
  if (this->SoftwareSerial::available()) {
 	return this->SoftwareSerial::readString();
  }
}


//
//PUBLIC METHODS
//

void ArduGSM::reset(){

  this->SoftwareSerial::print(F("AT\r\n"));
  while (_readSerial().indexOf("OK")==-1 ){
    this->SoftwareSerial::print(F("AT\r\n"));
  }
  //wait for sms ready
  while (_readSerial().indexOf("SMS")==-1 ){
  }
}

void ArduGSM::setPhoneFunctionality(){
  /*AT+CFUN=<fun>[,<rst>] 
  Parameters
  <fun> 0 Minimum functionality
  1 Full functionality (Default)
  4 Disable phone both transmit and receive RF circuits.
  <rst> 1 Reset the MT before setting it to <fun> power level.
  */
  this->SoftwareSerial::print (F("AT+CFUN=1\r\n"));
}


void ArduGSM::signalQuality(){
/*Response
+CSQ: <rssi>,<ber>Parameters
<rssi>
0 -115 dBm or less
1 -111 dBm
2...30 -110... -54 dBm
31 -52 dBm or greater
99 not known or not detectable
<ber> (in percent):
0...7 As RXQUAL values in the table in GSM 05.08 [20]
subclause 7.2.4
99 Not known or not detectable 
*/
  this->SoftwareSerial::print (F("AT+CSQ\r\n"));
  Serial.println(_readSerial());
}


void ArduGSM::activateBearerProfile(){
  this->SoftwareSerial::print (F(" AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" \r\n" ));_buffer=_readSerial();  // set bearer parameter 
  this->SoftwareSerial::print (F(" AT+SAPBR=3,1,\"APN\",\"internet\" \r\n" ));_buffer=_readSerial(); // set apn  
  this->SoftwareSerial::print (F(" AT+SAPBR=1,1 \r\n"));delay(1200);_buffer=_readSerial();// activate bearer context
  this->SoftwareSerial::print (F(" AT+SAPBR=2,1\r\n "));delay(3000);_buffer=_readSerial(); // get context ip address
}


void ArduGSM::deactivateBearerProfile(){
 this->SoftwareSerial::print (F("AT+SAPBR=0,1\r\n "));
  delay(1500);
}


bool ArduGSM::answerCall(){
   this->SoftwareSerial::print (F("ATA\r\n"));
   _buffer=_readSerial();
   //Response in case of data call, if successfully connected 
   if ( (_buffer.indexOf("OK") )!=-1 ) return true;  
   else return false;
}


void  ArduGSM::callNumber(char* number){
  this->SoftwareSerial::print (F("ATD"));
  this->SoftwareSerial::print (number);
  this->SoftwareSerial::print(F(";\r\n"));
}


uint8_t ArduGSM::getCallStatus(){
/*
  values of return:
 
 0 Ready (MT allows commands from TA/TE)
 2 Unknown (MT is not guaranteed to respond to tructions)
 3 Ringing (MT is ready for commands from TA/TE, but the ringer is active)
 4 Call in progress

*/
  this->SoftwareSerial::print (F("AT+CPAS\r\n"));
  _buffer=_readSerial();  
  return _buffer.substring(_buffer.indexOf("+CPAS: ")+7,_buffer.indexOf("+CPAS: ")+9).toInt();
}


bool ArduGSM::hangoffCall(){
  this->SoftwareSerial::print (F("ATH\r\n"));
  _buffer=_readSerial();
  if ( (_buffer.indexOf("OK") ) != -1) return true;
  else return false;
}


bool ArduGSM::sendSms(char* number,char* text){

   this->SoftwareSerial::print (F("AT+CMGF=1\r")); //set sms to text mode  
    _buffer=_readSerial();
    this->SoftwareSerial::print (F("AT+CMGS=\""));  // command to send sms
    this->SoftwareSerial::print (number);           
    this->SoftwareSerial::print(F("\"\r"));       
    _buffer=_readSerial(); 
    this->SoftwareSerial::print (text);
    this->SoftwareSerial::print ("\r"); 
	//change delay 100 to readserial	
    _buffer=_readSerial();
    this->SoftwareSerial::print((char)26);
    _buffer=_readSerial();
    //expect CMGS:xxx   , where xxx is a number,for the sending sms.
    if (((_buffer.indexOf("CMGS") ) != -1 ) ){
      return true;
    }
    else {
      return false;
    }
}


String ArduGSM::getNumberSms(uint8_t index){
  _buffer=readSms(index);
  Serial.println(_buffer.length());
  if (_buffer.length() > 10) //avoid empty sms
  {
    uint8_t _idx1=_buffer.indexOf("+CMGR:");
    _idx1=_buffer.indexOf("\",\"",_idx1+1);
    return _buffer.substring(_idx1+3,_buffer.indexOf("\",\"",_idx1+4));
  }else{
    return "";
  }
}



String ArduGSM::readSms(uint8_t index){
  this->SoftwareSerial::print (F("AT+CMGF=1\r")); 
  if (( _readSerial().indexOf("ER")) ==-1) {
    this->SoftwareSerial::print (F("AT+CMGR="));
    this->SoftwareSerial::print (index);
    this->SoftwareSerial::print("\r");
    _buffer=_readSerial();
    if (_buffer.indexOf("CMGR:")!=-1){
      return _buffer;
    }
    else return "";    
    }
  else
    return "";
}


bool ArduGSM::delAllSms(){ 
  this->SoftwareSerial::print(F("at+cmgda=\"del all\"\n\r"));
  _buffer=_readSerial();
  if (_buffer.indexOf("OK")!=-1)   {
    return true;
  }
  else {
    return false;
  }
  
}


void ArduGSM::RTCtime(int *day,int *month, int *year,int *hour,int *minute, int *second){
  this->SoftwareSerial::print(F("at+cclk?\r\n"));
  // if respond with ERROR try one more time. 
  _buffer=_readSerial();
  if ((_buffer.indexOf("ERR"))!=-1){
    delay(50);
    this->SoftwareSerial::print(F("at+cclk?\r\n"));
  } 
  if ((_buffer.indexOf("ERR"))==-1){
    _buffer=_buffer.substring(_buffer.indexOf("\"")+1,_buffer.lastIndexOf("\"")-1);  
    *year=_buffer.substring(0,2).toInt();
    *month= _buffer.substring(3,5).toInt();
    *day=_buffer.substring(6,8).toInt();
    *hour=_buffer.substring(9,11).toInt();
    *minute=_buffer.substring(12,14).toInt();
    *second=_buffer.substring(15,17).toInt();
 }
}

//Get the time  of the base of GSM
String ArduGSM::dateNet() {
  this->SoftwareSerial::print(F("AT+CIPGSMLOC=2,1\r\n "));
  _buffer=_readSerial();

  if (_buffer.indexOf("OK")!=-1 ){
    return _buffer.substring(_buffer.indexOf(":")+2,(_buffer.indexOf("OK")-4));
  } else
  return "0";      
}

// Update the RTC of the module with the date of GSM. 
bool ArduGSM::updateRtc(int utc){
  
  activateBearerProfile();
  _buffer=dateNet();
  deactivateBearerProfile();
  
  _buffer=_buffer.substring(_buffer.indexOf(",")+1,_buffer.length());
  String dt=_buffer.substring(0,_buffer.indexOf(","));
  String tm=_buffer.substring(_buffer.indexOf(",")+1,_buffer.length()) ;

  int hour = tm.substring(0,2).toInt();
  int day = dt.substring(8,10).toInt();

  hour=hour+utc;

  String tmp_hour;
  String tmp_day;
  //TODO : fix if the day is 0, this occur when day is 1 then decrement to 1, 
  //       will need to check the last month what is the last day .  
  if (hour<0){
    hour+=24;
    day-=1;
  }
  if (hour<10){

      tmp_hour="0"+String(hour);
  }else{
    tmp_hour=String(hour);
  }
  if (day<10){
    tmp_day="0"+String(day);    
  }else{
    tmp_day=String(day);
  }
    //for debugging
  //Serial.println("at+cclk=\""+dt.substring(2,4)+"/"+dt.substring(5,7)+"/"+tmp_day+","+tmp_hour+":"+tm.substring(3,5)+":"+tm.substring(6,8)+"-03\"\r\n");
  this->SoftwareSerial::print("at+cclk=\""+dt.substring(2,4)+"/"+dt.substring(5,7)+"/"+tmp_day+","+tmp_hour+":"+tm.substring(3,5)+":"+tm.substring(6,8)+"-03\"\r\n");
  if ( (_readSerial().indexOf("ER"))!=-1) {return false;}else return true;

   
}
