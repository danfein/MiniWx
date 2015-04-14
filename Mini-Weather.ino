/****************************************************************
 __    __           _   _                               _                                          _ 
/ / /\ \ \___  __ _| |_| |__   ___ _ __ /\ /\ _ __   __| | ___ _ __ __ _ _ __ ___  _   _ _ __   __| |
\ \/  \/ / _ \/ _` | __| '_ \ / _ \ '__/ / \ \ '_ \ / _` |/ _ \ '__/ _` | '__/ _ \| | | | '_ \ / _` |
 \  /\  /  __/ (_| | |_| | | |  __/ |  \ \_/ / | | | (_| |  __/ | | (_| | | | (_) | |_| | | | | (_| |
  \/  \/ \___|\__,_|\__|_| |_|\___|_|   \___/|_| |_|\__,_|\___|_|  \__, |_|  \___/ \__,_|_| |_|\__,_|
                                                                   |___/                             
Mini WiFi Weather Station

Spark Core Weather station, Sends Humidity and Temperature data to wunderground.com for logging/analysis

Pieced together by Dan Fein

Notes:
This project assumes battery power and to save power it will deep sleep between sends.

You will need to sign up for a free account at wunderground.com, to get your pass
When you register a station you will get an ID (If you send data you also get ad-free membership)
Sign up at http://www.wunderground.com/personal-weather-station/signup.asp

IF sending timestamped data:
Wunderground wants UTC Zulu, not local time, if your RTC is local, offset it in code.
Wunderground Upload guidelines: http://wiki.wunderground.com/index.php/PWS_-_Upload_Protocol

****************************************************************/

// This #include statement was automatically added by the Spark IDE.
    #include "Adafruit_DHT/Adafruit_DHT.h"

// Define Pins
    #define DHTPIN 2     // what pin we're connected to

// Setup Sensor
    #define DHTTYPE DHT22		// DHT 22 (AM2302)

    DHT dht(DHTPIN, DHTTYPE);

// Weather Underground Connection Details
    char SERVER[] = "rtupdate.wunderground.com"; // Realtime update server - for rapid sends
    //char SERVER [] = "weatherstation.wunderground.com"; //standard server - for slower sends
    char WEBPAGE [] = "GET /weatherstation/updateweatherstation.php?";

    //Station Identification
    char ID [] = "xxxxxx"; //Change to your Weather Underground station ID 
    char PASSWORD [] = "xxxxxx"; //Change to your Weather Underground account password

//Conection setup
    TCPClient client;
    
/****************************************************************/
    
void setup() 
{
	Serial.begin(9600);
	RGB.control(true);  // Turn down for what? The status light is crazy bright = wasted mAh battery.
	RGB.brightness(10); // This sets the RGB LED brightness : 0-256
    RGB.control(false); // release the LED to the system
    dht.begin();		// Startup the sensor
    
	Serial.println("---------------");
}

void loop(){

    // hold up    
        delay(2000); // let everything power up for a bit
        
    // grab some data	
        float humidity = dht.getHumidity();
        float tempf = dht.getTempFarenheit();
        float dewptc = dht.getDewPoint();
        float dewptf = (dewptc* 9 / 5 + 32);
       
   // Print to console for debugging, fun, and health.
        Serial.print("\nRetrieving information from sensor: ");
        Serial.print("Read sensor: ");
        Serial.print("Humidity: "); 
        Serial.print(humidity);
        Serial.print("% - ");
        Serial.print("Temp: "); 
        Serial.print(tempf);
        Serial.print("*F ");
        Serial.print("DewP: ");
        Serial.print(dewptf);
        
// Send data to Weather Underground
        Serial.println("connecting...");
     if (client.connect(SERVER, 80)) { 
        Serial.println("Connected");
        client.print(WEBPAGE);
        client.print("ID=");
        client.print(ID);
        client.print("&PASSWORD=");
        client.print(PASSWORD);
        client.print("&dateutc=now"); // "now" for timestamp if data is realtime. See upload protocol if not.
        client.print("&tempf=");
        client.print(tempf);
        client.print("&dewptf=");
        client.print(dewptf);
        client.print("&humidity=");
        client.print(humidity);
        //client.print("&action=updateraw");//If using Standard update server
        client.print("&softwaretype=SparkCore%20version1&action=updateraw&realtime=1&rtfreq=5");//If using Rapid Fire server
        client.println();
        Serial.println("Upload complete");
        } 
        else {
          Serial.println(F("Connection failed"));                 
          return;
          }
    //Hold up    
        delay(1000); //delay for a second to appreciate the work well done
    //Nap Time   
        Spark.sleep(SLEEP_MODE_DEEP,300); // sleep for number of seconds 300=5min 600=10min
}
