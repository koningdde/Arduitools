/************************************************************************************
  This is a example of ICStation Light cube
  All materials are provided by www.ICStation.com.
Should you have any unclear or need any other related material, please feel free to 
contact us via http://www.icstation.com/contact_us.php.
www.ICStation.com
   ICStation is based in Shenzhen, China, a city which is rich in electronics. 
With such manufacture power, we not only provide worldwide with all kinds of 
IC products, such as electronic devices and components, development modules, 
development boards, consumptive materials and so on, but also we are taking 
part in the designing and developing digital and analog circuit design which
 based on  microcontroller platforms.
 Arduino PIN used: PIN0,PIN1,PIN2,PIN3,PIN4,PIN5,PIN6,PIN7(Other pins can be used alone)
 
 
**************************************************************************************/

#include <ICStation_Light_cube.h>


ICStation_Light_cube  My_Light_cube;
// the setup routine runs once when you press reset:
void setup() {                
  //initial other
}
// the loop routine runs over and over again forever:
void loop() {

  My_Light_cube.run_example();
}
