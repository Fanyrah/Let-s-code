#include <ThreeWire.h>
#include <RtcDS1302.h>

#define buzzerPin 2
#define countof(a) (sizeof(a) / sizeof(a[0]))

ThreeWire myWire(6,5,7); // DATA, SCLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);

void setup () {
    Serial.begin(9600);
    pinMode(buzzerPin, OUTPUT);
    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);
    Rtc.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();
    if(!Rtc.IsDateTimeValid()) {
        // Common Causes: 1) first time you ran and the device wasn't running yet 2) the battery on the device is low or even missing
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }
    if (Rtc.GetIsWriteProtected()) {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }
    if (!Rtc.GetIsRunning()) {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }
    
    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
        digitalWrite(buzzerPin, LOW);
    }
    else if (now > compiled){
        Serial.println("RTC is newer than compile time. (this is expected)");
        digitalWrite(buzzerPin, LOW);
    }
    else{
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
        digitalWrite(buzzerPin, HIGH);
    }
    delay(500);
}
void loop (){
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    String schedule1 = "2 11/01/2022 12:30:30";
    triggerEvent(schedule1,playBuzzer);
    if (!now.IsValid()) {
        // Common Causes: 1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }
    delay(1000);
}

void printDateTime(const RtcDateTime& dt) {
    char datestring[25];
    snprintf_P(datestring,
            countof(datestring),
            PSTR("%1u %02u/%02u/%04u %02u:%02u:%02u"), dt.DayOfWeek(),
            dt.Month(), dt.Day(), dt.Year(),
            dt.Hour(), dt.Minute(), dt.Second() );
    Serial.println(datestring);
}

template <typename T>
void triggerEvent(String input, T const & event){
    /* parse the input*/
    String day = input.substring(0,1);
    String hour = input.substring(13,15);
    String minute = input.substring(16, 18);
    String second = input.substring(19, 22);
    int dayInt = day.toInt();
    int hourInt = hour.toInt();
    int minInt = minute.toInt();
    int secInt = second.toInt();
    /* trigger event based on the input*/
    RtcDateTime mNow = Rtc.GetDateTime();
    if(mNow.DayOfWeek()==dayInt && mNow.Hour()==hourInt && mNow.Minute()==minInt && mNow.Second()==secInt){
        event();
    }
}

void playBuzzer(){
    Serial.println("Buzzer Time!");
    delay(1000); //for this duration
}