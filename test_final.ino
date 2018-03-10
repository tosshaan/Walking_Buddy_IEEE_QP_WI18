/**
 * Description: Blind person cane with arduino uno
 * 
 * Components Used: Arduino Uno, Tilt Sensor, Vibration Motor, Audio Buzzer, Ultrasonic Range Sensor (x3)
 * 
 * Authors: Tosshaan Thapar, Allan Prince, Shivani Bhakta
 * Date: 3/9/2018
 */

              //////    CONSTANTS    //////
        
const int buffDistance = 4;
const int thresholdDistance = 61;


              //////    INPUTS    //////
          
int echoPin[] = {4, 6, 2};
int trigPin[] = {5, 7, 3};
int motor = 8;
int buzzer = 10;
int tilt = 9;

              //////    VARIABLES    //////
          
long duration[] = {0,0,0};
long cm[] = {0,0,0};
long prevDistance[] = {0,0,0};  //to check object is closing in

int freqState = LOW;   // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

bool gettingCloser = false;  
bool pos[] = {false, false, false};   // true when corresponding range sensor detects something within the threshhold distance
 
long time = 0;         // the last time the output pin was toggled
long debounce = 1500;  // the debounce time, increase if the output flickers
                       // this is the time difference you want to calculate


              //////    SETUP    //////

/*
 * Runs first and runs once, before everyting else
 */
void setup() {
  
  Serial.begin (9600);
  
  pinMode(trigPin[0], OUTPUT);
  pinMode(trigPin[1], OUTPUT);
  pinMode(trigPin[2], OUTPUT);
  pinMode(echoPin[0], INPUT);
  pinMode(echoPin[1], INPUT);
  pinMode(echoPin[2], INPUT);
 
  pinMode(buzzer, OUTPUT);
  pinMode(tilt, INPUT);
  pinMode(motor, OUTPUT);

  digitalWrite(tilt, HIGH);   // turn on the built in pull-up resistor
  digitalWrite(motor, HIGH);  // motor should be off to begin with
}

              //////    HELPER METHODS    //////

/*
 * Buzzer with longer delay
 */
void lowFreqBuzzer(int freqState) {
  digitalWrite(buzzer, freqState);
  delay(100);                      //wait for 100ms or 0.1s
}

/*
 * Buzzer with shorter delay
 */
void highFreqBuzzer(int freqState) {
  digitalWrite(buzzer,freqState);
  delay(10);                       //wait for 10ms
}

/*
 * Trigger the output of ultrasonic sensor (decided by index)
 */
void trig(int index){
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin[index], LOW);
  delayMicroseconds(50);
  digitalWrite(trigPin[index], HIGH);
  delayMicroseconds(50);
  digitalWrite(trigPin[index], LOW);
}

/*
 * Read the input from ultrasonic sensor (decided by index)
 * and populate the cm[] array
 */
void echo(int index){
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration[index] = pulseIn(echoPin[index], HIGH);
  
  // convert the time into a distance
  cm[index] = (duration[index]/2) / 29.1;
}

/*
 * trigger motor
 * LOW ==> vibrate
 * HIGH ==> don't vibrate
 */
void warningMotor(int i){
  digitalWrite(motor, LOW);
  //Serial.print("(");
  //Serial.print(i + 1);
  //Serial.print(") ");
  //Serial.println("Vibrate...");
  delay(200);
  digitalWrite(motor, HIGH);
}

/*
 * stops the motor from vibrating
 */
void stopMotor(){
  digitalWrite(motor, HIGH);
}

/*
 * sends output and receives input for each sensor; populates cm[] array
 */
void ultraSonicSensor(){
  for(int i = 0; i < 3; i++){
    trig(i);
    echo(i);
  }
}

/*
 * Reset the distance array
 */
void clearCm(){
  for(int i = 0; i < 3; i++){
    cm[i] = 0;
  }
}

void clearPrev() {
  for(int i = 0; i < 3; i++){
    prevDistance[i] = 0;
  }
}

/*
 * Reset values
 */
void reset(){
  clearCm();
}

/*
 * Prints all distances to serial output
 */
void printCm(){
  Serial.print("cm[]: ");
  Serial.print(cm[0]);
  Serial.print("cm    ");
  Serial.print(cm[1]);
  Serial.print("cm    ");  
  Serial.print(cm[2]);
  Serial.println("cm    ");
}

void printPrev(){
  Serial.print("prev[]: ");
  Serial.print(prevDistance[0]);
  Serial.print("cm    ");
  Serial.print(prevDistance[1]);
  Serial.print("cm    ");  
  Serial.print(prevDistance[2]);
  Serial.println("cm    ");
}

void printPos(){
  Serial.print("pos[]: ");
  Serial.print(pos[0]);
  Serial.print("    ");
  Serial.print(pos[1]);
  Serial.print("    ");  
  Serial.println(pos[2]);
}

              //////    LOOP    //////

/*
 * Main code, loops indefinitely
 */
void loop()
{
  int switchstate;

  /*
   * Range Sensor + Vibration motor
   */
   
  ultraSonicSensor();

  printCm();

  //when something closes in i.e. within threshold distance AND closer than before
  if(gettingCloser){
    bool notCloseAnymore = true;
    //int index = -1;
    for(int i = 0; i < 3; i++){
      if(pos[i] && prevDistance[i]!=0 && ((cm[i] < prevDistance[i] - buffDistance))){
        //Serial.print("(");
        //Serial.print(i + 1);
        //Serial.print(") ");
        //Serial.println("Something is getting closer");
        warningMotor(i);
        notCloseAnymore = false;
        //index = i;
      }
      else{
        //Serial.print("(");
        //Serial.print(i + 1);
        //Serial.print(") ");
        //Serial.println("not getting closer anymore");
        prevDistance[i] = 0;
        //clearPrev();
        pos[i] = false;
        stopMotor();
      }
    }
    if(notCloseAnymore){
      //Serial.println("not getting closer anymore");
      gettingCloser = false;
      clearPrev();
    }
  }

  //check if something is within threshhold distance for the first time
  else if(!gettingCloser){
    for(int i = 0; i < 3; i++){
      if(cm[i] != 0 && cm[i] < thresholdDistance){
        gettingCloser = true;
        pos[i] = true;
        prevDistance[i] = cm[i];
        //Serial.print("(");
        //Serial.print(i + 1);
        //Serial.print(") ");
      
        //Serial.println("Something is close");
      }
    }
  }

  //printPrev();
  //printPos();

  /*
   * End of Range + Motor
   */


  /*
   * Tilt sensor + Audio buzzer
   */

  reading = digitalRead(tilt);
 
  // If the switch changed, due to bounce or pressing...
  // reset the debouncing timer
  if (reading != previous) {
    time = millis();
  } 
 
  if ((millis() - time) > debounce) {
     // whatever the switch is at, its been there for a long time
     // so lets settle on it!
     switchstate = reading;
 
     // Now invert the output on the buzzer
     if (switchstate == HIGH)
       freqState = HIGH;
     else
       freqState = LOW;
  }
  
  highFreqBuzzer(freqState);
  
  // Save the last reading so we keep a running tally
  previous = reading;

  /*
   * End of Tilt + Buzzer
   */

  //for testing purposes; also to save battery
  delay(200);
 
  reset(); 
  

}
