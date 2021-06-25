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
  long prevT = 0;
  int i = 0;
  int SummDiffTRE = 0;
  float SummDeltaT = 0;
  float RPM = 0;
  float sinus = 0;

void loop() {
  sinus = abs(255*sin(prevT/1e6));
  setMotor(1,sinus);
  
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    RisingA = RisingAintt;
    RisingB = RisingBintt;

  }

  //Number of rising encoder edges per revolution is 105.6

  TotalRisingEdgesCurr = RisingA+RisingB;
  DiffTotalRisingEdges = TotalRisingEdgesCurr - TotalRisingEdgesPrev;
  TotalRisingEdgesPrev = TotalRisingEdgesCurr;


  // Condidition to smooth out RPM
  if (i<10){
    SummDiffTRE = SummDiffTRE + DiffTotalRisingEdges;
    SummDeltaT = SummDeltaT + deltaT;
    i++;
  }
  
  if (i==10){
    RPM = ((60*SummDiffTRE)/(SummDeltaT))/(105.6);
    SummDiffTRE = 0;
    SummDeltaT = 0;
    i=0;
  }

  
  
  Serial.print(sinus);
  Serial.print(" ");
  Serial.print(RPM);
  Serial.println();
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
 
  
