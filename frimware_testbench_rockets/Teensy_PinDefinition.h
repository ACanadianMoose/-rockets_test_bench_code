#ifndef TEENSYPINDEFH
#define TEENSYPINDEFH

// Definition des pins du Teensy 4.0 pour le projet RHINO.

// COMM SPI
#define PIN_MOSI        11
#define PIN_MISO        12
#define PIN_MSCK        13

// MAX31856
#define PIN_CS_T0       8
#define PIN_CS_T1       5
#define PIN_CS_T2       3
#define PIN_CS_T3       1
#define PIN_DATARDY_T0  7
#define PIN_DATARDY_T1  6
#define PIN_DATARDY_T2  4
#define PIN_DATARDY_T3  2

// DAC MAX5725
#define PIN_CS_DAC      16
#define PIN_IRQ_DAC     17

// COMM I2C (Ecran LCD)
#define PIN_SDA0        18
#define PIN_SCL0        19

// SparkFun HX711
#define PIN_LC50_DAT    14
#define PIN_LC50_CLK    15
#define PIN_LC350_DAT   9
#define PIN_LC350_CLK   10

// Autres pins
#define PIN_PUSH_BTN      0
#define PIN_LED           21
#define PIN_PRESSURE      22
#define PIN_LOW_VOLT_WARN 23



#endif