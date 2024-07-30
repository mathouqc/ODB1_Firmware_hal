// Send test NMEA sentence to debug L76LM33 driver code

void setup() {
  Serial.begin(9600);
}

int i = 0;

void loop() {
  Serial.print("GARBAGE");
  Serial.print("$GPRMC,203522.000,V,,,,,,,,,,,V*7D");
  Serial.println(i, DEC);
  i++;
  Serial.print("GARBAGE");
  delay(1200);
  Serial.print("$GPRMC,203523.200,V,,,,,,,,,,,V*7E");
  Serial.println(i, DEC);
  i++;
  delay(1200);
  Serial.print("$GPRMC,203524.400,V,,,,,,,,,,,V*2F");
  Serial.println(i, DEC);
  i++;
  Serial.print("GARBAGE");
  delay(1200);
  Serial.print("$GPRMC,203525.600,A,5109.0262,N,11401.8407,W,0.00,133.42,130522,,,A,V*06");
  Serial.println(i, DEC);
  i++;
  delay(1200);
  Serial.print("GARBAGE");
  Serial.print("$GPRMC,203526.800,A,5108.0619,N,11402.1695,W,0.00,133.42,130522,,,A,V*01");
  Serial.println(i, DEC);
  i++;
  delay(1200);
  Serial.print("$GPRMC,203528.000,A,5107.0944,N,11403.2196,W,0.00,133.42,130522,,,A,V*09");
  Serial.println(i, DEC);
  i++;
  Serial.print("GARBAGE");
  Serial.print("GPRMC,203528.000,A,5107.0944,N,11403.2196,W,0.00,133.42,130522,,,A,V*00");
  Serial.println(i, DEC);
  i++;
  delay(1200);
  Serial.print("$GNRMC,203529.200,A,5106.1980,N,11404.3572,W,0.00,133.42,130522,,,A,V*14");
  Serial.println(i, DEC);
  i++;
  delay(1200);
  Serial.print("$GNRMC,203530.400,A,5105.9461,N,11405.6197,W,0.00,133.42,130522,,,A,V*18");
  Serial.println(i, DEC);
  i++;
  delay(1200);
  Serial.print("$GNRMC,203531.600,A,5104.8646,N,11406.0679,W,0.00,133.42,130522,,,A,V*1E");
  Serial.println(i, DEC);
  i++;
  delay(1200);
}
