/************************************************************/
/*    NAME: Douglas Lima, Eduardo Eiras                     */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Serial.cpp                                      */
/*    DATE: 8/11/2022                                       */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "Serial.h"
#include <unistd.h>
#include <cmath>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <libserial/SerialStream.h>

using namespace std;
using namespace LibSerial;

//---------------------------------------------------------
// Constructor()

Serial::Serial()
{
  //Configurações para envio de dados via Serial

  endereco_porta_serial = "/dev/ttyUSB0"; //Porta real na lancha
  baudrate = 9600;
  portOpened = 0;

  // Valores padrões:
  rudder = 0; //Valor inicial do leme
  thrust = 0; //Valor inicial da máquina
  feedback_leme = "NULL";
  desired_gear = 0;
  last_gear = 0;

  min_thrust = 5;

  //Simulador de ângulo do leme
  //angulo_leme = 0; //Começa zerado
  
  thrust_convertido = "NULL"; //Valor inicial de máquina

}

//---------------------------------------------------------
// Comandos na Serial
// L{0,1,2}{D,R} comando de Leme 
// A{000-100} Atuador de máquina
// E{0,1,2} Engrazamento Neutro, Ré, Avante


//---------------------------------------------------------
// Destructor

Serial::~Serial()
{
  enviaSerial("LD0"); //Para o leme quando encerra o programa
  enviaSerial("A000"); //Para o atuador de máquina a 0%
  enviaSerial("E0");  // Marcha no neutro
  last_gear = 0;
  serial_port.Close();
  std::cout << "Serial port closed." << std::endl;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool Serial::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

     if(key == "DESIRED_RUDDER") 
       rudder = msg.GetDouble();
     else if(key == "DESIRED_THRUST")
        thrust = msg.GetDouble();
     else if(key == "ANGULO_LEME")
        angulo_leme = msg.GetDouble();
      else if(key == "DESIRED_GEAR")
        desired_gear = (int)msg.GetDouble();
     else if(key == "DEPLOY")
        deploy = msg.GetString(); 
     else if(key == "MOOS_MANUAL_OVERIDE")
        manual_overide = msg.GetString(); 
     else if(key == "RETURN")
        return_var = msg.GetString();
     else if(key == "DESIRED_SPEED")
        desired_speed = msg.GetDouble();
     else if(key == "NAV_SPEED")
        nav_speed = msg.GetDouble();
     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool Serial::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool Serial::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!
  // Pensando em colocar aqui nessa interação o envio direto da porta serial
  // Vou fazer um teste primeiro para saber qual é a saída

  if (desired_gear != last_gear) {
    if (thrust < min_thrust) {
      std::string gear_msg = "E" + std::to_string(desired_gear);
      enviaSerial(gear_msg);
      last_gear = desired_gear;
    }
  } 

  //if (last_gear != 0) {
    if ((int(thrust) > 0) && (int(thrust) <= 100)) {
      int roundedThrust = static_cast<int>(std::round(thrust));
      std::ostringstream oss;
      oss << "A" << std::setfill('0') << std::setw(3) << roundedThrust;
      thrust_convertido = oss.str(); //Envia o valor respectivo de máquina
    } 
    else if (int(thrust) <= 0) {
      thrust_convertido = "A000"; //Apenas zera o atuador 
    }

    enviaSerial(thrust_convertido); //Faço envio do comando de maquina para placa
  //}



  //Send desired rudder

  std::ostringstream osstring;
  if (rudder < 0){
    int intRudder = -1*rudder;
    //LE = Leme Esquerda (bombordo) 
    osstring << "LE" << std::setfill('0') << std::setw(2) << intRudder;
  }
  else {
    int intRudder = (int) rudder;
    //LD = Leme Direita (boreste)
    osstring << "LD" << std::setfill('0') << std::setw(2) << intRudder;
  }
  ultimo_comando = osstring.str(); 
  enviaSerial(ultimo_comando);

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Serial::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '=')); //lower parameter  name
    string value = line;

    bool handled = false;
    if(param == "serialport") {
      endereco_porta_serial = value;
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }

  //da permissoes para a porta usb

  std::string password = "eduard0";  // Replace with your actual sudo password
  std::string command = "echo '" + password + "' | sudo -S chmod 777 " +  endereco_porta_serial;
  int result = system(command.c_str());

  if (result == 0) {
        std::cout << " \nPermissions changed successfully to " + endereco_porta_serial << std::endl;
    } else {
        std::cerr << "\nFailed to change permissions." << std::endl;
        return(false);
    }

  std::string command_ = "stty -F " +  endereco_porta_serial + " -hupcl"; // Disable Hangup on Close
  int result_ = system(command_.c_str());
  if (result_ == 0) {
      std::cout << "\nHangup on Close Disabled" << std::endl;
  } else {
      std::cerr << "\nHangup on Close NOT Disabled" << std::endl;
      return(false);
  }

 // Open the serial port
  serial_port.Open(endereco_porta_serial);
  if (!serial_port.good()) {
      std::cerr << "Error: Unable to open serial port." << std::endl;
      portOpened = 0;
      return false;
  }
  else {
    portOpened = 1;
  }
  // Set baud rate
  serial_port.SetBaudRate(BaudRate::BAUD_9600);
  if (!serial_port.good()) {
      std::cerr << "Error: Unable to set baud rate." << std::endl;
      return false;
  }

  registerVariables();	
  return(true);

}

//---------------------------------------------------------
// Procedure: registerVariables()

void Serial::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("DESIRED_RUDDER", 0); //Registro da variável do leme
  Register("DESIRED_THRUST", 0); //Registro da variável da máquina
  Register("ANGULO_LEME", 0); //Registro da variável da máquina
  Register("DESIRED_GEAR", 0); //Registro da leitura de marcha
  Register("DEPLOY", 0); //Registro da variável deploy (start no pmarineviewer)
  Register("RETURN", 0); //Registro da variável return (start no pmarineviewer)
  Register("DESIRED_SPEED", 0); //Pega a veloc desejada
  Register("NAV_SPEED", 0); //Veloc do navio dada pelo GPS
  Register("MOOS_MANUAL_OVERIDE", 0); //Comando manual da lancha

}


//------------------------------------------------------------
// Procedure: buildReport()

bool Serial::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab(4);
  actab << "deploy | serial_read | portOpened | msg_serial ";
  actab.addHeaderLines();
  actab << deploy << serial_read << portOpened << ultimo_comando;
  m_msgs << actab.getFormattedString();

  m_msgs << endl << "============================================" << endl;

  ACTable actab2(1);
  actab2 << "endereco_porta_serial ";
  actab2.addHeaderLines();
  actab2 << endereco_porta_serial;
  m_msgs << actab2.getFormattedString();

  return(true);
}


void Serial::enviaSerial(std::string sentenca) 
{
  
  //Só manda se começar autonomo ou ficar no controle manual
  if (deploy == "true" || manual_overide == "true") {
    //Envio de feedback do leme para a porta serial
    try {
      serial_port << sentenca << std::endl;
    }
    catch (std::system_error& e)
    {
      std::cout << e.what();
    }
    std::getline(serial_port, serial_read);
    std::cout << "Received: " << serial_read << std::endl;

  }

}


