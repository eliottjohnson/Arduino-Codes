// Eliott Johnson
// Code to control a 4.4:1 Metal Gearmotor 25Dx63L mm LP 6V with 48 CPR Encoder
// with a TB9051FTG Single Brushed DC Motor Driver Carrier from Polulu
//
// It allows to automatically control de power needed to send to the pwm and the direction to
// follow a target RPM.

#include <util/atomic.h> // For the ATOMIC_BLOCK macro

//Encoder signals to determine position
#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE

//Speed and direction control 
#define PWM1 4 //GREEN
#define PWM2 5 //SALMON
#define ENB 6 //BLUE
#define EN 7 //YELLOW

//RGB LED
#define LEDR1 13
#define LEDG1 12
#define LEDB1 11

#define LEDR2 10
#define LEDG2 9
#define LEDB2 8

#define LEDR3 46
#define LEDG3 45
#define LEDB3 44

volatile int RisingAintt = 0;
volatile int RisingBintt = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ENCA,INPUT);
  pinMode(ENCB,INPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(ENB, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ENCA),readEncoderAR,RISING);
  attachInterrupt(digitalPinToInterrupt(ENCB),readEncoderBR,RISING);

  pinMode(LEDR1, OUTPUT);
  pinMode(LEDG1, OUTPUT);
  pinMode(LEDB1, OUTPUT);
  pinMode(LEDR2, OUTPUT);
  pinMode(LEDG2, OUTPUT);
  pinMode(LEDB2, OUTPUT);
  pinMode(LEDR3, OUTPUT);
  pinMode(LEDG3, OUTPUT);
  pinMode(LEDB3, OUTPUT);
  
}

/////////////////////////////////////////////////////////////////////////////////////
//                                     LOOP                                        //
/////////////////////////////////////////////////////////////////////////////////////
  int RisingA = 0; //Count the number of time the signal from encoder A has rose
  int RisingB = 0; 
  int TotalRisingEdgesCurr = 0;
  int TotalRisingEdgesPrev = 0;
  int DiffTotalRisingEdges = 0;
  int interval = 70; //The refresh rate. A good value is between 50 and 100.
  float RPM = 0;
  float deltaT = 0;
  int pwr = 0; //Power
  
  // PID constants
  float kp = 0.2; //proportional constant
  float kd = 0.01; //derivative constant
  float ki = 0.01; //integral constant
  
  int dir = 1; //motor direction
  int e; //error between targetRPM and actual RPM
  long prevT = 0; //previous time
  float eprev = 0; //previous error
  float eintegral = 0;

  int red = 0;
  int green = 0;
  int blue = 0;

  int pos = 0; 
  
void loop() {
  //int targetRPM = 750*sin(prevT/1e3);
  int targetRPM = 750;    
  dir = 1;
    if (targetRPM<0){
      dir = -1;
  }


  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    RisingA = RisingAintt;
    RisingB = RisingBintt;
  }

  
  long currT = millis();
  if (currT - prevT > interval) {
      deltaT = ((float) (currT - prevT))/( 1.0e3 );
      prevT = currT;
    
      TotalRisingEdgesCurr = RisingA+RisingB;
      DiffTotalRisingEdges = TotalRisingEdgesCurr - TotalRisingEdgesPrev;
      TotalRisingEdgesPrev = TotalRisingEdgesCurr;
       
      RPM = (float)((60*DiffTotalRisingEdges)/(deltaT))/(105.6); //Number of rising encoder edges per revolution is 105.6
    
      e = abs(targetRPM) - RPM; //error
      
      float dedt = (e-eprev)/(deltaT); // derivative
    
      eintegral = eintegral + e*deltaT; // integral
      
      pwr = pwr + kp*e + kd*dedt + ki*eintegral; //PID 
      
      if (pwr>255){ //Max PWM value is 255
        pwr=255;
      }
      Serial.print(targetRPM);
      Serial.print(" ");
      Serial.print(dir*RPM);
      Serial.println();
  }    
  // signal the motor
  setMotor(dir,pwr);
  
  // One rotation takes 60000/RPM millisecondes
  analogWrite(LEDR1, 255);
  analogWrite(LEDG1, 255);
  analogWrite(LEDB1, 255);
  
  analogWrite(LEDR2, 255);
  analogWrite(LEDG2, 255);
  analogWrite(LEDB2, 255);
  
  analogWrite(LEDR3, 255);
  analogWrite(LEDG3, 255);
  analogWrite(LEDB3, 255);
  
  delay(60000/RPM);
  analogWrite(LEDR1, red);
  analogWrite(LEDG1, green);
  analogWrite(LEDB1, blue);
  
  analogWrite(LEDR2, red);
  analogWrite(LEDG2, green);
  analogWrite(LEDB2, blue);
  
  analogWrite(LEDR3, red);
  analogWrite(LEDG3, green);
  analogWrite(LEDB3, blue);
  delay(1);
}

/////////////////////////////////////////////////////////////////////////////////////
//                                    FUNCTIONS                                    //
/////////////////////////////////////////////////////////////////////////////////////
void setMotor(int dir, int pwmVal){ //pwmVal from 0 to 255
  digitalWrite(EN, HIGH);
  digitalWrite(ENB, LOW);
  if(dir == 1){
    analogWrite(PWM1,pwmVal);
    analogWrite(PWM2,0);
  }
  else if(dir == -1){
    analogWrite(PWM1,0);
    analogWrite(PWM2,pwmVal); //reverse direction
  }
  else{
    analogWrite(PWM1,0);
    analogWrite(PWM1,0);
  }
}

void readEncoderAR(){
  RisingAintt++;
  }
void readEncoderBR(){
  RisingBintt++;
  }
