#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>

DFPlayerMini_Fast myMP3;

#define ARDUINO_TX 13//should connect to TX of the Serial MP3 Player module
#define ARDUINO_RX 12//connect to RX of the module
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);

float PWM_KIRI;
float PWM_KANAN;
int Kecepatan;
float member_Sensor2, member_Sensor3;
float Sensor2, Sensor3;
float min1, min2, min3, min4, min5, min6, min7, min8, min9, min10;
float R1A, R2A, R3A, R4A, R5A, R6A, R7A, R8A, R9A, R1B, R2B, R3B, R4B, R5B, R6B, R7B, R8B, R9B;
int Bertambah = 30, Normal = 0 , Berkurang = -100;
float a, b, c;
float Jauh1, Dekat1, SedangE;
float Jauh2, Dekat2, SedangF;
float PWM_A  = 0;
float PWM_B = 0;

int X1;
int X2;
int X3;
int X4;
int X5;
int state = 0;

int MotorA_PWM = 9; //motor kiri
int MotorB_PWM = 10; // motor kanan
int CWatauCCW_1 = 11; //abu abu
//int CWatauCCW_2 = 12; //abu abu
int Brake = 8; //Putih x2
int InfraredA = 6;// pin 6 of Arduino is connected to the output of the sensor inrared Kiri
int InfraredB = 7;// pin 6 of Arduino is connected to the output of the sensor inrared Kanan


void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  myMP3.begin(mySerial);
  myMP3.volume(30);
  delay(10);
  pinMode(3, OUTPUT); //Spare Relay Add  Merah
  pinMode(4, OUTPUT); //Spare Relay Add  Kuning
  pinMode(5, OUTPUT); //Spare Relay Add  Hijau
  pinMode(6, INPUT_PULLUP); //setting Arduino pin 6 as output Sensor infrared Kiri
  pinMode(7, INPUT_PULLUP); //setting Arduino pin 7 as output Sensor infrared kanan
  pinMode (2, INPUT_PULLUP ); //arduino push ON
  pinMode (A0, INPUT); // sensor kiri 1
  pinMode (A1, INPUT); // sensor kiri 2
  pinMode (A2, INPUT); // sensor kanan 1
  pinMode (A3, INPUT); // sensor Kanan 2
  pinMode (A4, INPUT); // sensor Kanan 2
  pinMode (9, OUTPUT); //pwmA. kabel servo = hijau
  pinMode (10, OUTPUT); //pwmB. kabel servo = hijau
  pinMode (8, OUTPUT); //brake. kabel servo = putih


  Serial.println("IR Sensor Readings:: ");
  myMP3.loop(1);

}
void loop()
{

 
  Serial.print("PWM :");
  Serial.print(Kecepatan);
  Serial.print(" \t");
  Serial.print("X2 :");
  Serial.print("\t");
  Serial.print(X2); // nilai snsor tcrt500
  Serial.print("\t");
  Serial.print("X3:");
  Serial.print("\t");
  Serial.print(X3);
  Serial.print("\t");
  Serial.print(" pwmKiri: ");
  Serial.print("\t");
  Serial.print(PWM_A); // nilai defuzifikasi motor kiri
  Serial.print("\t");
  Serial.print(" pwmKanan: ");
  Serial.print("\t");
  Serial.print(PWM_B); // defuzifikasi motor kanan
  Serial.print("\t");
  //  Serial.print("totalPWMkiri:");
  //  Serial.print("\t");
  //  Serial.print(PWM_KIRI);
  //  Serial.print("\t");
  //  Serial.print("totalPWMkanan: ");
  //  Serial.print("\t");
  //  Serial.print(PWM_KANAN);
  //  Serial.print("\t");
  Serial.print("X1 : ");
  Serial.print("\t");
  Serial.print(X1);
  Serial.print("\t");
  Serial.print("\t");
  Serial.print("X4 : ");
  Serial.print("\t");
  Serial.print(X4);
  Serial.print("\t");
  Serial.print("\t");
  Serial.print("X5 : ");
  Serial.print("\t");
  Serial.print(X5);
  Serial.print("\t");


  //  Serial.print("\t");
  //  if (digitalRead(6) == HIGH && digitalRead(7) == HIGH) {
  //    Serial.print("AMAN");
  //  }
  //  if (digitalRead(6) == LOW | digitalRead(7) == LOW) {
  //    Serial.print("TERDETEKSI");
  //  }
  //  Serial.print("\t");
  //  Serial.print(digitalRead(2));
  //  Serial.print("\t");
  //
  //  Serial.print(X1);
  //  Serial.print("\t");
  //  Serial.print(X2);
  //  Serial.print("\t");
  //  Serial.print(X3);
  //  Serial.print("\t");
  //  Serial.print(X4);
  //  Serial.print("\t");
  //  Serial.print(X5);
  //  Serial.print("\t");
  //
  Serial.println();

  
  Kecepatan = 150;
  X2 = analogRead ( A1); // KANAN
  X3 = analogRead ( A2); // KIRI
  Sensor2 = X2; // KANAN
  Sensor3 = X3; // KIRI

  membership();
  defuzifikasi_motorkiri(); // PWM_A
  defuzifikasi_motorkanan(); // PWM_B

  PWM_KANAN = Kecepatan + (PWM_B);
  PWM_KIRI = Kecepatan + (PWM_A);
  /* PWM_a = MOTOR KIRI
     PWM_B = MOTOR KANAN
  */


  if (PWM_KIRI < 0) {
    PWM_KIRI = 0;
  }
  if (PWM_KANAN < 0) {
    PWM_KANAN = 0;
  }



  if (digitalRead(2) == HIGH) {
    sistem();
  }
  else {
    Maju();
    delay(500);
  }






}

void sistem() {
  if (digitalRead(6) == HIGH && digitalRead(7) == HIGH) {
    linefollower();
  }
  if (digitalRead(6) == LOW | digitalRead(7) == LOW) {
    Berhenti() ;
    digitalWrite(3, HIGH);
  }

}
void linefollower() {

  X4 = analogRead (A0);
  // X2 = analogRead (A1);// gak ikut, buat fuzzy
  //X3 = analogRead (A2);// gak ikut, buat fuzzy
  X1 = analogRead (A3);
  X5 = analogRead(A4);



  if (X4 > 450  )
  {
    Kiri();
    digitalWrite(3, LOW);
  }

  if ( X1 > 450)
  {
    Kanan();
    digitalWrite(3, LOW);
  }

  if (  X1 < 450  && X4 < 450 )
  {
    Maju  ();
    digitalWrite(3, LOW);
  }


  if (X5 > 750) // sensor follower
  {
    Berhenti();
    digitalWrite(3, HIGH);
  }


}
void Maju()
{
  analogWrite (9, PWM_KIRI); //motor kiri, kecepatan 75, untuk ubah speed ganti saja angka 75 nya. angka maximal 255. kabel servo = hijau
  analogWrite (10, PWM_KANAN); //motor kanan, kecepatan 75, untuk ubah speed ganti saja angka 75 nya. angka maximal 255. kabel servo = hijau
  digitalWrite (8, LOW); //pin nomor 8 untuk brake

}

void Kanan()
{
  analogWrite (9, PWM_KIRI); //motor kiri, kecepatan 50, untuk ubah speed ganti saja angka 50 nya. angka maximal 255. kabel servo= hijau
  analogWrite (10, 0); //motor kanan, kecepatan 0, untuk ubah speed ganti saja angka 0 nya. angka maximal 255. kabel servo = hijau
  digitalWrite (Brake, LOW);

}

void Kiri()

{
  analogWrite (9, 0); //motor kiri, kecepatan 0, untuk ubah speed ganti saja angka 0 nya. angka maximal 255. kabel servo = hijau
  analogWrite (10, PWM_KANAN); //motor kanan, kecepatan 50, untuk ubah speed ganti saja angka 50 nya. angka maximal 255. kabel servo = hijau
  digitalWrite (Brake, LOW);

}

void Berhenti()
{
  analogWrite (9, 0); //motor kiri, kecepatan 0, untuk ubah speed ganti saja angka 0 nya. angka maximal 255. kabel servo = hijau
  analogWrite (10, 0); //motor kanan, kecepatan 0, untuk ubah speed ganti saja angka 0 nya. angka maximal 255. kabel servo = hijau
  digitalWrite (Brake, HIGH);

}



float Input1 (float a, float b, float c)
{
  if ((Sensor2 >= a) && (Sensor2 < b))
  {
    member_Sensor2 = (Sensor2 - a) / (b - a);
  }
  if ((Sensor2 >= b) && (Sensor2 < c))
  {
    member_Sensor2 = (c - Sensor2) / (c - b);
  }
  if ((Sensor2 < 50) || (Sensor2 > 500))
  {
    member_Sensor2  = 1;
  }
  if ((Sensor2 > c) || (Sensor2 < a))
  {
    member_Sensor2 = 0;
  }
}

float Input2 (float a, float b, float c)
{
  if ((Sensor3 >= a) && (Sensor3 < b))// NAIK
  {
    member_Sensor3 = (Sensor3 - a) / (b - a);
  }
  if ((Sensor3 >= b) && (Sensor3 < c))
  {
    member_Sensor3 = (c - Sensor3) / (c - b); // TURUN
  }
  if ((Sensor3 < 50) || (Sensor3 > 500))
  {
    member_Sensor3  = 1;
  }
  if ((Sensor3 > c) || (Sensor3 < a))
  {
    member_Sensor3 = 0;
  }
}

void membership()
{
  member_Sensor2 = 0;
  Input1 (a = 0, b = 50, c = 250); //Range, Buat jAUH
  Jauh1 = member_Sensor2;


  Input1(a = 50, b = 250, c = 500); //Buat sEDNG
  SedangE = member_Sensor2;

  Input1(a = 250, b = 500, c = 1023); //Buat dEKAT
  Dekat1 = member_Sensor2;


  //========================= MEMBERSHIP Kekeruhan ======================================

  member_Sensor3 = 0;
  Input2 (a = 0, b = 50, c = 250); //Buat Jernih
  Jauh2 = member_Sensor3;


  Input2(a = 50, b = 250, c = 500); //Buat Keruh
  SedangF = member_Sensor3;

  Input2(a = 250, b = 500, c = 1023); //Buat SangatKeruh
  Dekat2 = member_Sensor3;



  //================================= INFERENSI RULES ===============================================================

  min1 = min (Jauh1, Jauh2);
  min4 = min(Jauh1, SedangF);
  min5 = min(Jauh1, Dekat2);

  min2 = min (SedangE, Jauh2);
  min6 = min(SedangE, SedangF);
  min7 = min(SedangE, Dekat2);

  min3 = min (Dekat1, Jauh2);
  min8 = min (Dekat1, SedangF);
  min9 = min (Dekat1, Dekat2);


}

void defuzifikasi_motorkiri () {

  R1A = min1 * Normal; //S2 = JAUH,jauh
  R4A = min4 * Bertambah; //          ,sedang
  R5A = min5 * Bertambah; //         ,dekat

  R2A = min2 * Normal; //S2 = SEDANG,Jauh
  R6A = min6 * Normal; //          ,sedang
  R7A = min7 * Normal; //          ,Dekat

  R3A = min3 * Berkurang; //S2 = DEKAT, Jauh
  R8A = min8 * Berkurang; //          ,Sedang
  R9A = min9 * Normal; //          ,Dekat

  PWM_A = (R1A + R2A + R3A + R4A + R5A + R6A + R7A + R8A + R9A) / (min1 + min2 + min3 + min4 + min5 + min6 + min7 + min8 + min9);

  //PWM_A = (R1A+R2A+R3A+R4A+R5A)/(min1+min2+min3+min4+min5);
}


void defuzifikasi_motorkanan () {

  R1B = min1 * Normal; //Jauh, Jauh
  R4B = min4 * Normal; //Jauh, Sedang
  R5B = min5 * Berkurang; //Jauh, Dekat

  R2B = min2 * Bertambah; //Sedang, Jauh
  R6B = min6 * Normal; //Sedang, Sedang
  R7B = min7 * Berkurang; //Sedang, Dekat

  R3B = min3 * Bertambah; //Dekat, Jauh
  R8B = min8 * Normal; //Dekat, Sedang
  R9B = min9 * Normal; //Dekat, Dekat

  PWM_B = (R1B + R2B + R3B + R4B + R5B + R6B + R7B + R8B + R9B) / (min1 + min2 + min3 + min4 + min5 + min6 + min7 + min8 + min9);
  //PWM_B = (R1B+R2B+R3B+R4B+R5B)/(min1+min2+min3+min4+min5);
}
