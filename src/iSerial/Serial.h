/************************************************************/
/*    NAME: Douglas Lima                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Serial.h                                          */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#ifndef Serial_HEADER
#define Serial_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <libserial/SerialStream.h>

using namespace LibSerial;

class Serial : public AppCastingMOOSApp
{
 public:
   Serial();
   ~Serial();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

   void enviaSerial(std::string sentenca); //Função que envia os dados via serial para o PIC

 private: // Configuration variables
    std::string endereco_porta_serial;
    int baudrate; // Baudrate a ser usada para envio dos dados na porta serial
    std::string serial_read;

 private: // State variables
    SerialStream serial_port;
    double rudder;
    double thrust;
    int desired_gear;
    int last_gear;
    double min_thrust;
    double angulo_leme;
    double desired_speed;
    double nav_speed;
    double limite_negativo;
    double limite_positivo;
    double erro_maximo_devagar;
    double erro_minimo_devagar;
    double erro_maximo_rapido;
    double erro_minimo_rapido;
    bool portOpened;
    std::string ultimo_comando;
    std::string leme_placa;
    std::string sentenca;
    std::string thrust_convertido;
    std::string deploy;
    std::string manual_overide;
    std::string return_var;
    std::string feedback_leme;
};

#endif 
