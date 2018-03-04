/*
 *
 *
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
 */

/*int trigPin1 = 1;    //Trig - green Jumper
int echoPin1 = 2;    //Echo - yellow Jumper
int trigPin2 = 3;    //Trig - green Jumper
int echoPin2 = 4;    //Echo - yellow Jumper
int trigPin3 = 5;    //Trig - green Jumper
int echoPin3 = 6;    //Echo - yellow Jumper
*/

int trigPin[] = {0, 2, 4};
int echoPin[] = {1, 3, 5};
int buzzer = 8;
int tilt = 9;
int motor = 10;

long duration[3], cm[3];

int threshholdDistance = 60;  //(cm)
int prevDistance[3];          //to check object is closing in

int freqState = LOW;   // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

bool gettingCloser = false;  
bool pos[3];            
 
// the following variables are long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 3000;  // the debounce time, increase if the output flickers
                       // this is the time difference you want to calculate

void setup() {
  //Serial Port begin
  Serial.begin (9600);
  
  //Define inputs and outputs
  pinMode(trigPin[0], OUTPUT);
  pinMode(trigPin[1], OUTPUT);
  pinMode(trigPin[2], OUTPUT);
  pinMode(echoPin[0], INPUT);
  pinMode(echoPin[1], INPUT);
  pinMode(echoPin[2], INPUT);
 
  pinMode(buzzer, OUTPUT);
  pinMode(tilt, INPUT);
  digitalWrite(tilt, HIGH);   // turn on the built in pull-up resistor
  pinMode(motor, OUTPUT);
}

/*
void lowFreqBuzzer(int freqState) {
  digitalWrite(buzzer, freqState);
  delay(1000);//wait for 1s
  digitalWrite(buzzer,freqState);
  delay(1000);//wait for 1s
}
*/

void highFreqBuzzer(int freqState) {
  digitalWrite(buzzer,freqState);
  delay(1);//wait for 100ms
  //digitalWrite(buzzer,freqState);
  //delay(1);//wait for 100ms
}

void trig(int index){
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin[index], LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin[index], HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin[index], LOW);
}

void echo(int index){
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin[index], INPUT);
  duration[index/2] = pulseIn(echoPin[index], HIGH);
 
  // convert the time into a distance
  cm[index/2] = (duration[index/2]/2) / 29.1;
}

void warningMotor(){
  digitalWrite(motor, HIGH);
  delay(10);
  digitalWrite(motor, HIGH);
  delay(10);
  digitalWrite(motor, LOW);
}

/*
 * sends output and receives input for each sensor
 */
void ultraSonicSensor(){
  for(int i = 0; i <=4; i+=2){
    trig(i);
    echo(i+1);
  }
}

void clearCm(){
  for(int i = 0; i < 3; i++){
    cm[i] = 0;
  }
}

void reset(){
  clearCm();
}

void loop()
{
  int switchstate;

  //populate cm[] array
  ultraSonicSensor();

  //when something closes in i.e. within threshold distance AND closer than before
  if(gettingCloser){
    bool notCloseAnymore = true;
    for(int i = 0; i < 3; i++){
      if(pos[i] && (cm[i] < prevDistance[i])){
        warningMotor();
        notCloseAnymore = false;
      }
    }
    if(notCloseAnymore){
      gettingCloser = false;
    }
  }

  //check if something is within threshhold distance for the first time
  for(int i = 0; i < 3; i++){
    if(cm[i] != 0 && cm[i] < threshholdDistance){
      gettingCloser = true;
      pos[i] = true;
      prevDistance[i] = cm[i];
    }
  }
  
  reading = digitalRead(tilt);

  /*
  if(digitalRead(tilt)){
    Serial.println("HIGH");
  }
  else{
    Serial.println("LOW");
  }
  */
 
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
      freqState = LOW;
    else
      freqState = HIGH;
  }
  
  highFreqBuzzer(freqState);
  
  // Save the last reading so we keep a running tally
  previous = reading;
  
  Serial.print(cm[0]);
  Serial.println("cm");
 
  reset(); 
  

}
