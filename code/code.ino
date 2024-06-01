/**
--------------------------------------------------------
______                                  _
| ___ \                                (_)
| |_/ /_ _ _ __   __ _ _ __  _   _ _ __ _ _   _  ___
|  __/ _` | '_ \ / _` | '_ \| | | | '__| | | | |/ _ \
| | | (_| | | | | (_| | |_) | |_| | |  | | |_| |  __/
\_|  \__,_|_| |_|\__, | .__/ \__,_|_|  |_|\__, |\___|
                  __/ | |                  __/ |
                 |___/|_|                 |___/
                  PANGPURIYE Fram!
                SUPER AI ENGINEER SEASON 4
                  BORAD PROTOTYPE
--------------------------------------------------------
**/

// SetUP NetPile  for Prototype borad
const char *Netpiemqtt_server = "broker.netpie.io";
const int Netpiemqtt_port = 1883;

// Client ID
// const char *Netpiemqtt_Client = "YOU-Client-ID";
// Token
// const char *Netpiemqtt_username = "YOU-UserName";
// Secret
// const char *Netpiemqtt_password = "YOU-Password";

// include lib  Handy Sese
#include <HandySense.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "time.h"

// include  pub topic
#include <pub_topic.h>
#include "SHT31.h"
#include <Wire.h>
#include <BH1750.h>
#include "MCP23008.h"

//  time
#include "time.h"
#include "TimeBFarm.h"
BFarmTime bfarmtime;

// humidity sensor
SHT31 sht;
BH1750 lightMeter;
MCP23008 MCP(0x24);

// Set init value
int set_Maxtemperature;
int setime_mintemperature;
int set_MaxHumidity;
int setime_minHumidity;
int set_soilMaxHumidity;
int set_soilMinHumidity;

// Set time to send data every 10  minunite
const long interval = 10000; // 10 Sec
unsigned long previousMillis = 0;
unsigned long currentMillis;

// set time for open Value
// set defal  time  on  7:00 AM Every Day
int time_hour = 7;
int time_min = 0;
int t_water = 30000; // MilliSec 1 Min
// test count time to sent
int count = 0;

//  count open  manual
// maual 0 is use
int manual = 0;

/*
*   Open  Water  function
*   use  Realy 0 to oepon  solenoid valve
    Delay  open  t_water
*/

static void openWater()
{

    // Turn  ON  LED 0 Status
    digitalWrite(const_relay_pin[0], HIGH);
    MCP.digitalWrite(0, HIGH);

    pub_topic("valve_status", 1);
    // Turn on the relay to open water
    digitalWrite(const_relay_pin[0], HIGH);

    // Delay for the specified water duration
    delay(t_water);

    // Sent Status  to  netpile
    pub_topic("valve_status", 0);
}

// Callback function to handle incoming messages from Netpie
void Netpiecallback(String topic, byte *payload, unsigned int length)
{
    // Print the received message and topic for debugging
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    // Convert the payload to a string
    String message;
    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    // set hour   time
    if (topic == String("@private/set_hour_time"))
    {
        time_hour = message.toInt();
        Serial.println(time_hour);
        Serial.println("Update hour time");
        pub_topic("time_hour", time_hour);
    }

    // set min
    if (topic == String("@private/set_min_time"))
    {
        time_min = message.toInt();
        Serial.println("Update min time");
        pub_topic("time_hour", time_min);
    }

    // set max  terprerature
    if (topic == String("@private/max_temperature"))
    {
        set_Maxtemperature = message.toInt();
        Serial.println(set_Maxtemperature);
    }
    // set min terperature
    else if (topic == String("@private/min_temperature"))
    {
        setime_mintemperature = message.toInt();
        Serial.println(setime_mintemperature);
    }
    // set  max humidity
    else if (topic == String("@private/max_humidity"))
    {
        set_MaxHumidity = message.toInt();
        Serial.println(set_MaxHumidity);
    }
    // set min  humidity
    else if (topic == String("@private/min_humidity"))
    {
        setime_minHumidity = message.toInt();
        Serial.println(setime_minHumidity);
    }
    // set  max soli humidity
    else if (topic == String("@private/max_Soli_humidity"))
    {
        set_soilMaxHumidity = message.toInt();
        Serial.println(set_soilMaxHumidity);
    }
    // set min soli  humidity
    else if (topic == String("@private/min_Soli_humidity"))
    {
        set_soilMinHumidity = message.toInt();
        Serial.println(set_soilMinHumidity);
    }

    //  oepn Realy 0
    // open Water
    else if (topic == String("@private/relay0on"))
    {
        openWater();
    }
    // close  Realy 0
    else if (topic == String("@private/relay0off"))
    {
        digitalWrite(const_relay_pin[0], LOW);
        MCP.digitalWrite(0, LOW);
        manual = 0; // Set manual mode to 0
        pub_topic("valve_status", 0);
    }
    // opeon Realy1
    else if (topic == String("@private/relay1on"))
    {
        digitalWrite(const_relay_pin[1], HIGH);
        MCP.digitalWrite(1, HIGH);
    }
    // Close Realy1
    else if (topic == String("@private/relay1off"))
    {
        digitalWrite(const_relay_pin[1], LOW);
        MCP.digitalWrite(1, LOW);
    }
    // Open relay2
    else if (topic == String("@private/relay2on"))
    {
        digitalWrite(const_relay_pin[2], HIGH);
        MCP.digitalWrite(2, HIGH);
    }
    // Close relay2
    else if (topic == String("@private/relay2off"))
    {
        digitalWrite(const_relay_pin[2], LOW);
        MCP.digitalWrite(2, LOW);
    }
    //  Open  Realy3
    else if (topic == String("@private/relay3on"))
    {
        digitalWrite(const_relay_pin[3], HIGH);
        MCP.digitalWrite(3, HIGH);
    }
    // Close  Realy3
    else if (topic == String("@private/relay3off"))
    {
        digitalWrite(const_relay_pin[3], LOW);
        MCP.digitalWrite(3, LOW);
    }

    // Print MQTT callback message for debugging
    Serial.println("MQTT CALLBACK");
    Serial.println(topic);
}

void sentData()
{

    pub_topic("set_maxTemp", set_Maxtemperature);
    pub_topic("set_minTemp", setime_mintemperature);

    pub_topic("set_maxHump", set_MaxHumidity);
    pub_topic("set_minHump", setime_minHumidity);

    pub_topic("set_soilMax", set_soilMaxHumidity);
    pub_topic("set_soilMin", set_soilMinHumidity);

    pub_topic("time_water", t_water / 1000);

    pub_topic("valve_status", 0);
}

// setUP function
void setup()
{
    Wire.begin();
    Wire.setClock(10000);
    sht.begin(0x44);
    lightMeter.begin();
    MCP.begin();
    MCP.pinMode8(0x00);
    Serial.begin(115200);

    // setup Wifi
    // SSDID, PASSWRORD

    // ห้องบ้าน
    // WiFi.begin("house 5_2.4GHz", "SPAI40000");

    //  Dev ทำงานในห้องงาน
    // WiFi.begin("SUPER AI6_2.4GHz", "SPAI40000");

    //  Dev หน้างาน
    WiFi.begin("IoT_2.4GHz", "SPAI40000");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
    Serial.println(String("++++++++++++++++++++++++++"));
    Serial.println(String("Hello! PANGPURIYE  Farm!"));
    Serial.println(String("=========================="));

    Netpieclient.setServer(Netpiemqtt_server, Netpiemqtt_port);

    Netpieclient.setCallback(Netpiecallback);
    LED_WIFI = 26;
    LED_SERVER = 27;

    // set PinRealy
    setPin_Relay(32, 33, 25, 26);
    setPin_ErrorSensor(19, 18, 5);

    type_RTC = 0;

    // setup default value
    set_Maxtemperature = 27;
    setime_mintemperature = 22;
    set_MaxHumidity = 55;
    setime_minHumidity = 45;
    set_soilMaxHumidity = 65;
    set_soilMinHumidity = 55;

    sentData();
}

/**
 * Sets the relay pin to open water if the current time matches the specified time.
 * If manual mode is enabled, turns off the relay pin 0 .
 */
void SetTimeOpenWater()
{
    // Check if the current time matches the specified time
    if (bfarmtime.getHour() == time_hour && bfarmtime.getMinute() == time_min)
    {
        // Turn on the relay to open water
        digitalWrite(const_relay_pin[0], HIGH);
        // Delay for the specified water duration
        delay(t_water);
        // Turn off the relay after the water duration
        digitalWrite(const_relay_pin[0], LOW);
        pub_topic("valve_status", 1);
    }
    // If manual mode is enabled, make sure the relay is off
    else if (manual == 0)
    {
        digitalWrite(const_relay_pin[0], LOW);
        pub_topic("valve_status", 0);
    }
}

/**
 * Update Time to netpile
 */
void updatesetTime()
{
    //  setupdate Time
    pub_topic("time_hour", time_hour);
    pub_topic("time_min", time_min);
}

void Serial_Monitor()
{
    // Display  Data in Seral
    Serial.print(String("Temp = "));
    Serial.println((sht.getTemperature()));
    Serial.print(String("Hump = "));
    Serial.println((sht.getHumidity()));
    Serial.print(String("Soil = "));
    Serial.println(ReadAnalog_from_MPC3424(3, 1380000, 424000, 0, 100));
    Serial.print(String("Light = "));
    Serial.println((lightMeter.readLightLevel()));
}

/*
loop
*/
void loop()
{
    sht.read();
    Serial.println(sht.getTemperature(), 1);

    // check  Netpile  is connnected network!
    if (!Netpieclient.connected())
    {
        while (!Netpieclient.connected())
        {
            Serial.print("Attempting NETPIE2020 connection...");
            if (Netpieclient.connect(Netpiemqtt_Client, Netpiemqtt_username,
                                     Netpiemqtt_password))
            {
                Serial.println("NETPIE2020 connected");
                Netpieclient.subscribe("@private/#");
            }
            else
            {
                Serial.print("failed, rc=");
                Serial.print(client.state());
                Serial.println("try again in 5 seconds");
                delay(5000);
            }
        }
    }

    Netpieclient.loop();

    // Sync Time with wifi
    bfarmtime.sync();

    // time now
    currentMillis = millis();

    // Send data to Netpie every  interval time
    if (currentMillis - previousMillis >= interval)
    {
        // Update the previousMillis to the current time
        previousMillis = currentMillis;

        // Sent data to Netpile

        // sent Sencer Data
        pub_topic("temperature", (sht.getTemperature()));
        pub_topic("humidity", (sht.getHumidity()));
        pub_topic("light", (lightMeter.readLightLevel()));
        pub_topic("soil", ReadAnalog_from_MPC3424(3, 1380000, 424000, 0, 100));

        //  Send data to Netpie
        pub_topic("Sent_Data", count);

        // Increment count for tracking purposes
        count += 1;
    }

    // call function set time  opeon Water

    Serial_Monitor();
    updatesetTime();
    SetTimeOpenWater();
    //  delay 15 sec
    delay(30000);
    Serial.println(String("----End----"));
}
