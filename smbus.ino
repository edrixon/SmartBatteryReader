/*
 * 
 * 
 * Ed's SMbus reader for smart batteries
 *  - display on 16x2 LCD display, supports DFRobot RGB backlight or Adafruit/Waveshare I2C modules
 *  - display on serial port - 115200 baud, uses VT100 escape codes to position the cursor, so doesn't work with IDE serial monitor
 *  - LCD displays data in six pages, for three seconds per page
 *  - serial display updates every ten seconds
 *  - I2C bus runs at 100kHz
 * 
 * Hardware
 *  - doesn't need pull-up resistors on I2C bus because display modules have them fitted
 *  - SCA goes to "D" on battery
 *  - SCL goes to "C" on battery
 *  - "T" on battery not connected
 *  - battery used to power processor and display
 *  - dedicated hardware used ATMega328 @16MHz, programmed with USB serial module from Arduino IDE as an Uno
 *  - power from 7805 regulator
 *  - battery connector is AMP/TE 5787446-1
 * 
 */

#include <Wire.h>

#define DFR_DISPLAY                         // DFRobot display module
//#define ADAFRUIT_DISPLAY                  // Adafruit display module
#define WITH_SERIAL                         // Send output to serial port as well

#define BUS_ADDRESS   0x0b                  // Bus address for batteries - defined in SMbus standard
#define BAUD_RATE     115200                // Serial output speed
#define IIC_SPEED     100000                // I2C bus speed
#define TICK_MS       100                   // Tick time
#define LCD_TICKS     30                    // Ticks between LCD pages
#define SERIAL_TICKS  100                   // Ticks between serial display updates
#define LCD_PAGES     6                     // Number of pages of LCD information
#define LCD_COLUMNS   16                    // Number of columns on LCD
#define LCD_ROWS      2                     // Number of rows on LCD
#define LCD_BUS_ADDR  0x2d                  // I2C bus address for DFRobot display

#include "ArduinoSMBus.h"

unsigned long int lastMillis;
boolean ledOn;
unsigned long int tickCount;
int lcdPage;

ArduinoSMBus battery(BUS_ADDRESS);

#ifdef ADAFRUIT_DISPLAY

#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void lcdSetup()
{
    Serial.println("Built for Adafruit LCD");
    
    lcd.begin(LCD_COLUMNS, LCD_ROWS);
    lcd.setBacklight(1);
}

#endif

#ifdef DFR_DISPLAY

#include "DFRobot_RGBLCD1602.h"

DFRobot_RGBLCD1602 lcd(LCD_BUS_ADDR, LCD_COLUMNS, LCD_ROWS);  //16 characters and 2 lines of show

void lcdSetup()
{
    Serial.println("Built for DFRobot LCD");
  
    lcd.init();
//    lcd.setRGB(255, 255, 255);
    lcd.setRGB(255, 255, 0);
}

#endif

void showBatteryLCD()
{ 
    unsigned short int mDate;
    BatteryMode mode;
    int x;
    
    mode = battery.batteryMode();

    lcd.clear();
    lcd.setCursor(0, 0);

    switch(lcdPage)
    {
        case 0:
            lcd.print("Mkr : ");
            lcd.print(battery.manufacturerName());
            lcd.setCursor(0, 1);
            lcd.print("Type: ");
            lcd.print(battery.deviceName());
            break;

        case 1:
            lcd.print("Design capacity");
            lcd.setCursor(0, 1);
            lcd.print(battery.designVoltage() / 1000.0);
            lcd.print("V, ");
            lcd.print(battery.designCapacity() / 1000.0);
            if(mode.capacity_mode == 1)
            {
                lcd.print("Wh");
            }
            else
            {
                lcd.print("Ah");
            }
            break;

        case 2:
            mDate = battery.manufactureDate();
            lcd.print("Date: ");
            x = mDate &0x001f;
            if(x < 10)
            {
                lcd.print("0");
            }
            lcd.print(x);
            lcd.print("-");
            x = (mDate & 0x01e0) >> 5;
            if(x < 10)
            {
                lcd.print("0");
            }
            lcd.print(x);
            lcd.print("-");
            lcd.print(((mDate & 0xfe00) >> 9) + 1980);
            lcd.setCursor(0, 1);
            lcd.print("S/N : ");
            lcd.print(battery.serialNumber());
            break;

        case 3:
            lcd.print("Temp  : ");
            lcd.print(battery.temperatureC() / 10.0);
            lcd.print("C");
            lcd.setCursor(0, 1);
            lcd.print("Cycles: ");
            lcd.print(battery.cycleCount());
            break;

        case 4:
            lcd.print("Charge: ");
            lcd.print(battery.absoluteStateOfCharge());
            lcd.print("%");
            break;

        case 5:
            lcd.print("Est. capacity");
            lcd.setCursor(0, 1);
            lcd.print(battery.fullCapacity() / 1000.0);
            if(mode.capacity_mode == 1)
            {
                lcd.print("Wh");
            }
            else
            {
                lcd.print("Ah");
            }
            break;

        case 6:
            lcd.print("Est. remaining");
            lcd.setCursor(0, 1);
            lcd.print(battery.remainingCapacity() / 1000.0);
            if(mode.capacity_mode == 1)
            {
                lcd.print("Wh");
            }
            else
            {
                lcd.print("Ah");
            }
            break;

        default:
            lcdPage = 0;
    }

    if(lcdPage == LCD_PAGES)
    {
        lcdPage = 0;
    }
    else
    {
        lcdPage++;
    }
}

#ifdef WITH_SERIAL

char cls[] = { 0x1b, '[', '2', 'J', 0x1b, '[', 'H', '\0' };

void gotoXY(int x, int y)
{
    char mv[16];

    sprintf(mv, "%c[%d;%dH", 0x1b, x, y);
    Serial.print(mv);
}

void ln(int l, char fillChar)
{
    int x;

    gotoXY(l, 1);
    Serial.print("|");
    for(x = 0; x < 61; x++)
    {
        Serial.print(fillChar);
    }
    Serial.print("|");
}

void showBattery()
{ 
    unsigned short int mDate;
    BatteryMode mode;
    
    mode = battery.batteryMode();

    Serial.print(cls);

    ln(1, '-');
    ln(2, ' ');
    ln(3, '-');
    ln(4, ' ');
    ln(5, ' ');
    ln(6, ' ');
    ln(7, '-');
    ln(8, ' ');
    ln(9, ' ');
    ln(10, ' ');
    ln(11, '-');

    gotoXY(2, 21);
    Serial.print("Ed's SMbus battery reader");

    gotoXY(4, 4);
    Serial.print("Manufacturer    : ");
    Serial.print(battery.manufacturerName());

    gotoXY(4, 34);
    Serial.print("Type            : ");
    Serial.print(battery.deviceName());

    gotoXY(6, 4);
    Serial.print("Voltage         : ");
    Serial.print(battery.designVoltage() / 1000.0);
    Serial.print("V");

    gotoXY(6, 34);
    Serial.print("Capacity        : ");
    Serial.print(battery.designCapacity() / 1000.0);
    if(mode.capacity_mode == 1)
    {
        Serial.println("Wh");
    }
    else
    {
        Serial.println("Ah");
    }

    mDate = battery.manufactureDate();
    gotoXY(5, 4);
    Serial.print("Manufacture date: ");
    Serial.print(mDate & 0x001f);
    Serial.print("-");
    Serial.print((mDate & 0x01e0) >> 5);
    Serial.print("-");
    Serial.print(((mDate & 0xfe00) >> 9) + 1980);

    gotoXY(5, 34);
    Serial.print("Serial number   : ");
    Serial.print(battery.serialNumber());

    gotoXY(8, 4);
    Serial.print("Temperature     : ");
    Serial.print(battery.temperatureC() / 10.0);
    Serial.print("C");

    gotoXY(8, 34);
    Serial.print("Cycle count     : ");
    Serial.print(battery.cycleCount());

    gotoXY(9, 4);
    Serial.print("Charge          : ");
    Serial.print(battery.absoluteStateOfCharge());
    Serial.println("%");

    gotoXY(10, 4);
    Serial.print("Est. capacity   : ");
    Serial.print(battery.fullCapacity() / 1000.0);
    if(mode.capacity_mode == 1)
    {
        Serial.print("Wh");
    }
    else
    {
        Serial.print("Ah");
    }

    gotoXY(10, 34);
    Serial.print("Est. remaining  : ");
    Serial.print(battery.remainingCapacity() / 1000.0);
    if(mode.capacity_mode == 1)
    {
        Serial.print("Wh");
    }
    else
    {
        Serial.print("Ah");
    }
}

#else

void showBattery()
{
}

#endif

void serialSetup()
{
    Serial.begin(BAUD_RATE);
    Serial.println();
    Serial.println("Ed's SMbus battery reader");
    Serial.println();
}

void setup()
{
    serialSetup();
  
    lcdSetup();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Battery reader");
    lcd.setCursor(0, 1);
    lcd.print("Ed Rixon, 2024");

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    ledOn = false;

    delay(5000);

    lastMillis = millis();
    tickCount = 0;
    lcdPage = 0;

    Wire.setClock(IIC_SPEED);
}

void loop()
{
    unsigned long int nowMillis;
  
    nowMillis = millis();
    if(nowMillis - lastMillis > TICK_MS)
    {
        lastMillis = millis();
        if(ledOn == true)
        {
            ledOn = false;
            digitalWrite(LED_BUILTIN, LOW);
        }
        else
        {
            ledOn = true;
            digitalWrite(LED_BUILTIN, HIGH);
        }

        if(tickCount % LCD_TICKS == 0)
        {
            showBatteryLCD();
        }

        if(tickCount % SERIAL_TICKS == 0)
        {
            showBattery();
        }

        tickCount++;
    }
}
