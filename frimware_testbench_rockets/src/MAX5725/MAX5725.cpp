#include "MAX5725.h"
#include <SPI.h>
#include <stdlib.h>

  MAX5725::MAX5725(uint8_t spi_cs, SPIClass *_spi)
    :  spi_dev(spi_cs, 1000000, SPI_BITORDER_MSBFIRST, SPI_MODE1, _spi){

  }
  
  void MAX5725::begin(void) {
    spi_dev.begin();

    //set ref at 2.5V always on
    set_ref(REF_2V5);

    //All power at normal
    set_power(0xFF, POWER_MODE_NORMAL);

    //for all dac, disable watchdog, gating, load pin and clear pin
    set_config(0xFF,CONFIG_DATA_L_WDOG_DIS, 0, 0, 0);

    //for all dacs, set default value to mid point (ref/2)
    set_default(0xFF, DEFAULT_DATA_L_VAL_MID);
  }


  void MAX5725::load_value(MAX5725_chanel_t ch, double value){
    double value_12bits_double;
    uint16_t value_12bits;

    if(value > 1){
      value = 1;
    }else if(value<0){
      value = 0;
    }

    value_12bits_double = value * (double)MAX_VALUE;
    value_12bits = value_12bits_double;

    load_value(ch,value_12bits);
  }


  void MAX5725::load_value(MAX5725_chanel_t ch, uint16_t value){
    uint8_t command;
    uint8_t dac_selection;
    uint8_t value_high;
    uint8_t value_low;

    switch (ch)
    {
    case OUT_0:
      dac_selection = DAC_0;
      break;
    case OUT_1:
      dac_selection = DAC_1;
      break;
    case OUT_2:
      dac_selection = DAC_2;
      break;
    case OUT_3:
      dac_selection = DAC_3;
      break;
    case OUT_4:
      dac_selection = DAC_4;
      break;
    case OUT_5:
      dac_selection = DAC_5;
      break;
    case OUT_6:
      dac_selection = DAC_6;
      break;
    case OUT_7:
      dac_selection = DAC_7;
      break;
    default:
      //should not happend, just in case
      dac_selection = DAC_ALL;
      break;
    }

    command = CODEn_LOAD | dac_selection;
    //we only consider the 12 least significant bit

    //the bits 3 to 0 from value are the 4 most significant bits of value_low
    value_low = 0XF0 & (value << 4);

    //the bits 11 to 4 from value are the 8 bits if value_high
    value_high = 0XFF &((uint16_t)value >> 4);

    writeData(command, value_high, value_low);
  }

  void MAX5725::load_value_adjusted(MAX5725_chanel_t ch, double value, 
      double minVal, double maxVal){
    
    //we adjust the value using a linear equation:
    // y = m x +b
    //where:
    // x is the parameter value
    // y is the value sent to the function load_calue (betwen 0 and 1)
    // m is the slope such that y=0 when x==minVal and y=1 when x==maxVal
    // b is the is the y intercept such that y=0 when x==minVal and y=1 when x==maxVal
    //
    //       val_adjusted
    //            ^
    //            |1     x
    //            |    x
    //            |  x
    //            |x
    //           x|
    //         x  |
    //       x    |
    //     x      |0
    //------------+--------------->val
    //   minVal         maxVal

    double m, b;
    double val_adjusted;

    m = 1/(maxVal-minVal); // m = deltaY/deltaX
    b = -1*m*minVal; //0=m*minVal+b  <=> b=-m*minVal

    //y=mx+b
    val_adjusted = m *value + b;

    if(val_adjusted > 1) val_adjusted = 1;
    if(val_adjusted < 0) val_adjusted = 0;

    load_value(ch, val_adjusted);
  }



  void MAX5725::set_ref(uint8_t refValue){
    writeData(refValue, DONT_CARE, DONT_CARE);
  }

  void MAX5725::set_power(uint8_t dac_selection, uint8_t mode){
    writeData(POWER, dac_selection, mode);
  }

  void MAX5725::set_config(uint8_t dac_selection, uint8_t wd, bool gate, bool ldac, bool clear){

    uint8_t data_low;

    //set the value to disable/enable gate/ldac/clr
    uint8_t set_gate = (gate) ? CONFIG_DATA_L_GATE_ENB: CONFIG_DATA_L_GATE_DIS;
    uint8_t set_ldac = (ldac) ? CONFIG_DATA_L_LDAC_ENB: CONFIG_DATA_L_LDAC_DIS;
    uint8_t set_clr = (clear) ? CONFIG_DATA_L_CLR_ENB: CONFIG_DATA_L_CLR_DIS;

    data_low = wd | set_gate | set_ldac | set_clr;
    
    writeData(CONFIG, dac_selection, data_low);
  }

  void MAX5725::set_default(uint8_t dac_selection, uint8_t default_val){
    writeData(DEFAULT_CONFIG, dac_selection, default_val);
  }


  void MAX5725::writeData(uint8_t command, uint8_t dataH, uint8_t dataL){
    uint8_t buffer[3] = {command, dataH, dataL};

    spi_dev.write(buffer, 3);
  }

