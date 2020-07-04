void setup() {
  Serial.begin(9600);
}

void loop() {
  int a = analogRead(A0);
  Serial.print("Water level = ");
  Serial.println(a);
}
