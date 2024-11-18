#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

#include "ArduinoSMBus.h"

unsigned long int lastMillis;
boolean ledOn;
unsigned long int tickCount;
int lcdScreen;

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
ArduinoSMBus battery(0x0b); // RRC batteries are always 0x0b

char cls[] = { 0x1b, '[', '2', 'J', 0x1b, '[', 'H', '\0' };

void showBatteryLCD()
{ 
    unsigned short int mDate;
    BatteryMode mode;
    int x;
    
    mode = battery.batteryMode();

    lcd.clear();
    lcd.setCursor(0, 0);

    switch(lcdScreen)
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
            lcdScreen = 0;
    }

    if(lcdScreen == 6)
    {
        lcdScreen = 0;
    }
    else
    {
        lcdScreen++;
    }
}

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

void setup()
{
    Serial.begin(9600);
    Serial.println();
    Serial.println("Ed's SMbus battery reader");
    Serial.println();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    ledOn = false;

    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Battery reader");
    lcd.setCursor(0, 1);
    lcd.print("Ed Rixon, 2024");
    lcd.setBacklight(1);
    delay(5000);

    lastMillis = millis();
    tickCount = 0;
    lcdScreen = 0;
}

void loop()
{
    unsigned long int nowMillis;
  
    nowMillis = millis();
    if(nowMillis - lastMillis > 1000)
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

        if(tickCount % 3 == 0)
        {
            showBatteryLCD();
        }

        if(tickCount % 10 == 0)
        {
            showBattery();
        }

        tickCount++;
    }
}
