/*///////////////////////////////////////////////////////////////
* firmware_testbench_rockets.ino
* Description : Fichier principal pour la carte d'acquisition 
*               des donnees du banc d'essaie ROCKETS.
*
* - Possibilite d'ajouter un code d'initialisation lance seulement 
*   si le bouton est pese lors du demarrage du systeme. (non utilise)
*
* 2021/08/09 - MBlais :
*  - Version du code fonctionnel pour la demonstration finale 
*  
* 2021/07/22 - MBlais : 
*  - Version initial du code. Creation du .h Teensy_PinDefinition
*    contenant tous les assignations des pins du microcontroleur.
*  - Initialisation de la comm serie, comm I2C et les HX711.
*  - Ajout d'un mode de calibration si le btn S1 est pese.
*      Pour l'instant, utilise pour mettre les LC a zero.
*    
*////////////////////////////////////////////////////////////////

/*///////////////////////////////////////////////////////////////
*       INCLUDES REQUIS POUR LE FONCTIONNEMENT DU SYSTEME
*//////////////////////////////////////////////////////////////////////////////

// Teensy_PinDefinition.h contient toutes les assignations des
// pattes du Teensy 4.0. du systeme.
#include "Teensy_PinDefinition.h"

#include <LiquidCrystal_I2C.h>
#include <Adafruit_MAX31856.h>
#include "HX711.h"
#include "src/MAX5725/MAX5725.h"
#include "Bounce2.h"


/*///////////////////////////////////////////////////////////////
*             DEFINITION DES CONSTANTES DU SYSTEME
*//////////////////////////////////////////////////////////////////////////////

// Ces valeurs sont utiliser pour definir les minimums et les
// maximums des sorties 0-10v allant vers le Labjack.

//         Vout DAC
//            ^
//            |10    x
//            |    x
//            |  x
//            |x
//           x|
//         x  |
//       x    |
//     x      |0
//------------+--------------->val
//   minVal         maxVal

#define DAC_TC_MINVAL    -100  // degree C
#define DAC_TC_MAXVAL     400  // 
#define DAC_LC50_MINVAL   -5   // Kg
#define DAC_LC50_MAXVAL   50   // 
#define DAC_LC350_MINVAL  -5   // 
#define DAC_LC350_MAXVAL  350  // 

#define OUT_LC50  OUT_1  // definition des outputs du DAC
#define OUT_LC350 OUT_0  // MAX5725 selon les sorties de 
#define OUT_T0    OUT_2  // notre systeme.
#define OUT_T1    OUT_3
#define OUT_T2    OUT_4 
#define OUT_T3    OUT_5 

#define PRESSURE_RES 284.16 // Resolution du capteur de pression.
                            // L'ADC a 10bits donc 0 a 1023
                            // resolution = pression max / 2^10.

/*///////////////////////////////////////////////////////////////
*             DEFINITION DES VARIABLES ET OBJETS
*//////////////////////////////////////////////////////////////////////////////

// Initialisation de l'objet bouton utilisant la librairie
// Bounce2.h pour effectuer un anti-rebonds logiciel.
Bounce2::Button button = Bounce2::Button();

// Initialisation de l'ecran LCD utilisant la librairie
// LiquidCrystal_I2C.h
LiquidCrystal_I2C lcd(0x27,16,2);

// Variables utilisees par les loadcells
//
// LCXX_OFFSET : Valeur lu lorsqu'aucune charge est presente.
// LCXX_DIVIDER: Valeur lu / poids connu.
HX711 scale50, scale350; 
//const long LC50_OFFSET = -18160;    // Val pour loadcell 1000Kg
//const float LC50_DIVIDER = 2322.86; // Val pour loadcell 1000Kg
const long LC50_OFFSET = 0;   // Val pour sim 50 Kg (Demo ELE400)
const double LC50_DIVIDER = 120; // Val pour sim 50 Kg (Demo ELE400)
const long LC350_OFFSET = -17960;   // Val pour loadcell 1000Kg
const long LC350_DIVIDER = 2322.86; // Val pour loadcell 1000Kg
double weigth50, weigth350 = 0;

// Variables utilisees par les MAX31856
Adafruit_MAX31856 TC0 = Adafruit_MAX31856(PIN_CS_T0);
Adafruit_MAX31856 TC1 = Adafruit_MAX31856(PIN_CS_T1);
Adafruit_MAX31856 TC2 = Adafruit_MAX31856(PIN_CS_T2);
Adafruit_MAX31856 TC3 = Adafruit_MAX31856(PIN_CS_T3);
double tempT0, tempT1, tempT2, tempT3 = 0;

// Initialisation du DAC utlisant la librairie
// MAX5725.h
MAX5725 dac = MAX5725(16, &SPI);

// Variable utilisees pour l'affichage des donnees sur l'ecran LCD.
#define LCD_NB_PAGES 4
int count4LCD = 0; // utiliser pour le rafraichissement des donnees sur l'ecran.
int pageLCD = 0;   // utiliser pour changer l'affichage une fois le bouton appuye.

int countLED = 0;  // Utiliser pour faire clignoter la led.

bool calibrationMode = 0;
double freq = 0;

/*///////////////////////////////////////////////////////////////
*                            SETUP
*//////////////////////////////////////////////////////////////////////////////
void setup() {
  // setup() est appele une seule fois au debut du programme.
  
  // Initialisation des I/O
  button.attach(PIN_PUSH_BTN, INPUT_PULLUP); 
  button.interval(5); // 5 millisecondes de debouncing
  button.setPressedState(LOW); // bouton appuye lorsque signal LOW.
  pinMode(PIN_LED, OUTPUT);
  
  // Initalisation de la communication serie.
  Serial.begin(57600); 
  Serial.print("Serial Initialized.\n");

  LCD_init();   // Init de l'ecran LCD.
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
  //scale350.tare();

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

  dac.begin();

  // Si le bouton est appuye lors du demarrage, on active le mode calibration.
  if(digitalRead(PIN_PUSH_BTN) == LOW) calibrationMode = 1;

  pinMode(PIN_LOW_VOLT_WARN, INPUT);

  
  lcd.clear();
}


/*///////////////////////////////////////////////////////////////
*                     BOUCLE PRINCIPALE
*//////////////////////////////////////////////////////////////////////////////
void loop() {

  if(calibrationMode){
  /* Cette section du code est appele seulement si
   * le bouton est appuye lors du demarrage du systeme.
   * 
   * Ajouter le code voulu dans cette section.
   */ 

  }

  // Pour calculer la frequence d'operation
  unsigned long loopStartTime = micros();

  button.update(); // doit etre appele a chaque boucle.

  // Si le bouton est pese, on change l'info afficher
  // Si on est rendu a la derniere page d'info, on retourne 
  // au premiere informations a afficher.
  // Voir LCD.ino pour modifier/ajouter des pages.
  if (button.pressed()) {
    if((pageLCD + 1) == LCD_NB_PAGES) pageLCD = 0;
    else pageLCD++;
  }
  
  // Lecture de la cellule de charge de 50 Kg
  // Bloquant pour l'instant si les HX711 ne sont pas connectes
  while(!scale50.is_ready()) delay(1);     // PEUT ETRE INUTILE UNE FOIS LES MAXs IMPLEMENTES.
  weigth50 = scale50.get_units();

  // Lecture de la cellule de charge de 350 Kg
  // Bloquant pour l'instant si les HX711 ne sont pas connectes
  while(!scale350.is_ready()) delay(1);    // PEUT ETRE INUTILE UNE FOIS LES MAXs IMPLEMENTES.
  weigth350 = scale350.get_units();

  // Lecture des thermocouple. Seulement si DRDY est actif (signal LOW)
  if(!digitalRead(PIN_DATARDY_T0)){tempT0 = TC0.readThermocoupleTemperature();}
  if(!digitalRead(PIN_DATARDY_T1)){tempT1 = TC1.readThermocoupleTemperature();}
  if(!digitalRead(PIN_DATARDY_T2)){tempT2 = TC2.readThermocoupleTemperature();}
  if(!digitalRead(PIN_DATARDY_T3)){tempT3 = TC3.readThermocoupleTemperature();}

  // Lecture de la valeur de l'entree analogique pour le capteur de pression.
  int pressureBits = analogRead(PIN_PRESSURE);
  double pressureReal = double(pressureBits/PRESSURE_RES);

  // section pour les sorties du DAC
  dac.load_value_adjusted(OUT_T0,    tempT0,    DAC_TC_MINVAL,    DAC_TC_MAXVAL);
  dac.load_value_adjusted(OUT_T1,    tempT1,    DAC_TC_MINVAL,    DAC_TC_MAXVAL);
  dac.load_value_adjusted(OUT_T2,    tempT2,    DAC_TC_MINVAL,    DAC_TC_MAXVAL);
  dac.load_value_adjusted(OUT_T3,    tempT3,    DAC_TC_MINVAL,    DAC_TC_MAXVAL);
  dac.load_value_adjusted(OUT_LC50,  weigth50,  DAC_LC50_MINVAL,  DAC_LC50_MAXVAL);
  dac.load_value_adjusted(OUT_LC350, weigth350, DAC_LC350_MINVAL, DAC_LC350_MAXVAL);

  // Lecture du signal "low voltage warning" (HIGH si Vin < 10.8 )
  // si Vin < 10.8, la LED D1 clignote.
  // CETTE OPTION N'A PAS ETE VERIFIEE.
  if(digitalRead(PIN_PRESSURE)){
    if(countLED < 100) { digitalWrite(PIN_LED, HIGH); }
    else { digitalWrite(PIN_LED, LOW); }
    countLED++;
    if(countLED > 200) { countLED = 0; }
  }
  else digitalWrite(PIN_LED, LOW);

  // Section pour l'affichage sur l'ecran.
  // le if reduit la vitesse de rafraichissement pour faciliter la lecture.
  if(count4LCD >= 25){
    // Ecriture des données sur le LCD.
    LCD_write_page(pageLCD);
    LCD_update_readings(pageLCD, weigth50, weigth350, tempT0, tempT1, tempT2, tempT3, pressureReal, freq);
    count4LCD = 0;
  }
  else count4LCD++;

  // calcul de la freq d'operation
  unsigned long loopEndTime = micros();
  freq = 1000000/double((loopEndTime - loopStartTime)); // 10^-9

  // Affichage des resultats sur le Serial
//  Serial.println("freq :" + String(freq,2) + " - " + String(weigth50) + " - " + String(weigth350));
//  Serial.println("Temperature : T0 : " + String(tempT0,2) + "- T1 : " + String(tempT1,2) + 
//                             "- T2 : " + String(tempT2,2) + "- T3 : "+ String(tempT3,2));

}
