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
#define LEDR 11
#define LEDG 13
#define LEDB 12

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

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  
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
  float kd = 0.01; //derivatice constant
  float ki = 0.0; //integral constant
  
  int dir = 1; //motor direction
  int e; //error between targetRPM and actual RPM
  long prevT = 0; //previous time
  float eprev = 0; //previous error
  float eintegral = 0;

  int r = 255;
  int g = 255;
  int b = 255;
  
void loop() {
  int targetRPM = 750*sin(prevT/1e3);   
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
  analogWrite(LEDR, abs(255-pwr));
  analogWrite(LEDG, abs(255-pwr));
  analogWrite(LEDB, abs(255-pwr));
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
 
  
