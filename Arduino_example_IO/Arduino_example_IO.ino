void setup() {

  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(A5, 1);
  pinMode(13, 1);

  pinMode(8, 0); // Switch
  pinMode(7, 0);

  digitalWrite(A5, 0);
  digitalWrite(13, 0);
  //Serial.end();
}

void loop() {
  // put your main code here, to run repeatedly:
if (digitalRead(8))
{
  Serial.print("Switch on port 8\n");
  digitalWrite(A5, 1);
}

if (digitalRead(7))
{
  Serial.print("Interruptor on port:7\n");
  digitalWrite(13, 1);
  
}
delay(1000);

}
