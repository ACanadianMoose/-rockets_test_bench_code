/*///////////////////////////////////////////////////////////////
* firmware_testbench_rockets.ino
* Description : Fichier principal pour la carte d'acquisition 
*               des donnees du banc d'essaie ROCKETS.
*
* 2021/07/22 - MBlais : 
*  - Version initial du code. Creation du .h Teensy_PinDefinition
*    contenant tous les assignations des pins du microcontroleur.
*  - Initialisation de la comm serie, comm I2C et les HX711.
*  - Ajout d'un mode de calibration si le btn S1 est pese.
*      Pour l'instant, utilise pour mettre les LC a zero.
* 
*  A IMPLÉMENTER : 
*    Loadcells    : - Conversion des valeurs en Kg
*                   - Envoyer les valeurs sur l'ADC (vers labjack)
*    
*    Thermocouple : - Activer SPI
*                   - Communiquer avec MAX3156
*                   - Envoyer valeurs sur l'ADC
*
*    Ecran LCD    : - Tester le I2C (3.3v -> 5v levelshift)
*                   - Ajouter l'info requise sur l'ecran
*                   - Ajuster backlight
*    
*////////////////////////////////////////////////////////////////


#include "Teensy_PinDefinition.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include "HX711_fnct.h"

// Initialisation des differentes fonctions du Teensy
LiquidCrystal_I2C lcd(0x27,16,2);

// Variables utilisees par les loadcells
HX711 scale50, scale350; 
const long LC50_OFFSET = -247000;   // A CHANGER
const long LC50_DIVIDER = 1;        // A CHANGER
const long LC350_OFFSET = -167000;  // A CHANGER
const long LC350_DIVIDER = 1;       // A CHANGER

bool calibrationMode = 0;

void setup() {
  // setup() est appele une seule fois au debut du programme.
  
  // Initialisation des I/O
  pinMode(PIN_PUSH_BTN, INPUT_PULLUP);

  Serial.begin(57600); 
  Serial.print("Serial Initialized.\n");


  Wire.begin(); // Init l'I2C du Teensy
  init_LCD();   // Init de l'ecran LCD.
  Serial.print("I2C interface initialized.\n");

  // Initialisation des circuits SparkFun HX711
  scale50.begin(PIN_LC50_DAT, PIN_LC50_CLK);
  scale50.set_scale(LC50_DIVIDER);
  scale50.set_offset(LC50_OFFSET);
  Serial.print("50 Kg Scale initialized.\n");

  scale350.begin(PIN_LC350_DAT, PIN_LC350_CLK);
  scale350.set_scale(LC350_DIVIDER);
  scale350.set_offset(LC350_OFFSET);
  Serial.print("350 Kg Scale initialized.\n");

  if(digitalRead(PIN_PUSH_BTN) == LOW) calibrationMode = 1;

}

void loop() {
  // put your main code here, to run repeatedly:

  if(calibrationMode){
  /* Cette section du code est appele selon si
   * le bouton est appuye lors du demarrage du systeme.
   * 
   * Sequence de calibration :
   * - "Tare" des deux loadcells (Remise a zero)
   * 
   */
    Serial.print("ENTERING TESTING MODE.\n");
    Serial.print("Setting scales to zero in 3 seconds\n");
    delay(3000);
    scale50.tare();
    scale350.tare();
    Serial.print("scale reseted to zero.\n");

    Serial.print("EXITING TESTING MODE.\n");
    delay(500);
    calibrationMode = 0;
  }

  
  // Pour calculer la frequence d'operation
  unsigned long loopStartTime = micros();

  // Lecture de la cellule de charge de 50 Kg
  while(!scale50.is_ready()) delay(1);     // PEUT ETRE INUTILE UNE FOIS LES MAXs IMPLEMENTES.
  long weigth50 = read_Loadcell(&scale50);

  // Lecture de la cellule de charge de 350 Kg
  while(!scale350.is_ready()) delay(1);    // PEUT ETRE INUTILE UNE FOIS LES MAXs IMPLEMENTES.
  long weigth350 = read_Loadcell(&scale350);


  // calcul de la freq d'operation
  unsigned long loopEndTime = micros();
  float freq = 1000000/float((loopEndTime - loopStartTime)); // 10^-9
  Serial.println("freq :" + String(freq,4) + " - " + String(weigth50) + " - " + String(weigth350));
}
