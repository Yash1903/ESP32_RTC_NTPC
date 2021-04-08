#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
// WiFi details
char ssid[]   = "LIMELIGHT LAB";
const char *password = "TATA@123";
//////
int period = 1000;
unsigned long time_now = 0;
////
String R_time="";

// I use static IP's, and found i had to put in DNS to make it work properly.
//IPAddress local_IP(XXX, XXX, XXX, XXX);
//IPAddress gateway(XXX, XXX, XXX, XXX);
//IPAddress subnet(XXX, XXX, XXX, XXX);
//IPAddress primaryDNS(XXX,XXX,XXX,XXX);
//IPAddress secondaryDNS(XXX,XXX,XXX,XXX);
// NTP

WiFiUDP ntpUDP;
// i'm in PDT which is -7 from GMT.
NTPClient timeClient(ntpUDP, "us.pool.ntp.org", 19800);
// RTC Libraries
#include <Wire.h> 
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);

void WiFi_Setup(){
  if (!WiFi.status() != WL_CONNECTED) {
    Serial.println("STA Failed to configure");
  }
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");
}

// ************************************
// ** RTC Setup
// ************************************
void RTC_Update(){
  // Do udp NTP lookup, epoch time is unix time - subtract the 30 extra yrs (946684800UL) library expects 2000
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime()-946684800UL;
  Rtc.SetDateTime(epochTime); 
}
bool RTC_Valid(){
  bool boolCheck = true; 
  if (!Rtc.IsDateTimeValid()){
    Serial.println("RTC lost confidence in the DateTime!  Updating DateTime");
    boolCheck = false;
    RTC_Update();    
  }
  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now.  Updating Date Time");
    Rtc.SetIsRunning(true);
    boolCheck = false;
    RTC_Update();
  }
}

void setup(){
  Serial.begin(115200);
  WiFi_Setup();
  // Start NTP Time Client
  timeClient.begin();
  delay(2000);
  timeClient.update();
  Rtc.Begin();
  RTC_Update();
}

void loop() {

//if(millis() >= time_now + period){
//        time_now += period;
  if (!RTC_Valid()){
    RTC_Update();
  }
//}
  RtcDateTime currTime = Rtc.GetDateTime();
  printDateTime(currTime);
  
//  Serial.println(timeClient.getFormattedTime());
  delay(1000);
 
}
// Utility print function
#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u,%02d:%02d:%02d"),
            
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
//  Serial.println(datestring);
  char buffer [25] = "";
  sprintf(buffer, "%02d:%02d:%02d", dt.Hour(), dt.Minute(), dt.Second());
// Serial.println(buffer);
 R_time = buffer;
 Serial.println(R_time);
}
