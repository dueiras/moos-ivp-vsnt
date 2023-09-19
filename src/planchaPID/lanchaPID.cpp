/************************************************************/
/*    NAME: Douglas Lima                                              */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: lanchaPID.cpp                                        */
/*    DATE: December 29th, 1963                             */
/************************************************************/

#include "MBUtils.h"
#include "ACTable.h"
#include "lanchaPID.h"
#include "PIDParameterReader.h"
#include <cmath>
#include <string>
#include <vector>
#include <iterator>

using namespace std;

//Variáveis globais
double nav_heading;  // Rumo atual do navio
double desired_heading;  // Rumo desejado do navio
double antigo_desired_heading = 0; //Desired Heading anterior
double nav_speed;  // Velocidade atual do navio
double desired_speed;  // Velocidade desejada do navio

double desired_thrust;  // Saída do controle de velocidade
double desired_rudder;  // Saída do controle de rumo

double speed_kp = 0.0; //kp da veloc
double speed_ki = 0.0; //ki da veloc
double speed_kd = 0.0; //kd da veloc

double heading_kp = 0.0; //kp do heading
double heading_ki = 0.0; //ki do heading
double heading_kd = 0.0; //kd do heading

double dt = 0.01; //Time step (change this if MOOS tick configuration is changed)

std::string deploy; //deploy
std::string moos_manual_overide;
std::string moos_manual_override;

std::string constant_heading; //Variável para verificar se o rumo constante é verdadeiro
double setpoint_heading; //Variável em que eu coloco o setpoint de rumo
double antigo_setpoint_heading = 0;

//Limite do desired_thrust e do desired_rudder
double max_desired_thrust=50;
double min_desired_thrust=0;
double max_desired_rudder=38;
double min_desired_rudder=-38;

//Cria os objetos para o controle PID com valores iniciais de kp, ki e kd
//PIDController speedController(speed_kp, speed_ki, speed_kd);
//PIDController headingController(heading_kp, heading_ki, heading_kd);
PIDController speedController("/home/dueiras/VSNT/moos-ivp-vsnt/src/planchaPID/speed_pid_parameters.txt");
PIDController headingController("/home/dueiras/VSNT/moos-ivp-vsnt/src/planchaPID/heading_pid_parameters.txt");
//---------------------------------------------------------
// Constructor()

PIDController::PIDController(const std::string& filename)
{
  // Read
  std::vector<double> pidParams = readPIDParameters(filename);
  kp = pidParams[0];
  ki = pidParams[1];
  kd = pidParams[2];
  integral = 0;
  prevError = 0;
}

//---------------------------------------------------------
// Calculate()

double PIDController::Calculate(double desired, double current, double dt) {
    double error = desired - current;

    //Passando para a escala de -180 a 180
    if (error < -180){
      error += 360;
    } else if (error > 180){
      error -= 360;
    }

    integral += error*dt; //Multiplico pelo período dt de tempo
    double derivative = (error - prevError)/dt;

    double output = kp * error + ki * integral + kd * derivative;

    prevError = error;

    return output;
} // Calculate

//---------------------------------------------------------
// setKP()

void PIDController::setKP(double KP) {
  kp = 100;
}

//---------------------------------------------------------
// setKI()

void PIDController::setKI(double KI) {
  ki = KI;
  integral = 0;
}

//---------------------------------------------------------
// setKD()

void PIDController::setKD(double KD) {
  kd = KD;
}

//---------------------------------------------------------
// getKP()

double PIDController::getKP() {
  return kp;
}

//---------------------------------------------------------
// getKI()

double PIDController::getKI() {
  return ki;
}

//---------------------------------------------------------
// getKD()

double PIDController::getKD() {
  return kd;
}

//---------------------------------------------------------
// resetIntegral()

void PIDController::resetIntegral() {
  integral = 0.0;
}

//---------------------------------------------------------
// Constructor()

lanchaPID::lanchaPID()
{
}

//---------------------------------------------------------
// Destructor

lanchaPID::~lanchaPID()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool lanchaPID::OnNewMail(MOOSMSG_LIST &NewMail)
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

     if(key == "NAV_HEADING") 
       nav_heading = msg.GetDouble();
     else if(key == "DESIRED_HEADING") 
       desired_heading = msg.GetDouble();
     else if(key == "NAV_SPEED") 
       nav_speed = msg.GetDouble();
     else if(key == "DESIRED_SPEED") 
       desired_speed = msg.GetDouble();  
     else if(key == "SPEED_KP") 
       speedController.setKP(msg.GetDouble());
     else if(key == "SPEED_KI") 
       speedController.setKI(msg.GetDouble());
     else if(key == "SPEED_KD") 
       speedController.setKD(msg.GetDouble());
     else if(key == "HEADING_KP") 
       headingController.setKP(msg.GetDouble());
     else if(key == "HEADING_KI") 
       headingController.setKI(msg.GetDouble());
     else if(key == "HEADING_KD") 
       headingController.setKD(msg.GetDouble());
     else if(key == "DEPLOY") 
       deploy = msg.GetString();   
     else if(key == "MOOS_MANUAL_OVERIDE") 
       moos_manual_overide = msg.GetString();  
     else if(key == "MOOS_MANUAL_OVERRIDE") 
       moos_manual_override = msg.GetString();  
     else if(key == "CONSTANT_HEADING") 
       constant_heading = msg.GetString();  
     else if(key == "SETPOINT_HEADING") 
       setpoint_heading = msg.GetDouble();  
    


     else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
       reportRunWarning("Unhandled Mail: " + key);
   }
	
   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool lanchaPID::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool lanchaPID::Iterate()
{
  AppCastingMOOSApp::Iterate();
  // Do your thing here!

  // Loop de controle
  
  // Calcule o controle de velocidade
  desired_thrust = speedController.Calculate(desired_speed, nav_speed,dt);

  //Limita a saída
  if (desired_thrust < min_desired_thrust) { 
    desired_thrust = min_desired_thrust;
  } else if (desired_thrust > max_desired_thrust) {
    desired_thrust = max_desired_thrust;
  }

  // Calcule o controle de rumo
  //Caso o rumo constante esteja ativado o setpoint vai ser esse rumo
  if (constant_heading == "true"){
    //zero o erro integral se o setpoint mudar
    if (setpoint_heading != antigo_setpoint_heading) {
      //headingController.integral = 0; //Zero o erro integral
      headingController.resetIntegral();
      antigo_setpoint_heading = setpoint_heading; 
    }
    desired_rudder = headingController.Calculate(setpoint_heading, nav_heading,dt);
  } else {
    if (desired_heading != antigo_desired_heading){
      //headingController.integral = 0; //Zero o erro integral
      headingController.resetIntegral();
      antigo_desired_heading = desired_heading;
    }
    desired_rudder = headingController.Calculate(desired_heading, nav_heading,dt);
  }
  
  //Limita a saída
  if (desired_rudder < min_desired_rudder) { 
    desired_rudder = min_desired_rudder;
  } else if (desired_rudder > max_desired_rudder) {
    desired_rudder = max_desired_rudder;
  }

  // Imprima as saídas dos controladores

  //Publico DESIRED_RUDDER e DESIRED_THRUST
  //Só publica quando deploy for true e moos_manual_override for false
  if (deploy == "true" && moos_manual_overide == "false"){
    Notify("DESIRED_THRUST", desired_thrust);
    Notify("DESIRED_RUDDER", desired_rudder);
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool lanchaPID::OnStartUp()
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
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "foo") {
      handled = true;
    }
    else if(param == "bar") {
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void lanchaPID::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("NAV_HEADING", 0);
  Register("DESIRED_HEADING", 0);
  Register("NAV_SPEED", 0);
  Register("DESIRED_SPEED", 0);
  Register("SPEED_KP", 0);
  Register("SPEED_KI", 0);
  Register("SPEED_KD", 0);
  Register("HEADING_KP", 0);
  Register("HEADING_KI", 0);
  Register("HEADING_KD", 0);
  Register("DEPLOY", 0);
  Register("MOOS_MANUAL_OVERIDE", 0);
  Register("MOOS_MANUAL_OVERRIDE", 0);
  Register("CONSTANT_HEADING",0);
  Register("SETPOINT_HEADING",0);

}

//------------------------------------------------------------
// Procedure: buildReport()

bool lanchaPID::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File:                                       " << endl;
  m_msgs << "============================================" << endl;

  ACTable actab1(4);
  actab1 << "DESIRED_THRUST | DESIRED_RUDDER | CONSTANT_HEADING | SETPOINT_HEADING ";
  actab1.addHeaderLines();
  actab1 << desired_thrust << desired_rudder << constant_heading << setpoint_heading;
  m_msgs << actab1.getFormattedString();
  m_msgs << endl;
  m_msgs << "---------------------------------------------------" << endl;

  ACTable actab2(3);
  actab2 << "SPEED_KP | SPEED_KI | SPEED_KD ";
  actab2.addHeaderLines();
  actab2 << speedController.getKP() << speedController.getKI() << speedController.getKD() ;
  m_msgs << actab2.getFormattedString();
  m_msgs << endl;
  m_msgs << "---------------------------------------------------" << endl;

  ACTable actab3(3);
  actab3 << "HEADING_KP | HEADING_KI | HEADING_KD";
  actab3.addHeaderLines();
  actab3 << headingController.getKP() << headingController.getKI() << headingController.getKD();
  m_msgs << actab3.getFormattedString();
  m_msgs << endl;


  return(true);
}

