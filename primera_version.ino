#include <UTFT.h>
#include <UTouch.h>

#include <Servo.h>
//#include <UTFT_tinyFAT.h>
//#include <tinyFAT.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include <SimpleTimer.h>



#define X_KMH 50
#define Y_KMH 36
#define X_RPM 0
#define Y_RPM 0

//Pines de cambios....
#define PIN_0RA 8
#define PIN_1RA 9
#define PIN_2DA 10
#define PIN_3RA 11
#define PIN_4TA 12
#define PIN_5TA 13

//Pin del sensor de rpm (cooler)...
#define PIN_RPM 18

//Pines del relay del burro de arranque y servo acelerador

#define PIN_SERVO 57 //Analog input pin 3 acelerador
#define PIN_CEBA 56 //Analog input pin 2 cebador
#define PIN_BURRO 55 //Analog input pin 1 burro
#define PIN_CONTA 54 //Analog input pin 0 contacto
#define PIN_POWSV 58 //Analog input pin 4 Fuente para los servos
#define PIN_NAFTA A7 //Analog input pin 7 sensor de nafta


#define MAX_MSJ 18
#define MSJ_SHOW 6


#define COLOR_FONT_R 255
#define COLOR_FONT_G 255
#define COLOR_FONT_B 255

#define COLOR_BACK_R 0
#define COLOR_BACK_G 0
#define COLOR_BACK_B 0

//Rectangulo
#define LINE_THICK 5


#define SLIDER_LENGTH 100
#define SLIDER_BUTTON_H 10

#define X_MARCHA 45
#define Y_MARCHA 140

#define x_hora 15
#define y_hora 8


struct config_t
{
  volatile long m_totales_i;
  volatile long m_totales_f;
  int  acelerador;
  int  cuanto_acel;    
  int  cuanto_desacel;   
  int  burro;
  int  cebador;
  int  guardado;
  int  clave;
} config;



// Declare which fonts we will be using
extern uint8_t SevenSegNumFont[];
extern uint8_t BigFont[];
extern uint8_t SmallFont[];

word res;

UTFT myGLCD(ITDB32S,38,39,40,41);
UTouch myTouch(6,5,4,3,2);
Servo myAcelerador; // Servo acelerador
Servo myCebador; // Servo acelerador

//init the real time clock
Rtc_Pcf8563 rtc;
SimpleTimer timer;

boolean contacto = false;

unsigned long ahora;
unsigned long antes;

int KMH = 0;
int RPM = 0;
int NAFTA = 100;
int TEMP = 888;
int previa = 99; //Marcha previa


int timerNafta ;
int timerOdo;
int timerHora;
int timerMarcha;
int timerEscucha;


int acelerado = 750;
int desacelerado = 1000;
int cuanto = 80;

char textBuffer[81];

int x, y;

String t_mensajes[] = {
  "","","","","","","","","","","","",""}; // 7?
int idx_mensajes = -1;

//boolean repetir_login = true;
boolean repetir_login = false;

char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";


void printKmTotales()
{

  String aux_str;
  char aux_char[30];
  int idx = 0;
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0,0,0);
  myGLCD.setColor(0, 255, 0);


  aux_str = String(config.m_totales_f);
  if (aux_str.length() > 2)
    aux_str = aux_str.substring(0,2);

  myGLCD.print(aux_str, 250,110);
  myGLCD.print(".", 237,110);
  myGLCD.printNumI(config.m_totales_i,160,110,5,'0');

}

void printMarcha()
{
  //Pines :
  // punto muerto: D8 PIN_0RA
  // primera: D9 PIN_1RA
  // segunda: D10 PIN_2DA
  // tercera: D11 PIN_3RA
  // cuarta: D12 PIN_4TA
  // quinta: D13 PIN_5TA

  int numero;

  for (int i = 8; i < 14 ; i++)
  {
    if ( digitalRead(i) > 0)
    {
      numero = i;
      break;
    }
  }
  numero = numero - 8; //Corrije el desplazamiento de los pines respecto de la marcha actual
  marcha(X_MARCHA,Y_MARCHA,numero);

}


void printKmh( int velocidad )
{
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setBackColor(0,0,0);
  myGLCD.setColor(0, 255, 0);

  myGLCD.printNumI(velocidad, X_KMH, Y_KMH, 3, '0');

}

int linea_mensaje = 50;

#define y_linea_mensaje = 150;


void agregarMensaje( String m )
{
  String t_aux[30];
  String l_aux;
  boolean alto;
  int idx = 0;
  int longitud = m.length() / MAX_MSJ;
  if ( m.length() % MAX_MSJ > 0)
    longitud++;
  //split message if too large...
  alto = false;
  for ( int p = 1; !alto ; p++)
  {
    l_aux = m.substring(idx, (idx + MAX_MSJ));
    idx = idx + MAX_MSJ;
    t_mensajes[++idx_mensajes] = l_aux;
    if ( idx > m.length() )
      alto = true;
  }
  if (idx_mensajes >= MSJ_SHOW)
  {
    //Displace messages up (or down?)
    for ( int n = 0; n <= MSJ_SHOW ; n++)
    {
      l_aux = t_mensajes[idx_mensajes - n];
      t_aux[MSJ_SHOW - n] = l_aux;
    }
    idx_mensajes = MSJ_SHOW;

    for ( int p = 0; p <= MSJ_SHOW; p++)
    {
      t_mensajes[p] = t_aux[p];
    }
  }
  printMessages();
}


#define y_msj 150
#define x_msj 170


void printMessages()
{
  int i = 0;
  String aux = "";
  // Print messages table each line on it's own, inside(hopefully) the info-box
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255,255,255);
  myGLCD.setBackColor(0,0,0);
  for ( i = 0; i<= MSJ_SHOW; i++) //Print up to MSJ_SHOW messages
  {
    aux = t_mensajes[i];
    while(1)
    {
      aux.concat(" ");
      if ( aux.length() >= MAX_MSJ )
        break;
    }
    // myGLCD.print(t_mensajes[i],x_msj,(y_msj + (i * 13))); //Advance each Y coordinate by the amount of font height (8?)
    myGLCD.print(aux,x_msj,(y_msj + (i * 12))); //Advance each Y coordinate by the amount of font height (8?)
  }
  if ( i != MSJ_SHOW)
  {
    aux = "";
    for ( int j ; j <= MAX_MSJ ; j++)
      aux.concat(" "); //Fill a blank line of MAX_MSJ length

    for ( i ; i<= MSJ_SHOW; i++) //Print up to MSJ_SHOW blank lines
      myGLCD.print(aux,x_msj,(y_msj + (i * 12))); //Advance each Y coordinate by the amount of font height (8?)
  }

}



void imprimirHora()
{
  String hora = "";
  char *valores;
  String minuto = "";
  /*
valores = rtc.formatTime();
   hora = strtok( valores, ":");
   minuto = strtok( NULL, ":");
   hora = hora + ":" + minuto;
   */
  // Saved to time counter
  unsigned long t = millis();

  long r;
  long h = t / 3600000 ; // divide by millisecs per hour => hrs
  r = t % 3600000;
  int m = r / 60000; // divide rest by millisecs per minute => mins
  r = r % 60000;
  int s = r / 1000; // divide rest by millisecs per second => secs

  if ( m < 10 )
    hora = hora + "0" + m;
  else
    hora = hora + m;
  hora = hora + ":";
  if ( s < 10 )
    hora = hora + "0" + s;
  else
    hora = hora + s;

  //Debugging de la hora...
  hora = hora + " - 29/04/2013";


  myGLCD.setFont(BigFont);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.setBackColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.print(hora,x_hora,y_hora);

}


int baja_nafta = 500;

void imprimirNafta()
{
  int gas = 0;
  long real = 0;

  real = analogRead(PIN_NAFTA);
  gas = map(real,baja_nafta,0,0,10);
  NAFTA = gas;
  medidorNafta(120,130,real,gas);
}

void mainScreen()
{


  timer.enable(timerHora);
  myGLCD.clrScr();
  lineaCabecera();
  messageBox();


  myGLCD.setFont(BigFont);
  //Arrancar
  rectangulo (10,40,150,130);
  myGLCD.print("ARRANCAR",17,80);

  //Viajar
  rectangulo (10,145,150,230);
  myGLCD.print("VIAJAR",35,180);

  //Setup
  circulo(255,85,50);
  myGLCD.print("SETUP",217,82);

}


void menuPrincipal()
{

  if(repetir_login)
  {
    agregarMensaje("Solicitando password");
    initLogin();
    agregarMensaje("Succesful login!");
  }
  mainScreen();
  printMessages();
}

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Inicializacion"));
  agregarMensaje("Inicializacion");
  // Initial setup

  myGLCD.InitLCD(LANDSCAPE);

  myGLCD.clrScr();

  myTouch.InitTouch(LANDSCAPE);

  myTouch.setPrecision(PREC_HI);

  pinMode(PIN_RPM, INPUT); //Velocimetro (PIN 18 para el interrupt)
  digitalWrite(PIN_RPM,HIGH);// Internal pull up resistor(?)

  delay(100);

  messageBox();
  //Inicializacion Servo
  pinMode(PIN_SERVO, OUTPUT);
  pinMode(PIN_CEBA, OUTPUT);
  pinMode(PIN_BURRO, OUTPUT);
  pinMode(PIN_CONTA, OUTPUT);

  pinMode(PIN_0RA, INPUT); //Deteccion de marcha 0
  digitalWrite(PIN_0RA, HIGH); //Pull up resistor
  pinMode(PIN_1RA, INPUT); //Deteccion de marcha 1
  digitalWrite(PIN_1RA, HIGH); //Pull up resistor
  pinMode(PIN_2DA, INPUT); //Deteccion de marcha 2
  digitalWrite(PIN_2DA, HIGH);//Pull up resistor
  pinMode(PIN_3RA, INPUT); //Deteccion de marcha 3
  digitalWrite(PIN_3RA, HIGH);//Pull up resistor
  pinMode(PIN_4TA, INPUT); //Deteccion de marcha 4
  digitalWrite(PIN_4TA, HIGH);//Pull up resistor
  pinMode(PIN_5TA, INPUT); //Deteccion de marcha 5
  digitalWrite(PIN_5TA, HIGH);//Pull up resistor

  pinMode(PIN_NAFTA, INPUT);

  attachInterrupt(5, contar, RISING); //PIN_RPM
  agregarMensaje("Cargando preferencias...");
  cargar();


  agregarMensaje("Setup de timers..");
  timerEscucha = timer.setInterval(2000, callEscuchar);
  timerNafta = timer.setInterval(10000, imprimirNafta);
  timerHora = timer.setInterval(1000, imprimirHora);
  timerOdo = timer.setInterval(config.guardado,guardarOdometro);
  timerMarcha = timer.setInterval(200, printMarcha);
  timer.disable(timerNafta);
  timer.disable(timerHora);
  timer.disable(timerOdo);
  timer.disable(timerMarcha);
  agregarMensaje("Fin inicializacion");
  Serial.println(F("Fin Inicializacion"));

  menuPrincipal();

}


void cargar()
{
  EEPROM_readAnything(0, config);
}

void waitForIt()
{
  while (myTouch.dataAvailable())
    myTouch.read();
}


void printWelcome()
{
  Serial.println(F("HOLA"));
  Serial.println(F("Secuencia de comandos disponibles:"));
  Serial.println(F("KMTOT --> Kilometros totales, enteros y decimales por separado"));
  Serial.println(F("KMSET --> Para setear kilometros totales, enteros y decimales por separado"));
  Serial.println(F("START --> Arranque de la moto"));
  Serial.println(F("CONTA --> Pone en contacto la moto"));
  Serial.println(F("LOCKS --> Pide password al usuario"));
  Serial.println(F("VIAJA --> Ingresa a la pantalla de viajar"));
  Serial.println(F("POWER --> Ingresa a la pantalla de arranque"));
  // Serial.println("");


  Serial.println(F("GOODB --> Para cerrar sesion"));
  Serial.println(F(""));
}

int conectado = 0;

void escuchar( int donde )
{
  /*
Protocolo de comunicacion:
   PC: HELLO
   YO: HOLA
   PC: [CODIGO1]
   YO: [RESPUESTA1]
   PC: [CODIGO2]
   YO: [RESPUESTA2]
   .
   .
   .
   PC: [CODIGON]
   YO: [RESPUESTAN]
   PC: GOODB
   YO: CHAU
   Se deben recibir de a 5 caracteres los comandos (FIJO!)
   donde: contiene desde donde se llama la funcion de escuchar (calentar, arrancar, viajar, etc)
   0 = lo op principal
   1 = calentar
   2 = viajar
   */

#define LC_PRINCIPAL 0
#define LC_CALENTAR 1
#define LC_VIAJAR 2

  int incomingByte = 0;
  char letra;
  String palabra = String();
  if ( Serial.available() >= 5) //<--- fijoo!
  {
    for(int i = 0; i < 5; i++)
    {
      letra = Serial.read();
      palabra.concat(letra);
    }
    // Serial.print(F("Palabra tiene: "));
    // Serial.println(palabra);
    if ( palabra == "HELLO")
    {
      conectado = 1;
      palabra = "";
      printWelcome();
      return;
    }
    if (conectado != 0 )
    {
      if ( palabra == "KMTOT")
      {
        Serial.println(F("Km totales, parte entera:"));
        Serial.println(config.m_totales_i);
        Serial.println(F("Km totales, parte decimal (5 digitos)"));
        Serial.println(config.m_totales_f);
      }
      else if ( palabra == "START" )
      {
        if ( donde != LC_PRINCIPAL )
          Serial.println(F("No se permite arrancar desde un submenu"));
        else
          arranqueAutomatico();
      }
      else if ( palabra == "CONTA")
      {
        digitalWrite(PIN_CONTA,HIGH);
        Serial.println(F("En contacto"));
      }
      else if ( palabra == "LOCKS" )
      {
        Serial.println(F("Bloqueando pantalla"));
        initLogin();
        menuPrincipal();
        Serial.println(F("Desbloqueado"));
      }
      else if ( palabra == "VIAJA" )
      {
        Serial.println(F("Ingresando a viajar"));
        viajar();
        menuPrincipal();
      }
      else if ( palabra == "POWER" )
      {
        Serial.println(F("Ingresando en power screen"));
        powerScreen();
      }
      else if ( palabra == "KMSET")
      {
        palabra = "";
        Serial.println("Ingrese la parte decimal: (5 digitos)");
        while ( Serial.available() < 5 ); //wait
        for(int i = 0; i < 5; i++)
        {
          letra = Serial.read();
          palabra.concat(letra);
        }
        char str[10];
        palabra.toCharArray(str,10);
        config.m_totales_f = atol(str);
        Serial.print("Km totales f seteado en :");
        Serial.println(config.m_totales_f);
        palabra = "";
        Serial.println("Ingrese la parte entera: (5 digitos)");
        while ( Serial.available() < 5 ); //wait
        for(int i = 0; i < 5; i++)
        {
          letra = Serial.read();
          palabra.concat(letra);
        }
        char str_aux[10];
        palabra.toCharArray(str_aux,10);
        config.m_totales_i = atol(str_aux);
        Serial.print("Km totales i seteado en :");
        Serial.println(config.m_totales_i,DEC);
      }
    }
    else
      Serial.println(F("No se ha iniciado la sesion"));
    if ( palabra == "GOODB")
    {
      conectado = 0;
      Serial.println(F("CHAU"));
    }
  }
}


void loop()
{

  timer.run();

  if (myTouch.dataAvailable())
  {

    myTouch.read();
    x=myTouch.getX();
    y=myTouch.getY();

    if ( (x < 150) && ( x > 0) )
    {
      if ( ( y < 130 ) && ( y > 40 )) //opcion Arrancar
      {
        waitForIt();
        arrancarMoto();
      }
      else if ( ( y > 145 ) && ( y < 230 ) ) //opcion viajar
      {
        waitForIt();
        viajar();
        // Serial.println("Sale de viajar");
        menuPrincipal();
      }
    }
    // circulo(255,85,50);
    if ( x > 200 && y > 40 && y < 140 )
    {
      waitForIt();
      opciones();
      menuPrincipal();


    }
  }
}

void opciones()
{

  boolean salir = false;

  setupScreen();

  while (!salir)
  {
    timer.run();
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      // 10,40,155,96);
      if( x > 10 && x < 155 && y > 30 && y < 96)
      {//Hora
        waitForIt();
        setHora();
        setupScreen();
      }
      //Salir
      if ( (x < 31 ) && ( y > 210 ) )
      {
        salir = true;
      }
      //Mas opciones
      if ( (x > 269 ) && ( y > 200 ) )
      {
        waitForIt();
        masOpciones(); //Mas opciones
        setupScreen();
      }
      if( x > 10 && x < 155 && y > 106 && y < 162)
      {//Hora
        waitForIt();
        confArranque();
        setupScreen();
      }
      if( x > 170 && x < 320 && y > 30 && y < 96)
      {//Nafta
        waitForIt();
        confNafta();
        setupScreen();
      }
    }
  }
}

void confArranque()
{

  boolean salir = false;
  boolean guardado = false;
  int valor_burro = 0;
  int aux_valor_burro = 0;
  int valor_cebador = 0;
  int aux_valor_cebador = 0;
  int valor_acelerador = 0;
  int aux_valor_acelerador = 0;
  int valor_tacel = 0;
  int aux_valor_tacel = 0;
  int valor_tdesacel = 0;
  int aux_valor_tdesacel = 0;

  confArranqueScreen();

  while (!salir)
  {

    timer.run();

    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      //Salir
      if ( (x < 31 ) && ( y > 210 ) )
      {
        if (!guardado)
        {
          rectangulo(30,100,310,200);
          myGLCD.setColor(255,0,0);
          myGLCD.setFont(BigFont);
          myGLCD.setBackColor(COLOR_BACK_R,COLOR_BACK_G,COLOR_BACK_B);
          myGLCD.print("NO SE GUARDARON",41,130);
          myGLCD.print("LOS CAMBIOS",51,150);
          delay(1000);
        }
        salir = true;
      }
      //Salir
      if ( (x > 290 ) && ( y > 210 ) )
      {
        //Llamar rutina de guardado
        guardado = true;
        EEPROM_writeAnything(0, config);
      }
      if ( (x > 10 ) && (x < 150 ) && ( y > 70 ) && ( y < 110 ) )
      {
        //Tiempo giro del burro...
        valor_burro = round( ( x - 30 ) / 20);
        if ( valor_burro != aux_valor_burro )
        {
          sliderH(30,90,valor_burro);
          aux_valor_burro = valor_burro;
          //Guardar valor burro de arranque
          config.burro = map(valor_burro,0,5,500,2500); 
        }
      }

      if ( (x > 10 ) && (x < 150 ) && ( y > 120 ) && ( y < 160 ) )
      {
        //Cebador
        valor_cebador = round( ( x - 30 ) / 20);
        if ( valor_cebador != aux_valor_cebador )
        {
          sliderH(30,140,valor_cebador);
          aux_valor_cebador = valor_cebador;
          //Guardar valor cebador
          config.cebador = map(valor_cebador,0,5,500,2500);
        }
      }

      if ( (x > 160 ) && (x < 300 ) && ( y > 70 ) && ( y < 110 ) )
      {
        //Acelerador
        valor_acelerador = round( ( x - 180 ) / 20);
        if ( valor_acelerador != aux_valor_acelerador )
        {
          sliderH(180,90,valor_acelerador);
          aux_valor_acelerador = valor_acelerador;
          //Guardar valor acelerador
          config.acelerador = map(valor_acelerador,0,5,70,120);
        }
      }

      if ( (x > 160 ) && (x < 300 ) && ( y > 120 ) && ( y < 160 ))
      {
        //Tiempo Acelerado
        valor_tacel = round( ( x - 180 ) / 20);
        if ( valor_tacel != aux_valor_tacel )
        {
          sliderH(180,140,valor_tacel);
          aux_valor_tacel = valor_tacel;
          //Guardar valor tiempo acelerado
          config.cuanto_acel = map(valor_tacel,0,5,500,2500);
        }
      }

      if ( (x > 160 ) && (x < 300 ) && ( y > 170 ) && ( y < 210 ))
      {
        //Tiempo Desacelerado
        valor_tdesacel = round( ( x - 180 ) / 20);
        if ( valor_tdesacel != aux_valor_tdesacel )
        {
          sliderH(180,190,valor_tdesacel);
          aux_valor_tdesacel = valor_tdesacel;
          //Guardar valor tiempo desacelerado
          config.cuanto_desacel = map(valor_tdesacel,0,5,500,2500);
        }
      }
    }
  }
}

void masOpciones()
{
  boolean salir = false;

  setupScreen2();

  while (!salir)
  {
    timer.run();
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      //Salir
      if ( (x < 31 ) && ( y > 210 ) )
      {
        salir = true;
      }


    }
  }
}

void setupScreen()
{
  myGLCD.clrScr();
  lineaCabecera();
  botonSiguiente();
  botonVolver();

  myGLCD.setFont(BigFont);
  //Hora y fecha
  rectangulo(10,40,155,96);
  myGLCD.print("HORA",50,60);

  //Arranque
  rectangulo(10,106,155,162);
  myGLCD.print("ARRANQUE",20,126);

  //Nafta
  rectangulo(165,40,300,96);
  myGLCD.print("NAFTA",192,60);

  //Guardado
  rectangulo(165,106,300,162);
  myGLCD.print("GUARDADO",170,126);

}

void setupScreen2()
{

  myGLCD.clrScr();
  lineaCabecera();
  botonVolver();

  //Calentar
  rectangulo(10,40,155,96);
  myGLCD.print("CALENTAR",20,60);


  rectangulo(165,40,300,96);
  myGLCD.print("CLAVE",192,60);

}

void setHora()
{
  boolean salir = false;
  int dia = 29;
  int mes = 4;
  int anio = 2013;
  int hh = 16;
  int mm = 25;

  boolean guardado = false;

  confHoraScreen(hh,mm,dia,mes,anio);

  while (!salir)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      waitForIt();
      if( x > 17 && x < 67 )
      {//DIA

        if ( y > 1 && y < 35 )
          dia++;
        else if ( y > 85 && y < 125 )
          dia--;
        if ( dia > 31 )
          dia = 1;
        else if (dia < 1)
          dia = 31;
        guardado = false;
        printDia(dia);
      }

      if( x > 102 && x < 142 )
      {//MES
        if ( y > 1 && y < 35 )
          mes++;
        else if ( y > 85 && y < 125 )
          mes--;
        if ( mes > 12 )
          mes = 1;
        else if (mes < 1)
          mes = 12;
        printMes(mes);
        guardado = false;
      }

      if( x > 220 && x < 260 )
      {//Anio
        if ( y > 1 && y < 35 )
          anio++;
        else if ( y > 85 && y < 125 )
          anio--;
        if ( anio > 2020 )
          anio = 2012;
        else if (anio < 2012)
          anio = 2020;
        printAnio(anio);
        guardado = false;
      }

      if( x > 52 && x < 92 )
      {//Hora
        if ( y > 100 && y < 140 )
          hh++;
        else if ( y > 190 && y < 230 )
          hh--;
        if ( hh > 23 )
          hh = 00;
        else if (hh < 00)
          hh = 23;
        printHh(hh);
        guardado = false;
      }

      if( x > 174 && x < 214 )
      {//Minuto
        if ( y > 100 && y < 140 )
          mm++;
        else if ( y > 190 && y < 230 )
          mm--;
        if ( mm > 59 )
          mm = 00;
        else if (mm < 00)
          mm = 59;
        printMm(mm);
        guardado = false;
      }

      if( x < 45 && y > 210 )
      {//Volver
        if (!guardado)
        {
          rectangulo(30,100,310,200);
          myGLCD.setColor(255,0,0);
          myGLCD.setFont(BigFont);
          myGLCD.setBackColor(COLOR_BACK_R,COLOR_BACK_G,COLOR_BACK_B);
          myGLCD.print("NO SE GUARDARON",41,130);
          myGLCD.print("LOS CAMBIOS",51,150);
          delay(1000);
          salir = true;
        }
      }

      if ( (x < 31 ) && ( y > 210 ) )
      {//Guardar
        guardado = true;
      }
    }

  }



}


void printDia(int dia)
{
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 215, 0);
  myGLCD.printNumI(dia,15,35,2,'0');
}


void printMes(int mes)
{
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 215, 0);
  myGLCD.printNumI(mes,100,35,2,'0');
}

void printAnio(int anio)
{
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 215, 0);
  myGLCD.printNumI(anio,187,35,4,'0');
}

void printHh(int hh)
{
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 215, 0);
  myGLCD.printNumI(hh,50,150,2,'0');
}
void printMm(int mm)
{
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 215, 0);
  myGLCD.printNumI(mm,172,150,2,'0');


}
void confHoraScreen(int hh,int mm,int dia,int mes,int anio)
{

  myGLCD.clrScr();
  //Boton volver
  botonVolver();
  //Boton guardar
  botonGuardar();


  //Dia
  sumador(37,5);
  rectangulo(10,30,85,90);
  restador(37,95);
  printDia(dia);


  //Mes
  sumador(122,5);
  rectangulo(95,30,170,90);
  restador(122,95);
  printMes(mes);

  //Año
  sumador(240,5);
  rectangulo(180,30,319,90);
  restador(240,95);
  printAnio(anio);

  //Hora
  sumador(72,120);
  rectangulo(45,145,120,205);
  restador(72,210);
  printHh(hh);


  //Minuto
  sumador(194,120);
  rectangulo(167,145,242,205);
  restador(194,210);
  printMm(mm);

}




volatile long ultima;
volatile long ultima_aux;
volatile long aux;
/*
999 <----> 3600000
 0.000376 <-----> x
 */
void contar()
{
  ultima_aux = millis() - aux; //Save delta of time since last pulse...
  if ( ultima_aux > 5 )
  {
    ultima = ultima_aux;
    aux = millis(); //Remember when this pulse arrived...
    config.m_totales_f = config.m_totales_f + 376; //Total traveled meters since last pulse
    if ( config.m_totales_f >= 1000000 )
    {
      config.m_totales_i++;
      config.m_totales_f = config.m_totales_f - 1000000;
    }
  }
}





void guardarOdometro()
{
  EEPROM_writeAnything(0, config);
}


void callEscuchar()
{
  escuchar(0);
}

void medidorRpm(int x1, int y1, int cuanto)
{
  int ancho = 40;
  // 30 100 22 ejemplo
  int y = 160 + y1; //Para ir de abajo para arriba
  int linea = 160 / 22;
  //40 x 110 px de tamaño, cada 5 px 1 medida (22 max)
  rectangulo(x1,y1,x1+ancho, y1+160);
  myGLCD.setColor(0, 255,0); //Verde
  for (int i = 1 ; i<22;i++)
  {
    if (i > cuanto)
      myGLCD.setColor(0, 0,0); //Negro
    else
      switch(i)
      {
      case 10:
        myGLCD.setColor(255,255,0); //Amarillo
        break;
      case 15:
        myGLCD.setColor(255, 0,0); //rojo
        break;
      }
    myGLCD.fillRect(x1+LINE_THICK,y-(i*linea)+1,(x1+ancho-LINE_THICK),y-(i*linea)-3);
    myGLCD.fillRect(x1+LINE_THICK,y-(i*linea)+1,(x1+ancho-LINE_THICK),y-(i*linea)-2);
    myGLCD.fillRect(x1+LINE_THICK,y-(i*linea)+1,(x1+ancho-LINE_THICK),y-(i*linea)-1);
    // myGLCD.fillRect(x1+LINE_THICK,y-(i*5)+2,(x1+ancho-LINE_THICK),y-(i*5)-2);
    myGLCD.fillRect(x1+LINE_THICK,y-(i*linea),(x1+ancho-LINE_THICK),y-(i*linea));

  }
}

void medidorNafta( int x1, int y1, int real,int cuanto)
{
  int y = 240; //Para ir de abajo para arriba
  int ancho = 20;
  String numero = String(real);
  //40 x 110 px de tamaño, cada 5 px 1 medida (10 max)
  rectangulo(x1,y1,x1+ancho, y1+100);
  y = y1 + 100;
  myGLCD.setFont(SmallFont);
  myGLCD.print(numero,x1+ancho+15,y-25,90);
  myGLCD.setColor(255,0,0); //Rojo
  for (int i = 1 ; i<10;i++)
  {
    if (i > cuanto)
      myGLCD.setColor(0, 0,0); //Negro
    else
      switch(i)
      {
      case 6:
        myGLCD.setColor(0, 255,0); //Amarillo
        break;
      case 3:
        myGLCD.setColor(255, 255,0); //rojo
        break;
      }
    myGLCD.fillRect(x1+LINE_THICK,y-(i*10)+3,x1+ancho-LINE_THICK,y-(i*10)+3);
    myGLCD.fillRect(x1+LINE_THICK,y-(i*10)+2,x1+ancho-LINE_THICK,y-(i*10)+2);
    myGLCD.fillRect(x1+LINE_THICK,y-(i*10)+1,x1+ancho-LINE_THICK,y-(i*10)+1);
    myGLCD.fillRect(x1+LINE_THICK,y-(i*10),x1+ancho-LINE_THICK,y-(i*10));
    myGLCD.fillRect(x1+LINE_THICK,y-(i*10)-1,x1+ancho-LINE_THICK,y-(i*10)-1);
    myGLCD.fillRect(x1+LINE_THICK,y-(i*10)-2,x1+ancho-LINE_THICK,y-(i*10)-2);
    myGLCD.fillRect(x1+LINE_THICK,y-(i*10)-3,x1+ancho-LINE_THICK,y-(i*10)-3);
  }
}

void velocimetro( int x, int y, int velocidad)
{
  rectangulo(x,y,x+105,y+60);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(80,255,143);
  myGLCD.printNumI(velocidad, x+LINE_THICK,y+LINE_THICK, 3,'0');

}

void ponerVelocidad(int x, int y, int velocidad)
{
  myGLCD.setColor(0,255,0);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.printNumI(velocidad, x+LINE_THICK,y+LINE_THICK, 3,'0');
}

void marcha(int x, int y, int marcha)
{

  if (marcha != previa)
  {
    previa = marcha;
    String texto = String(marcha);
    rectangulo(x,y,x+45,y+60);
    myGLCD.setFont(SevenSegNumFont);
    myGLCD.setColor(255,70,43);
    myGLCD.print(texto, x+LINE_THICK+3,y+LINE_THICK);
  }

}

void luces(int x, int y, int posicion, int bajas, int altas)
{

  //Limpiar el area
  myGLCD.setColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.fillRect(x+2,y-20,x+38,y+20);
  myGLCD.setColor(100,100,255);
  myGLCD.fillCircle(x,y,20);

  if (posicion == 1 )
  {

    myGLCD.setColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
    myGLCD.fillCircle(x,y,15);
  }
  else
  {
    //Cortar el semicirculo
    myGLCD.setColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
    myGLCD.fillRect(x+2,y-20,x+38,y+20);

    if (bajas == 1)
    {
      myGLCD.setColor(100,100,255);
      myGLCD.drawLine(x+5,y-8 ,x+35,y);
      myGLCD.drawLine(x+5,y-4 ,x+35,y+4);
      myGLCD.drawLine(x+5,y ,x+35,y+8);
      myGLCD.drawLine(x+5,y+4 ,x+35,y+12);
      myGLCD.drawLine(x+5,y+8 ,x+35,y+16);
    }
    else
    {
      myGLCD.setColor(100,100,255);
      myGLCD.drawLine(x+5,y-8 ,x+35,y-8);
      myGLCD.drawLine(x+5,y-4 ,x+35,y-4);
      myGLCD.drawLine(x+5,y ,x+35,y ) ;
      myGLCD.drawLine(x+5,y+4 ,x+35,y+4);
      myGLCD.drawLine(x+5,y+8 ,x+35,y+8);


    }
  }

}

void odometro(int x, int y)
{
  String aux_str;
  int aux_mts = 0;

  aux_mts = config.m_totales_f / 10000;

  myGLCD.setFont(BigFont);
  myGLCD.setColor(100,100,255);
  myGLCD.printNumI(aux_mts, x+95,y,2,'0');
  myGLCD.print(".", (x+80),y);
  myGLCD.printNumI(config.m_totales_i,x,y,5,'0');
}




void viajarScreen()
{
  myGLCD.clrScr();
  lineaCabecera();
  botonVolver();
  messageBox();

  //Draw everything empty

  medidorRpm(1,40,0);
  medidorNafta(120,130,0,0);


  //160,145,310,230);



  odometro(160,125);
  marcha(X_MARCHA, Y_MARCHA,0);
  velocimetro(100,40,0);
  luces(260,70,1,0,0);

  printMessages();

}


void debugScreen()
{

  boolean salir = false;
  long aux_ultima = 0;
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  myGLCD.print("Debug Screen:",20,30);
  myGLCD.print("ultima: ",20,70) ;
  myGLCD.printNumI(ultima,150,70) ;
  while( !salir)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      if ( y > 140 )
        salir = true;
    }
    if (aux_ultima != ultima )
    {
      myGLCD.print("ultima: ",20,70) ;
      myGLCD.printNumI(ultima,150,70) ;
      aux_ultima = ultima;
    }

  }
  previa = 99; //Para que muestre la marcha, aunque sea la misma...
  viajarScreen();
}


void viajar()
{
  int sentido = 1;
  int kmh_aux = 0;

  boolean salir = false;

  previa = 99;
  viajarScreen();

  //Enable timers...
  timer.enable(timerNafta);
  timer.enable(timerHora);
  timer.enable(timerOdo);
  timer.enable(timerMarcha);

  while( !salir)
  {
    timer.run();

    if( myTouch.dataAvailable())
    {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      if ( x > 100 && x < 205 && y > 40 && y < 100)
        debugScreen();
      else
        salir = true;
    }

    if ( ( millis() - antes ) >= 500 )
    {
      if ( ( millis() - aux ) > 1500 )
      {
        ultima = 0;
        KMH = 0;
      }
      else
      {
        /*
si ultima ----------> 0,376
         entonces 3600000----------> X metros
         */
        kmh_aux = 3600000 * 0.000376 / ultima ;
        if ( kmh_aux < 300 )
          KMH = ( kmh_aux + KMH ) / 2;

      }
      ////////////////////////////////////////////////////////////////
      ponerVelocidad(100,40,KMH);
      odometro(160,125);
      printMarcha();
      antes = millis();
    }
  }

  timer.disable(timerNafta);
  timer.disable(timerHora);
  timer.disable(timerOdo);
  timer.disable(timerMarcha);
  //Save values into file before leaving screen
  guardarOdometro();
  // myTouch.read();

}




void Arrancar()
{


}

void loginScreen()
{
  int x, y;


  myGLCD.clrScr();
  myGLCD.setFont(BigFont);
  // Draw the upper row of buttons
  for (x=0; x<5; x++)
  {

    rectangulo(10+(x*60), 10, 60+(x*60), 60);
    myGLCD.printNumI(x+1, 27+(x*60), 27);
  }
  // Draw the center row of buttons
  for (x=0; x<5; x++)
  {
    rectangulo(10+(x*60), 70, 60+(x*60), 120);
    if (x<4)
      myGLCD.printNumI(x+6, 27+(x*60), 87);
  }
  myGLCD.print("0", 267, 87);

  // Draw the lower row of buttons
  rectangulo(10, 130, 150, 180);
  myGLCD.print("Clear", 40, 147);

  rectangulo(160, 130, 300, 180);
  myGLCD.print("Enter", 190, 147);
}



void initLogin()
{
  loginScreen();

  while (repetir_login)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();

      if ((y>=10) && (y<=60)) // Upper row
      {
        if ((x>=10) && (x<=60)) // Button: 1
        {
          waitForIt(10, 10, 60, 60);
          updateStr('1');
        }
        if ((x>=70) && (x<=120)) // Button: 2
        {
          waitForIt(70, 10, 120, 60);
          updateStr('2');
        }
        if ((x>=130) && (x<=180)) // Button: 3
        {
          waitForIt(130, 10, 180, 60);
          updateStr('3');
        }
        if ((x>=190) && (x<=240)) // Button: 4
        {
          waitForIt(190, 10, 240, 60);
          updateStr('4');
        }
        if ((x>=250) && (x<=300)) // Button: 5
        {
          waitForIt(250, 10, 300, 60);
          updateStr('5');
        }
      }

      if ((y>=70) && (y<=120)) // Center row
      {
        if ((x>=10) && (x<=60)) // Button: 6
        {
          waitForIt(10, 70, 60, 120);
          updateStr('6');
        }
        if ((x>=70) && (x<=120)) // Button: 7
        {
          waitForIt(70, 70, 120, 120);
          updateStr('7');
        }
        if ((x>=130) && (x<=180)) // Button: 8
        {
          waitForIt(130, 70, 180, 120);
          updateStr('8');
        }
        if ((x>=190) && (x<=240)) // Button: 9
        {
          waitForIt(190, 70, 240, 120);
          updateStr('9');
        }
        if ((x>=250) && (x<=300)) // Button: 0
        {
          waitForIt(250, 70, 300, 120);
          updateStr('0');
        }
      }

      if ((y>=130) && (y<=180)) // Upper row
      {
        if ((x>=10) && (x<=150)) // Button: Clear
        {
          waitForIt(10, 130, 150, 180);
          stCurrent[0]='\0';
          stCurrentLen=0;
          myGLCD.setColor(0, 0, 0);
          myGLCD.fillRect(0, 224, 319, 239);
        }
        if ((x>=160) && (x<=300)) // Button: Enter
        {
          waitForIt(160, 130, 300, 180);
          if (stCurrentLen>0)
          {
            for (x=0; x<stCurrentLen+1; x++)
            {
              stLast[x]=stCurrent[x];
            }
            stCurrent[0]='\0';
            stCurrentLen=0;
            myGLCD.setColor(0, 0, 0);
            myGLCD.fillRect(0, 208, 319, 239);
            myGLCD.setColor(0, 255, 0);
            myGLCD.print(stLast, LEFT, 208);
            delay(100);

            if (stLast[0] == '1' && stLast[1] == '8' && stLast[2] == '0' && stLast[3] == '9' )
            {
              repetir_login = false;
              myGLCD.setColor(255, 0, 0);
              myGLCD.print("Correcto!", CENTER, 192);
              delay(800);
            }
            else
            {
              myGLCD.setColor(255, 0, 0);
              myGLCD.print("Codigo erroneo!", CENTER, 192);
              delay(500);
              myGLCD.print(" ", CENTER, 192);
              delay(500);
              myGLCD.print("Codigo erroneo!", CENTER, 192);
              delay(500);
              myGLCD.print(" ", CENTER, 192);
              myGLCD.setColor(0, 255, 0);
            }
          }
          else
          {
            myGLCD.setColor(255, 0, 0);
            myGLCD.print("Ingrese un codigo!", CENTER, 192);
            delay(500);
            myGLCD.print(" ", CENTER, 192);
            delay(500);
            myGLCD.print("Ingrese un codigo!", CENTER, 192);
            delay(500);
            myGLCD.print(" ", CENTER, 192);
            myGLCD.setColor(0, 255, 0);
          }
        }
      }
    }
  }
}

// Draw a red frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

void updateStr(int val)
{

  if (stCurrentLen<20)
  {
    stCurrent[stCurrentLen]=val;
    stCurrent[stCurrentLen+1]='\0';
    stCurrentLen++;
    myGLCD.setColor(0, 255, 0);
    myGLCD.setBackColor (0, 0, 0);
    myGLCD.print(stCurrent, LEFT, 224);
  }
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.print("MUY LARGO!!", CENTER, 192);
    delay(500);
    myGLCD.print(" ", CENTER, 192);
    delay(500);
    myGLCD.print("MUY LARGO!!", CENTER, 192);
    delay(500);
    myGLCD.print(" ", CENTER, 192);
    myGLCD.setColor(0, 255, 0);
  }
}




boolean encendido = false;

void girarBurro()
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.setFont(SmallFont);
  myGLCD.print("GIRA!", 233, 85);
  digitalWrite(PIN_BURRO,HIGH);
  waitForIt();
  digitalWrite(PIN_BURRO,LOW);
  myGLCD.setColor(200, 200, 210);
  myGLCD.print("GIRAR", 233, 85);

}

void arrancarScreen()
{
  myGLCD.clrScr();
  lineaCabecera();
  botonVolver();

  //Automatico
  rectangulo(10,70,155,170);
  myGLCD.setFont(BigFont);
  myGLCD.print("AUTO",50,110);

  //Manual
  rectangulo(165,70,310,170);
  myGLCD.print("MANUAL",190,110);
}


void autoScreen()
{
  myGLCD.clrScr();

  botonVolver();
  lineaCabecera();

  //EMPEZAR
  rectangulo(10,70,155,170);
  myGLCD.setFont(BigFont);
  myGLCD.print("EMPEZAR",27,110);

  //PARAR
  rectangulo(165,70,310,170);
  myGLCD.print("PARAR",195,110);


}

void sliderH(int x, int y, int valor)
{

  //Borrado de la zona...
  myGLCD.setColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.fillRect(x-10,y-10,x+SLIDER_LENGTH+10,y+10);


  //Linea
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.drawLine(x,y,x + SLIDER_LENGTH,y);
  myGLCD.drawLine(x,y,x + SLIDER_LENGTH + 1,y);

  for (int i=0; i <= 5; i++)
  {
    myGLCD.drawLine((x+(i*20)),y-2,(x+(i*20)),y+3);

  }

  //Valor esta entre 0 y 5
  myGLCD.fillCircle((x + (20 * valor)),y,SLIDER_BUTTON_H);
  myGLCD.setColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.drawCircle((x + (20 * valor)),y,4);


  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);

}

void manualScreen( boolean conta)
{
  myGLCD.clrScr();
  botonVolver();
  lineaCabecera();

  //Contacto
  rectangulo(10,40,103,140);
  myGLCD.setFont(BigFont);
  if (conta)
    myGLCD.print("ON ",35,80);
  else
    myGLCD.print("OFF",35,80);

  //CEBADOR
  rectangulo(113,40,203,140);
  myGLCD.print("CHOKE",119,75);
  myGLCD.print("OFF",130,95); //Variable?

  //calentar
  rectangulo(213,40,306,140);
  myGLCD.print("START",219,80);

  //Seccion Sliders
  myGLCD.setFont(SmallFont);

  //Aceleador
  sliderH(30,180,0); //Variable?
  myGLCD.print("Acelerador",30,150);

  sliderH(180,180,0); //Variable?
  myGLCD.print("Cebador",190,150);
}



boolean verSiParo()
{
  //Verifica si sigo ...
  if (myTouch.dataAvailable())
  {
    myTouch.read();
    x=myTouch.getX();
    y=myTouch.getY();
    if ( x > 165 && x < 310 && y > 70 && y < 170 )
    {//Dentro de "parar"?
      return false;
    }
  }
  return true;
}
void arranqueAutomatico()
{
  autoScreen();
  boolean salir = false;
  while(!salir)
  {
    timer.run();
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      if ( (x < 31 ) && ( y > 210 ) )
      {
        salir = true;
      }
      else if ( x > 10 && x < 155 && y > 70 && y < 170 )
      {
        // agregarMensaje("Secuencia de arranque");
        if (verSiParo())
        {
          myAcelerador.attach(PIN_SERVO); // attaches the servo on pin to the servo object
          myCebador.attach(PIN_CEBA); // attaches the servo on pin to the servo object
          myAcelerador.write(0); // desacelerado
          myCebador.write(0); // sin cebador
          delay(300);
          if (verSiParo())
          {
            // agregarMensaje("Cebando el motor...");
            myCebador.write(config.cebador);
            //agregarMensaje("Acelerando...");
            myAcelerador.write(config.acelerador); //Acelero
            delay(300); //espero a los servos
            if (verSiParo())
            {
              // agregarMensaje("Girando burro!");
              digitalWrite(PIN_BURRO,HIGH); //Doy contacto al burro
              ///////////////////////////////////////////////
              //              Esto va por codigo!!! config.burro?
              delay(config.burro); //Espero que arranque
              ///////////////////////////////////////////////
              if (verSiParo())
              {
                digitalWrite(PIN_BURRO,LOW); //Corto el burro
                // agregarMensaje("Burro detenido");
                delay(5000); //Espero 5 segundos
                if (verSiParo())
                {
                  // agregarMensaje("Quitando cebador");
                  myCebador.write(0); // y saco el cebador
                  // agregarMensaje("Liberando servo cebador");
                  myCebador.detach();
                }
              }
            }
          }
        }
      }
    }
  }



}

void arranqueManual()
{

  int valor_acelerador = 0;
  int aux_valor_acelerador = 0;
  int cebador_default = 3;
  int valor_cebador = 0;
  int aux_valor_cebador = 0;
  boolean salir = false;

  boolean cebador = false;

  manualScreen(contacto);

  digitalWrite(PIN_POWSV,HIGH); // Enciende la potencia de los servos
  delay(600);
  myAcelerador.attach(PIN_SERVO); // attaches the servo on pin to the servo object
  myCebador.attach(PIN_CEBA); // attaches the servo on pin to the servo object
  myAcelerador.write(0); // desacelerado
  myCebador.write(0 ); // sin cebador

  while( !salir)
  {
    timer.run();
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      if ( x > 0 && x < 31 && y > 210 && y < 239)
      {
        waitForIt();
        salir = true;
      }
      else if ( x > 113 && x < 203 && y > 40 && y < 140)
      {
        waitForIt();
        myGLCD.setFont(BigFont);
        if ( cebador )
        {
          myGLCD.print("OFF",130,95);
          // digitalWrite(PIN_CONTA,HIGH);
          myCebador.write(0);
          sliderH(180,180,0);
          valor_cebador = 0;
          aux_valor_cebador = 0;
        }
        else
        {
          waitForIt();
          myGLCD.print("ON ",130,95);
          myCebador.write(map(cebador_default,0,5,0,180));
          sliderH(180,180,cebador_default);
          valor_cebador = cebador_default;
          aux_valor_cebador = cebador_default;
        }
        cebador = !cebador;        
      }
      // rectangulo(213,40,306,140);
      else if ( x > 213 && x < 306 && y > 40 && y < 140)
      {
        myGLCD.setColor(255,0,0);
        myGLCD.fillRoundRect(213,40,306,140);
        //Hacer girar el burro
        digitalWrite(PIN_BURRO,HIGH);
        waitForIt();
        digitalWrite(PIN_BURRO,LOW);
        rectangulo(213,40,306,140);
        myGLCD.setFont(BigFont);
        myGLCD.print("START",219,80);
      }
      else if ( x > 10 && x < 103 && y > 40 && y < 140)
      {
        waitForIt();
        myGLCD.setFont(BigFont);
        if ( contacto )
        {
          Serial.println("Apago");
          myGLCD.print("OFF",35,80);
          digitalWrite(PIN_CONTA,HIGH);
        }
        else
        {
          Serial.println("prendo");
          myGLCD.print("ON ",35,80);
          digitalWrite(PIN_CONTA,LOW);
        }
        contacto = !contacto;        
      }
      else if (x > 25 && x < 135 && y > 170 && y < 190)
      { //Acelerador
        valor_acelerador = round( ( x - 30 ) / 20);
        if ( valor_acelerador != aux_valor_acelerador )
        {
          sliderH(30,180,valor_acelerador);
          aux_valor_acelerador = valor_acelerador;
          myAcelerador.write(map(valor_acelerador,0,5,0,180));
        }
      }
      else if (x > 175 && x < 285 && y > 170 && y < 190)
      { //Cebador
        valor_cebador = round( ( x - 180 ) / 20);
        if ( valor_cebador != aux_valor_cebador )
        {
          sliderH(180,180,valor_cebador);
          aux_valor_cebador = valor_cebador;
          if ( aux_valor_cebador >= 1)
          {
            myGLCD.setFont(BigFont);
            myGLCD.print("ON ",130,95);
            cebador = true;
            myCebador.write(map(valor_cebador,0,5,0,180));
          }
          else
          {
            myGLCD.setFont(BigFont);
            myGLCD.print("OFF",130,95);
            cebador = false;
            myCebador.write(0);
          }
        }
      }

    }
  }
  myAcelerador.write(0); // desacelerado
  myCebador.write(0 ); // sin cebador
  delay(300);
  digitalWrite(PIN_POWSV,LOW); // Apaga la potencia de los servos
  myAcelerador.detach();
  myCebador.detach();

}

void arrancarMoto()
{
  arrancarScreen();
  boolean salir = false;
  while( !salir)
  {
    timer.run();
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      if ( (x < 31 ) && ( y > 210 ) )
      {
        salir = true;
      }
      else if ( ( x > 10 ) && ( x < 155 ) && ( y > 70) && ( y < 170 ) )
      {
        //Automatico
        arranqueAutomatico();
        arrancarScreen();
      }
      else if ( ( x > 165 ) && ( x < 310) && ( y > 70) && ( y < 170 ) )
      {
        arranqueManual();
        arrancarScreen();
      }
    }
  }
  menuPrincipal();
}

void powerScreen()
{
  arrancarScreen();
  boolean salir = false;
  while( !salir)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      if ( (x < 70 ) && ( y > 160 ) )
      {
        salir = true;
      }
      if ( ( x > 70 ) && ( x < 190 ) && ( y > 30) && ( y < 120 ) )
      {
        //contact on / off
        if (contacto)
        {
          waitForIt();
          digitalWrite(PIN_CONTA,LOW);
          myGLCD.setFont(SmallFont);
          myGLCD.print("OFF", 105, 83);
          contacto = false;

        }
        else
        {
          waitForIt();
          digitalWrite(PIN_CONTA,HIGH);
          myGLCD.setFont(SmallFont);
          myGLCD.print("ON ", 105, 83);
          contacto = true;
        }
      }
      if ( ( x > 190 ) && ( x < 310 ) && ( y > 30) && ( y < 120 ) )
      {
        //encendido del burro, solo mientras este presionado
        if (!encendido)
        {
          agregarMensaje("Spinning engine...");
          girarBurro();
        }
      }
    }
  }
  menuPrincipal();
}



void rectangulo( int x1, int y1, int x2, int y2)
{
  myGLCD.setBackColor (COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.fillRoundRect (x1, y1, x2, y2);
  myGLCD.setColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.fillRoundRect (x1+LINE_THICK, y1+LINE_THICK, x2-LINE_THICK, y2-LINE_THICK);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);

}

void circulo ( int x , int y , int radio )
{
  myGLCD.setBackColor (COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.fillCircle(x,y,radio);
  myGLCD.setColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.fillCircle(x,y,radio - LINE_THICK);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
}

void lineaCabecera()
{
  //Linea cabecera
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.drawLine(1,30,319,30);
  myGLCD.drawLine(1,31,319,31);

  timer.enable(timerHora);

}

void botonGuardar()
{
  //Boton guardar
  rectangulo(289,210,319,239);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.setFont(BigFont);
  myGLCD.print("S",296,218);
}

void botonSiguiente()
{
  //Boton guardar
  rectangulo(289,210,319,239);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.setFont(BigFont);
  myGLCD.print("+",296,218);
}

void botonVolver()
{
  rectangulo(1,210,31,239);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.setFont(BigFont);
  myGLCD.print("B",9,218);
}
void messageBox()
{
  //Caja de mensajes
  rectangulo(160,145,319,239);

}

void sumador(int x, int y )
{

  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.fillRoundRect(x,y,x+20,y+20);
  myGLCD.setColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.drawLine(x+5,y+10,x+15,y+10);
  myGLCD.drawLine(x+10,y+5,x+10,y+15);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
}

void restador(int x, int y)
{
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.fillRoundRect(x,y,x+20,y+20);
  myGLCD.setColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.drawLine(x+5,y+10,x+15,y+10);
  // myGLCD.drawLine(x+10,y+5,x+10,y+15);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
}

void confArranqueScreen()
{
  myGLCD.clrScr();
  lineaCabecera();
  botonVolver();
  botonGuardar();
  int aux = 0;

  aux = map(config.burro,500,2500,0,5); 

  //Tiempo giro del burro...
  sliderH(30,90,aux);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(65, 140 ,255);
  myGLCD.print("Burro de arranque",30,60);

  aux = map(config.cebador,500,2500,0,5); 
  //Cebador
  sliderH(30,140,aux);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(65, 140 ,255);
  myGLCD.print("Cebador",30,110);

  aux = map(config.acelerador,70,120,0,5); 
  //Acelerador
  sliderH(180,90,aux);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(55, 140 ,255);
  myGLCD.print("Acelerador",180,60);

  aux = map(config.cuanto_acel,500,2500,0,5); 
  //Tiempo Acelerado..
  sliderH(180,140,aux);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(55, 140 ,255);
  myGLCD.print("Tiempo Acelerado",180,110);

  aux = map(config.cuanto_desacel,500,2500,0,5); 
  //Tiempo Desacelerado..
  sliderH(180,190,aux);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(55, 140 ,255);
  myGLCD.print("Tiempo Desacelerado",180,150);

}


void confNafta()
{

  boolean salir = false;
  boolean guardado = false;

  setupNaftaScreen();


  while( !salir)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      //Salir
      if ( (x < 31 ) && ( y > 210 ) )
      {
        if (!guardado)
        {
          rectangulo(30,100,310,200);
          myGLCD.setColor(255,0,0);
          myGLCD.setFont(BigFont);
          myGLCD.setBackColor(COLOR_BACK_R,COLOR_BACK_G,COLOR_BACK_B);
          myGLCD.print("NO SE GUARDARON",41,130);
          myGLCD.print("LOS CAMBIOS",51,150);
          delay(1000);
        }
        salir = true;
      }
      //Salir
      else if ( (x > 290 ) && ( y > 210 ) )
      {
        //Llamar rutina de guardado
        guardado = true;
      }
      else if ( (x < 200 && x > 100 ) && ( y > 45 && y < 75 ) )
      {
        baja_nafta += 100;
        myGLCD.printNumI(baja_nafta,112,95,4,'0');
      }
      else if ( (x < 200 && x > 100 ) && ( y > 135 && y < 175 ) )
      {
        baja_nafta -= 100;
        myGLCD.printNumI(baja_nafta,112,95,4,'0');
      }
    }

  }
}
void setupNaftaScreen()
{

  myGLCD.clrScr();
  lineaCabecera();
  botonVolver();
  botonGuardar();

  //Bajo nivel de nafta
  sumador(160,65);
  rectangulo(100,90,250,150);
  restador(160,155);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(55, 140 ,255);
  myGLCD.printNumI(baja_nafta,112,95,4,'0');

}
