#include <UTFT.h>
#include <UTouch.h>

#include <Servo.h> 
#include <UTFT_tinyFAT.h>
#include <tinyFAT.h>
#include <avr/pgmspace.h>
#include <Wire.h>
#include <Rtc_Pcf8563.h>

#define X_KMH 50
#define Y_KMH 36
#define X_RPM 0
#define Y_RPM 0

#define ARCHIVO "LOG.DAT"
#define ODOMETRO "KM_LOG.DAT"

//Pines de cambios....
#define PIN_0RA   8
#define PIN_1RA   9
#define PIN_2DA   10
#define PIN_3RA   11
#define PIN_4TA   12
#define PIN_5TA   13

//Pin del sensor de rpm (cooler)...
#define PIN_RPM   18  

//Pines del relay del burro de arranque y servo acelerador

#define PIN_SERVO 57  //Analog input pin 0  acelerador
#define PIN_CEBA  56  //Analog input pin 2  cebador 
#define PIN_BURRO 55  //Analog input pin 1  burro
#define PIN_CONTA 54  //Analog input pin 3  contacto


#define MAX_MSJ 17
#define MSJ_SHOW 5

// Declare which fonts we will be using
extern uint8_t SevenSegNumFont[];
extern uint8_t BigFont[];
extern uint8_t SmallFont[];

word res;

UTFTtf          myGLCD(ITDB32S,38,39,40,41);  
UTouch         myTouch(6,5,4,3,2);
Servo myAcelerador;  // Servo acelerador
Servo myCebador;  // Servo acelerador
//init the real time clock
Rtc_Pcf8563 rtc;


unsigned long ahora;
unsigned long antes;

int KMH = 0;
int RPM = 0;
int NAFTA = 100;
int TEMP = 888;

volatile float m_totales   = 0;

int acelerado    = 750;
int desacelerado = 1000;
int cuanto       = 80;

char textBuffer[81];

int x, y;

void printKmTotales()
{

  double km_aux = m_totales;  // 1000;
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0,0,0);
  myGLCD.setColor(0, 255, 0);     

  myGLCD.printNumF(km_aux, 2, 170, 110, '.', 8, '0');

}

void printMarcha()
{
  //Pines :
  // punto muerto: D8
  // primera:      D9
  // segunda:      D10
  // tercera:      D11
  // cuarta:       D12
  // quinta:       D13
  int numero;
  char marcha;
  for (int i = 8; i < 14 ; i++)
  {
    if ( digitalRead(i) > 0)
    {
      numero = i;
      break;
    }
  }
  if (numero == 0)
    marcha = '?';
  else
    marcha = numero - 8;
  //    itoa((numero - 8),marcha,10); //obtener nro de marcha
}


void printKmh( int velocidad )
{
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setBackColor(0,0,0);
  myGLCD.setColor(0, 255, 0);     

  myGLCD.printNumI(velocidad, X_KMH, Y_KMH, 3, '0');

}


// Generated from: barra verde.png
// Time generated: 06/02/2013 11:28:51 a.m.
// Dimensions    : 17x20 pixels
// Size          : 680 Bytes

prog_uint16_t barra_verde[0x78] PROGMEM ={
  0x1782, 0x1762, 0x1723, 0x16C3, 0x1664, 0x1625, 0x15A7, 0x0D48, 0x0CC9, 0x0C6A, 0x1781, 0x1762, 0x1723, 0x16C4, 0x1665, 0x1626,   // 0x0010 (16)
  0x15A7, 0x0D48, 0x0CC9, 0x0C6A, 0x1782, 0x1762, 0x1723, 0x16C3, 0x0E65, 0x1606, 0x1587, 0x0D28, 0x0CC9, 0x0C6A, 0x1782, 0x1762,   // 0x0020 (32)
  0x1723, 0x16C4, 0x1665, 0x0E06, 0x0D87, 0x0D28, 0x0CC9, 0x0C6A, 0x1782, 0x1762, 0x1703, 0x16C4, 0x1665, 0x0DE6, 0x0D87, 0x0D28,   // 0x0030 (48)
  0x0CA9, 0x0C4B, 0x1782, 0x1762, 0x1703, 0x16A4, 0x0E45, 0x0DE6, 0x0D87, 0x0D08, 0x0CAA, 0x0C4B, 0x1782, 0x1742, 0x0F03, 0x16A4,   // 0x0040 (64)
  0x0E45, 0x0DE6, 0x0D67, 0x0D09, 0x0CAA, 0x0C4B, 0x1782, 0x1742, 0x1703, 0x16A4, 0x1645, 0x0DE6, 0x1568, 0x0D09, 0x0CAA, 0x0C4B,   // 0x0050 (80)
  0x1782, 0x1742, 0x16E3, 0x16A4, 0x0E45, 0x0DC6, 0x0D68, 0x0D09, 0x0C8A, 0x0C2B, 0x1782, 0x1743, 0x16E3, 0x0EA4, 0x0E25, 0x0DC6,   // 0x0060 (96)
  0x0D68, 0x0CE9, 0x0C8A, 0x0C2B, 0x1762, 0x1743, 0x16E3, 0x0E84, 0x0E26, 0x0DC6, 0x0D48, 0x0CE9, 0x0C8A, 0x0C2B, 0x1782, 0x1723,   // 0x0070 (112)
};


prog_uint16_t barra_amarilla[0x78] PROGMEM ={
  0xEF82, 0xE762, 0xDF22, 0xD723, 0xCEE3, 0xBEA4, 0xB665, 0xAE45, 0x9E06, 0x8DC7, 0xEF82, 0xE762, 0xDF22, 0xD723, 0xCEE3, 0xBEA4,   // 0x0010 (16)
  0xB665, 0xAE45, 0x9E06, 0x8DC6, 0xEF82, 0xE762, 0xDF22, 0xD723, 0xCEE3, 0xBEA4, 0xB665, 0xAE45, 0x9E06, 0x8DC6, 0xEF82, 0xE762,   // 0x0020 (32)
  0xDF42, 0xD723, 0xCEE3, 0xBEA4, 0xB665, 0xAE65, 0x9E06, 0x8DC6, 0xEF82, 0xE762, 0xDF42, 0xD723, 0xCEE3, 0xBEA4, 0xB665, 0xAE45,   // 0x0030 (48)
  0x9E06, 0x8DC6, 0xEF82, 0xE762, 0xDF22, 0xD723, 0xCEE3, 0xBEA4, 0xB665, 0xAE45, 0x9E06, 0x8DC6, 0xEF82, 0xE762, 0xDF22, 0xD723,   // 0x0040 (64)
  0xCEE3, 0xBEA4, 0xB665, 0xAE45, 0x9E06, 0x8DC6, 0xEF82, 0xE762, 0xDF22, 0xD723, 0xCEE3, 0xBEA4, 0xB665, 0xAE45, 0x9E06, 0x8DC6,   // 0x0050 (80)
  0xEF82, 0xE762, 0xDF43, 0xD723, 0xCEE3, 0xBEA4, 0xB665, 0xAE45, 0x9E06, 0x8DC6, 0xEF82, 0xE762, 0xDF43, 0xD723, 0xCEE3, 0xBEA4,   // 0x0060 (96)
  0xB685, 0xAE65, 0x9E26, 0x8DC7, 0xEF82, 0xE762, 0xDF43, 0xD723, 0xCEE3, 0xBEA4, 0xB665, 0xAE45, 0x9E06, 0x8DC6, 0xEF82, 0xE762,   // 0x0070 (112)
};


prog_uint16_t barra_roja[0x78] PROGMEM ={
  0xF086, 0xE0A6, 0xD8C6, 0xC907, 0xB928, 0xA168, 0x91A9, 0x7A09, 0x6A2A, 0x526B, 0xF086, 0xE0A6, 0xD8C6, 0xC907, 0xB928, 0xA168,   // 0x0010 (16)
  0x91A9, 0x7A09, 0x6A2A, 0x526B, 0xF086, 0xE0A6, 0xD8C6, 0xC907, 0xB928, 0xA168, 0x91A9, 0x7A09, 0x6A2A, 0x5A8B, 0xF086, 0xE0A6,   // 0x0020 (32)
  0xD8C6, 0xC907, 0xB928, 0xA168, 0x91A9, 0x7A09, 0x6A4A, 0x526B, 0xF086, 0xE0A6, 0xD8C6, 0xC907, 0xB928, 0xA168, 0x91A9, 0x7A09,   // 0x0030 (48)
  0x6A2A, 0x526B, 0xF086, 0xE0A6, 0xD8C6, 0xC907, 0xB928, 0xA168, 0x91A9, 0x79E9, 0x6A2A, 0x526B, 0xF086, 0xE0A6, 0xD8C6, 0xC907,   // 0x0040 (64)
  0xB928, 0xA168, 0x91A9, 0x79E9, 0x6A4A, 0x526B, 0xF086, 0xE0A6, 0xD8C6, 0xC907, 0xB928, 0xA168, 0x91A9, 0x79E9, 0x6A2A, 0x526B,   // 0x0050 (80)
  0xF086, 0xE0A6, 0xD8C6, 0xC907, 0xB927, 0xA168, 0x91A9, 0x7A09, 0x6A4A, 0x526B, 0xF086, 0xE0A6, 0xD8C6, 0xC907, 0xB927, 0xA168,   // 0x0060 (96)
  0x91A9, 0x7A09, 0x6A2A, 0x526B, 0xF086, 0xE0A6, 0xD8C6, 0xC907, 0xB927, 0xA168, 0x91A9, 0x7A09, 0x6A4A, 0x526B, 0xF086, 0xE0A6,   // 0x0070 (112)
};

prog_uint16_t barra_negra[0x78] PROGMEM ={
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0010 (16)
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0020 (32)
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0030 (48)
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0040 (64)
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0050 (80)
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0060 (96)
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0070 (112)
};

int debugstep = 0;

void printRpm( int rpeme, int sentido)
{  
  int x1 = 1;
  int y1 = 25;
  int x2 = 40;
  int y2 ;
  /*
  myGLCD.drawBitmap(40,  100, 10, 12, barra_verde,40,5,6);
   myGLCD.drawBitmap(80,  100, 10, 12, barra_amarilla,80,5,6);
   myGLCD.drawBitmap(120, 100, 10, 12, barra_roja,120,5,6);
   
   barras = 10x12
   */

  myGLCD.setColor(255,0,0);
  if (rpeme == 10)
  {
    myGLCD.drawBitmap(68,  177, 10, 12, barra_roja,90,5,6);
  }

  if (rpeme == 9)
  {
    myGLCD.drawBitmap(57,  177, 10, 12, barra_roja,85,5,6);
    if (sentido < 0){  
      myGLCD.drawBitmap(70,  177, 10, 12, barra_negra,90,5,6);
    }
  }
  if (rpeme == 8)
  {
    myGLCD.drawBitmap(46,  178, 10, 12, barra_amarilla,80,5,6);
    if (sentido < 0){  
      myGLCD.drawBitmap(57,  177, 10, 12, barra_negra,85,5,6);
    }
  }    
  if (rpeme == 7)
  {
    myGLCD.drawBitmap(35,  180, 10, 12, barra_amarilla,70,5,6);
    if (sentido < 0){  
      myGLCD.drawBitmap(46,  178, 10, 12, barra_negra,80,5,6);
    }
  }    
  if (rpeme == 6)
  {
    myGLCD.drawBitmap(24,  184, 10, 12, barra_amarilla,62,5,6);
    if (sentido < 0){  
      myGLCD.drawBitmap(35,  180, 10, 12, barra_negra,70,5,6);
    }
  }    
  if (rpeme == 5)
  {
    myGLCD.drawBitmap(15,  190, 10, 12, barra_verde,43,5,6);
    if (sentido < 0){  
      myGLCD.drawBitmap(24,  184, 10, 12, barra_negra,62,5,6);
    }
  }    
  if (rpeme == 4)
  {
    myGLCD.drawBitmap(9, 197, 10, 12, barra_verde,25,5,6);
    if (sentido < 0){  
      myGLCD.drawBitmap(15,  190, 10, 12, barra_negra,43,5,6);
    }
  }    
  if (rpeme == 3)
  {
    myGLCD.drawBitmap(5,  208, 10, 12, barra_verde,15,5,6);
    if (sentido < 0){  
      myGLCD.drawBitmap(9, 197, 10, 12, barra_negra,25,5,6);
    }
  }    
  if (rpeme == 2)
  {
    myGLCD.drawBitmap(2,  218, 10, 12, barra_verde,10,5,6);
    if (sentido < 0){  
      myGLCD.drawBitmap(5,  208, 10, 12, barra_negra,15,5,6);
    }
  }    
  if (rpeme == 1)
  {
    myGLCD.drawBitmap(1, 228, 10, 12, barra_verde);    
    if (sentido < 0){  
      myGLCD.drawBitmap(2,  218, 10, 12, barra_negra,10,5,6);
    }
  }    
}

int linea_mensaje = 50;

#define y_linea_mensaje = 150;

String t_mensajes[] = { 
  "","","","","",""}; // 6?
int idx_mensajes = -1;




void agregarMensaje( String m )
{
  String t_aux[30];
  String l_aux;
  int idx = 0;
  int longitud = m.length() / MAX_MSJ;
  if ( m.length() % MAX_MSJ  > 0)
    longitud++;
  //split message if too large...
  for ( int p = 1; p <= longitud ; p++)
  {
    l_aux = m.substring(idx, (idx + MAX_MSJ));
    idx = idx + MAX_MSJ;    
    t_mensajes[++idx_mensajes] =  l_aux;
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
}


#define y_msj 140
#define x_msj 150


void printMessages()
{
  // Print messages table each line on it's own inside(maybe) the info-box
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255,255,255);
  myGLCD.setBackColor(0,0,0);
  for ( int i = 0; i<= MSJ_SHOW; i++) //Print up to MSJ_SHOW messages
  {
    myGLCD.print(t_mensajes[i],x_msj,(y_msj + (i * 13))); //Advance each Y coordinate by the amount of font height (8?)
  }
}

#define x_hora  195
#define y_hora  45

void imprimirHora()
{
  char *valores;
  String hora;
  String minuto;

  myGLCD.setFont(BigFont);
  myGLCD.setColor(255,255,255);
  myGLCD.setBackColor(0,0,0);
  valores = rtc.formatTime();
  
  hora   = strtok( valores, ":");
  minuto = strtok( NULL, ":");  
  hora = hora + ":" + minuto;

  myGLCD.print(hora,x_hora,y_hora); 
  
}


void imprimirNafta()
{
  myGLCD.setColor(255,255,255);
  myGLCD.setFont(SmallFont);
  myGLCD.print("1023",280,90);
}

void menuPrincipal()
{
  myGLCD.clrScr();
  Serial3.println(debugstep++);   
  res =  myGLCD.loadBitmap(0,0,320,240,"principa.raw");
  if (res != 0)
  {
    myGLCD.print("error principa.raw",0,0); 
    myGLCD.printNumI(res,0,80);
    agregarMensaje("Error loading principa.raw");   
    delay(2000);
  }

  imprimirHora();
  imprimirNafta();
}


void setup()
{
  Serial3.begin(9600);
  Serial3.println("Inicializacion");  
  agregarMensaje("Inicializacion");  
  // Initial setup
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();

  myTouch.InitTouch(LANDSCAPE);
  myTouch.setPrecision(PREC_HI);  

  pinMode(PIN_RPM, INPUT);  //Velocimetro  (PIN 18 para el interrupt)
  digitalWrite(PIN_RPM,HIGH);// Internal pull up resistor(?)
  Serial3.println(debugstep++);  
  myGLCD.setFont(BigFont);

  file.initFAT();
  agregarMensaje("Cargando preferencias...");  
  cargar();

  agregarMensaje("Pantalla de inicio");  

  //Inicializacion Servo  
  pinMode(PIN_SERVO, OUTPUT);
  pinMode(PIN_CEBA,  OUTPUT);  
  pinMode(PIN_BURRO, OUTPUT);
  pinMode(PIN_CONTA, OUTPUT);
  agregarMensaje("Setup de pinouts");  

  pinMode(8,  INPUT);    //Deteccion de marcha   0
  digitalWrite(8, HIGH);  //Pull up resistor
  pinMode(9,  INPUT);    //Deteccion de marcha   1
  digitalWrite(9, HIGH); //Pull up resistor  
  pinMode(10, INPUT);    //Deteccion de marcha   2
  digitalWrite(10, HIGH);//Pull up resistor  
  pinMode(11, INPUT);    //Deteccion de marcha   3
  digitalWrite(11, HIGH);//Pull up resistor  
  pinMode(12, INPUT);    //Deteccion de marcha   4
  digitalWrite(12, HIGH);//Pull up resistor  
  pinMode(13, INPUT);    //Deteccion de marcha   5
  digitalWrite(13, HIGH);//Pull up resistor  

  attachInterrupt(5, contar, RISING);  
  agregarMensaje("Solicitando password");  

  //  initLogin();
  menuPrincipal();
 
  Serial3.println("Fin Inicializacion");  
}


void cargar()
{
  /*
Estructura del archivo ARCHIVO (max 80):
   largo ---- tipo  ----------   significado 
   9999       int                tiempo acelerado    (calentar)
   9999       int                tiempo desacelerado (calentar)
   999        int                angulo aceleracion  (calentar)
   */

  word result = 0;
  if (file.exists(ARCHIVO))
  {

    // Carga  los valores desde el archivo
    res=file.openFile(ARCHIVO, FILEMODE_TEXT_READ);
    //    Serial3.println("abrir archivo:");
    //    Serial3.print(res);
    if (res==NO_ERROR)
    {

      result=file.readLn(textBuffer, 80);
      if ((result!=EOF) and (result!=FILE_IS_EMPTY))
      {
        acelerado = atoi(textBuffer);           
        //        Serial3.print("Acelerado");
        //        Serial3.println(acelerado);
      }              
      result=file.readLn(textBuffer, 80);              
      if ((result!=EOF) and (result!=FILE_IS_EMPTY))
      {
        desacelerado = atoi(textBuffer);                
        //        Serial3.print("Descelerado");
        //        Serial3.println(desacelerado);        
      }    
      result=file.readLn(textBuffer, 80);              
      if ((result!=EOF) and (result!=FILE_IS_EMPTY))
      {
        cuanto = atoi(textBuffer);                
        //        Serial3.print("Cuanto");
        //       Serial3.println(cuanto);        
      }                            
      file.closeFile();
    }
  }  
  else
  {
    //    Serial3.print("No existe el archivo: ");
    //  Serial3.println(ARCHIVO);
  }
  if ( file.exists(ODOMETRO) )
  {
    res = file.openFile(ODOMETRO, FILEMODE_TEXT_READ);
    //    Serial3.println("Abriendo archivo odometro");
    //    Serial3.print(res);
    if (res == NO_ERROR)
    {
      result = file.readLn(textBuffer, 80);
      /*      Serial3.print("result: ");
       Serial3.println(result);
       Serial3.print("EMPTY: ");
       Serial3.println(FILE_IS_EMPTY);*/
      if ((result!=EOF) and (result!=FILE_IS_EMPTY))
      {
        //        Serial3.print("Se leyo del odometro:");
        //        Serial3.println(textBuffer);
        m_totales = atof(textBuffer);                
        //        Serial3.print("m_totales:");
        //        Serial3.println(m_totales);       
      }
      file.closeFile();
    }

  }
}


void waitForIt()
{
  while (myTouch.dataAvailable())
    myTouch.read();
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
#define LC_CALENTAR  1
#define LC_VIAJAR    2

  int incomingByte = 0;
  char letra;
  String palabra = "";
  if ( Serial3.available() >= 5)  //<--- fijoo!
  {
    for(int i = 0; i < 5; i++)
    {
      letra = Serial3.read();
      palabra.concat(letra);
    }
    Serial3.print("Palabra tiene: ");
    Serial3.println(palabra);
    if ( palabra == "HELLO")
    {
      conectado = 1;
      Serial3.println("HOLA");
    }
    if (conectado != 0 )
    {
      if ( palabra == "KMTOT")
      {
        Serial3.println(m_totales);        
      }
      else if ( palabra == "START" ) 
      {
        if ( donde != LC_PRINCIPAL )
          Serial3.println("No se permite arrancar desde un submenu");
        else
          Arrancar();
      }
      else if ( palabra == "CONTA")
            {
                digitalWrite(PIN_CONTA,LOW);
                Serial3.println("En contacto");
            }
      else if ( palabra == "PANON" )
      {
        myGLCD.lcdOn();
        Serial3.println("Pantalla encendida");
      }
      else if ( palabra == "PANOF" )
      {
        myGLCD.lcdOff();
        Serial3.println("Pantalla apagada");        
      }
      else if ( palabra == "LOCKS" )
      {
        Serial3.println("Bloqueando pantalla");        
        initLogin();
        Serial3.println("Desbloqueado");        
      }            
    }
    else
      Serial3.println("No se ha iniciado la sesion");   
    if ( palabra == "GOODB")
    {
      conectado = 0;
      Serial3.println("CHAU");     
    }
  }
}

long hora_refresh = 0;
void loop()
{

  if (millis() - hora_refresh > 500)
  {
    imprimirHora();
    hora_refresh = millis();
    printMessages();
  }
  /* Rutina de escucha del Serial3*/
  escuchar( 0 ); //0 = llamado desde el lo op principal

  if (myTouch.dataAvailable())
  {
    myTouch.read();
    x=myTouch.getX();
    y=myTouch.getY();

    if ( (x < 120) && ( x > 0) )
    {
      if ( ( y < 50 ) && ( y > 0 )) //opcion Arrancar
      {
        waitForIt();
        powerScreen();
      }
      else if ( ( y > 90 ) && ( y < 150 ) ) //opcion viajar
      {
        waitForIt();
        agregarMensaje("Ingreso en viajar");
        viajar();
      }
      else if ( ( y > 190 ) ) //opcion Calentar
      {
        waitForIt();        
        Calentar();  
      }
    }
    else if( x > 190 && y < 95  )
    {
      waitForIt();              
      ponerEnHora();
      menuPrincipal();
    }
  }
}

volatile long ultima;
volatile long ultima_aux;
volatile long aux;
/*
     
 999  <----> 3600000
 0.000376  <----->  x 
 
 */
void contar()
{                       //0,000376
  m_totales = m_totales + 0.000376;   //Total traveled m
  ultima_aux = millis() - aux;           //Save delta of time since last pulse...
  if ( ultima_aux > 5 )
  {
    ultima = ultima_aux;
    aux = millis();                    //Remember when this pulse arrived...
  }
}

void ponerEnHora()
{

  //  rtc.setTime(18, 02, 00);
  int hora = 0;
  int minuto = 0;
  char *valores;

  boolean  salir = false;
  int x1 = 0;
  int y1 = 0;
  myGLCD.clrScr(); 
  res =  myGLCD.loadBitmap(0,0,320,240,"hora.raw");

  if (res != 0)
  {
    myGLCD.print("error loading hora.raw",0,0); 
    delay(2000);
    return;
  }

  //  int sensors[] = int(split(myString, ','));

  //  valores = int( split(rtc.formatTime(), ':'));

  valores = rtc.formatTime();
  hora   = atoi ( strtok( valores, ":") );
  //  Serial3.print("Hora");
  //  Serial3.println(hora);
  minuto = atoi ( strtok( NULL, ":") );
  //  Serial3.print("Minuto");
  //  Serial3.println(minuto);

  while(!salir)
  {
    myGLCD.printNumI(hora,90,110,2,'0'); 
    myGLCD.printNumI(minuto,203,110,2,'0');     
    if (myTouch.dataAvailable())
    {
      myTouch.read(); 
      x1=myTouch.getX();
      y1=myTouch.getY();    
      waitForIt(); 
      if (x1 > 85 && x1 < 150)      
      {
        if ( y1 > 35  && y1 < 90 )
        {
          // hora arriba
          hora ++;
          if (hora > 23) hora = 0;
        }  
        else if ( y1 > 143 && y1 < 198 ) 
        {
          // hora abajo
          hora --;   
          if (hora < 0) hora = 23;       
        }
      }
      else 
        if ( x1 > 199 && x1 < 263 )
      {

        if ( y1 > 35  && y1 < 90 )
        {
          // minuto arriba
          minuto++;
          if ( minuto > 59 ) 
          {
            minuto = 0;
            hora++;
          }
        }  
        else if ( y1 > 143 && y1 < 198 ) 
        {
          // minuto abajo
          minuto--;
          if ( minuto < 0 ) 
          {
            minuto = 59;
            hora--;
          }          
        }          
      }
      else
        if ( x1 < 60 )
        {
          if ( y1 < 50  )
          {
            //Boton salir
            return;
          }
          else if ( y1 > 190 )
          {
            //Boton guardar
            rtc.setTime(hora, minuto, 00);
            return;
          }                
        }
    }
  }
}


boolean guardarOdometro()
{
  if ( file.exists(ODOMETRO)   )
  {
    Serial3.println("Existe archivo odometro");
    res = file.delFile(ODOMETRO);

    if (res)
    {
      Serial3.println("Se borra el archivo odometro");
      agregarMensaje("Borrado archivo ODOMETRO");
    } 
    else
    {
      Serial3.println("NO se borra el archivo odometro");
      agregarMensaje("No se borra archivo ODOMETRO");
      return false;
    }
  }    

  if (file.create(ODOMETRO))
  {
    Serial3.println("se crea archivo odometro");
    res=file.openFile(ODOMETRO, FILEMODE_TEXT_WRITE);
  }
  else
  {
    Serial3.print("Error al crear el archivo: ");
    Serial3.println(res);
    agregarMensaje("Error creando ODOMETRO");
    return false;
  }

  if (res == NO_ERROR)
  {
    word result=0;
    ftoa(textBuffer,m_totales,3);
    Serial3.print("Intento guardar:");
    Serial3.println(textBuffer);
    result=file.writeLn(textBuffer);

    if (result == NO_ERROR)
    {
      Serial3.println("Se guardo el valor del odometro");
      agregarMensaje("Odometro guardado.");
    }
    else
    {
      Serial3.println("NO se guardo el valor del odometro");
      agregarMensaje("Error al guardar odometro");
      return false;
    }

    file.closeFile();
    delay(200);
    return true;

  }
  else
  {
    Serial3.println("No se crea el archivo, sale");
    return false;

  }
}


void viajar()
{

  int rp = 1;
  int sentido = 1;

  int i = 0;
  int kmh_aux = 0;
  long contador = 0;
  long promedio = 0;  
  boolean igual;
  float m_totales_aux = 0;  
  unsigned long ultimo_cont = 0;

  myGLCD.clrScr(); 

  res =  myGLCD.loadBitmap(0,0,320,240,"viajar2.raw");

  if (res != 0)
  {
    myGLCD.print("Error loading viajar2.raw",0,0); 
    agregarMensaje("Error loading viajar2.raw");   
    delay(2000);
  }





  printRpm(1,1);
  printRpm(2,1);
  printRpm(3,1);
  printRpm(4,1);
  printRpm(5,1);
  printRpm(6,1);  
  printRpm(7,1);
  printRpm(8,1);
  printRpm(9,1);  
  printRpm(10,1); 


  imprimirNafta();


  while( !myTouch.dataAvailable())
  {


    escuchar(2);
    if ( ( millis() - antes ) >= 500 )
    {      

      imprimirHora();
      printMessages();
      if ( ( millis() - aux )  > 1500 )
      {
        ultima    = 0;
        KMH = 0;     
      }
      else
      {
        /*  si    ultima ----------> 0,376
         entonces  3600000----------> X metros
         
         */
        kmh_aux = 3600000 * 0.376 /  ultima / 1000 ;      
        if ( kmh_aux < 300 )
          KMH = kmh_aux;
        ultimo_cont = millis() - antes;
      }
      ////////////////////////////////////////////////////////////////
      printKmh(KMH);         
      printKmTotales();
      //      printMarcha();
      antes = millis();
    }
  }

  //Save values into file before leaving screen
  Serial3.println("intento de guardar datos...");
  guardarOdometro();
  myTouch.read();  
  menuPrincipal();
}

void Arrancar()
{

  Serial3.println("Comienza el arranque del motor...");
  agregarMensaje("Secuencia de arranque");   
  myAcelerador.attach(PIN_SERVO); // attaches the servo on pin to the servo object 
  myCebador.attach(PIN_CEBA);     // attaches the servo on pin to the servo object   
  myAcelerador.write(0);          // desacelerado  
  myCebador.write(0);             // sin cebador    
  delay(300);
  Serial3.println("Aplicando cebador...");
  agregarMensaje("Cebando el motor...");   
  myCebador.write(300);
  Serial3.println("Acelerando el motor...");
  agregarMensaje("Acelerando...");   
  myAcelerador.write(90);                         //Acelero
  delay(300);                                //espero al servo
  Serial3.println("Girando burro!");
  agregarMensaje("Girando burro!");   
  digitalWrite(PIN_BURRO,HIGH);              //Doy contacto al burro
  delay(1000);                               //Espero 1 segundo que arranque
  digitalWrite(PIN_BURRO,LOW);               //Corto el burro
  Serial3.println("Burro frenado");
  agregarMensaje("Burro detenido");   
  Serial3.println("Soltando cebador");
  delay(1000);                               //Espero 1 segundo
  agregarMensaje("Quitando cebador");     
  myCebador.write(0);                        // y saco el cebador
  agregarMensaje("Liberando servo cebador");     
  myCebador.detach();
  Serial3.println("Secuencia de arranque finalizada, pasando a calentar...");
  agregarMensaje("Pasando a calentar");     
  Calentar();
}


boolean guardarCalentar()
{
  agregarMensaje("Saving presets...");     
  if ( file.exists(ARCHIVO)   )
  {
    noInterrupts();
    res = file.delFile(ARCHIVO);
    interrupts();
    if (res)
    {
      Serial3.println("Se borra el archivo odometro");
      agregarMensaje("File erased");     
    }
    else
    {
      Serial3.println("NO se borra el archivo odometro");
      agregarMensaje("Error deleting ARCHIVO");     
      return false;
    }
  }    
  noInterrupts();
  if (file.create(ARCHIVO))
  {
    res=file.openFile(ARCHIVO, FILEMODE_TEXT_WRITE);
  }
  else
  {
    Serial3.print("Error al crear el archivo: ");
    agregarMensaje("Error creating ARCHIVO");     
    Serial3.println(res);
    return false;
  }
  interrupts();
  if (res == NO_ERROR)
  {

    noInterrupts();       
    word result=0;
    itoa(acelerado,textBuffer,10);
    result=file.writeLn(textBuffer);
    if (result == NO_ERROR)
    {      
      itoa(desacelerado,textBuffer,10);
      result=file.writeLn(textBuffer);
      if (result == NO_ERROR)
      {
        itoa(cuanto,textBuffer,10);
        result=file.writeLn(textBuffer);
      }
    }    

    interrupts();
    if (result == NO_ERROR)
    {
      Serial3.println("Se guardo el valor del odometro");
      agregarMensaje("Presets saved");
      return true;
    }
    else
    {
      Serial3.println("NO se guardo el valor del odometro");
      agregarMensaje("Error saving presets");
      return false;
    }
    noInterrupts();
    file.closeFile();
    interrupts();
  }
  else
    return false;
}


void Calentar()
{
  int pos = 180;
  int x1 = 10;
  int x2 = 90;
  int x3 = 166;
  int x4 = 242;

  int y1 = 36;
  int y2 = 138;

  int flecha_alto = 66;
  int flecha_ancho = 66;

  int parar_alto = 66;
  int parar_ancho = 66;

  long antes = 0;
  long ahora = 0;  

  if (!myAcelerador.attached())
  {
    agregarMensaje("Tomando control del acelerador");     
    myAcelerador.attach(PIN_SERVO);

  }

  myGLCD.clrScr();  
  res = myGLCD.loadBitmap(0,0,320,240,"calentar.raw");
  if (res != 0)
  {
    myGLCD.print("error loading calentar.raw",0,0); 
    agregarMensaje("Error loading calentar.raw");     
    delay(2000);
  }
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);  
  myGLCD.setBackColor(0, 0, 0);  
  myGLCD.setColor(255, 255, 255);
  myGLCD.printNumI(acelerado,x2,110,4,'0');
  myGLCD.printNumI(cuanto,x3,110,3,'0'); 
  myGLCD.printNumI(desacelerado,x4,110,4,'0');
  while(1)
  {
    escuchar( 1 ); // Llamado desde Calentar
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();   
      if ( ( x >= x2) && (x <= (x2+flecha_ancho) ) )
      {
        waitForIt();     
        if ( ( y >= y1) && (y <= (y1+flecha_alto) ) )
        {
          if ( acelerado <= 6000 ) 
            acelerado = acelerado + 250;
        }
        else if ( ( y >= y2 ) && (y <= (y2+flecha_alto) ) )
        {
          if ( acelerado >= 500 ) 
            acelerado = acelerado - 250;        
        }     
        myGLCD.setBackColor(0, 0, 0);  
        myGLCD.setColor(255, 255, 255);
        myGLCD.printNumI(acelerado,x2,110,4,'0');         
      }       

      if ( ( x >= x3) && (x <= (x3+flecha_ancho) ) )
      {
        waitForIt();     
        if ( ( y >= y1) && (y <= (y1+flecha_alto) ) )
        {
          if ( cuanto <= 170 ) 
            cuanto = cuanto + 10;
        }
        else if ( ( y >= y2 ) && (y <= (y2+flecha_alto) ) )
        {
          if ( cuanto >= 20 ) 
            cuanto = cuanto - 10;        
        }     
        myGLCD.setBackColor(0, 0, 0);  
        myGLCD.setColor(255, 255, 255);
        myGLCD.printNumI(cuanto,x3,110,3,'0');         
      }            
      if ( ( x >= x4) && (x <= (x4+flecha_ancho) ) )
      {
        waitForIt();     
        if ( ( y >= y1) && (y <= (y1+flecha_alto) ) )
        {
          if ( desacelerado < 6000 ) 
            desacelerado = desacelerado + 250;
        }
        else if ( ( y >= y2 ) && (y <= (y2+flecha_alto) ) )
        {
          if ( desacelerado >= 750 ) 
            desacelerado = desacelerado - 250;        
        }     
        myGLCD.setBackColor(0, 0, 0);  
        myGLCD.setColor(255, 255, 255);
        myGLCD.printNumI(desacelerado,x4,110,4,'0');         
      }     

      if ( ( x >= x1) && (x <= (x1+parar_ancho) ) )
      {
        waitForIt();     
        if ( ( y >= y2) && (y <= (y2+parar_alto) ) )
        {
          agregarMensaje("Fin calentamiento...");     
          myAcelerador.write(0);    // desacelerado por las dudas
          myAcelerador.detach();    // Soltar acelerador
          break;               
        }
      }  
    }  

    //Movimiento del acelerador
    ahora = millis();   
    if ( pos == 0)
    {                    
      if ( (ahora - antes) >= desacelerado )
      {
        pos = cuanto;
        res =  myGLCD.loadBitmap(1,1,60,55,"exclama.raw");
        if (res != 0)
        {
          myGLCD.print("error exclama.raw",0,0); 
          agregarMensaje("Error loading exclama.raw");     
        }
        antes = ahora;        
      }
    }
    else 
    {          
      if ( (ahora - antes) >= acelerado )
      {
        pos = 0;
        myGLCD.setColor(0,0,0);        
        myGLCD.fillRect(1,1,61,56);
        antes = ahora;        
      }     
    } 
    myAcelerador.write(pos);    //         
  }

  //Save values into file before leaving screen
  guardarCalentar();
  menuPrincipal();
}

char *ftoa(char *a, double f, int precision)
{
  long p[] = { 
    0,10,100,1000,10000,100000,1000000,10000000,100000000                                                                                                                                           };

  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}

void strcat(char* original, char appended)
{
  while (*original++)
    ;
  *original++ = appended;
  if (appended)
    *original = '\0';
}

char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";

void loginScreen()
{
  int x, y;  


  myGLCD.clrScr();
  myGLCD.setBackColor (0, 0, 255);
  // Draw the upper row of buttons
  for (x=0; x<5; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*60), 10, 60+(x*60), 60);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*60), 10, 60+(x*60), 60);
    myGLCD.printNumI(x+1, 27+(x*60), 27);
  }
  // Draw the center row of buttons
  for (x=0; x<5; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*60), 70, 60+(x*60), 120);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*60), 70, 60+(x*60), 120);
    if (x<4)
      myGLCD.printNumI(x+6, 27+(x*60), 87);
  }
  myGLCD.print("0", 267, 87);
  // Draw the lower row of buttons
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (10, 130, 150, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 130, 150, 180);
  myGLCD.print("Clear", 40, 147);
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (160, 130, 300, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (160, 130, 300, 180);
  myGLCD.print("Enter", 190, 147);
  myGLCD.setBackColor (0, 0, 0);
}



void initLogin()
{

  boolean repetir_login = true;

  loginScreen();

  while (repetir_login)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();

      if ((y>=10) && (y<=60))  // Upper row
      {
        if ((x>=10) && (x<=60))  // Button: 1
        {
          waitForIt(10, 10, 60, 60);
          updateStr('1');
        }
        if ((x>=70) && (x<=120))  // Button: 2
        {
          waitForIt(70, 10, 120, 60);
          updateStr('2');
        }
        if ((x>=130) && (x<=180))  // Button: 3
        {
          waitForIt(130, 10, 180, 60);
          updateStr('3');
        }
        if ((x>=190) && (x<=240))  // Button: 4
        {
          waitForIt(190, 10, 240, 60);
          updateStr('4');
        }
        if ((x>=250) && (x<=300))  // Button: 5
        {
          waitForIt(250, 10, 300, 60);
          updateStr('5');
        }
      }

      if ((y>=70) && (y<=120))  // Center row
      {
        if ((x>=10) && (x<=60))  // Button: 6
        {
          waitForIt(10, 70, 60, 120);
          updateStr('6');
        }
        if ((x>=70) && (x<=120))  // Button: 7
        {
          waitForIt(70, 70, 120, 120);
          updateStr('7');
        }
        if ((x>=130) && (x<=180))  // Button: 8
        {
          waitForIt(130, 70, 180, 120);
          updateStr('8');
        }
        if ((x>=190) && (x<=240))  // Button: 9
        {
          waitForIt(190, 70, 240, 120);
          updateStr('9');
        }
        if ((x>=250) && (x<=300))  // Button: 0
        {
          waitForIt(250, 70, 300, 120);
          updateStr('0');
        }
      }

      if ((y>=130) && (y<=180))  // Upper row
      {
        if ((x>=10) && (x<=150))  // Button: Clear
        {
          waitForIt(10, 130, 150, 180);
          stCurrent[0]='\0';
          stCurrentLen=0;
          myGLCD.setColor(0, 0, 0);
          myGLCD.fillRect(0, 224, 319, 239);
        }
        if ((x>=160) && (x<=300))  // Button: Enter
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
            Serial3.print("El codigo ingresado es:");
            Serial3.println(stLast);

            if (stLast[0] == '1'  &&  stLast[1] == '8'  &&  stLast[2] == '0'  &&   stLast[3] == '9' )
            {
              repetir_login = false;                 
              myGLCD.setColor(255, 0, 0);
              myGLCD.print("Correcto!", CENTER, 192);   
              agregarMensaje("Succesful login!");              
            }
            else
            {
              myGLCD.setColor(255, 0, 0);
              myGLCD.print("Codigo erroneo!", CENTER, 192);
              delay(500);
              myGLCD.print("                  ", CENTER, 192);
              delay(500);
              myGLCD.print("Codigo erroneo!", CENTER, 192);
              delay(500);
              myGLCD.print("                  ", CENTER, 192);
              myGLCD.setColor(0, 255, 0);           
              agregarMensaje("Wrong code entered");   
            }
          }
          else
          {
            myGLCD.setColor(255, 0, 0);
            myGLCD.print("Ingrese un codigo!", CENTER, 192);
            delay(500);
            myGLCD.print("                     ", CENTER, 192);
            delay(500);
            myGLCD.print("Ingrese un codigo!", CENTER, 192);
            delay(500);
            myGLCD.print("                     ", CENTER, 192);
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
    myGLCD.print("           ", CENTER, 192);
    delay(500);
    myGLCD.print("MUY LARGO!!", CENTER, 192);
    delay(500);
    myGLCD.print("           ", CENTER, 192);
    myGLCD.setColor(0, 255, 0);
  }
}




boolean encendido = false;
boolean contacto  = false;

void girarBurro()
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.setFont(SmallFont);  
  myGLCD.print("GIRA!", 233, 85);
  digitalWrite(PIN_BURRO,HIGH);
  contacto = true;
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(200, 200, 210);
  myGLCD.print("GIRAR", 233, 85);    
  agregarMensaje("Spining engine...");   
}

void powerScreen()
{
  myGLCD.setBackColor(0,0,0);
  myGLCD.clrScr(); 

  res =  myGLCD.loadBitmap(60,30,108,114,"amarillo.raw");
  if (res != 0)
  {
    myGLCD.print("error loading amarillo.raw",0,0); 
    delay(2000);
  }

  myGLCD.setFont(SmallFont);  
  myGLCD.setColor(210, 200, 200);
  if (contacto)
    myGLCD.print("ON ", 105, 83);  
  else
    myGLCD.print("OFF", 105, 83);  


  res =  myGLCD.loadBitmap(198,30,109,119,"blanco.raw");
  if (res != 0)
  {
    myGLCD.print("error loading blanco.raw",0,0); 
    delay(2000);
  }
  myGLCD.setFont(SmallFont);  
  myGLCD.setColor(200, 200, 210);
  myGLCD.print("GIRAR", 233, 85);  
  res =  myGLCD.loadBitmap(0,174,66,66,"parar.raw");
  if (res != 0)
  {
    myGLCD.print("error loading parar.raw",0,0); 
    delay(2000);
  }

  boolean salir = false;

  while( !salir)
  {
    escuchar(4);
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
          myGLCD.print("OFF", 105, 83);            
          contacto = false;
         
        }
        else
        {
          waitForIt();
          digitalWrite(PIN_CONTA,HIGH);
          myGLCD.print("ON ", 105, 83);            
          contacto = true;         
        }
      }
      if ( ( x > 190 ) && ( x < 310 ) && ( y > 30) && ( y < 120 ) )
      {
        //encendido del burro, solo mientras este presionado
        if (!encendido)        
        {
          girarBurro();
          digitalWrite(PIN_BURRO,LOW);
          contacto = false;          
        }
      }      
    }
  }

  menuPrincipal();
}
