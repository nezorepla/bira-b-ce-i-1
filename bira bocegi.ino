#include "Timer.h"

#include <OneWire.h> // OneWire kütüphanesini ekliyoruz.



#define BUTTON1_PIN 4 
#define TEMP_PIN 3 
#define LED_PIN 5 
#define COUNTER_PIN 6 

 
 OneWire ds(TEMP_PIN);  // 2. Dijital pinde.


///////////////////////--- BUBBLE COUNTER
const  uint16_t Period = 1000; // 1 SANİYE [0-65535] or [0x0000-0xFFFF]
 
int ToplamBaloncukAdet;
int PeriodBaloncukAdet;
float SaniyeOrtalamaBaloncukAdet;
 long IkiBaloncukArasindaGecenSureMs  = 0;
 unsigned long  prevMil=0;
bool Counter_Pin_LastValue;
bool Counter_Pin_Value;


///////////////////////--- ZAMAN


/*  CrossRoads Time-clock  */

unsigned long previousTime = 0;
byte seconds  = 0;
byte minutes  = 0;
byte hours  = 0;
byte days  = 0;
//unsigned char gecenZaman ;

/*#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)

 Useful Macros for getting elapsed time
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)  
  */
 





//////////////////// BUTON
int ButonBasiliGecenSureMs=0;
bool KisaPress=false;
bool UzunPress=false;




/////////////////////// --- EKRAN
#include "HCuOLED.h"
#include "SPI.h" 
 
#define CS_DI 10
#define DC_DI 9
#define RST_DI 8
HCuOLED HCuOLED(SH1106, CS_DI, DC_DI, RST_DI); // For SH1106 displays (HCMODU0058 & HCMODU0059)



uint8_t  Gecerli_Ekran;

/*
// testValue
unsigned long long testValue     = 0xFFFFFFFFFFFFFFFF; // 18446744073709551615

// 1 byte -> [0-255] or [0x00-0xFF]
uint8_t         number8     = testValue; // 255
unsigned char    numberChar    = testValue; // 255

// 2 bytes -> [0-65535] or [0x0000-0xFFFF]
uint16_t         number16     = testValue; // 65535
unsigned short    numberShort    = testValue; // 65535

// 4 bytes -> [0-4294967295] or [0x00000000-0xFFFFFFFF]
uint32_t         number32     = testValue; // 4294967295
unsigned int     numberInt    = testValue; // 4294967295

 // 8 bytes -> [0-18446744073709551615] or [0x0000000000000000-0xFFFFFFFFFFFFFFFF]
uint64_t             number64         = testValue; // 18446744073709551615
unsigned long long     numberLongLong    = testValue; // 18446744073709551615
*/








Timer t;

bool ekranAktif=true;
byte uykuSuresi=60;
byte uykuIcinGecen=0;

void sleepBool(){
  if(!ekranAktif){
  ekranAktif=true;
  uykuIcinGecen=0;}
  }

void button(){

 
  
  bool BPin_Value = digitalRead(BUTTON1_PIN);

if(BPin_Value){
  ButonBasiliGecenSureMs++;
//  Counter_Pin_LastValue=1;
  }//else{ButonBasiliGecenSureMs=0;}


if(ButonBasiliGecenSureMs >1000 /*&& ButonBasiliGecenSureMs<4000*/ &&!BPin_Value)
{KisaPress=false;UzunPress=true;}


if(ButonBasiliGecenSureMs >0 && ButonBasiliGecenSureMs<1000 &&!BPin_Value)
{KisaPress=true;UzunPress=false;}
//else{KisaPress=false;}





if(KisaPress){
  if(Gecerli_Ekran==3){
    Gecerli_Ekran=0;}else{
  Gecerli_Ekran++;
  }
sleepBool();
  }

if(KisaPress||UzunPress){KisaPress=false;UzunPress=false;ButonBasiliGecenSureMs=0;}
 }


float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}
  
void zaman()
{
// I  think using microseconds is even more accurate
 if (millis() >= (previousTime)) 
  {
     previousTime = previousTime + 1000;  // use 100000 for uS
  /*   */ seconds = seconds +1;

  saniyeBasiIslemi();
     if (seconds == 60)
     {

        seconds = 0;
        minutes = minutes +1;
     }
     if (minutes == 60)
     {
        minutes = 0;
        hours = hours +1;
     }
     if (hours == 24)
     {
        hours = 0;
        days=days+1;
     }
 /*    
  gecenZaman= char(days) ;
  gecenZaman= gecenZaman + (':');
  gecenZaman=gecenZaman + char(hours)  ;//printDigits(hours);  
  gecenZaman= gecenZaman + (':');
  gecenZaman= gecenZaman + char(minutes);//printDigits(minutes);
  gecenZaman= gecenZaman + (':');
  gecenZaman  =gecenZaman + char(seconds)  ;// printDigits(seconds);

 
 */ } // end 1 second
   

} // end void


 

 

void setup() { 

 Serial.begin(115200);

 Serial.println("sifirlandi!");
 
Counter_Pin_LastValue = digitalRead(COUNTER_PIN);

ToplamBaloncukAdet=0;
PeriodBaloncukAdet=0;
SaniyeOrtalamaBaloncukAdet =0;
int tickEvent = t.every(Period, PerBubble);
//int tickEvent2 = t.every(Period, zaman);
 HCuOLED.Reset();


pinMode(LED_PIN, OUTPUT);   
pinMode(COUNTER_PIN, INPUT); 
//pinMode(TEMPERATURE_PIN, INPUT);   

Gecerli_Ekran=0;


}

 void saniyeBasiIslemi()
{
if(ekranAktif){uykuIcinGecen++;}

if(uykuSuresi==uykuIcinGecen){ekranAktif=false;Ekran5();}

}

 
void loop() {

button();




if(ekranAktif){

switch (Gecerli_Ekran) {

case 0:                

Ekran1(); 
break;
case 1:                

Ekran2(); //GECEN SURE
break;  

case 2:                
Ekran3();
break;
case 3:                
Ekran4();

break;

}
}

 
 BalonSay();

zaman();
    t.update();
}



void BalonSay(){ 
  
  Counter_Pin_Value = digitalRead(COUNTER_PIN);

if(Counter_Pin_Value==1){
  IkiBaloncukArasindaGecenSureMs++;
  Counter_Pin_LastValue=1;
  }

if(Counter_Pin_LastValue==1 && Counter_Pin_Value==0 && IkiBaloncukArasindaGecenSureMs>5) {
 digitalWrite(LED_PIN,1);

  Counter_Pin_LastValue=0;
     ToplamBaloncukAdet++;
    PeriodBaloncukAdet++;
  //  Serial.println(count);
 IkiBaloncukArasindaGecenSureMs=0;
  }
  digitalWrite(LED_PIN,0);
 }

void PerBubble()
{
  unsigned long curMil = millis();

 
  unsigned long deltat = curMil-prevMil;
 SaniyeOrtalamaBaloncukAdet  = 1000*(float) PeriodBaloncukAdet / (float)deltat;
 /* String toplam = String(ToplamBaloncukAdet);  
  String ortalama = String(f);
  String periyot = String(PeriodBaloncukAdet);
  Serial.print("sure: ");
 Serial.print(deltat);  
 Serial.print(" toplam: ");
 Serial.print(toplam);
  Serial.print(" ortalama: ");
 Serial.print(ortalama);
  Serial.print(" periyot: ");
 Serial.println(periyot);
 */
  prevMil = millis();
PeriodBaloncukAdet =0;
  
//Serial.println(count);
//delay(1000);
}
/*
float getTemperature(){
  float temperature =( analogRead(TEMPERATURE_PIN) / 9.31 ) ;// - 2.2;  
  return temperature;
}
*/

void Ekran1()
{
HCuOLED.ClearBuffer(); 
HCuOLED.Cursor(3,3);
HCuOLED.SetFont(Terminal_8pt);
HCuOLED.Print("SICAKLIK");

 float temperature = getTemp();
  // Sensörden gelen sıcaklık değerini Serial monitörde yazdırıyoruz.
//  Serial.print("Sicaklik: ");
Serial.println(temperature);

  
// float sicaklik =  ( analogRead(TEMPERATURE_PIN) / 9.31 );
 
// Serial.println(sicaklik);
 
HCuOLED.Cursor(4,20);
HCuOLED.SetFont(MedProp_11pt);
HCuOLED.Print(temperature, 4, 1);  
HCuOLED.Refresh();     

 
}

 
void Ekran2()
{
 
 HCuOLED.ClearBuffer(); 
HCuOLED.Cursor(3,3);
HCuOLED.SetFont(Terminal_8pt);
HCuOLED.Print("GECEN SURE");
 
   
HCuOLED.SetFont(MedProp_11pt);

HCuOLED.Cursor(4,20); 
if(days < 1){
HCuOLED.Print("0");
 }else{
 HCuOLED.Print(days);
 }
 
HCuOLED.Cursor(25,20);
HCuOLED.Print("Gun ");


//saat
byte xc1=4;
if(hours < 10){
HCuOLED.Cursor(xc1,35); 
HCuOLED.Print("0");
//
HCuOLED.Cursor((xc1+10),35); 
if(hours < 1){
HCuOLED.Print("0");
 }else{
 HCuOLED.Print(hours);
 }
 }else{HCuOLED.Cursor((xc1),35); 
if(hours < 1){
HCuOLED.Print("0");
 }else{
 HCuOLED.Print(hours);
 } } 
HCuOLED.Cursor((xc1+20),35); 
HCuOLED.Print(":");
 
//dakika
byte xc2=31;
if(minutes < 10){
HCuOLED.Cursor(xc2,35); 
HCuOLED.Print("0");
//
HCuOLED.Cursor((xc2+10),35); 
if(minutes < 1){
HCuOLED.Print("0");
 }else{
 HCuOLED.Print(minutes);
 }
 }else{HCuOLED.Cursor((xc2),35); 
if(minutes < 1){
HCuOLED.Print("0");
 }else{
 HCuOLED.Print(minutes);
 } } 
HCuOLED.Cursor((xc2+20),35); 
HCuOLED.Print(":");

//saniye
byte xc3=57;
if(seconds < 10){
HCuOLED.Cursor(xc3,35); 
HCuOLED.Print("0");
//
HCuOLED.Cursor((xc3+10),35); 
if(seconds < 1){
HCuOLED.Print("0");
 }else{
 HCuOLED.Print(seconds);
 }
 }else{HCuOLED.Cursor((xc3),35); 
if(seconds < 1){
HCuOLED.Print("0");
 }else{
 HCuOLED.Print(seconds);
 } } 
//HCuOLED.Cursor((xc3+20),35); 
//HCuOLED.Print(":");




/*
HCuOLED.Cursor(60,20);
HCuOLED.Print(0+hours);
HCuOLED.Cursor(80,20);
HCuOLED.Print("Saat ");
*/

/*
HCuOLED.Cursor(24,20);
HCuOLED.Print(seconds);
*/
HCuOLED.Refresh();     
}

void Ekran3()
{HCuOLED.ClearBuffer(); 
HCuOLED.Cursor(3,3);
HCuOLED.SetFont(Terminal_8pt);
HCuOLED.Print("TOPLAM ADET");
 
 
HCuOLED.Cursor(4,20);
HCuOLED.SetFont(MedProp_11pt);
HCuOLED.Print(ToplamBaloncukAdet);
HCuOLED.Refresh();     
}
void Ekran4()
{HCuOLED.ClearBuffer(); 
HCuOLED.Cursor(3,3);
HCuOLED.SetFont(Terminal_8pt);
HCuOLED.Print("SANIYE ORTALAMA");
 

HCuOLED.Cursor(4,20);
HCuOLED.SetFont(MedProp_11pt);
HCuOLED.Print(SaniyeOrtalamaBaloncukAdet);
HCuOLED.Refresh();     
}
void Ekran5()
{HCuOLED.ClearBuffer(); 
HCuOLED.Cursor(3,3);
HCuOLED.SetFont(Terminal_8pt);
HCuOLED.Print("kapali");

HCuOLED.Refresh();     
}
