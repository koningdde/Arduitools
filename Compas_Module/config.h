//define pin numbers

int windRichting[] = {1,9,8,10,2,6,4,20,16,80,64,192,128,160,32,33};



  // Set declination angle on your location and fix heading1
  // You can find your declination on: http://magnetic-declination.com/
  // (+) Positive or (-) for negative
  // For Bytom / Poland declination angle is 4'26E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / M_PI);
  float declinationAngle = (0.0 + (47.0 / 60.0)) / (180 / M_PI);
