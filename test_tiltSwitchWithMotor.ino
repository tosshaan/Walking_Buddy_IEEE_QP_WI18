/* Better Debouncer
 * 
 * This debouncing circuit is more rugged, and will work with tilt switches!
 *
 * http://www.ladyada.net/learn/sensor/tilt.html
 */

 /*
  * The tilt works such that it is only not vibrating at the upright position, 
  * and in all other positions, it vibrates 
  * (which we assume is when it falls in a certain orientation.
  */
 
int tilt = 8;         // the number of the input pin
int motor = 11;       // the number of the output pin
 
int motorState = LOW; // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
 
// the following variables are long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 50;   // the debounce time, increase if the output flickers
 
void setup()
{
  pinMode(tilt, INPUT);
  digitalWrite(tilt, HIGH);   // turn on the built in pull-up resistor
  pinMode(motor, OUTPUT);
}
 
void loop()
{
  int switchstate;
 
  reading = digitalRead(tilt);
 
  // If the switch changed, due to bounce or pressing...
  if (reading != previous) {
    // reset the debouncing timer
    time = millis();
  } 
 
  if ((millis() - time) > debounce) {
     // whatever the switch is at, its been there for a long time
     // so lets settle on it!
     switchstate = reading;
 
     // Now invert the output on the pin11 motor
    if (switchstate == HIGH)
      motorState = LOW;
    else
      motorState = HIGH;
  }
  digitalWrite(motor, motorState);
 
  // Save the last reading so we keep a running tally
  previous = reading;
}
