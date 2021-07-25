#define LEDB1 11
#define LEDR1 13
#define LEDG1 12

#define LEDB2 10
#define LEDR2 9
#define LEDG2 8

#define LEDG3 44
#define LEDR3 45
#define LEDB3 46
void setup() {                
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

void loop() {

  analogWrite(LEDR1, 0);
  analogWrite(LEDG1, 255);
  analogWrite(LEDB1, 255);
  
  analogWrite(LEDR2, 0);
  analogWrite(LEDG2, 0);
  analogWrite(LEDB2, 0);
  delay(75);
  analogWrite(LEDR2, 255);
  analogWrite(LEDG2, 255);
  analogWrite(LEDB2, 255);
  delay(75);
  analogWrite(LEDR2, 0);
  analogWrite(LEDG2, 0);
  analogWrite(LEDB2, 0);
  delay(75);
  analogWrite(LEDR2, 255);
  analogWrite(LEDG2, 255);
  analogWrite(LEDB2, 255);
  delay(75);
  
  analogWrite(LEDR3, 255);
  analogWrite(LEDG3, 0);
  analogWrite(LEDB3, 255);
  delay(1000);
}
