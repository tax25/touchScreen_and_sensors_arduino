#ifndef H_ALL_DEFINES
#define H_ALL_DEFINES

// DHT22 pin defines
#define pinSensore 22
#define tipoSensore DHT22

//clock defines
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

#endif
