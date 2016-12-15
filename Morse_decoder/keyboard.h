// PS/2 Keyboard and Serial

#include <ps2.h>

// Initialize keyboard using Pin 6 = data & pin 7 = clock
PS2 Keyboard(7, 6);

unsigned char code;

void keyboardInit() {
  char ack;
  Keyboard.write(0xff); // send reset code
  ack = Keyboard.read(); // byte, kbd does self test
  ack = Keyboard.read(); // another ack when self test is done
}

String keys() {
  String a;
  
    // read 3 scan codes per keystroke, discarding the first two
      code = Keyboard.read(); code = Keyboard.read(); code = Keyboard.read();

    // The scan code in each case statement on the left corresponds
  // with the character sent to the Serial.print() function on the right:
    switch (code) {
    case 0x15: Serial.print("q"); break;
    case 0x16: Serial.print("l"); break;
    case 0x1a: Serial.print("y"); break;
    case 0x1b: Serial.print("s"); break;
    case 0x1c: Serial.print("a"); break;
    case 0x1d: Serial.print("w"); break;
    case 0x1e: Serial.print("2"); break;
    case 0x21: Serial.print("c"); break;
    case 0x22: Serial.print("x"); break;
    case 0x23: Serial.print("d"); break;
    case 0x24: Serial.print("e"); break;
    case 0x25: Serial.print("4"); break;
    case 0x26: Serial.print("3"); break;
    case 0x29: Serial.print(" "); break;
    case 0x2a: Serial.print("v"); break;
    case 0x2b: Serial.print("f"); break;
    case 0x2c: Serial.print("t"); break;
    case 0x2d: Serial.print("r"); break;
    case 0x2e: Serial.print("5"); break;
    case 0x31: Serial.print("n"); break;
    case 0x32: Serial.print("b"); break;
    case 0x33: Serial.print("h"); break;
    case 0x34: Serial.print("g"); break;
    case 0x35: Serial.print("y"); break;
    case 0x36: Serial.print("6"); break;
    case 0x39: Serial.print(","); break;
    case 0x3a: Serial.print("m"); break;
    case 0x3b: Serial.print("j"); break;
    case 0x3c: Serial.print("u"); break;
    case 0x3d: Serial.print("7"); break;
    case 0x3e: Serial.print("8"); break;
    case 0x41: Serial.print(","); break;
    case 0x42: Serial.print("k"); break;
    case 0x43: Serial.print("i"); break;
    case 0x44: Serial.print("o"); break;
    case 0x45: Serial.print("0"); break;
    case 0x46: Serial.print("9"); break;
    case 0x49: Serial.print("."); break;
    case 0x4a: Serial.print("-"); break;
    case 0x4b: Serial.print("l"); break;
    case 0x4c: Serial.print(" "); break;
    case 0x4d: Serial.print("p"); break;
    case 0x4e: Serial.print("+"); break;
    case 0x61: Serial.print("<"); break;
    case 0x69: Serial.print("l"); break;
    case 0x6b: Serial.print("4"); break;
    case 0x6c: Serial.print("7"); break;
    case 0x70: Serial.print("0"); break;
    case 0x71: Serial.print(","); break;
    case 0x72: Serial.print("2"); break;
    case 0x73: Serial.print("5"); break;
    case 0x74: Serial.print("6"); break;
    case 0x75: Serial.print("8"); break;
    case 0x79: Serial.print("+"); break;
    case 0x7a: Serial.print("3"); break;
    case 0x7b: Serial.print("-"); break;
    case 0x7c: Serial.print("*"); break;
    case 0x7d: Serial.print("9"); break;
    case 0x5A: Serial.println("ent"); break;
  }
  return a;
  }

