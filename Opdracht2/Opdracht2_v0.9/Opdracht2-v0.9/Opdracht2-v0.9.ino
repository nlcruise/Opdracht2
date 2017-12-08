#include <MultiFuncShield.h> //includeer de libary die benodigd is om bepaalde functies van het MFS te gebruiken
int randNumber; //declareer de variabele randNumber
int buttonState; //declareer de variabele buttonState
unsigned long previousMillis = 0; //declareer de positieve variabele previousMillis en ken 0 toe;
unsigned int interval = 2000;
unsigned int buttonPushes = 0;
const char hexDigits[16] = "0123456789ABCDEF";
#define ledPin0 13 //deze regel en de komende drie regels initiliseren constantes (want ze veranderen niet) welke de pinsnummers van de LED's bevatten
#define ledPin1 12
#define ledPin2 11
#define ledPin3 10
#define buttonPin A1
void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(A5)); //initialiseer de random-nummer generator en wijs een ongebruikte pin toe om zo willekeurig mogelijke waardes te krijgen
  Timer1.initialize(); //De timer, welke benodigd is om het display op de MFS te gebruiken, initialiseren
  MFS.initialize(&Timer1); //initialiseer de MFS
  Serial.begin(9600);
}
void displayBinary(int decimalNumber) {
  //in alle if-else statements in deze method wordt het decimale getal omgezet en binair door een led te laten branden als hij te delen is door door bepaalde machten van 2 (4 bits)
  //Bron: opdracht1b, methode voor omzetten dec naar bin zelf bedacht
  int binCounter = decimalNumber; //ken het random generereerde nummer toe aan variabele binCounter
  if (binCounter / 8 > 0) {
    digitalWrite(ledPin0, LOW);
    binCounter = binCounter - 8;
  }
  else {
    digitalWrite(ledPin0, HIGH);
  }
  if (binCounter / 4 > 0) {
    binCounter = binCounter - 4;
    digitalWrite(ledPin1, LOW);
  }
  else {
    digitalWrite(ledPin1, HIGH);
  }
  if (binCounter / 2 > 0) {
    binCounter = binCounter - 2;
    digitalWrite(ledPin2, LOW);
  }
  else {
    digitalWrite(ledPin2, HIGH);
  }
  if (binCounter / 1 > 0) {
    digitalWrite(ledPin3, LOW);
    binCounter = binCounter - 1;
  }
  else {
    digitalWrite(ledPin3, HIGH);
  }
}
void loop() {
  unsigned long currentMillis = millis();
  // put your main code here, to run repeatedly:
  randNumber = random(16); //ken een willkeurig getal in het bereik [1-15] toe aan variabele randNumber
  displayBinary(randNumber); //roep de functie displayBinary aan om de LED's op binaire wijze het getal te laten zien
  buttonPushes = 0; //ken 0 toe aan variabele buttonPushes
  while (currentMillis - previousMillis < interval) { //begin een while-loop die alleen door blijft lopen als de tijd sinds de laatste 2 seconden die verstreken zijn zonder user-input, anders wordt deze niet meer uitgevoerd
    currentMillis = millis(); //ken de huidige tijd van de timer toe aan de variabele die in deze loop gebruikt wordt om te kijken of twee seconden zonder user-input verstreken zijn
    buttonState = !digitalRead(buttonPin); //inverteer de LOW en HIGH-waardes van de buttons om het intuïtief beter te begrijpen
    if (buttonState == HIGH) { //deze if-statement en de komende else-statement worden gebruikt voor debouncing
      delay(50); //delay voor debouncing
      buttonState = !digitalRead(buttonPin); //inverteer de LOW en HIGH-waardes van de buttons om het intuïtief beter te begrijpen
      if (buttonState == LOW) { //zie voorgaande if-statement
        buttonPushes++; //er is een keer op de knop gedrukt, dus de teller die het aantal knopdrukken bijhoudt gaat met 1 omhoog
        previousMillis = currentMillis; //dit zorgt ervoor dat de loop weer 2 seconden door blijft lopen, want currentMillis - previousMillis moet kleiner zijn dan 2000 ms om de loop nogmaals te laten uitvoeren
      }
    }
  }
  Serial.println(randNumber);
  if (buttonPushes == randNumber && buttonPushes != 0) { //als het aantal knopdrukjes - na twee seconden wachten - gelijk is aan het willekeurige generereerde nummer, voer dit statement uit
      int hexCounter = buttonPushes;
      if (hexCounter / 1 > 0) {
        MFS.write(hexDigits[hexCounter / 1], 3);
      }
    previousMillis = currentMillis;
  }
  else {
    Serial.println("Je deed er te lang over!");
    previousMillis = currentMillis;
  }
  MFS.writef
}
