/**
 * @file ArduinoSMBus.cpp
 * @author Christopher Lee (clee@unitedconsulting.com)
 * @brief Function definitions for the ArduinoSMBus class.
 * @version 1.1
 * @date 2024-03-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ArduinoSMBus.h"

/**
 * @brief Construct a new ArduinoSMBus:: ArduinoSMBus object.
 * 
 * @param batteryAddress 
 */
ArduinoSMBus::ArduinoSMBus(uint8_t batteryAddress) {
  _batteryAddress = batteryAddress;
  Wire.begin();
}

/**
 * @brief Set the battery's I2C address.
 * Can be used to change the address after the object is created
 * @param batteryAddress 
 */
void ArduinoSMBus::setBatteryAddress(uint8_t batteryAddress) {
  _batteryAddress = batteryAddress;
}

/**
 * @brief Get the battery's remaining capacity alarm.
 * Returns the battery's remaining capacity alarm threshold value, in mAh.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::remainingCapacityAlarm() {
  return readRegister(REMAINING_CAPACITY_ALARM);
}

/**
 * @brief Get the battery's remaining time alarm.
 * Returns the battery's remaining time alarm threshold value, in minutes.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::remainingTimeAlarm() {
  return readRegister(REMAINING_TIME_ALARM);
}

/**
 * @brief Get the battery's mode.
 * 
 * This method reads the battery's mode register, which contains various settings and status bits.
 * It then creates a BatteryMode struct and sets its fields based on the bits in the mode.
 * 
 * @return BatteryMode A struct containing the following fields:
 * - internal_charge_controller: bit 0 of the mode register
 * - primary_battery_support: bit 1 of the mode register
 * - condition_flag: bit 7 of the mode register
 * - charge_controller_enabled: bit 8 of the mode register
 * - primary_battery: bit 9 of the mode register
 * - alarm_mode: bit 13 of the mode register
 * - charger_mode: bit 14 of the mode register
 * - capacity_mode: bit 15 of the mode register
 */
BatteryMode ArduinoSMBus::batteryMode() {
  // Read the battery mode from the device...
  uint16_t mode = readRegister(BATTERY_MODE);

  // Create a BatteryMode struct and set its fields based on the mode
  BatteryMode batteryMode;
  batteryMode.internal_charge_controller = mode & 0x0001;
  batteryMode.primary_battery_support = (mode >> 1) & 0x0001;
  batteryMode.condition_flag = (mode >> 7) & 0x0001;
  batteryMode.charge_controller_enabled = (mode >> 8) & 0x0001;
  batteryMode.primary_battery = (mode >> 9) & 0x0001;
  batteryMode.alarm_mode = (mode >> 13) & 0x0001;
  batteryMode.charger_mode = (mode >> 14) & 0x0001;
  batteryMode.capacity_mode = (mode >> 15) & 0x0001;

  // Return the struct
  return batteryMode;
}

/**
 * @brief Get the battery's temperature.
 * Returns the battery temperature in Kelvin.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::temperature() {
  return readRegister(TEMPERATURE);
}

/**
 * @brief Get the battery's temperature in Celsius.
 * Returns the battery temperature in 0.1 degrees Celsius.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::temperatureC() {
  uint16_t temperatureKelvin = readRegister(TEMPERATURE);
  uint16_t temperatureCelsius = temperatureKelvin - 2731; // Convert from Kelvin to Celsius
  return temperatureCelsius;
}

/**
 * @brief Get the battery's temperature in Fahrenheit.
 * Returns the battery temperature in 0.1 degrees Fahrenheit.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::temperatureF() {
  uint16_t temperatureKelvin = readRegister(TEMPERATURE);
  uint16_t temperatureFahrenheit = (temperatureKelvin * 18 - 45967) / 10; // Convert from Kelvin to Fahrenheit
  return temperatureFahrenheit;
}

/**
 * @brief Get the battery's voltage.
 * Returns the sum of all cell voltages, in mV.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::voltage() {
  return readRegister(VOLTAGE);
}



/**
 * @brief Get the battery's current.
 * Returns the battery measured current (from the coulomb counter) in mA.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::current() {
  return readRegister(CURRENT);
}

/**
 * @brief Get the battery's average current.
 * Returns the average current in a 1-minute rolling average, in mA.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::averageCurrent() {
  return readRegister(AVERAGE_CURRENT);
}

/**
 * @brief Get the battery's state of charge error.
 * Returns the battery's margin of error when estimating SOC, in percent
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::maxError() {
  return readRegister(MAX_ERROR);
}

/**
 * @brief Get the battery's current relative charge.
 * Returns the predicted remaining battery capacity as a percentage of fullChargeCapacity()
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::relativeStateOfCharge() {
  return readRegister(REL_STATE_OF_CHARGE);
}

/**
 * @brief Get the battery's absolute charge.
 * Returns the predicted remaining battery capacity as a percentage of designCapacity()
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::absoluteStateOfCharge() {
  return readRegister(ABS_STATE_OF_CHARGE);
}

/**
 * @brief Get the battery's capacity.
 * Returns the predicted battery capacity when fully charged, in mAh.
 * For some batteries, this may be in 10s of mWh, if the BatteryMode() register (0x03) is set that way
 * See protocol documentation for details.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::remainingCapacity() {
  return readRegister(REM_CAPACITY);
}

/**
 * @brief Get the battery's full capacity.
 * Returns the predicted battery capacity when fully charged, in mAh.
 * For some batteries, this may be in 10s of mWh, if the BatteryMode() register (0x03) is set that way
 * See protocol documentation for details.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::fullCapacity() {
  return readRegister(FULL_CAPACITY);
}

/**
 * @brief Get the battery's time to empty.
 * Returns the predicted time to empty, in minutes, based on current instantaneous discharge rate.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::runTimeToEmpty() {
  return readRegister(RUN_TIME_TO_EMPTY);
}

/**
 * @brief Get the battery's average time to empty.
 * Returns the predicted time to empty, in minutes, based on 1-minute rolling average discharge rate.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::avgTimeToEmpty() {
  return readRegister(AVG_TIME_TO_EMPTY);
}

/**
 * @brief Get the battery's time to full.
 * Returns the predicted time to full charge, in minutes, based on 1-minute rolling average charge rate.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::avgTimeToFull() {
  return readRegister(AVG_TIME_TO_FULL);
}

/**
 * @brief Get the battery's status.
 * 
 * This function reads the BatteryStatus register and returns a struct with its value.
 * The BatteryStatus register indicates various alarm conditions and states of the battery.
 * These include over charge, termination charge, over temperature, termination discharge,
 * remaining capacity, remaining time, initialization, discharging, fully charged, and fully discharged states.
 * 
 * @return BatteryStatus A struct containing the status of each bit in the BatteryStatus register.
 */
BatteryStatus ArduinoSMBus::batteryStatus() {
  uint16_t status = readRegister(BATTERY_STATUS);
  BatteryStatus batteryStatus;

  batteryStatus.over_charged_alarm = status & (1 << 15);
  batteryStatus.term_charge_alarm = status & (1 << 14);
  batteryStatus.over_temp_alarm = status & (1 << 12);
  batteryStatus.term_discharge_alarm = status & (1 << 11);
  batteryStatus.rem_capacity_alarm = status & (1 << 9);
  batteryStatus.rem_time_alarm = status & (1 << 8);
  batteryStatus.initialized = status & (1 << 7);
  batteryStatus.discharging = status & (1 << 6);
  batteryStatus.fully_charged = status & (1 << 5);
  batteryStatus.fully_discharged = status & (1 << 4);

  return batteryStatus;
}

/**
 * @brief Get the battery's design charging current.
 * Returns the desired design charging current of the battery, in mA.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::chargingCurrent() {
  return readRegister(CHARGING_CURRENT);
}

/**
 * @brief Get the battery's design charging voltage.
 * Returns the desired design charging voltage of the battery, in mV.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::chargingVoltage() {
  return readRegister(CHARGING_VOLTAGE);
}


/**
 * @brief Check if the battery status is OK.
 * Check for any alarm conditions in the battery status. These include over charge, 
 * termination charge, over temperature, termination discharge alarms. If any of these alarms are set, the battery is not OK.
 * 
 * @return bool True if the battery status is OK, false otherwise.
 */
bool ArduinoSMBus::statusOK() {
  BatteryStatus status = this->batteryStatus();
  return !(status.over_charged_alarm || status.term_charge_alarm || status.over_temp_alarm || 
           status.term_discharge_alarm);
}

/**
 * @brief  Get the battery's cycle count.
 * Returns the number of discharge cycles the battery has experienced.
 * A cycle is defined as an amount of discharge equal to the battery's design capacity.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::cycleCount() {
  return readRegister(CYCLE_COUNT);
}

/**
 * @brief Get the battery's design capacity.
 * Returns the theoretical maximum capacity of the battery, in mAh.
 * For some batteries, this may be in 10 mWh, if the BatteryMode() register (0x03) is set to CAPM 1.
 * See TI protocol documentation for details.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::designCapacity() {
  return readRegister(DESIGN_CAPACITY);
}

/**
 * @brief Get the battery's design voltage.
 * Returns the nominal voltage of the battery, in mV.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::designVoltage() {
  return readRegister(DESIGN_VOLTAGE);
}

/**
 * @brief  Get the battery's manufacture date.
 * Returns the date the battery was manufactured, in the following format: 
 * Day + Month*32 + (Year–1980)*512
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::manufactureDate() {
  return readRegister(MANUFACTURE_DATE);
}

/**
 * @brief Get the manufacture year from the manufacture date.
 * @return int 
 */
int ArduinoSMBus::manufactureYear() {
  uint16_t manufactureDate = this->manufactureDate();
  int year = ((manufactureDate >> 9) & 0x7F) + 1980;
  return year;
}

/**
 * @brief Get the Serial Number from the battery.
 * 
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::serialNumber() {
  return readRegister(SERIAL_NUMBER);
}

/**
 * @brief Get the Manufacturer Name from the battery.
 * 
 * @return const char* 
 */
const char* ArduinoSMBus::manufacturerName() {
  static char manufacturerName[21]; // 20 characters plus null terminator
  readBlock(MANUFACTURER_NAME, reinterpret_cast<uint8_t*>(manufacturerName), 20);
  manufacturerName[20] = '\0'; // Null-terminate the C-string
  return manufacturerName;
}

/**
 * @brief Get the Device Name from the battery.
 * 
 * @return const char* 
 */
const char* ArduinoSMBus::deviceName() {
  static char deviceName[21]; // Assuming the device name is up to 20 characters long
  readBlock(DEVICE_NAME, reinterpret_cast<uint8_t*>(deviceName), 20);
  deviceName[20] = '\0'; // Null-terminate the C-string
  return deviceName;
}

/**
 * @brief Get the Device Chemistry from the battery.
 * 
 * @return const char* 
 */
const char* ArduinoSMBus::deviceChemistry() {
  static char deviceChemistry[5];
  readBlock(DEVICE_CHEMISTRY, reinterpret_cast<uint8_t*>(deviceChemistry), 8);
  deviceChemistry[4] = '\0';
  return deviceChemistry;
}

/**
 * @brief Get the State of Health from the battery.
 * Returns the estimated health of the battery, as a percentage of design capacity
 * This command is not supported by all batteries.
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::stateOfHealth() {
  uint8_t data[2];
  readBlock(STATE_OF_HEALTH, data, 2);
  uint16_t stateOfHealth = (data[1] << 8) | data[0];
  return stateOfHealth;
}







/**
 * @brief Read a register from the battery.
 * Reads a standard 16-bit register from the battery.
 * @param reg 
 * @return uint16_t 
 */
uint16_t ArduinoSMBus::readRegister(uint8_t reg) {
//int16_t ArduinoSMBus::readRegister(int reg) {
  Wire.beginTransmission(_batteryAddress);
  Wire.write(reg);
  Wire.endTransmission();
  
  delay(10);
  
  Wire.requestFrom(_batteryAddress, 2);
  
  if(Wire.available()) {
    return Wire.read() | Wire.read() << 8;
  } else {
    return 0;
  }
}

/**
 * @brief Reads a block of data from the battery.
 * Length of block is specified by the length parameter.
 * @param reg 
 * @param data 
 * @param length 
 */
void ArduinoSMBus::readBlock(uint8_t reg, uint8_t* data, uint8_t length) {
//void ArduinoSMBus::readBlock(int reg, uint8_t* data, uint8_t length) {
  Wire.beginTransmission(_batteryAddress);
  Wire.write(reg);
  Wire.endTransmission(false);

  delay(10); // Add a small delay to give the device time to prepare the data

  uint8_t count = Wire.requestFrom(_batteryAddress, length + 1); // Request one extra byte for the length

  if (Wire.available()) {
    count = Wire.read(); // The first byte is the length of the block
  }

  for (uint8_t i = 0; i < count && i < length; i++) {
    if (Wire.available()) {
      data[i] = Wire.read();
    }
  }
}
