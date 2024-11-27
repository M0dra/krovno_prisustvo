const int dugme = 8;
const int trig1 = 9;            
const int eho1 = 10;
const int trig2 = 11;      
const int eho2 = 12;
unsigned long t1;  // vremenski trenutak aktivacije prvog senzora
unsigned long t2;  // vremenski trenutak aktivacije drugog senzora
int ljudi = 0;  
long duration = 0; 
bool donji;
bool gornji; 
bool tipka = false; 
float d1;
float d2;

void  setup() { 
  pinMode(dugme, INPUT);
  pinMode(trig1, OUTPUT); 
  pinMode(trig2, OUTPUT); 
  pinMode(eho1, INPUT);
  pinMode(eho2, INPUT);
  digitalWrite(trig1, LOW);
  digitalWrite(trig2, LOW);
  Serial.begin(9600);      
}


void Obavestenje(bool a, int ljudi){    //funnkcija za obavestenja u serial monitoru
  if(a)
    Serial.println("Neko je usao u krov :)");
  else
    Serial.println("Neko je izasao iz krov :)");

  Serial.print("trenutno ima ");
  Serial.print(ljudi);
  Serial.println(" osoba u krovu");
  Serial.println(d1);
  Serial.println(d2);
}

void loop(){ 
  
  digitalWrite(trig1, LOW);
  delayMicroseconds(2);
  digitalWrite(trig1, HIGH);
  delayMicroseconds(20); 
  digitalWrite(trig1, LOW);
  duration = pulseIn(eho1, HIGH);

  d1 = (duration * 0.034) / 2;

  delay(150);
    
  digitalWrite(trig2, LOW);
  delayMicroseconds(2);
  digitalWrite(trig2, HIGH);
  delayMicroseconds(20); 
  digitalWrite(trig2, LOW);
  duration = pulseIn(eho2, HIGH);

  d2 = (duration * 0.034) / 2;

  delay(150);
  
  if(d1 < 50){//ako senzor detektuje objekat na manje od 50cm dize flag i pamti vreme
    donji = true;
    t1 = millis();
    //Serial.println(d1);
  }
  if(d2 < 50){//ako senzor detektuje objekat na manje od 50cm dize flag i pamti vreme
    gornji = true;
    t2 = millis();
    //Serial.println(d1);
  }
  if(gornji*donji){//ako su oba upanjena
    if(t1 > t2){   //pogledaj koji se senzor prvi upalio
      ljudi++;
      Obavestenje(true,ljudi);
    }else{
      ljudi--;
      Obavestenje(false,ljudi);
    }
    gornji = false;//vrati zastave na dole
    donji = false;
    delay(150);
  }
  if(ljudi < 0){
    Serial.println("Neki retard je napravio gresku");//SEJFTI FICRZ
  }
  if(ljudi > 100){
    Serial.println("Neki retard stoji na stepenicama");
  }
  tipka = digitalRead(dugme);//DUGME DA GA VRATI NAN NULU
  if(tipka)
    ljudi = 0;


}

