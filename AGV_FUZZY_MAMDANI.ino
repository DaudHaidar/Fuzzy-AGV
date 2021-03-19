#include<SoftwareSerial.h>
#include <Fuzzy.h>
#include <DFPlayerMini_Fast.h>

float CycleTime, pwm, out;
float Cepat1, SedangA, Lama1;
float Lambat2, SedangB, Cepat2, SedangC;
float rule1, rule2a, rule2b, rule3;
float PWM1 ;
float PWM2 ;
float pwm1 ;
float pwm2 ;
int RangeStandard;
int RangeBerhenti;
float max1, max2;

// FUZZY = PWM
Fuzzy *fuzzy = new Fuzzy();

// FuzzyInput TCRT5000_2
FuzzySet *Normal1 = new FuzzySet(0, 0, 100, 250);
FuzzySet *AgakSerongKanan = new FuzzySet(100, 250, 250, 500);
FuzzySet *SerongKanan = new FuzzySet (250, 500, 1023, 1023);

// FuzzyInput TCRT5000_3
FuzzySet * Normal2 = new FuzzySet(0, 0, 100, 250);
FuzzySet * AgakSerongKiri = new FuzzySet(100, 250, 250, 500);
FuzzySet * SerongKiri = new FuzzySet(250, 500, 1023, 1023);


// FuzzyOutputPWM1
FuzzySet *Berkurang1 = new FuzzySet(-100, -100, -50, 0);
FuzzySet *NormalA = new FuzzySet(-10, 0, 0, 10);
FuzzySet *Bertambah1 = new FuzzySet(0, 10, 20, 20);

// FuzzyOutputPWM2
FuzzySet *Berkurang2 = new FuzzySet(-100, -100, -50, 0);
FuzzySet *NormalB = new FuzzySet(-10, 0, 0, 10);
FuzzySet *Bertambah2 = new FuzzySet(0, 10, 20, 20);




DFPlayerMini_Fast myMP3;

#define ARDUINO_TX 13//should connect to TX of the Serial MP3 Player module
#define ARDUINO_RX 12//connect to RX of the module
SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX);

int X1;
int X2;
int X3;
int X4;
int X5;
int state = 0;
int Sensor1 = 0;
int Sensor2 = 0;
int Sensor3 = 0;
int Sensor4 = 0;
int PWMawal ;
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
  pinMode (A1, INPUT); // sensor kiri 2, buat fuzzy
  pinMode (A2, INPUT); // sensor kanan 1, buat fuzzy
  pinMode (A3, INPUT); // sensor Kanan 2
  pinMode (11, INPUT); // Sensor buat berhenti
  pinMode (9, OUTPUT); //pwmA. kabel servo = hijau
  pinMode (10, OUTPUT); //pwmB. kabel servo = hijau
  pinMode (8, OUTPUT); //brake. kabel servo = putih

  // FuzzyInput
  FuzzyInput *TCRT5000_2 = new FuzzyInput(1);

  TCRT5000_2->addFuzzySet(Normal1);
  TCRT5000_2->addFuzzySet(AgakSerongKanan);
  TCRT5000_2->addFuzzySet(SerongKanan);
  fuzzy->addFuzzyInput(TCRT5000_2);

  FuzzyInput *TCRT5000_3 = new FuzzyInput(2);

  TCRT5000_3->addFuzzySet(Normal2);
  TCRT5000_3->addFuzzySet(AgakSerongKiri);
  TCRT5000_3->addFuzzySet(SerongKiri);
  fuzzy->addFuzzyInput(TCRT5000_3);

  // FuzzyOutput
  FuzzyOutput *PWM1 = new FuzzyOutput(1);

  PWM1->addFuzzySet(Berkurang1);
  PWM1->addFuzzySet(NormalA);
  PWM1->addFuzzySet(Bertambah1);
  fuzzy->addFuzzyOutput(PWM1);

  // FuzzyOutput
  FuzzyOutput *PWM2 = new FuzzyOutput(2);

  PWM2->addFuzzySet(Berkurang2);
  PWM2->addFuzzySet(NormalB);
  PWM2->addFuzzySet(Bertambah2);
  fuzzy->addFuzzyOutput(PWM2);

  // Building FuzzyRule
  FuzzyRuleAntecedent *ifTCRT5000_2Normal1AndTCRT5000_3Normal2 = new FuzzyRuleAntecedent();
  ifTCRT5000_2Normal1AndTCRT5000_3Normal2->joinWithAND(Normal1, Normal2);
  FuzzyRuleConsequent *thenPWM1Normal1PWM2Normal2 = new FuzzyRuleConsequent();
  thenPWM1Normal1PWM2Normal2->addOutput(NormalA);
  thenPWM1Normal1PWM2Normal2->addOutput(NormalB);
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, ifTCRT5000_2Normal1AndTCRT5000_3Normal2,  thenPWM1Normal1PWM2Normal2);// Output coba hapus AND nya nanti
  fuzzy->addFuzzyRule(fuzzyRule1);

  // Building FuzzyRule
  FuzzyRuleAntecedent *ifTCRT5000_2Normal1AndTCRT5000_3AgakSerongKiri = new FuzzyRuleAntecedent();
  ifTCRT5000_2Normal1AndTCRT5000_3AgakSerongKiri->joinWithAND(Normal1, AgakSerongKiri);
  FuzzyRuleConsequent *thenPWM1Bertambah1PWM2Normal2 = new FuzzyRuleConsequent();
  thenPWM1Bertambah1PWM2Normal2->addOutput(Bertambah1);
  thenPWM1Bertambah1PWM2Normal2->addOutput(NormalB);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, ifTCRT5000_2Normal1AndTCRT5000_3AgakSerongKiri,  thenPWM1Bertambah1PWM2Normal2);// Output coba hapus AND nya nanti
  fuzzy->addFuzzyRule(fuzzyRule2);

  // Building FuzzyRule
  FuzzyRuleAntecedent *ifTCRT5000_2Normal1AndTCRT5000_3SerongKiri = new FuzzyRuleAntecedent();
  ifTCRT5000_2Normal1AndTCRT5000_3SerongKiri->joinWithAND(Normal1, SerongKiri);
  FuzzyRuleConsequent *thenPWM1Bertambah1PWM2Berkurang2 = new FuzzyRuleConsequent();
  thenPWM1Bertambah1PWM2Berkurang2->addOutput(Bertambah1);
  thenPWM1Bertambah1PWM2Berkurang2->addOutput(Berkurang2);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, ifTCRT5000_2Normal1AndTCRT5000_3AgakSerongKiri,  thenPWM1Bertambah1PWM2Berkurang2);// Output coba hapus AND nya nanti
  fuzzy->addFuzzyRule(fuzzyRule3);

  // Building FuzzyRule
  FuzzyRuleAntecedent *ifTCRT5000_2AgakSerongKananAndTCRT5000_3Normal2 = new FuzzyRuleAntecedent();
  ifTCRT5000_2AgakSerongKananAndTCRT5000_3Normal2->joinWithAND(AgakSerongKanan, Normal2);
  FuzzyRuleConsequent *thenPWM1Berkurang1PWM2Bertambah2 = new FuzzyRuleConsequent();
  thenPWM1Berkurang1PWM2Bertambah2->addOutput(Berkurang1);
  thenPWM1Berkurang1PWM2Bertambah2->addOutput(Bertambah2);
  FuzzyRule *fuzzyRule4 = new FuzzyRule(4, ifTCRT5000_2AgakSerongKananAndTCRT5000_3Normal2,  thenPWM1Berkurang1PWM2Bertambah2);// Output coba hapus AND nya nanti
  fuzzy->addFuzzyRule(fuzzyRule4);

  // Building FuzzyRule
  FuzzyRuleAntecedent *ifTCRT5000_2SerongKananAndTCRT5000_3Normal = new FuzzyRuleAntecedent();
  ifTCRT5000_2SerongKananAndTCRT5000_3Normal->joinWithAND(SerongKanan, Normal2);
  FuzzyRuleConsequent *thenPWM1BerkurangPWM2Bertambah = new FuzzyRuleConsequent();
  thenPWM1BerkurangPWM2Bertambah->addOutput(Berkurang1);
  thenPWM1BerkurangPWM2Bertambah->addOutput(Bertambah2);
  FuzzyRule *fuzzyRule5 = new FuzzyRule(5, ifTCRT5000_2SerongKananAndTCRT5000_3Normal,  thenPWM1BerkurangPWM2Bertambah);// Output coba hapus AND nya nanti
  fuzzy->addFuzzyRule(fuzzyRule5);


  myMP3.loop(1);



}



void loop() {





  X3 = analogRead ( A1);
  X2 = analogRead ( A2);

  int input1 = X3;
  int input2 = X2;

  fuzzy->setInput(1, input1);
  fuzzy->setInput(2, input2);
  fuzzy->fuzzify();
  float pwm1 = fuzzy->defuzzify(1);
  float pwm2 = fuzzy->defuzzify(2);
  PWMawal = 150;
  PWM1 = PWMawal + (pwm1);
  PWM2 = PWMawal + (pwm2);

  if ( PWM1 < 0) {
    PWM1 = 0 ;
  }
  if ( PWM2 < 0) {
    PWM2 = 0 ;
  }

  Serial.print("PWM :");
  Serial.print(PWMawal);
  Serial.print(" \t");
  Serial.print("Sensor2:");
  Serial.print("\t");
  Serial.print(input1);
  Serial.print("\t");
  Serial.print("Sensor3:");
  Serial.print("\t");
  Serial.print(input2);
  Serial.print("\t");
  Serial.print(" pwmKiri: ");
  Serial.print("\t");
  Serial.print(pwm1);
  Serial.print("\t");
  Serial.print(" pwmKanan: ");
  Serial.print("\t");
  Serial.print(pwm2);
  Serial.print("\t");
  Serial.print("\t");
  Serial.print("totalPWMkiri:");
  Serial.print("\t");
  Serial.print(PWM1);
  Serial.print("\t");
  Serial.print("\t");
  Serial.print("totalPWMkanan: ");
  Serial.print("\t");
  Serial.print(PWM2);
  Serial.print("\t");

  Serial.println();



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
  }


}
void linefollower() {

  X4 = analogRead (A0);
  // X2 = analogRead (A1);// gak ikut, buat fuzzy
  //X3 = analogRead (A2);// gak ikut, buat fuzzy
  X1 = analogRead (A3);
  X5 = analogRead(A4);

  RangeStandard = 450;
  RangeBerhenti = 750;

  if (X4 > RangeStandard  )
  {
    Kiri();
    digitalWrite(3, LOW);
  }

  if ( X1 > RangeStandard)
  {
    Kanan();
    digitalWrite(3, LOW);
  }

  if (  X1 < RangeStandard  && X4 < RangeStandard )
  {
    Maju  ();
    digitalWrite(3, LOW);
  }

  if (X5 > RangeBerhenti ) // sensor follower
  {
    Berhenti();
    digitalWrite(3, HIGH);
  }

}
void Maju()
{
  analogWrite (9, PWM1); //motor kiri,
  analogWrite (10, PWM2); //motor kanan,
  digitalWrite (8, LOW); //pin nomor 8 untuk brake

}

void Kanan()
{
  analogWrite (9, PWM1); //motor kiri,
  analogWrite (10, 0); //motor kanan,
  digitalWrite (Brake, LOW);

}

void Kiri()

{
  analogWrite (9, 0); //motor kiri,
  analogWrite (10, PWM2); //motor kanan,
  digitalWrite (Brake, LOW);

}

void Berhenti()
{
  analogWrite (9, 0); //motor kiri,
  analogWrite (10, 0); //motor kanan,
  digitalWrite (Brake, HIGH);

}

void MP3 () {


  X5 = analogRead(A4);

  switch ( state ) {
    case 0 :
      if (X5 < 500 ) {
        myMP3.loop(1);
        state = 1;
      }
      break;

    case 1 :
      if (X5 > 500) {
        myMP3.loop(2);
        state = 0;
      }
      break;


  }
}
