/* Non du fichier : HX711.ino
 * 
 * Description    : Contient toutes les fonctions pour 
 *                  acquisitionner les cellules de charges. 
 */////////////////////////////////////////////////////////////////////////////
#include "HX711_fnct.h"


long read_Loadcell(HX711* scale){
//    if(scale->wait_ready_timeout(LC_TIMEOUT)){
        long reading = scale->get_value();
        return reading;    
//    }
//    else{
//Serial.print("Data not ready / HX711 not found...\n");
//        return 0;
//    }

}