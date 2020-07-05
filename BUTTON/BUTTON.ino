int BUTTON = 8;
bool cur = false;
bool pre = false;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON,INPUT);
}

void loop() {
  pre = digitalRead(BUTTON);
  if(pre != cur){
    Serial.println(pre);
    cur = digitalRead(BUTTON);
    delay(100);
  }
}
