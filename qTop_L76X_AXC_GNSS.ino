/*****************************************************************************
  This is the Demo of using qTop GNSS L76X shield 
  plugged into Arduino MKR Compatible or Adafruit Feather Compatible board.

  Hardware Setup:
    Arduino MKR Compatible or Adafruit Feather Compatible board;
    qTop-L76-Int-AFC / qTop-L76-Int-AMC or qTop-L76-Ext-AFC / qTop-L76-Ext-AMC (IBT-QTN-AFC-L76-I / IBT-QTN-AMC-L76-I / 
        IBT-QTN-AFC-L76-E / IBT-QTN-AMC-L76-E) with GNSS antenna connected for "E" options.

    * For qBoard-A or qBoard-B "ESP32 Dev Module" should be choosen as board in Arduino IDE.

  Project HW Option:
    You need to set these defines:
      - IOT_BOARD_TYPE : CPU Board type
      - QTOP_GNSS_SHIELD_TYPE : qTop GNSS Shield type
      - LOG_GPS : GNSS Receiver ASCII Output Logging

..Libs used:
    Adafruit_GPS by Adafruit
    GPS_HardwareSerial_Parsing by Adafruit

..Examples used:
    GPS_HardwareSerial_Parsing.ino by Adafruit

====================================================================================================
Revision History:

Author                          Date                Revision NUmber          Description of Changes
------------------------------------------------------------------------------------

iotbotscom                04/18/2023               1.0.0                        Initial release

*****************************************************************************/
// Includes
#include <Adafruit_GPS.h>
#include <stdio.h>

// IOT Board options
#define IOT_BOARD_TYPE_HUZZAH32     1  // ESP32 Adafruit Feather IOT board
#define IOT_BOARD_TYPE_QBOARDA      2  // ESP32 Arduino MKR Compatible (AMC) IOT board
#define IOT_BOARD_TYPE_MKR          3  // Arduino MKR IOT board
#define IOT_BOARD_TYPE_QBOARDB      4  // ESP32 Adafruit Feather Compatible (AFC) IOT board
#define IOT_BOARD_TYPE_QBOARDX      5  // ESP32 Arduino MKR Compatible (AMC) QWARKS IOT board

// qTop Shield options
#define QTOP_GNSS_SHIELD_TYPE_L76_INT       1
#define QTOP_GNSS_SHIELD_TYPE_L76_EXT       2


// Project HW option : Adafruit Feather Huzzah ESP32 Board + qTop GNSS Patch Antenna AFC Shield
#define IOT_BOARD_TYPE              IOT_BOARD_TYPE_HUZZAH32
#define QTOP_GNSS_SHIELD_TYPE       QTOP_GNSS_SHIELD_TYPE_L76_INT

// Project HW option : Arduino MKR Board + qTop GNSS Patch Antenna AMC Shield
//#define IOT_BOARD_TYPE              IOT_BOARD_TYPE_MKR
//#define QTOP_GNSS_SHIELD_TYPE       QTOP_GNSS_SHIELD_TYPE_L76_INT

// Project HW option : qBoardA (Arduino MKR Compatible Board) + qTop BG95 AMC Shield
//#define IOT_BOARD_TYPE              IOT_BOARD_TYPE_QBOARDA
//#define QTOP_GNSS_SHIELD_TYPE       QTOP_GNSS_SHIELD_TYPE_L76_INT

// Project HW option : qBoardB (Feather Compatible Board) + qTop GNSS Patch Antenna AFC Shield
//#define IOT_BOARD_TYPE              IOT_BOARD_TYPE_QBOARDB
//#define QTOP_GNSS_SHIELD_TYPE       QTOP_GNSS_SHIELD_TYPE_L76_INT

///////////////////////////////////////////////////////////////////////////////

// Modem HW Pins
#if (defined IOT_BOARD_TYPE) && (IOT_BOARD_TYPE == IOT_BOARD_TYPE_HUZZAH32)
// Huzzah ESP32
#define GPS_ON_PIN          13
#define GPS_RESET_PIN       32
#elif (defined IOT_BOARD_TYPE) && (IOT_BOARD_TYPE == IOT_BOARD_TYPE_QBOARDA)
//qBoard-A
#define GPS_ON_PIN          2
#define GPS_RESET_PIN       27
#elif (defined IOT_BOARD_TYPE) && (IOT_BOARD_TYPE == IOT_BOARD_TYPE_QBOARDB)
//qBoard-B
#define GPS_ON_PIN          5
#define GPS_RESET_PIN       32
#elif (defined IOT_BOARD_TYPE) && (IOT_BOARD_TYPE == IOT_BOARD_TYPE_MKR)
//Arduino MKR
#define GPS_ON_PIN          20
#define GPS_RESET_PIN       6
#else
#error IOT_BOARD_TYPE is not defined, please define.
#endif

// Module Serial
#if (defined(ESP32))
// ESP32 specific
#define GPSSerial Serial2
#else
// AVR specific
#define GPSSerial Serial1
#endif

// GPS Object
Adafruit_GPS GPS(&GPSSerial);

// GPS Info Timer
uint32_t timer = millis();

// Set LOG_GPS to 'true' to see GPS Receiver UART output
//#define LOG_GPS true
#define LOG_GPS false

// Printf Buf
char printf_buf[32];

///////////////////////////////////////////////////////////////////////////////
void setup() {

  // Set console baud rate
  Serial.begin(115200);

  // To get Serial Up and running, especially for MKR boards
  delay(5000);

  Serial.print("\r\n***************************************************************************************************\r\n");
  Serial.print("\r\n **** IOT-BOTS.COM **** \r\n");
  Serial.print("\r\n **** qTop Quectel L76X GNSS Shield Demo **** \r\n");
#if (defined IOT_BOARD_TYPE) && (IOT_BOARD_TYPE == IOT_BOARD_TYPE_HUZZAH32)
  Serial.print("\r\n **** Adafruit Feather ESP32 Board **** \r\n");
#elif (defined IOT_BOARD_TYPE) && (IOT_BOARD_TYPE == IOT_BOARD_TYPE_MKR)
  Serial.print("\r\n **** Arduino MKR (MKRZERO) Board **** \r\n");
#elif (defined IOT_BOARD_TYPE) && (IOT_BOARD_TYPE == IOT_BOARD_TYPE_QBOARDA)
  Serial.print("\r\n **** IOT-BOTS qBoard-A (MKR Compatible) Board **** \r\n");
#elif (defined IOT_BOARD_TYPE) && (IOT_BOARD_TYPE == IOT_BOARD_TYPE_QBOARDB)
  Serial.print("\r\n **** IOT-BOTS qBoard-B (Feather Compatible) Board **** \r\n");
#else
  Serial.print("\r\n **** !!! No Controller Board Defined !!! **** \r\n");
#endif
  Serial.print("\r\n***************************************************************************************************\r\n");

  Serial.println("\r\n---- Setup Started ----\r\n");

  // GPIO Init
  pinMode(GPS_ON_PIN, OUTPUT);
  digitalWrite(GPS_ON_PIN, LOW);

  pinMode(GPS_RESET_PIN, OUTPUT);
  digitalWrite(GPS_RESET_PIN, LOW);

  // GPS UART Init
  GPS.begin(9600);

  // Module On Begin
  digitalWrite(GPS_ON_PIN, HIGH);

  delay(100);

  digitalWrite(GPS_RESET_PIN, HIGH);
  // Module On End

  Serial.println("\r\n---- Setup Done ----");
}

///////////////////////////////////////////////////////////////////////////////
void loop() {

  char c = 0;

  // Read data from the GPS UART
   c = GPS.read();

  // GPS Output Logging, if enabled
  if(LOG_GPS == true)
  {
    if(c)
    {
        Serial.print(c);
    }
  }

  // Process NMEA Sentence
  if(GPS.newNMEAreceived())
  {
    //Serial.print(GPS.lastNMEA());

    if(GPS.parse(GPS.lastNMEA()))
    {
      // Print out GPS Info every 3 sec
      if(millis() - timer > 3000)
      {
        timer = millis();

        sprintf(printf_buf, "Date : %02d:%02d:%02d\r\n", GPS.day, GPS.month, GPS.year);
        Serial.print(printf_buf);
        sprintf(printf_buf, "Time : %02d:%02d:%02d\r\n", GPS.hour, GPS.minute, GPS.seconds);
        Serial.print(printf_buf);
        sprintf(printf_buf, "Fix : %d\r\n", (int)GPS.fix);
        Serial.print(printf_buf);
        sprintf(printf_buf, "Quality : %d\r\n", (int)GPS.fixquality);
        Serial.print(printf_buf);
        if(GPS.fix)
        {
          Serial.print("Location: ");
          Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
          Serial.print(", ");
          Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);

          sprintf(printf_buf, "Speed : %d\r\n", (int)GPS.speed);
          Serial.print(printf_buf);
          sprintf(printf_buf, "Altitude : %d\r\n", (int)GPS.altitude);
          Serial.print(printf_buf);
          sprintf(printf_buf, "Heading : %d\r\n", (int)GPS.angle);
          Serial.print(printf_buf);
          sprintf(printf_buf, "Sats : %d\r\n", (int)GPS.satellites);
          Serial.print(printf_buf);
        }
        Serial.println("");
      }
    }

  }
}
