#define PUMP D0

void initPump(){
  pinMode(PUMP, OUTPUT);
  digitalWrite(PUMP, LOW);  
}

void runPump(int t){
  digitalWrite(PUMP, HIGH);
  delay(t);
  digitalWrite(PUMP, LOW); 
  delay(t); 
}

void killPump(){
  digitalWrite(PUMP, LOW);  
}


void checkSanitizerLevel(){
  if(30 > readFluidLevel()){
    digitalWrite(D4, LOW);
    httpGet(SANTZR_URL);
  }else{
    digitalWrite(D4, HIGH);
  }
}
