void init_LCD(void)
{
    printf("Initialisation de l'ecran LCD");
    lcd.init(); 
    lcd.init();
    // Affichage de départ
    lcd.backlight();
    lcd.setCursor(1,0);
    lcd.print("Initialisation");
    lcd.setCursor(6,1);
    lcd.print("....");
}