#include <SPI.h>                               //فراخوانی کتابخانه های مورد نیاز
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DHT.h>  
#include <ESP8266WiFi.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire); // مشخصکردن ابعاد نمایشگر و نوع درایور
 
String apiKey = "MOF12ZGK61DKYH08";       // کد بدست آمده از پلتفرم اینترنت اشیا 
 
const char *ssid =  "C1F3R";                       //نام شبکه وای فای برای اتصال
const char *pass =  "314159265";                     //پسوورد شبکه وای فای مدنظر
const char* server = "api.thingspeak.com";
 
#define DHTPIN 2                    //پین اتصال برای پایه دیتا در سنسور دما و رطوبت
 
DHT dht(DHTPIN, DHT21);   //مشخص کردن نوع سنسور مورد استفاده شده و معرفی پین اتصال
 
WiFiClient client;
 
void setup() 
{
       Serial.begin(115200);

       display.begin(SSD1306_SWITCHCAPVCC, 0x3C);   //شروع ارتباط i2c برای نمایشگر oled

        display.display();  // پاک کردن بافر برای نمیش مناسب مقادیر در ادامه
        delay(1000);

        display.clearDisplay();
        display.display();
       
       delay(10);
       dht.begin();   //شروع محاسبه مقادیر از طرف سنسور 
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) //تلاش برای اتصال به شبکه وای فای معرفی شده در بخش قبل
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}
 
void loop() 
{
  
      float h = dht.readHumidity();         //متغیر برای ذخیره مقادیر رطوبت بدست آمده
      float t = dht.readTemperature();      //متغیر برای ذخیره مقادیر دما بدست آمده  
 
              display.setTextSize(2);      
              display.setTextColor(SSD1306_WHITE);
              display.setCursor(40,10);
              display.print(t);         // چاپ مقادیر بدست آمده مربوط به دما
              display.setTextSize(1);
              display.setCursor(0,0);
              display.print("Temperature:");  
              display.display();
              display.clearDisplay();

              delay(5000);
          
              display.setTextSize(2);      
              display.setTextColor(SSD1306_WHITE);
              display.setCursor(40,10);
              display.print(h);         // چاپ مقادیر بدست آمده مربوط به رطوبت
              display.setTextSize(1);
              display.setCursor(0,0);
              display.print("Humidity:");
              display.display();
              display.clearDisplay();

              delay(5000);

              
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 
                         if (client.connect(server,80))   //"184.106.153.149" or api.thingspeak.com
                      {  
                                  // در چند خط پایین اقدام به برقراری ارتباط با پلتفرم کرده ایم
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");    //پرینت مقادیر در سریال مانیتور
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.println("%. Send to Thingspeak.");
                        }
          client.stop();
          
          Serial.println("Waiting...");
  
  delay(500);
  yield();
}
