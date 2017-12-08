#include <MultiFuncShield.h> //includeer de libary die benodigd is om bepaalde functies van het MFS te gebruiken

/*
 * Opdracht 2 (nummerraadspel)
 * Gemaakt door Wouter Honselaar (17053994) en Peter Ng (17065186)
 * Klas 2A, Delft
 * Functies:
 * - Je hebt 2 seconden de tijd om op knop A1 te drukken
 * - Na elke keer dat je erop drukt reset de tij
 * - Als 2 seconden voorbij zijn gaat de beurt over
 * - Als je op knopje A2 kun je de highscore zien
 * - Als 2 seconden voorbij zijn en je evenveel op het knopje hebt gedrukt als er binair met de LED's wordt weergegeven (D1 = 8, D4 = 1) win je de beurt
 * - Als je te weinig of te veel op knopje A1 drukt dan komt er na 2 seconden 'FAIL' in het scherm te staan
 * - Als je sneller bent dan de huidige highscore gaat de speaker piepen
 * - Op het display wordt tijdens het spelen hexadecimaal weergegeven hoe vaak je op A1 hebt gedrukt
 * Opmerking: deze source staat ook op het GitHub-account van Wouter: https://github.com/nlcruise/Opdracht2
 * Bronnen:
 * Voorbeelden uit libary van MultiFunctionShield, namelijk 'Beeper', 'Buttons', 'Led_Display' en 'Led_Lights'
 * https://www.arduino.cc/reference/en/language/functions/time/millis/
 * Code van Gert den Neijsel uit PowerPoint-presentatie hoorcollege Embedded Systems
 * Opdracht 1b, Wouter Honselaar
 * Arduino-voorbeeld 'StateChangeDetection'
 * 
 */
unsigned int randNumber; //declareer de variabele randNumber
unsigned int buttonState; //declareer de variabele buttonState
unsigned int buttonStateA2; //declareer de variabele buttonStateA2
unsigned long previousMillis = 0; //declareer de positieve variabele previousMillis en ken 0 toe;
const unsigned int interval = 2000; //declareer de constate interval, waarin staat hoe lang gewacht moet worden totdat de beurt voorbij gaat zonder dat er op het knopje is gedrukt (wordt vooral gebruikt voor testdoeleinden, want een hogere interval maakt het wat makkelijker)
unsigned int buttonPushes = 0; //declareer de variabele waarin het aantal knopdrukjes per beurt wordt opgeslagen
const char hexDigits[16] = "0123456789ABCDEF"; //deze array van chars wordt gebruikt om te bepalen welke hexadecimale cijfers moeten worden weergegeven op het display
float winnaarsTijd = 30000; //de maximale tijd die mogelijk is om erover te doen is 15*2=30 s, dus elke tijd moet gelijk zijn of sneller dan 30 s om het eerste potje te 'winnen'
#define ledPin0 13 //deze regel en de komende regels initiliseren constantes (want ze veranderen niet) welke de pinsnummers van de LED's en de button's bevatten
#define ledPin1 12
#define ledPin2 11
#define ledPin3 10
#define buttonPin A1
#define buttonPinA2 A2
void setup() {
  //de code tussen deze accolades wordt bij het starten van het programma uitgevoerd:
  randomSeed(analogRead(A5)); //initialiseer de random-nummer generator en wijs een ongebruikte pin toe om zo willekeurig mogelijke waardes te krijgen
  Timer1.initialize(); //De timer, welke benodigd is om het display op de MFS te gebruiken, initialiseren
  MFS.initialize(&Timer1); //initialiseer de MFS
  //Serial.begin(9600); //dit is gebruikt om te debuggen
}
void displayBinary(int decimalNumber) { //begin van de functie waarmee het getal binair wordt weergegeven
  //in alle if-else statements in deze method wordt het decimale getal omgezet en binair door een led te laten branden als hij te delen is door door bepaalde machten van 2 (4 bits)
  //Bron: opdracht1b, methode voor omzetten dec naar bin zelf bedacht
  int binCounter = decimalNumber; //ken het random generereerde nummer toe aan variabele binCounter
  if (binCounter / 8 > 0) { //is het getal deelbaar door 8?
    digitalWrite(ledPin0, LOW); //Zo ja, zet ledPin0 aan
    binCounter = binCounter - 8; //En trek 8 af van binCounter om de andere ledjes correcte waardes te laten weergeven
  }
  else { //is het getal niet deelbaar door 8?
    digitalWrite(ledPin0, HIGH); //zet ledpin0 uit (dit is nodig omdat de led anders permanent blijft branden als hij één keer tijdens de loop heeft gebrand)
  }
  if (binCounter / 4 > 0) { //is het getal deelbaar door 8?
    binCounter = binCounter - 4; //Trek 4 af van binCounter om de andere ledjes correcte waardes te laten weergeven
    digitalWrite(ledPin1, LOW); //Zo ja, zet ledpin1 aan
  }
  else { //Is het getal niet deelbaar door 4?
    digitalWrite(ledPin1, HIGH); //zet ledin1 uit (dit is nodig omdat de led anders permanent blijft branden als hij één keer tijdens de loop heeft gebrand)
  }
  if (binCounter / 2 > 0) { //Is het getal deelbaar door 2?
    binCounter = binCounter - 2; //Trek 2 af van binCounter om de andere ledjes correcte waardes te laten weergeven
    digitalWrite(ledPin2, LOW); //zet ledPin2 aan
  }
  else { //Is het getal deelbaar door 2?
    digitalWrite(ledPin2, HIGH); //zet ledPin2 uit
  }
  if (binCounter / 1 > 0) { //Is het getal deelbaar door 1?
    digitalWrite(ledPin3, LOW); //Zo ja, zet ledPin3 aan
  }
  else { //Nee (oftewel: is binCounter gelijk aan nul?)
    digitalWrite(ledPin3, HIGH); //Zet ledPin3 uit
  }
}
void loop() { //het gedeelte in dit blok wordt oneindig achter elkaar uitgevoerd
  unsigned long currentMillis = millis(); //we gebruiken de variabele currentMillis om de tijd date de Arduino aanstaat in ms op te slaan
  randNumber = random(1, 15); //ken een willkeurig getal in het bereik [1-15] toe aan variabele randNumber
  displayBinary(randNumber); //roep de functie displayBinary aan om de LED's op binaire wijze het getal te laten zien
  buttonPushes = 0; //ken 0 toe aan variabele buttonPushes
  float tijd = 0; //deze variabele wordt gebruikt om de tijd op te slaan dat het kost om het getal te raden
  while (currentMillis - previousMillis < interval) { //begin een while-loop die alleen door blijft lopen als de tijd sinds de laatste 2 seconden die verstreken zijn zonder user-input, anders wordt deze niet meer uitgevoerd
    buttonStateA2 = !digitalRead(buttonPinA2); //dit blok is hetzelfde als het volgende blok waarbij debounce wordt toegepast en dit blok dienst voor het tonen van de highscore als op knop A2 wordt gedrukt
    if (buttonStateA2 == LOW) {
      delay(1); //het werkt met een zeer kleine delay goed, omdat anders sommige knopdrukjes op A1 met een grotere delay niet worden gedetecteerd en zonder delay worden drukjes op A2 vaak niet gedetecteerd
      buttonStateA2 = !digitalRead(buttonPinA2);
      if (buttonStateA2 == HIGH) {
      MFS.write((int) winnaarsTijd); //in een latere 'MFS.write'-regel staat waarom het gecast wordt naar int
      previousMillis = currentMillis; //dit zorgt ervoor dat het tonen van de highscore geen tijd kost
      buttonPushes = 0; //om ervoor te zorgen dat mensen niet valsspelen om te tijd te resetten tijdens een potje wordt het aantal knopdrukjes gelijk gesteld aan 0
      }
    }
    float tijdAan = currentMillis - previousMillis; //tijd2 wordt gebruikt om de tijd dat het kost om het getal te raden te corrigeren met de tijd dat de Arduino aanstaat, deze variabele staat hier omdat dit na elke drukknop bij moet worden gewerkt
    currentMillis = millis(); //ken de huidige tijd van de timer toe aan de variabele die in deze loop gebruikt wordt om te kijken of twee seconden zonder user-input verstreken zijn
    buttonState = !digitalRead(buttonPin); //lees de sensorwaarde van de knop en inverteer de LOW en HIGH-waardes van de buttons om het intuïtief beter te begrijpen
    if (buttonState == HIGH) { //dit if-statement en het komende else-statement worden gebruikt voor debouncing
      delay(50); //delay voor debouncing
      buttonState = !digitalRead(buttonPin); //lees de sensorwaarde van de knop en inverteer de LOW en HIGH-waardes van de buttons om het intuïtief beter te begrijpen
      if (buttonState == LOW) { //zie voorgaande if-statement
        buttonPushes++; //er is een keer op de knop gedrukt, dus de teller die het aantal knopdrukken bijhoudt gaat met 1 omhoog
        previousMillis = currentMillis; //dit zorgt ervoor dat de loop weer 2 seconden door blijft lopen, want currentMillis - previousMillis moet kleiner zijn dan 2000 ms om de loop nogmaals te laten uitvoeren
        tijd = tijd + tijdAan; //dit wordt gebruikt om de tijd dat het kost om het getal te raden bij te werken
        MFS.write(""); //dit maakt het display leeg, zodat de tijd die de vorige beurt duurde er eventueel niet meer opstaat
        MFS.write(hexDigits[buttonPushes / 1], 3); //dit zorgt ervoor dat de waarde van het random genereerde getal hexadecimaal op het display komt te staan
      }
    }
  }
  Serial.println(randNumber); //dit gebruikte ik om te debuggen
  if (buttonPushes == randNumber && buttonPushes != 0) { //als het aantal knopdrukjes - na twee seconden wachten - gelijk is aan het willekeurige generereerde nummer, voer dit statement uit
    MFS.write((int)tijd); //laat op het display zien hoe lang de speler erover heeft gedaan om het nummer te raden (gecast naar int omdat het display float's met veel decimalen niet zomaar kan weergeven)
    if (tijd < winnaarsTijd) { //als de tijd kleiner is dan de snelste tijd, dan wordt de code in dit blok uitgevoerd
      winnaarsTijd = tijd; //de winnaarstijd wordt gelijk gesteld aan de tijd die de speler erover heeft gedaan
      MFS.beep(5, 5, 4, 2); //ga piepen als je het goed hebt
    }
  }
  else if (buttonPushes > 0) {
    MFS.write(""); //niet op tijd op de knopjes gedrukt maar wel gedrukt? Dan wordt het display leeg gemaakt
    MFS.write("FAIL"); //als de speler het potjes niet succesvol heeft afgerond, komt er 'FAIL' op het display te staan
  }
  previousMillis = currentMillis; //corrigeer de tijd zodat de volgende beurt weer 2 seconden zal duren
}
