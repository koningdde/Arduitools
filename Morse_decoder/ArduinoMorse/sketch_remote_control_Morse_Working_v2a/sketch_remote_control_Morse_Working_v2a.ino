// Send message by remote control
// Tested with Philips remote. Will not work with other protocols!
// interrupt problem avoided by removal of 'tone' function
// replaced by providing a 555 oscillator and switching pin 13
// May 17, 2015 by Barry Block, for testing a Morse en/decoder sketch

#include <Arduino.h>
#include <IRremote.h>

byte signalPin = 13; // will control oscillator speaker
unsigned int wpm = 20; // to start with
unsigned int elementWait = 1200 / wpm;
boolean signal_state = false, newline = false, n;
unsigned long value;

unsigned int IRpin = 11;
IRrecv irrecv(IRpin);
decode_results results;

void setup() {
  pinMode(signalPin, OUTPUT);
  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the IR receiver
  screen_refresh(); menu(); // show options on terminal screen
}

void loop() {
  if (irrecv.decode(&results))
  {
    delay(200); // debounce further key speak
    //Serial.println(results.value, DEC); // Print to terminal 'results.value' of key pressed for testing
    value = (results.value);
    whatToDo(value); // pass on to case statements
    Serial.println(); if (newline && n) send_cr(); n = true; // some selections turn 'n' off
    irrecv.resume();   // Receive the next value
  }
}

void whatToDo(unsigned long)
{
  //Serial.print(value); // find codes for key of interest
  switch (value) {
    case 65: case 2113: case 483140465LL: case 3782414109LL: //1 Philips remote control key
      send_hi_k(); break;
    case 66: case 2114: case 414237880LL: case 1787241860LL: //2
      send_cq(); break;
    case 67: case 2115: case 2847395506LL: case 1851701854LL: //3
      send_73(); break;
    case 68: case 2116: case 3422820078LL: case 500856762LL: //4
      send_599(); break;
    case 69: case 2117: case 3439597697LL: case 517634381LL: //5
      send_qth(); break;
    case 70: case 2118: case 2785123463LL: case 4158127443LL: //6
      send_equipment(); break;
    case 71: case 2119: case 2907184054LL: case 1911490402LL: //7
      send_QRM(); break;
    case 72: case 2120: case 4115108598LL: case 1193145282LL: //8
      send_DX(); break;
    case 73: case 2121: case 4131886219LL: case 1209922903LL: //9
      send_name(); break;
    case 64: case 2112: case 533473320LL: case 3832746964LL: //0
      send_all_messages(); break;
    case 3513153484LL: case 213879840LL:
      send_all_chars(); break;
    case 84: case 2132: case 3807691156LL: case 3944024704LL: //up arrow
      faster(); n = false; break;
    case 83: case 2131: case 3884316884LL: case 962353568LL: //down arrow
      slower(); n = false; break;
    case 117: case 2165: case 1659525391LL: case 3032529371LL: //ok
      screen_refresh(); menu(); n = false; break;
    case 74: case 2122: case 106820438LL: case 243153986LL: // back arrow (return) clear screen on real terminal screens
      screen_refresh(); n = false; send_clr(); break;
    case 77: case 2125: case 2529646239LL: case 3902650219LL: //mute, pitch adj.
      pitch_adj(); n = false; break;
    case 2212600856LL: case 1216907204LL: newline = !newline; break; // home, carrage return
    case 1007251640LL: case 11557988LL: send_clr(); break; // text, clear screen
    default: send_e(); send_r(); send_r(); n = false; break;
  }
}

void menu() {
  Serial.println(F("Send Morse code by remote v2 (c) Barry Block b5@xs4all.nl\n"));
  Serial.print("Speed: "); Serial.print(wpm); Serial.println(F(" wpm"));
  Serial.println(F("Remote control options:\n"));

  Serial.println(F("Up Arrow> Send Morse 5 wpm faster"));
  Serial.println(F("Dn Arrow> Send Morse 5 wpm slower"));
  Serial.println(F("BackArrow> Clear terminal screens"));
  Serial.println(F("text> send clear terminal screen"));
  Serial.println(F("Mute> To toggle long tone to adjust pitch of osc."));
  Serial.print(F("home> toggle newline: ")); if (newline) Serial.println("on"); else Serial.println("off");
  Serial.println(F("OK> Show this menu\n"));
  Serial.println(F("subtitle> Send all characters"));
  Serial.println(F("1> Hi Hi"));
  Serial.println(F("2> CQ CQ CQ (Call)"));
  Serial.println(F("3> 73"));
  Serial.println(F("4> 599"));
  Serial.println(F("5> QTH"));
  Serial.println(F("6> My Equipment"));
  Serial.println(F("7> QRM"));
  Serial.println(F("8> CQ DX (Call)"));
  Serial.println(F("9> My name is Barry"));
  Serial.println(F("0> Go Crazy. Send all messages"));

}

void send_hi_k() { //1
  Serial.println(F("HI HI K"));
  send_h(); send_i(); word_space(); send_h(); send_i(); word_space(); send_k(); word_space();
}

void send_cq() { //2
  Serial.println(F("CQ CQ CQ DE WB6CPR WB6CPR"));
  for (int i = 0; i < 3; i++) {
    send_c(); send_q(); word_space();
  }
  send_de(); word_space(); send_call(); word_space(); send_call(); word_space();
}

void send_de() {
  send_d(); send_e(); word_space();
}

void send_call() {
  send_w(); send_b(); send_6(); send_c(); send_p(); send_r(); word_space();
}

void send_73() { //3
  Serial.println(F("73 73 K"));
  send_7(); send_3(); word_space(); send_7(); send_3(); word_space(); send_k(); word_space();
}

void send_599() { //4
  Serial.println(F("599 599 K"));
  send_5(); send_9(); send_9(); word_space(); send_5(); send_9(); send_9(); word_space(); send_k(); word_space();
}

void send_qth() { //5
  Serial.println(F("QTH QTH AMSTERDAM AMSTERDAM"));
  send_q(); send_t(); send_h(); word_space(); send_q(); send_t(); send_h(); word_space(); send_location(); word_space(); send_location();
  word_space();
}

void send_location() {
  send_a(); send_m(); send_s(); send_t(); send_e(); send_r(); send_d(); send_a(); send_m(); word_space();
}

void send_equipment() { //6
  Serial.println(F("ARUDINO UNO, RECEIVER R5000"));
  send_a(); send_r(); send_d(); send_u(); send_i(); send_n(); send_o(); word_space(); send_u(); send_n(); send_o(); send_comma();
  word_space(); send_r(); send_e(); send_c(); send_e(); send_i(); send_v(); send_e(); send_r(); word_space(); send_r(); send_5();
  send_0(); send_0(); send_0(); word_space();
}

void send_QRM() { //7
  Serial.println(F("QRM QRM, AGAIN PLEASE "));
  send_q(); send_r(); send_m(); word_space(); send_q(); send_r(); send_m(); send_comma(); word_space(); send_a(); send_g(); send_a();
  send_i(); send_n(); word_space(); send_p(); send_l(); send_e(); send_a(); send_s(); send_e(); word_space();
}

void send_DX() { //8
  Serial.println(F("CQ DX CQ DX CQ DX (Call)"));
  send_c(); send_q(); word_space(); send_d(); send_x(); word_space(); send_c(); send_q(); word_space(); send_d(); send_x(); word_space();
  send_c(); send_q(); word_space(); send_d(); send_x(); word_space(); send_d(); send_e(); word_space(); send_call(); word_space();
}

void send_name() { //9
  Serial.println(F("MY NAME IS BARRY"));
  send_m(); send_y(); word_space(); send_n(); send_a(); send_m(); send_e(); word_space(); send_i(); send_s(); word_space();
  send_b(); send_a(); send_r(); send_r(); send_y(); word_space();
}

void send_all_messages() { //0
  send_hi_k(); send_dash(); word_space(); send_cq(); send_dash(); word_space(); send_73(); send_dash(); word_space(); send_599();
  send_dash(); word_space(); send_qth(); send_dash(); word_space(); send_location(); send_dash(); word_space(); send_equipment();
  send_dash(); word_space(); send_QRM(); send_dash(); word_space(); send_DX(); send_dash(); word_space(); send_name(); word_space();
}

void screen_refresh() {
  Serial.write(27); // ESC
  Serial.write("[2J"); // clear screen
  Serial.write(27); // ESC
  Serial.write("[H"); // cursor to home
}

void pitch_adj() {
  Serial.println(F("Click 'Mute' again to start / stop long tone."));
  signal_state = !signal_state;
  if (signal_state == true) digitalWrite(signalPin, HIGH);
  else digitalWrite(signalPin, LOW);
}

void send_dash() {
  letter_space(); dah(); dit(); dit(); dit(); dit(); dah(); letter_space();
}

void dit() {
  digitalWrite(signalPin, HIGH);
  delay(elementWait);      // milliseconds - one dit
  digitalWrite(signalPin, LOW);
  delay(elementWait);
}

void dah() {
  digitalWrite(signalPin, HIGH);
  delay(elementWait * 3);
  digitalWrite(signalPin, LOW);
  delay(elementWait);
}

void letter_space() {
  delay(elementWait * 3);
}

void word_space() {
  delay(elementWait * 6);
}

void faster() {
  wpm = wpm + 5; // increment by 5 till 120
  if (wpm > 200)
  {
    wpm = 200; Serial.println(F("Speed limit is 200 wpm"));
  }
  elementWait = 1200 / wpm; show_wpm();
}

void slower() {
  wpm = wpm - 5; // decrement by 5 till 5
  if (wpm < 5)
  {
    wpm = 5; Serial.println(F("5 wpm is the slowest speed...yawn"));
  }
  elementWait = 1200 / wpm;  show_wpm();
}

void show_wpm() {
  Serial.print(F("wpm:")); Serial.print(wpm);
}

void send_a() {
  dit(); dah(); letter_space();
}

void send_b() {
  dah(); dit(); dit(); dit();  letter_space();
}

void send_c() {
  dah(); dit(); dah(); dit(); letter_space();
}

void send_d() {
  dah(); dit(); dit(); letter_space();
}

void send_e() {
  dit(); letter_space();
}

void send_f()
{
  dit(); dit(); dah(); dit(); letter_space();
}

void send_g() {
  dah(); dah(); dit(); letter_space();
}

void send_h() {
  dit(); dit(); dit(); dit(); letter_space();
}

void send_i() {
  dit(); dit(); letter_space();
}

void send_j()
{
  dit(); dah(); dah(); dah(); letter_space();
}

void send_k() {
  dah(); dit(); dah(); letter_space();
}

void send_l() {
  dit(); dah(); dit(); dit(); letter_space();
}

void send_m() {
  dah(); dah(); letter_space();
}

void send_n() {
  dah(); dit(); letter_space();
}

void send_o() {
  dah(); dah(); dah(); letter_space();
}

void send_p() {
  dit(); dah(); dah(); dit(); letter_space();
}

void send_q() {
  dah(); dah(); dit(); dah(); letter_space();
}

void send_r() {
  dit(); dah(); dit(); letter_space();
}

void send_s() {
  dit(); dit(); dit(); letter_space();
}

void send_t() {
  dah(); letter_space();
}

void send_u() {
  dit(); dit(); dah(); letter_space();
}

void send_v() {
  dit(); dit(); dit(); dah(); letter_space();
}

void send_w() {
  dit(); dah(); dah(); letter_space();
}

void send_x() {
  dah(); dit(); dit(); dah(); letter_space();
}

void send_y() {
  dah(); dit(); dah(); dah(); letter_space();
}

void send_z() {
  dah(); dah(); dit(); dit(); letter_space();
}

void send_0() {
  dah(); dah(); dah(); dah(); dah(); letter_space();
}

void send_1() {
  dit(); dah(); dah(); dah(); dah(); letter_space();
}

void send_2() {
  dit(); dit(); dah(); dah(); dah(); letter_space();
}

void send_3() {
  dit(); dit(); dit(); dah(); dah(); letter_space();
}

void send_4() {
  dit(); dit(); dit(); dit(); dah(); letter_space();
}

void send_5() {
  dit(); dit(); dit(); dit(); dit(); letter_space();
}

void send_6() {
  dah(); dit(); dit(); dit(); dit(); letter_space();
}

void send_7() {
  dah(); dah(); dit(); dit(); dit(); letter_space();
}

void send_8() {
  dah(); dah(); dah(); dit(); dit(); letter_space();
}

void send_9() {
  dah(); dah(); dah(); dah(); dit(); letter_space();
}

void send_comma() {
  dah(); dah(); dit(); dit(); dah(); dah(); letter_space();
}

void send_period() {
  dit(); dah(); dit(); dah(); dit(); dah();  letter_space();
}

void send_question()
{
  dit(); dit(); dah(); dah(); dit(); dit(); letter_space();
}

void send_exclamation()
{
  dah(); dit(); dah(); dit(); dah(); dah(); letter_space();
}

void send_colon()
{
  dah(); dah(); dah(); dit(); dit(); dit(); letter_space();
}

void send_quote()
{
  dit(); dah(); dit(); dit(); dah(); dit(); letter_space();
}

void send_apostrophe()
{
  dit(); dah(); dah(); dah(); dah(); dit(); letter_space();
}

void send_equals()
{
  dah(); dit(); dit(); dit(); dah(); letter_space();
}

void send_at() //@
{
  dit(); dah(); dah(); dit(); dah(); dit(); letter_space();
}

void send_lbracket()
{
  dah(); dit(); dah(); dah(); dit(); letter_space();
}

void send_rbracket()
{
  dah(); dit(); dah(); dah(); dit(); dah(); letter_space();
}

void send_dollar()
{
  dit(); dit(); dit(); dah(); dit(); dit(); dah(); letter_space();
}

void send_and()
{
  dit(); dah(); dit(); dit(); dit(); letter_space();
}

void send_semi()
{
  dah(); dit(); dah(); dit(); dah(); dit(); letter_space();
}

void send_plus()
{
  dit(); dah(); dit(); dah(); dit(); letter_space();
}

void send_under()
{
  dit(); dit(); dah(); dah(); dit(); dah(); letter_space();
}

void send_caret()
{
  dit(); dit(); dah(); dah(); letter_space();
}

void send_cr() // carrage return, 7 chars. line feed would be 8 chars. so this has to do double duty
{
  dah(); dit(); dah(); dit(); dit(); dah(); dit(); letter_space(); // cr or, fk which I find simpler to send
}

void send_clr() // clear screen
{
  dit(); dah(); dit(); dit(); dit(); dah(); dit(); // lr or asn or rf. asn has a flow
}

void send_all_chars()
{
  send_a(); send_b(); send_c(); send_d(); send_e(); send_f(); send_g(); send_h(); send_i(); send_j(); send_k(); send_l(); send_m();
  send_n(); send_o(); send_p(); send_q(); send_r(); send_s(); send_t(); send_u(); send_v(); send_w(); send_x(); send_y(); send_z();
  word_space(); send_0(); send_1(); send_2(); send_3(); send_4(); send_5(); send_6(); send_7(); send_8(); send_9();
  word_space(); send_comma(); send_period(); send_question(); send_exclamation(); send_colon(); send_quote(); send_apostrophe(); send_at();
  send_equals(); send_lbracket(); send_rbracket(); send_dollar(); send_and(); send_semi(); send_plus(); send_under(); send_caret();
}
