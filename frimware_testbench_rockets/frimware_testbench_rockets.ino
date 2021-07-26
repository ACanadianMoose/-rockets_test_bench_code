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
*                   X Communiquer avec MAX31856
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
#include <Adafruit_MAX31856.h>
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
long weigth50, weigth350 = 0;

// Variables utilisees par les MAX31856
Adafruit_MAX31856 TC0 = Adafruit_MAX31856(PIN_CS_T0);
Adafruit_MAX31856 TC1 = Adafruit_MAX31856(PIN_CS_T1);
Adafruit_MAX31856 TC2 = Adafruit_MAX31856(PIN_CS_T2);
Adafruit_MAX31856 TC3 = Adafruit_MAX31856(PIN_CS_T3);
float tempT0, tempT1, tempT2, tempT3 = 0;

bool calibrationMode = 0;

void setup() {
  // setup() est appele une seule fois au debut du programme.
  
  // Initialisation des I/O
  pinMode(PIN_PUSH_BTN, INPUT_PULLUP);

  // Initalisation de la communication serie.
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

  // Initialisation des MAX31856
  pinMode(PIN_DATARDY_T0, INPUT); // Les signaux DRDY en input.
  pinMode(PIN_DATARDY_T1, INPUT);
  pinMode(PIN_DATARDY_T2, INPUT);
  pinMode(PIN_DATARDY_T3, INPUT);
  if(!TC0.begin()) Serial.print("TC0 - error initialising\n");   // Demmarage de la comm SPI
  if(!TC1.begin()) Serial.print("TC1 - error initialising\n");   // et des settings par defaut
  if(!TC2.begin()) Serial.print("TC2 - error initialising\n");   // des MAX31856
  if(!TC3.begin()) Serial.print("TC3 - error initialising\n");
  TC0.setThermocoupleType(MAX31856_TCTYPE_K); // Utilisation de thermocouple
  TC1.setThermocoupleType(MAX31856_TCTYPE_K); // Type K.
  TC2.setThermocoupleType(MAX31856_TCTYPE_K);
  TC3.setThermocoupleType(MAX31856_TCTYPE_K);
  TC0.setConversionMode(MAX31856_CONTINUOUS); // Desactivation du moyennage
  TC1.setConversionMode(MAX31856_CONTINUOUS);
  TC2.setConversionMode(MAX31856_CONTINUOUS);
  TC3.setConversionMode(MAX31856_CONTINUOUS);

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

  delay(10);
  
  // Pour calculer la frequence d'operation
  unsigned long loopStartTime = micros();

  // Lecture de la cellule de charge de 50 Kg
  // Bloquant pour l'instant si les HX711 ne sont pas connectes
  while(!scale50.is_ready()) delay(1);     // PEUT ETRE INUTILE UNE FOIS LES MAXs IMPLEMENTES.
  weigth50 = read_Loadcell(&scale50);

  // Lecture de la cellule de charge de 350 Kg
  // Bloquant pour l'instant si les HX711 ne sont pas connectes
  while(!scale350.is_ready()) delay(1);    // PEUT ETRE INUTILE UNE FOIS LES MAXs IMPLEMENTES.
  weigth350 = read_Loadcell(&scale350);

  // Lecture des thermocouple. Seulement si DRDY est actif (signal LOW)
  if(!digitalRead(PIN_DATARDY_T0)){tempT0 = TC0.readThermocoupleTemperature();}
  if(!digitalRead(PIN_DATARDY_T1)){tempT1 = TC1.readThermocoupleTemperature();}
  if(!digitalRead(PIN_DATARDY_T2)){tempT2 = TC2.readThermocoupleTemperature();}
  if(!digitalRead(PIN_DATARDY_T3)){tempT3 = TC3.readThermocoupleTemperature();}


  // calcul de la freq d'operation
  unsigned long loopEndTime = micros();
  float freq = 1000000/float((loopEndTime - loopStartTime)); // 10^-9

  // Affichage des resultats sur le Serial
  Serial.println("freq :" + String(freq,2) + " - " + String(weigth50) + " - " + String(weigth350));
  Serial.println("Temperature : T0 : " + String(tempT0,2) + "- T1 : " + String(tempT1,2) + 
                             "- T2 : " + String(tempT2,2) + "- T3 : "+ String(tempT3,2));
}
