void setup() {
  
  Serial.begin(9600);
  //Accel A
  pinMode(A0, INPUT); // Y
  pinMode(A1, INPUT); // X
  //Accel B
  pinMode(A2, INPUT); // Y
  pinMode(A3, INPUT); // X
  //Accel C
  pinMode(A4, INPUT); // Y
  pinMode(A5, INPUT); // X
}

#define PRINT_ALL
//#define PRINT_B

int pico = 0;
int vale = 1024;

void loop() {

  //printa todos os vals analogicos
  #ifdef PRINT_ALL
    Serial.print("\t Accel A  Y: "); 
    Serial.print(analogRead(A0));
    Serial.print("  Z: "); 
    Serial.print(analogRead(A1));
    Serial.print("\t\tAccel B  Y: "); 
    Serial.print(analogRead(A2));
    Serial.print("  Z: "); 
    Serial.print(analogRead(A3));
    Serial.print("\t\tAccel C  Y: "); 
    Serial.print(analogRead(A4));
    Serial.print("  Z: "); 
    Serial.print(analogRead(A5));
    Serial.print("\r\n");
    delay(10);
  #endif

  //printa valor max e min de analogRead A3
  #ifdef PRINT_B
    if(analogRead(A3) > pico) pico = analogRead(A3);
    
    if(analogRead(A3) < vale) vale = analogRead(A3);
  
    Serial.print(vale);
    Serial.print("\t");  
    Serial.println(pico);
    
    delay(1);
  #endif
}
