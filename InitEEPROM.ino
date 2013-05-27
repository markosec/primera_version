#include <EEPROM.h>
#include <EEPROMAnything.h>


struct config_t
{
  long m_totales_i;
  long m_totales_f;
  int  acelerador;
  int  cuanto_acel;    
  int  cuanto_desacel;   
  int  burro;
  int  cebador;
  int  guardado;
  int  clave;
} config;



void setup()
{
  config.m_totales_i    = 40000;
  config.m_totales_f    = 0;
  config.acelerador     = 90;
  config.cuanto_acel    = 750;
  config.cuanto_desacel = 750;
  config.burro          = 1000;
  config.cebador        = 90;
  config.guardado       = 300000;
  config.clave          = 1863;
    
  EEPROM_writeAnything(0, config);
  
}

void loop()
{
  
}
