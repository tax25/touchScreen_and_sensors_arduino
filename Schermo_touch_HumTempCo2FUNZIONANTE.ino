//v1.0 (new code!) (release date: 20 April 2020)
//v1.5 (minor bug fixes) (release date: 22 April 2020)
//v2.0 (graphics imporvements, new function introduction) (release date: 22 April 2020, 20 mins after)
//v2.5 (minor bug fixes) (release date: 24 April 2020)
//v3.0 (bug fixes and change page speed improvement) (release date: 27 April 2020)
//v3.5 (last 10 hours temperature graphic) (release date: 1 May 2020)
//v4.0 (last ten hours temperature graphic improvement) (release date: 4 May 2020)

//Touch Screen Libraries
#include <TouchScreen.h>
#include <Elegoo_TFTLCD.h>
#include <Elegoo_GFX.h> 

//DHT22 sensor defines and libraries 
#include <DHT.h>
#define pinSensore 22
#define tipoSensore DHT22

//clock defines and libraries
#include <virtuabotixRTC.h>  
#define pin1 23
#define pin2 24
#define pin3 25

//define grafica temperatura
#define xRettTemp 10
#define yRettTemp 45

//define grafica generale
#define lunghRett 220
#define altRett   40

//define grafica umidità
#define xRettUm 10
#define yRettUm 160

//define grafica co2
#define xRettCo2 10 
#define yRettCo2 230

//define grafica time
#define xRettT 80
#define yRettT 280
//define dei vari pin
#define LCD_CS A3 
#define LCD_CD A2 
#define LCD_WR A1 
#define LCD_RD A0
#define LCD_RESET A4
#define YP A3 
#define XM A2  
#define YM 9   
#define XP 8 

//color defines to make them more human-readable
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

//touch parameters
#define TS_LEFT 120
#define TS_RT 900
#define TS_TOP 70
#define TS_BOT 920

#define MINPRESSURE 5
#define MAXPRESSURE 1000

#define tempStorage 10

//objects creation
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);    //creo il touchscreen
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);    //dichiaro lo schermo
Elegoo_GFX_Button celsiusButton, fahrenheitButton, RTMbutton, GraphButton, exitButton;    //creo i due bottoni
DHT dht22(pinSensore, tipoSensore);    //creo il sensore
virtuabotixRTC myRtc(pin1, pin2, pin3);

float oldValueTemp = NULL;
float oldValueHum = NULL;
int oldValueCo2 = NULL;
bool isFahrenheit = false, hasTempTypeChanged = true, isQuitted = false;
float low = 21.9, medium = 22.0, high = 26.0;
int oldHours, oldMinutes, oldSeconds;
float tempMeasurements[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

bool isRTMButtonToggled = false, isGraphButtonToggled = false, isToHomeButtonToggled = true;
bool isFirstTime = true; // Used in drawGraph to draw the view only the first time, then we will waiting the close button; 
float temperature = 0.0; 
bool isSecondZeroPassed = false;

//SETUP
void setup() {
  Serial.begin(9600);
  uint16_t identifier = tft.readID(); //uint = unsigned int
  tft.begin(identifier);              // inizializzo lo schermo
  tft.setRotation(2);                 // setto la rotazione in orizzonatale
  tft.fillScreen(BLACK);              // setto lo sfondo a NERO
  RTMbutton.initButton(&tft, 115, 115, 50, 20, WHITE, BLUE, WHITE, "RTM", 2);     // colori = colore del contorno, colore dello sfondo del tasto, colore della scritta
  GraphButton.initButton(&tft, 115, 210, 70, 25, WHITE, BLUE, WHITE, "Graph", 2); // colori = colore del contorno, colore dello sfondo del tasto, colore della scritta
  //Buttons inside RTM initializations
  celsiusButton.initButton(&tft, 35, 110, 50, 20, WHITE, BLUE, BLACK, "C", 2);    // colori = colore del contorno, colore dello sfondo del tasto, colore della scritta
  fahrenheitButton.initButton(&tft, 110, 110, 50, 20, WHITE, BLUE, BLACK, "F", 2);
  
  dht22.begin();     // inizializzo il sensore
  
  //l'istruzione qui sotto va eseguita solo una volta, si carica lo sketch una volta con l'orario e poi lo si ricarica con la linea commentata
  //se no si resetterà l'orario e ripartirà sempre dalla stessa ora
  //myRtc.setDS1302Time(00, 31, 21, 5, 24, 4, 2020); //secondi, minuti, ore, giorno della settimana, giorno del mese, mese, anno

}
//LOOP
void loop() {

  if (isToHomeButtonToggled) {
    RTMbutton.drawButton();
    GraphButton.drawButton();
  
    tft.setCursor(55,10);
    tft.setTextColor(RED);
    tft.setTextSize(2);
    tft.println("Home Screen");
  
    //Dividing line 
    tft.drawRect(15, 30, 210, 2, WHITE);
  
    //REAL TIME MONITORING
    tft.setTextSize(2);
    tft.setCursor(65, 40);
    tft.setTextColor(GREEN);
    tft.println("Real Time");
    tft.setCursor(60, 60);
    tft.println("Monitoring");
    tft.setCursor(85, 80);
    tft.println("(RTM):");
  
    //TEMPERATURE AND HUMIDITY GRAPHIC
    tft.setCursor(40, 150);
    tft.setTextColor(GREEN);
    tft.println("Temperature &");
    tft.setCursor(25, 170);
    tft.println("Humidity Graphic:");

    isToHomeButtonToggled = false;

    //Resetting variables to make the print faster
    oldValueHum = NULL;
    oldValueTemp = NULL;
    oldValueCo2 = NULL;
    oldHours = NULL;
    oldMinutes = NULL;
    hasTempTypeChanged = true;
  }
  
   temperature = dht22.readTemperature(isFahrenheit);
   printingTime();
   
   if (isRTMButtonToggled) {
    RTM();
   } else if (isGraphButtonToggled) {
    drawGraph();
   } else {
    homeScreenView();
   }

   
 
}

void homeScreenView(){

    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
    
    if ((p.z > MINPRESSURE) && (p.z < MAXPRESSURE)){
        p.x = map(p.x, TS_LEFT, TS_RT, tft.width(),0);
        p.y = tft.height()-map(p.y, TS_TOP, TS_BOT, tft.height(),0);      
    }
    //RTM button
    if (RTMbutton.contains(p.x, p.y)) {
    RTMbutton.press(true);  
      } else {
        RTMbutton.press(false);
      }
   
   if (RTMbutton.justPressed() == true){
    RTMbutton.drawButton(true);
    tft.fillScreen(BLACK);
    isRTMButtonToggled = true;
    RTM();
   }
   if (RTMbutton.justReleased() == true){
      RTMbutton.drawButton(false);
     }
     //Graph button
   if (GraphButton.contains(p.x, p.y)){
      GraphButton.press(true);
   }else {
      GraphButton.press(false);
   }
   if (GraphButton.justPressed() == true){
      GraphButton.drawButton(true);
      tft.fillScreen(BLACK);
      drawGraph();
      isGraphButtonToggled = true;
   }

}

//RTM function, RTM button
void RTM(){

  if (oldValueHum == NULL) { // It is executed one time !
    exitButton.initButton(&tft, 185, 110, 50, 20, WHITE, BLUE, BLACK, "BACK", 2);
    //Temperature  
    tft.setCursor(10, 25);              // (x, y)
    tft.setTextSize(2);
    tft.setTextColor(RED);
    tft.println("Temperature:");
    tft.drawRect(xRettTemp, yRettTemp, lunghRett - 82, altRett, WHITE);

    //temperature unit of measurement 
    tft.drawRect(xRettTemp + 145, yRettTemp, lunghRett - 145, altRett, WHITE);
  
    //humidity unit of measurement 
    tft.setCursor(11, 140);
    tft.setTextSize(2);
    tft.println("Humidity(%):");
    tft.drawRect(xRettUm, yRettUm, lunghRett, altRett, WHITE);

  
    //co2 unit of measurement
    tft.setCursor(10, 210);
    tft.setTextSize(2);
    tft.println("Co2:");
    tft.drawRect(xRettCo2, yRettCo2, lunghRett, altRett, WHITE);

    //WATCH 
    tft.setCursor(11, 285);
    tft.println("Time: ");
    tft.drawRect(xRettT, yRettT, lunghRett - 70, altRett - 10, WHITE);
  
  
    //drawing Buttons
    celsiusButton.drawButton();
    fahrenheitButton.drawButton();
    exitButton.drawButton();
  }
  
  float humidity = 0.0;
  int  co2 = 0;
  int i = 0;
 
 //Touch
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    
 //reading and printing temperature
   
   printingTempValues(temperature, xRettTemp + 5, yRettTemp + 12, 2, xRettTemp + 145, yRettTemp);
   

 //Humidity Reading and printing
 //readHumidity() returns a float
    
    humidity = dht22.readHumidity(); //L'umidità è in percentuale
    printingHumValues(humidity, xRettUm +5, yRettUm + 12, 2);
    
 //co2 reading and printing, returns an int
    co2 = analogRead(11);
    printingCo2Values(co2, xRettCo2 + 5, yRettCo2 + 12, 2);
    
 
  //CLOCK
  //printingTime();  
    
 //touch
 
    if ((p.z > MINPRESSURE) && (p.z < MAXPRESSURE)){
        p.x = map(p.x, TS_LEFT, TS_RT, tft.width(),0);
        p.y = tft.height()-map(p.y, TS_TOP, TS_BOT, tft.height(),0);      
    }
   
  //are buttons pressed?
        //First button
   if (celsiusButton.contains(p.x, p.y)) {
    celsiusButton.press(true);  
      } else {
        celsiusButton.press(false);
      }
   
   if (celsiusButton.justPressed() == true){
    celsiusButton.drawButton(true);
    low = 21.9;
    medium = 22;
    high = 26;
    isFahrenheit = false;
    hasTempTypeChanged = true;
   }
   
      //Second button
   
   if (fahrenheitButton.contains(p.x, p.y)) {
      fahrenheitButton.press(true);  
      } else {
      fahrenheitButton.press(false);  
      }
  
  if (fahrenheitButton.justPressed() == true){
      fahrenheitButton.drawButton(true);
      low = 71,5;
      medium = 71,6;
      high = 78,8;
      isFahrenheit = true;
      hasTempTypeChanged = true;
   }

      //Are buttons released?
    
  if (celsiusButton.justReleased() == true){
      celsiusButton.drawButton(false);
     }
  if (fahrenheitButton.justReleased()== true) {
      fahrenheitButton.drawButton(false);
     }
     
  if (exitButton.contains(p.x, p.y)){
    exitButton.press(true);
  }else{
    exitButton.press(false);
  }
  if (exitButton.justPressed() == true){
    exitButton.drawButton(true);
    tft.fillScreen(BLACK);
    isToHomeButtonToggled = true;
    isRTMButtonToggled = false;
  }
  if (exitButton.justReleased() == true){
    exitButton.drawButton(false);
  }
 
}

//TEMPERATURE GRAPH
//the biggest rectangle possibly drawn means 36 celsius (96,8 F), the height of it is 288 px, 8 px means 1 degree celsius                      
void drawGraph(){
  if (isFirstTime) {
    //drawing the exit button
    exitButton.initButton(&tft, 200, 20, 50, 20, WHITE, BLUE, BLACK, "BACK", 2);
    exitButton.drawButton();
  
    //graphics of the graph
    tft.drawFastHLine(10, 310, 215, WHITE);
    tft.drawFastVLine(10, 10, 300, WHITE);
    
    //Drawing the graph
    temperatureGraphic(tempMeasurements, 8);

    isFirstTime = false;
  }

  //printingTime();
  
  //Exit button
  //Checks if the button is pressed
  
      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);
      pinMode(YP, OUTPUT);
      pinMode(XM, OUTPUT);
    
    if ((p.z > MINPRESSURE) && (p.z < MAXPRESSURE)){
        p.x = map(p.x, TS_LEFT, TS_RT, tft.width(),0);
        p.y = tft.height()-map(p.y, TS_TOP, TS_BOT, tft.height(),0);      
    }
    
    if (exitButton.contains(p.x, p.y)){
    exitButton.press(true);
    }else{
      exitButton.press(false);
    }
    if (exitButton.justPressed() == true){
      exitButton.drawButton(true);
      tft.fillScreen(BLACK);
      isFirstTime = true;
      isGraphButtonToggled = false;
      isToHomeButtonToggled = true;
    }
    if (exitButton.justReleased() == true){
      exitButton.drawButton(false);
    }
}

//temperature print
void printingTempValues(float measurement, int x, int y, int textSize, int xposMesUnit, int yposMesUnit){
  /*
   * In pratica ho una variabile globale che incamera ogni volta il valore della misura
   * così nel caso il nuovo valore sia uguale a quello vecchio non lo stampa.
   * Nel caso in cui il nuovo valore sia diverso dal precedente, il vecchio viene ristampato in nero
   * e il nuovo viene scritto nel colore che viene scelto in base alla grandezza del valore
  */

    // Cleaning old text type temperature
    if (hasTempTypeChanged) {
    if (isFahrenheit) {
      tft.setCursor(xposMesUnit + 30, yposMesUnit + 12);
      tft.setTextColor(BLACK);
      tft.print("C");
    } else {
       tft.setCursor(xposMesUnit + 30, yposMesUnit + 12);
       tft.setTextColor(BLACK);
       tft.print("F");
    }
    
    if (isFahrenheit){
      tft.setCursor(xposMesUnit + 30, yposMesUnit + 12);
      tft.setTextColor(RED);
      tft.print("F");
    } else {
        tft.setCursor(xposMesUnit + 30, yposMesUnit + 12);
        tft.setTextColor(RED);
        tft.print("C");
    }
    hasTempTypeChanged = false;
    }
  
  
  if (oldValueTemp != measurement) {
    if (oldValueTemp != NULL) {
      tft.setCursor(x, y);
      tft.setTextSize(textSize);
      tft.setTextColor(BLACK);
      tft.println(oldValueTemp);
      oldValueTemp = NULL;
    }

    tft.setCursor(x, y);
    tft.setTextSize(textSize);
    
    if(isnan(measurement)){
      tft.setTextColor(BLUE);
      tft.println("NO_DATA");
      oldValueTemp = NULL;
    } else if ((measurement <= low)){
       //Serial.println("Misura bassa: " + String(measurement));
       tft.setTextColor(BLUE);
       tft.println(measurement);
       oldValueTemp = measurement;
             }
     else if ((measurement > medium) && (measurement < high)){
      tft.setTextColor(YELLOW);
      tft.println(measurement);
      oldValueTemp = measurement;
              }
     else if (measurement > high){
      tft.setTextColor(RED);
      tft.println(measurement);
      oldValueTemp = measurement;
             }
  }
}
//Humidity print
void printingHumValues(float measurement, int x, int y, int textSize){
  
  if (oldValueHum != measurement) {
    if (oldValueHum != NULL) {
       tft.setCursor(x, y);
       tft.setTextSize(textSize);
       tft.setTextColor(BLACK);
       tft.println(oldValueHum);
       oldValueHum = NULL;
    }

    tft.setCursor(x, y);
    tft.setTextSize(textSize);
  
    if(isnan(measurement)){
       tft.setTextColor(BLUE);
       tft.println("NO_DATA");
       oldValueHum = NULL;
    }
     else if (measurement <= 20.0){
       tft.setTextColor(BLUE);
       tft.println(measurement);
       oldValueHum = measurement;
    }
     else if (measurement > 20.0 && measurement <= 59.9){
        tft.setTextColor(YELLOW);
        tft.println(measurement);
        oldValueHum = measurement;
     }
      else if (measurement >= 60){
        tft.setTextColor(RED);
        tft.println(measurement);
       oldValueHum = measurement;
    }
  }
}
//Co2 print
void printingCo2Values(int measurement, int x, int y, int textSize){

  float measurementH = measurement + (measurement*0.05); //Prints the new value if and only if the new value is bigger or smaller than the older +- 5% of itself
  float measurementL = measurement - (measurement*0.05);
  /*
  Serial.println("OldValue: " + String(oldValueCo2));
  Serial.println("MeasurementH: " + String(measurementH));
  Serial.println("MeasurementL: " + String(measurementL));
  Serial.println("Measurement: " + String(measurement));
  */
   if((oldValueCo2 > measurementH) || (oldValueCo2 < measurementL)){
    if (oldValueCo2 != NULL){
      tft.setCursor(x, y);
      tft.setTextSize(textSize);
      tft.setTextColor(BLACK);
      tft.println(oldValueCo2);      
    }
    tft.setCursor(x, y);
    tft.setTextSize(textSize);
    tft.setTextColor(MAGENTA);
    tft.println(measurement);
    oldValueCo2 = measurement;
  }
  
}

//TIME PRINTING
void printingTime(){
  
    
      myRtc.updateTime(); //funzione che aggiorna le variabili del tempo 
int i = 0;
      if (isRTMButtonToggled) {
        Serial.println("Ci sono");
        tft.setTextSize(2); 
        if ((myRtc.hours != oldHours) || (myRtc.minutes != oldMinutes)){
          Serial.println("********Ci sono");
          tft.setCursor(xRettT + 10, yRettT + 8);
          tft.setTextColor(BLACK);
          tft.println(String(oldHours) + " : " + String(oldMinutes));
          tft.setTextColor(BLUE);
          tft.setCursor(xRettT + 10, yRettT + 8);
          tft.println(String(myRtc.hours) + " : " + String(myRtc.minutes));
        }
      }
      
      oldHours   = myRtc.hours;
      oldMinutes = myRtc.minutes;
      
      //Serial.println("Secondi: " + String(myRtc.seconds) + " // prima dell'if = " + String(tempMeasurements[10]));
      if (int(tempMeasurements[10]) > 0){
        //Serial.println("Sono dentro l'if");
        for(i = 0; i < tempStorage - 1; i++){
          tempMeasurements[i] = tempMeasurements[i+1];
        }
        tempMeasurements[10] = 0;
        i = 10;
      }

      if (myRtc.hours > 1) {
        isSecondZeroPassed = true;
      }
      
      if ((myRtc.hours == 0) && (isSecondZeroPassed == true)) {
          isSecondZeroPassed = false;        
          tempMeasurements[i] = temperature;
          //Serial.println("Dentro seconds" + String(tempMeasurements[i]));
          //Serial.println("Indice = " + String(i));
          i++;
      }
}

void temperatureGraphic(float tempArray[], int multiplier){
  int x = 20;
  int y = 309; //the bigger the value the lower is the column
  int width = 10;
  int height = 0;



  for (int i = 0; i < tempStorage; i++){
      if (tempArray[i] > 0){
          height = tempArray[i] * multiplier;
          tft.drawRect(x, y - height,width, height, RED);
          tft.fillRect(x, y - height, width, height, RED );
          x+=20;
        }
      }
}
