#include "src/MAX5725/MAX5725.h"


MAX5725 dac = MAX5725(16, &SPI);

void setup() {
  // put your setup code here, to run once:
  printf("Allo");
  dac.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  set_all_dac(400);
  delay(5000);
  set_all_dac(650);
  delay(5000);

}

void set_all_dac(double value){
  const MAX5725_chanel_t channel_used[6] = { OUT_0, OUT_1, OUT_2, 
                                    OUT_3, OUT_4, OUT_5};
  for(int i=0; i<6; i++){
    dac.load_value_adjusted(channel_used[i], value, -100, 900);
  }
}

