void LCD_init(void)
{
    printf("Initialisation de l'ecran LCD");
    lcd.init(); // doit envoyer la commande init 2x.
    lcd.init();
    // Affichage de départ
    lcd.backlight();
    lcd.setCursor(1,0);
    lcd.print("Initialisation");
    lcd.setCursor(6,1);
    lcd.print("....");
}

// Ecriture du "template" selon la page
// selectionnee avec le bouton poussoir.
void LCD_write_page(int page){
    switch(page){
        case 0:
          lcd.setCursor(0,0);
          lcd.print("LC 50 Kg:       ");
          lcd.setCursor(0,1);
          lcd.print("LC 350Kg:       ");
        break;

        case 1:
          lcd.setCursor(0,0);
          lcd.print("TC 0 :          ");
          lcd.setCursor(0,1);
          lcd.print("TC 1 :          ");
        break;

        case 2:
          lcd.setCursor(0,0);
          lcd.print("TC 2 :          ");
          lcd.setCursor(0,1);
          lcd.print("TC 3 :          ");
        break;

        default:
        break;
    }
}

// Ecriture des donnees selon la page
// selectionnee avec le bouton poussoir.
void LCD_update_readings(int page, double LC50, double LC350, 
                         double T0, double T1, double T2, double T3){
    switch(page){
        case 0:
          lcd.setCursor(10,0);
          lcd.print("      ");
          lcd.setCursor(10,0);
          lcd.print(String(LC50,3));
          lcd.setCursor(10,1);
          lcd.print("      ");
          lcd.setCursor(10,1);
          lcd.print(String(LC350,2));
        break;

        case 1:
          lcd.setCursor(7,0);
          lcd.print("         ");
          lcd.setCursor(7,0);
          lcd.print(String(T0,2));
          lcd.setCursor(7,1);
          lcd.print("         ");
          lcd.setCursor(7,1);
          lcd.print(String(T1,2));
        break;

        case 2:
          lcd.setCursor(7,0);
          lcd.print("         ");
          lcd.setCursor(7,0);
          lcd.print(String(T2,2));
          lcd.setCursor(7,1);
          lcd.print("         ");
          lcd.setCursor(7,1);
          lcd.print(String(T3,2));
        break;

        default:
        break;
    }
}