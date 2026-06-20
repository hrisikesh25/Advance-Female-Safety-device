#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// GPS
SoftwareSerial gpsSerial(4, 3);   // RX, TX
TinyGPSPlus gps;

// GSM
SoftwareSerial gsmSerial(7, 8);   // RX, TX

// Panic Button
const int buttonPin = 2;

// Emergency Number
String phoneNumber = "+91XXXXXXXXXX";

void setup()
{
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);

  gpsSerial.begin(9600);
  gsmSerial.begin(9600);

  delay(3000);

  Serial.println("Women Safety Device Started");

  gsmSerial.println("AT");
  delay(1000);

  gsmSerial.println("AT+CMGF=1"); // SMS Text Mode
  delay(1000);
}

void loop()
{
  while (gpsSerial.available())
  {
    gps.encode(gpsSerial.read());
  }

  if (digitalRead(buttonPin) == LOW)
  {
    Serial.println("Emergency Button Pressed");

    sendEmergencySMS();

    delay(10000);
  }
}

void sendEmergencySMS()
{
  float latitude = 0;
  float longitude = 0;

  unsigned long start = millis();

  while (millis() - start < 10000)
  {
    while (gpsSerial.available())
    {
      gps.encode(gpsSerial.read());

      if (gps.location.isUpdated())
      {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
      }
    }
  }

  String message = "HELP! I am in danger.\n";

  if (gps.location.isValid())
  {
    message += "Location:\n";
    message += "https://maps.google.com/?q=";
    message += String(latitude, 6);
    message += ",";
    message += String(longitude, 6);
  }
  else
  {
    message += "GPS Location Not Available";
  }

  gsmSerial.println("AT+CMGF=1");
  delay(1000);

  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(phoneNumber);
  gsmSerial.println("\"");

  delay(1000);

  gsmSerial.print(message);

  delay(500);

  gsmSerial.write(26); // CTRL+Z

  delay(5000);

  Serial.println("Emergency SMS Sent");
}
