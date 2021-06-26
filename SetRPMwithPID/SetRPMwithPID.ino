#include <util/atomic.h> // For the ATOMIC_BLOCK macro

//Encoder signals to determine position
#define ENCA 2 // YELLOW
#define ENCB 3 // WHITE

//Speed and direction control 
#define PWM1 4 //GREEN
#define PWM2 5 //SALMON
#define ENB 6 //BLUE
#define EN 7 //YELLOW

volatile int RisingAintt = 0;
volatile int RisingBintt = 0;
volatile int FallingAintt = 0;
volatile int FallingBintt = 0;


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

}
////////
//LOOP//
///////
  int RisingA = 0;
  int RisingB = 0; 
  int TotalRisingEdgesCurr = 0;
  int TotalRisingEdgesPrev = 0;
  int DiffTotalRisingEdges = 0;
  int interval = 100;
  float RPM = 0;
  float deltaT = 0;
  int pwr = 0;
  
  // PID constants
  float kp = 0.2;
  float kd = 0.01;
  float ki = 0.0;
  // motor direction
  int dir = 1;
  int e;
  long prevT = 0;
  float eprev = 0;
  float eintegral = 0;
  
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

  //Number of rising encoder edges per revolution is 105.6

    long currT = millis();
    if (currT - prevT > interval) {
      deltaT = ((float) (currT - prevT))/( 1.0e3 );
      prevT = currT;
  
      TotalRisingEdgesCurr = RisingA+RisingB;
      DiffTotalRisingEdges = TotalRisingEdgesCurr - TotalRisingEdgesPrev;
      TotalRisingEdgesPrev = TotalRisingEdgesCurr;
       
      RPM = (float)((60*DiffTotalRisingEdges)/(deltaT))/(105.6); 

      //error
      e = abs(targetRPM) - RPM;
      

      // derivative
      float dedt = (e-eprev)/(deltaT);

      // integral
      eintegral = eintegral + e*deltaT;
      
      pwr = pwr + kp*e + kd*dedt + ki*eintegral;

      
      if (pwr>255){
        pwr=255;
      }

      
      Serial.print(targetRPM);
      Serial.print(" ");
      Serial.print(dir*RPM);
      Serial.println();
   }
  
  

    
   // signal the motor
   setMotor(dir,pwr);


  
  
  

}

//Functions

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
 
  
