#ifndef MAX5725_H
#define MAX5725_H
#include <Adafruit_SPIDevice.h>
#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

typedef enum {
    OUT_0,
    OUT_1,
    OUT_2,
    OUT_3,
    OUT_4,
    OUT_5,
    OUT_6,
    OUT_7,
}MAX5725_chanel_t;

class MAX5725
{

public:
    /**
     * @brief Construct a new MAX5725 object
     * 
     * @param spi_cs pin to use as the chip select
     * @param _spi SPIClass object to use for SPI transaction
     */
    MAX5725(uint8_t spi_cs, SPIClass *_spi = &SPI);

    /**
     * @brief initialize the object and the chip.
     * 
     */
    void begin(void);

    /**
     * @brief load a value to the dac. 1.0 represent the maximum
     * of the dac and 0 represent the minimum of the dac
     * 
     * @param ch channel to use
     * @param value value between 0 and 1.0
     */
    void load_value(MAX5725_chanel_t ch, double value);

    /**
     * @brief load a value to the dac. 4095 represent the maximum
     * of the dac and 0 represent the minimum of the dac
     * 
     * @param ch channel to use
     * @param value value between 0 and 4095
     */
    void load_value(MAX5725_chanel_t ch, uint16_t value);
private:

  /**
   * @brief Set the voltage reference to use
   * 
   * @param refValue reference to use (REF_EXT, REF_2V5, REF_2V0, REF_4V1)
   */
  void set_ref(uint8_t refValue);

  /**
   * @brief Set the power configuration
   * 
   * @param dac_selection select the dac (each bit match a dac, bit 0 = dac 0)
   * @param mode select the mode (POWER_MODE_NORMAL, POWER_MODE_1K, POWER_MODE_100K, POWER_MODE_HI_Z)
   */
  void set_power(uint8_t dac_selection, uint8_t mode);

  /**
   * @brief Set the configuration
   * 
   * @param dac_selection select the dac (each bit match a dac, bit 0 = dac 0)
   * @param wd watch dog setting
   * @param gate enable software gating
   * @param ldac enable latching using latch pin
   * @param clear enable clearing using clear pin
   */
  void set_config(uint8_t dac_selection, uint8_t wd, bool gate, bool ldac, bool clear);

  /**
   * @brief Set the register "default"
   * 
   * @param dac_selection select the dac (each bit match a dac, bit 0 = dac 0)
   * @param default_val value to rigth to the buffer
   */
  void set_default(uint8_t dac_selection, uint8_t default_val);


  /**
   * @brief Wrtie data to the MAX5725. Every transaction follows the same
   * structure: first 8bit for the command buffer then 16 bit if data
   * 
   * @param command commend buffer to send
   * @param dataH 8 most significant bit of the data to send
   * @param dataL 8 leat significant bit of the data to send
   */
  void writeData(uint8_t command, uint8_t dataH, uint8_t dataL);

  Adafruit_SPIDevice spi_dev;
  bool initialized = false;


  static const uint16_t MAX_VALUE = 4095;

  //------------------command values------------------
  static const uint8_t WDOG = 0b00010000;
  
  //REF: 0b00100ABB (A: REF POWER, BB: REF MODE)
  static const uint8_t REF_EXT = 0b00100000;
  static const uint8_t REF_2V5 = 0b00100001;
  static const uint8_t REF_2V0 = 0b00100010;
  static const uint8_t REF_4V1 = 0b00100011;


  static const uint8_t SW_GATE_CLR = 0b00110000;
  static const uint8_t SW_GATE_SET = 0b00110001;


  static const uint8_t WD_REFRESH  = 0b00110010;
  static const uint8_t WD_RESET    = 0b00110010;

  static const uint8_t SW_CLEAR    = 0b00110100;
  static const uint8_t SW_RESET    = 0b00110101;

  static const uint8_t CONFIG      = 0b01010000;
  static const uint8_t POWER       = 0b01000000;
  static const uint8_t DEFAULT_CONFIG     = 0b01100000;

  //DAC "n" command
  //for all DAC "n" command, the last 4 bits are the DAC selections bits
  //and must be set before sending the command (ex: command = RETURNn | DAC_0 )
  static const uint8_t RETURNn         = 0b01110000;
  static const uint8_t CODEn           = 0b10000000;
  static const uint8_t LOADn           = 0b10010000;
  static const uint8_t CODEn_LOAD_ALL  = 0b10100000;
  static const uint8_t CODEn_LOAD      = 0b10110000;

  //DAC selection
  static const uint8_t DAC_0 = 0;
  static const uint8_t DAC_1 = 1;
  static const uint8_t DAC_2 = 2;
  static const uint8_t DAC_3 = 3;
  static const uint8_t DAC_4 = 4;
  static const uint8_t DAC_5 = 5;
  static const uint8_t DAC_6 = 6;
  static const uint8_t DAC_7 = 7;
  static const uint8_t DAC_ALL = 0b00001000;

  //DAC "ALL" command
  static const uint8_t CODE_ALL          = 0b11000000;
  static const uint8_t LOAD_ALL          = 0b11000001;
  static const uint8_t CODE_ALL_LOAD_ALL = 0b11000010;
  static const uint8_t RETURN_ALL        = 0b11000011;

  //SPI command
  
  //Similarly to the DAC's "n" command, the last 4 bits of SPI_DATA_REQUEST
  //are the DAC selections bits and must be set before sending the command 
  //(ex: command = SPI_DATA_REQUEST | DAC_0 )
  static const uint8_t SPI_DATA_REQUEST  = 0b11010000;

  //For the SPI_READ commands the third bit is the DPHA bit, it must be set
  //before sending the command (ex: command = SPI_READ_STATUS | DPHA_1) more
  //information about the effect of DPHA can be found on page 19 the datasheet
  static const uint8_t SPI_READ_STATUS   = 0b11100000;
  static const uint8_t SPI_READ_DATA     = 0b11101000;
  static const uint8_t DPHA_0            = 0b00000000;
  static const uint8_t DPHA_1            = 0b00000100;

  //No op
  static const uint8_t NO_OP_1   = 0b11000100;
  static const uint8_t NO_OP_2   = 0b11001000;
  static const uint8_t NO_OP_3   = 0b11001100;



  //------------------Data values------------------

  static const uint8_t DONT_CARE = 0;

  // SW
  //For all SW commands, the bits 0 to 7 must be 0b00110000 and the bits 15 
  //to 8 0b10010110 (SW_GATE_CLR, SW_CLEAR, ... are bit 16 to 23)
  static const uint8_t SW_DATA_H = 0b10010110;
  static const uint8_t SW_DATA_L  = 0b00110000;

  // WD
  //For all WD commands, the bits 0 to 7 must be 0b00110000 and the bits 15 
  //to 8 0b10010110 (WD_REFRESH, WD_RESET, ... are bit 16 to 23)
  static const uint8_t WD_DATA_H = 0b10010110;
  static const uint8_t WD_DATA_L  = 0b00110000;


  // POWER
  //data high: multiple DAC slection (note: do NOT use DAC_0, DAC_1, ..., DAC_ALL,
  // refer to data sheet p.26 for proper use)
  static const uint8_t POWER_DATA_L_MODE_NORMAL  = 0b00000000;
  static const uint8_t POWER_DATA_L_MODE_1K      = 0b01000000;
  static const uint8_t POWER_DATA_L_MODE_100K    = 0b10000000;
  static const uint8_t POWER_DATA_L_MODE_HI_Z    = 0b11000000;
  //alias
  static const uint8_t POWER_MODE_NORMAL  = POWER_DATA_L_MODE_NORMAL;
  static const uint8_t POWER_MODE_1K      = POWER_DATA_L_MODE_1K;
  static const uint8_t POWER_MODE_100K    = POWER_DATA_L_MODE_100K;
  static const uint8_t POWER_MODE_HI_Z    = POWER_DATA_L_MODE_HI_Z;
  //CONFIG
  //data high: multiple DAC slection (note: do NOT use DAC_0, DAC_1, ..., DAC_ALL,
  // refer to data sheet p.27 for proper use)

  //data low: combine WDOG, GATE, LDAC and CLR 
  //(ex: data[0]= CONFIG_DATA_L_WDOG_DIS | CONFIG_DATA_L_GATE_DIS | CONFIG_DATA_L_LDAC_DIS | CONFIG_DATA_L_CLR_ENB)
  static const uint8_t CONFIG_DATA_L_WDOG_DIS  = 0b00000000;
  static const uint8_t CONFIG_DATA_L_WDOG_GATE = 0b01000000;
  static const uint8_t CONFIG_DATA_L_WDOG_CLR  = 0b10000000;
  static const uint8_t CONFIG_DATA_L_WDOG_HOLD = 0b11000000;

  static const uint8_t CONFIG_DATA_L_GATE_DIS = 0b001000000;
  static const uint8_t CONFIG_DATA_L_GATE_ENB = 0b000000000;

  static const uint8_t CONFIG_DATA_L_LDAC_DIS = 0b000100000;
  static const uint8_t CONFIG_DATA_L_LDAC_ENB = 0b000000000;

  static const uint8_t CONFIG_DATA_L_CLR_DIS  = 0b000010000;
  static const uint8_t CONFIG_DATA_L_CLR_ENB  = 0b000000000;

  //DEFAULT
  //data high: multiple DAC slection (note: do NOT use DAC_0, DAC_1, ..., DAC_ALL,
  // refer to data sheet p.28 for proper use)
  static const uint8_t DEFAULT_DATA_L_VAL_MZ   = 0b00000000;
  static const uint8_t DEFAULT_DATA_L_VAL_ZERO = 0b00100000;
  static const uint8_t DEFAULT_DATA_L_VAL_MID  = 0b01000000;
  static const uint8_t DEFAULT_DATA_L_VAL_FULL = 0b01100000;
  static const uint8_t DEFAULT_DATA_L_VAL_RET  = 0b10000000;

  //READ_REQUEST
  //data high combined INC and SEL refer to datasheet p.29
  //ex: data[1] = SPI_DATA_REQUEST_DATA_H_INC_DIS | SPI_DATA_REQUEST_DATA_H_SEL_CODE
  static const uint8_t SPI_DATA_REQUEST_DATA_H_INC_ENB = 0b10000000;
  static const uint8_t SPI_DATA_REQUEST_DATA_H_INC_DIS = 0b00000000;

  static const uint8_t SPI_DATA_REQUEST_DATA_H_SEL_DAC = 0b00000000;
  static const uint8_t SPI_DATA_REQUEST_DATA_H_SEL_CODE= 0b00100000;
  static const uint8_t SPI_DATA_REQUEST_DATA_H_SEL_RET = 0b01000000;
  static const uint8_t SPI_DATA_REQUEST_DATA_H_SEL_WDT = 0b01000000;

  //data low: dont care
  static const uint8_t SPI_DATA_REQUEST_DATA_L         = 0b00000000;

  //SPI_READ_DATA
  //refer to p.30 of datasheet


  //SPI_READ_STATUS
  //data high: read back of watch dog satus
  //data low: read back of clear status



};

#endif