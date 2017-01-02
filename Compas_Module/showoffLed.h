void ledShow() {
  for (int j = 0; j < 16; j++){
    registerWrite(windRichting[j]);
    delay(50);
  }
  for (int j = 15; j > 0; j--){
    registerWrite(windRichting[j]);
    delay(50);
  }
  
  for (int j = 0; j < 5; j++){
    registerWrite(updown[j]);
    delay(100);
  }

    for (int j = 5; j > 0; j--){
    registerWrite(updown[j]);
    delay(100);
  }
  
  registerWrite(255); //all led on
  delay(1000);
  registerWrite(0); //all led off

  }
