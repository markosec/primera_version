#include <UTFT.h>
#include <UTouch.h>
#include <UTFT_tinyFAT.h>
#include <tinyFAT.h>


UTFTtf myGLCD(ITDB32S,38,39,40,41);
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];

void setup()
{
  myGLCD.InitLCD(LANDSCAPE);
  myGLCD.clrScr();
  viajarScreen();
}
void loop()
{

}

#define COLOR_FONT_R 255
#define COLOR_FONT_G 255
#define COLOR_FONT_B 255

#define COLOR_BACK_R 0
#define COLOR_BACK_G 0
#define COLOR_BACK_B 0

//Rectangulo 
#define LINE_THICK 5

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

} 

void messageBox()
{
  //Caja de mensajes
  rectangulo(160,145,310,230); 
  myGLCD.setFont(SmallFont);
  myGLCD.print("12345678901234567",170,150);
  myGLCD.print("11111111111111111",170,160);
  myGLCD.print("22222222222222222",170,170);
  myGLCD.print("33333333333333333",170,180);
  myGLCD.print("44444444444444444",170,190);  
  myGLCD.print("55555555555555555",170,200);  
  myGLCD.print("66666666666666666",170,210);    

}

void setAutoScreen()
{

  botonGuardar();
  botonVolver();

  //Giro del burro
  sumador(70,5);
  rectangulo(10,30,160,90);
  restador(70,95);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(55, 140 ,255);        
  myGLCD.print("1500",15,35);

  //Cebador
  sliderH(30,180,2);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(55, 140 ,255);      
  myGLCD.print("Cebador",30,150);  


  //Aceleador
  sliderH(200,30,3);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(55, 140 ,255);      
  myGLCD.print("Acelerador",200,1);  

  //Up-time
  sliderH(200,105,2);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(55, 140 ,255);      
  myGLCD.print("Up-time",200,76);  


  //Down-time
  sliderH(200,180,2);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(55, 140 ,255);      
  myGLCD.print("Down-time",200,150);  

}
void setupScreen()
{
  lineaCabecera();

  myGLCD.setFont(BigFont);
  //Hora y fecha
  rectangulo(10,40,155,96);
  myGLCD.print("HORA",50,60);

  //Arranque
  rectangulo(10,106,155,162);  
  myGLCD.print("ARRANQUE",20,126);

  //Calentar  
  rectangulo(10,172,155,228);  
  myGLCD.print("CALENTAR",20,192);  

  //Nafta
  rectangulo(165,40,300,96);  
  myGLCD.print("NAFTA",192,60);  

  //Guardado
  rectangulo(165,106,300,162);  
  myGLCD.print("GUARDADO",170,126);  

  //Clave  
  rectangulo(165,172,300,232);  
  myGLCD.print("CLAVE",192,192);


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
  //  myGLCD.drawLine(x+10,y+5,x+10,y+15);
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);      
}

void configNafta()
{
  botonVolver();
  botonGuardar();

  //Valor de 0 a 1023 para la alerta
  sumador(140,45);
  rectangulo(80,70,230,130);
  restador(140,135);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(55, 140 ,255);        
  myGLCD.print("0450",90,75);

}

void configGuardado()
{
  botonVolver();
  botonGuardar();

  //Valor de 0 a 5 para guardar automaticamente en la SD
  
  //Guardado
  sliderH(100,120,4);
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(55, 140 ,255);          
  myGLCD.print("Tiempo entre guardados",60,80);  
 

}



void botonGuardar()
{
  //Boton guardar
  rectangulo(289,210,319,239);  
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);  
  myGLCD.setFont(BigFont);  
  myGLCD.print("S",296,218);  
}
void confHoraScreen()
{
  //Boton volver
  botonVolver();
  //Boton guardar
  botonGuardar();


  //Dia 
  sumador(37,5);
  rectangulo(10,30,85,90);
  restador(37,95);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 215, 0);        
  myGLCD.print("23",15,35);

  //Mes
  sumador(122,5);
  rectangulo(95,30,170,90);
  restador(122,95);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 215, 0);        
  myGLCD.print("04",100,35);  

  //Año
  sumador(240,5);
  rectangulo(180,30,319,90);
  restador(240,95);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 215, 0);        
  myGLCD.print("2013",187,35);    

  //Hora
  sumador(72,120);
  rectangulo(45,145,120,205);
  restador(72,210);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 215, 0);      
  myGLCD.print("15",50,150);  


  //Minuto
  sumador(194,120);
  rectangulo(167,145,242,205);
  restador(194,210);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 215, 0);      
  myGLCD.print("35",172,150);  


}
void mainScreen()
{ 
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

void botonVolver()
{
  rectangulo(1,210,31,239);  
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);  
  myGLCD.setFont(BigFont);  
  myGLCD.print("B",9,218);
}



#define SLIDER_LENGTH 100
#define SLIDER_BUTTON_H 10

void sliderH(int x, int y, int valor)
{
  //Linea
  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);
  myGLCD.drawLine(x,y,x + SLIDER_LENGTH,y);
  myGLCD.drawLine(x,y,x + SLIDER_LENGTH + 1,y);
  for (int i=0; i <= 5; i++)
  {
    myGLCD.drawLine((x+(i*20)),y-2,(x+(i*20)),y+3);
  }
  //Valor esta entre 0 y 5
  myGLCD.fillCircle((x + (20 * valor)),y,10);
  myGLCD.setColor(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B);
  myGLCD.drawCircle((x + (20 * valor)),y,4);

  myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);

}


void manualScreen()
{
  botonVolver();
  lineaCabecera();

  //Contacto
  rectangulo(10,40,103,140);
  myGLCD.setFont(BigFont);
  myGLCD.print("ON",35,80);

  //CEBADOR
  rectangulo(113,40,203,140);
  myGLCD.print("CHOKE",120,80); 

  //calentar
  rectangulo(213,40,306,140);
  myGLCD.print("START",219,80);

  //Seccion Sliders
  myGLCD.setFont(SmallFont);  

  //Aceleador
  sliderH(30,180,2);
  myGLCD.print("Acelerador",30,150);

  sliderH(180,180,4);  
  myGLCD.print("Cebador",190,150);
}

//cuanto es de 0 a 22 que tan alto se dibujan las rpm...
void medidorRpm(int x1, int y1, int cuanto)
{
  int ancho = 40;
  // 30  100 22 ejemplo
  int y = 160 + y1; //Para ir de abajo para arriba
  int linea = 160 / 22;
  //40 x 110 px de tamaño, cada 5 px 1 medida (22 max)
  rectangulo(x1,y1,x1+ancho, y1+160);
  myGLCD.setColor(0, 255,0);  //Verde
  for (int i = 1 ; i<22;i++)
  {
    if (i > cuanto)
      myGLCD.setColor(0, 0,0);  //Negro
    else  
      switch(i)
    {
    case 10:
      myGLCD.setColor(255,255,0);  //Amarillo
      break;
    case 15:
      myGLCD.setColor(255, 0,0);  //rojo
      break;    
    }
    myGLCD.fillRect(x1+LINE_THICK,y-(i*linea)+1,(x1+ancho-LINE_THICK),y-(i*linea)-3);    
    myGLCD.fillRect(x1+LINE_THICK,y-(i*linea)+1,(x1+ancho-LINE_THICK),y-(i*linea)-2);    
    myGLCD.fillRect(x1+LINE_THICK,y-(i*linea)+1,(x1+ancho-LINE_THICK),y-(i*linea)-1);
    //    myGLCD.fillRect(x1+LINE_THICK,y-(i*5)+2,(x1+ancho-LINE_THICK),y-(i*5)-2);
    myGLCD.fillRect(x1+LINE_THICK,y-(i*linea),(x1+ancho-LINE_THICK),y-(i*linea));

  }
}

void   medidorNafta( int x1, int y1, int real,int cuanto)
{
  int y = 240; //Para ir de abajo para arriba
  int ancho = 20;
  String numero = String(real);
  //40 x 110 px de tamaño, cada 5 px 1 medida (10 max)
  rectangulo(x1,y1,x1+ancho, y1+100);
  y = y1 + 100;
  myGLCD.setFont(SmallFont);
  myGLCD.print(numero,x1+ancho+15,y-25,90);
  myGLCD.setColor(255,0,0);  //Rojo
  for (int i = 1 ; i<10;i++)
  {
    if (i > cuanto)
      myGLCD.setColor(0, 0,0);  //Negro
    else      
      switch(i)
    {
    case 6:
      myGLCD.setColor(0, 255,0);  //Amarillo
      break;
    case 3:
      myGLCD.setColor(255, 255,0);  //rojo
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

void marcha(int x, int y, int marcha)
{

  String texto = String(marcha);
  rectangulo(x,y,x+45,y+60);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(255,70,43);   
  myGLCD.print(texto, x+LINE_THICK,y+LINE_THICK);  

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
      myGLCD.drawLine(x+5,y   ,x+35,y+8);  
      myGLCD.drawLine(x+5,y+4 ,x+35,y+12);  
      myGLCD.drawLine(x+5,y+8 ,x+35,y+16);    
    }
    else
    {
      myGLCD.setColor(100,100,255);  
      myGLCD.drawLine(x+5,y-8 ,x+35,y-8);
      myGLCD.drawLine(x+5,y-4 ,x+35,y-4);  
      myGLCD.drawLine(x+5,y   ,x+35,y  ) ;    
      myGLCD.drawLine(x+5,y+4 ,x+35,y+4);
      myGLCD.drawLine(x+5,y+8 ,x+35,y+8);  


    }
  } 

}


void odometro(int x, int y)
{  
  String aux_str;  
  myGLCD.setFont(BigFont);
  aux_str = String(m_totales_f);
  if (aux_str.length() > 2)
    aux_str = aux_str.substring(0,2);

  myGLCD.print(aux_str, x+60,y);
  myGLCD.print(".", (x+50),y);
  myGLCD.printNumI(m_totales_i,x,y,5,'0');
}

void viajarScreen()
{
  lineaCabecera();  
  botonVolver();
  messageBox();

  medidorRpm(1,40,22);
  medidorNafta(120,130,840,10);

  marcha(45,140,0);
  velocimetro(100,40,888);
  luces(260,70,0,1,1);
  odometro(x,110(140)  
//  rectangulo(100,40,100+105,40+60);
//  rectangulo(160,145,310,230); 

}

void autoScreen()
{
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

void arrancarScreen()
{
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

void cambioPassScreen()

{
 loginScreen();
 myGLCD.setColor(COLOR_FONT_R, COLOR_FONT_G, COLOR_FONT_B);     
 myGLCD.print("Clave anterior",40,190);
  
}













