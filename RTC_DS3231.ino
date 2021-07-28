/* ______________________________________Libraries______________________________________ */
#include <Wire.h>
#include <RTClib.h> //By adafruit
/* _____________________________Declarations_and_Variables______________________________ */
//RTC
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String timeStamp = "";
String dateStamp = "";
String dayStamp = "";
String H = "",M = "",S = "",DD = "",MM = "",YYYY = "";
/* ______________________________________Functions______________________________________ */
void getDetailedTime() {
  DateTime now = rtc.now();
  (now.hour()>=10)?(H = String(now.hour())):(H = "0" + String(now.hour()));
  (now.minute()>=10)?(M = String(now.minute())):(M = "0" + String(now.minute()));
  (now.second()>=10)?(S = String(now.second())):(S = "0" + String(now.second()));
  timeStamp = H +":" + M + ":" + S;
  (now.day()>=10)?(DD = String(now.day())):(DD = "0" + String(now.day()));
  (now.month()>=10)?(MM = String(now.month())):(MM = "0" + String(now.month()));
  YYYY = now.year();
  dateStamp = DD +"-" + MM + "-" + YYYY;
  dayStamp = daysOfTheWeek[now.dayOfTheWeek()];
}
/* ________________________________________Setup________________________________________ */
void setup() {
  Serial.begin(115200);
  Wire.begin();
  rtc.begin();
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(__DATE__, __TIME__));
}
/* ________________________________________Loop_________________________________________ */
void loop() {
  getDetailedTime();
  Serial.println(dateStamp);
  Serial.println(dayStamp);
  Serial.println(timeStamp);
  delay(1000);
}
