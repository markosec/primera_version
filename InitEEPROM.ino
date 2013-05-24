#include <EEPROM.h>
#include <EEPROMAnything.h>


struct config_t
{
  long m_totales_i;
  long m_totales_f;
  int  acelerado;
  int  desacelerado;
  int  cuanto;    
  int  burro;
  int  cebador;
  int  guardado;
  int  clave;
} config;



void setup()
{
  config.m_totales_i  = 40000;
  config.m_totales_f  = 0;
  config.acelerado    = 750;
  config.desacelerado = 1000;
  config.cuanto       = 90;
  config.burro        = 1000;
  config.cebador      = 90;
  config.guardado     = 300000;
  config.clave        = 1863;
    
  EEPROM_writeAnything(0, config);
  
}

void loop()
{
  
}
