void setBacklight() {
  if(state) {
    lcd.backlight();
  } else {
    lcd.noBacklight();
  }
}
