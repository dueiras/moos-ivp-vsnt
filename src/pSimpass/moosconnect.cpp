#include "moosconnect.h"
#include "LatLong.h"
#include <signal.h>
#include "MBUtils.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "../MOOSGeodesy.cpp" // adiciono a funcao de referencia

using namespace std;
const char _sep = ';';
bool flag = false;
double DESIRED_RUDDER = 0.0f;
double DESIRED_THRUST = 0.0f;
int DESIRED_GEAR = 2; // a vante
double NAV_LAT = 0.0f;
double NAV_LONG = 0.0f;
double NAV_X = 0.0f;
double NAV_Y = 0.0f;
double NAV_YAW = 0.0f;
double NAV_SPEED = 0.0f;
double NAV_HEADING = 0.0f;
double lat_datum = 0.0f;
double lon_datum = 0.0f;
string WPT_UPDATE = "";

QString route_label;
QString route_label_color;
QString route_edge_color;
QString route_vertex_color;
QString route_vertex_size;
QString route_edge_size;

bool latlong_updated = false;
LatLong *latlong = new LatLong();

bool MOOS_MANUAL_OVERRIDE = false;
bool MMO_toUpdate = false;
bool MMO_first = false;
// bool DG_first = false;

bool MOOS_DEPLOY = false;
bool MD_toUpdate = false;
bool MD_first = true;

bool MOOS_RETURN = false;
bool MR_toUpdate = false;
bool MR_first = true;

bool MOOS_END = false;
bool ME_toUpdate = false;
bool ME_first = true;

string MOOS_SHIP_STATUS = "";
bool MSS_toUpdate = false;
bool MSS_first = true;

//Camera Controller-------------------------------
double CAMERA_CNTRL_H = 0.0f;
double CAMERA_CNTRL_V = 0.0f;
double CAMERA_CNTRL_Z = 0.0f;
double camera_cntrl_h_int = 0.0f;
double camera_cntrl_v_int = 0.0f;
double camera_cntrl_z_int = 0.0f;
// double DESIRED_CAMERA_CNTRL_H = 0.0f; //desired
// double DESIRED_CAMERA_CNTRL_V = 0.0f; //desired
// double DESIRED_CAMERA_CNTRL_Z = 0.0f; //desired
// double MOV_CAMERA_CNTRL_H = 0.0f; //moving (value to be added)
// double MOV_CAMERA_CNTRL_V = 0.0f; //moving (value to be added)
// double MOV_CAMERA_CNTRL_Z = 0.0f; //moving (value to be added)
//------------------------------------------------

string IVPHELM_ALLSTOP = "";

MoosConnect::MoosConnect()
{
}

bool MoosConnect::OnStartUp()
{
  MOOS_MANUAL_OVERRIDE = SIMPASS::getInstancePtr()->GetSettings()->value("configuration/moos_manual_override").toBool();
  
  DESIRED_GEAR = 2;
  Notify("DESIRED_GEAR", DESIRED_GEAR);

  route_label = SIMPASS::getInstancePtr()->GetSettings()->value("route/label").toString();
  route_label_color = SIMPASS::getInstancePtr()->GetSettings()->value("route/label_color").toString();
  route_edge_color = SIMPASS::getInstancePtr()->GetSettings()->value("route/edge_color").toString();
  route_vertex_color = SIMPASS::getInstancePtr()->GetSettings()->value("route/vertex_color").toString();
  route_vertex_size = SIMPASS::getInstancePtr()->GetSettings()->value("route/vertex_size").toString();
  route_edge_size = SIMPASS::getInstancePtr()->GetSettings()->value("route/edge_size").toString();
  QCoreApplication::processEvents();
  Console::Debug("configuration/moos_manual_override: " + QString(MOOS_MANUAL_OVERRIDE ? "true" : "false"));
  MMO_first = false;
  // DG_first = false;
  return true;
}

void MoosConnect::ProcessEngine(const QStringList &msg)
{
  // if (msg.at(2).toDouble() != DESIRED_THRUST)
    // Console::Debug("ProcessEngine():" + msg.join(";"));
  DESIRED_THRUST = msg.at(2).toDouble();
}

void MoosConnect::ProcessRudder(const QStringList &msg)
{
  // if (msg.at(1).toDouble() != DESIRED_RUDDER)
    // Console::Debug("ProcessRudder():" + msg.join(";"));
  DESIRED_RUDDER = msg.at(1).toDouble();
}

void MoosConnect::ProcessOpenCPNGPSRoute(const QStringList &msg)
{

  int i = 1;
  QString msgnet = "";
  bool first = true;

  double n_x = 0;
  double n_y = 0;

  while (i < msg.size())
  {
    if (first)
    {
      first = false;
      msgnet = "polygon=";
    }
    else
    {
      msgnet += ":";
    }
    m_geodesy.Initialise(lat_datum, lon_datum);
    m_geodesy.LatLong2LocalGrid(msg.at(i).toDouble(), msg.at(i + 1).toDouble(), n_y, n_x);

    msgnet += QString::number(n_x) + "," + QString::number(n_y);
    i = i + 2;
  }

  // if (msgnet != "")
  // {
  //   msgnet += QString("},") +
  //     QString("label=") + route_label +
  //     QString(",label_color=") + route_label_color +
  //     QString(",edge_color=") + route_edge_color +
  //     QString(",vertex_color=") + route_vertex_color +
  //     QString(",vertex_size=") + route_vertex_size +
  //     QString(",edge_size=") + route_edge_size;
  // }

  // Console::Debug("ProcessOpenCPNGPSRoute():  " + msgnet);
  WPT_UPDATE = msgnet.toStdString();
  // Console::Debug("ProcessOpenCPNGPSRoute():" + msg.join(";"));
}

std::vector<std::string> MoosConnect::split(const std::string &s, char seperator)
{
  std::vector<std::string> output;

  std::string::size_type prev_pos = 0, pos = 0;

  while ((pos = s.find(seperator, pos)) != std::string::npos)
  {
    std::string substring(s.substr(prev_pos, pos - prev_pos));

    output.push_back(substring);

    prev_pos = ++pos;
  }

  output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

  return output;
}

void MoosConnect::ProcessSET_MOOS_MANUAL_OVERRIDE(const QStringList &msg)
{
  if ((msg.at(1) == "true") != MOOS_MANUAL_OVERRIDE)
  {
    Console::Debug("ProcessSET_MOOS_MANUAL_OVERRIDE():" + msg.join(";"));
    MOOS_MANUAL_OVERRIDE = (msg.at(1) == "true");
    QString msgnet = QString("MOOS_MO") + _sep +
                     QString((MOOS_MANUAL_OVERRIDE ? "true" : "false"));
    SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
    QCoreApplication::processEvents();
    MMO_toUpdate = true;
  }
}

void MoosConnect::ProcessMOOS_MANUAL_OVERRIDE_REQUEST(const QStringList &msg)
{
  Console::Debug("ProcessMOOS_MANUAL_OVERRIDE_REQUEST():" + msg.join(";"));
  QString msgnet = QString("MOOS_MO") + _sep +
                   QString((MOOS_MANUAL_OVERRIDE ? "true" : "false"));
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  QCoreApplication::processEvents();
}

void MoosConnect::ProcessMOOS_SHIP_STATUS_REQUEST(const QStringList &msg)
{
  Console::Debug("ProcessMOOS_SHIP_STATUS_REQUEST():" + msg.join(";"));
  QString msgnet = QString("MOOS_SHIP_STATUS") + _sep +
                   QString::fromStdString(MOOS_SHIP_STATUS);
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  QCoreApplication::processEvents();
}

bool MoosConnect::OnNewMail(MOOSMSG_LIST &Mail)
{
  QCoreApplication::processEvents();

  // process it
  MOOSMSG_LIST::iterator q;
  for (q = Mail.begin(); q != Mail.end(); q++)
  {
    // q->Trace();
    CMOOSMsg &msg = *q;

    string key = msg.m_sKey;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;

    if (!flag)
    {
      SIMPASS::getInstancePtr()->GetNetworkManager()->SendAppId("pSimpass");
      flag = true;
      QCoreApplication::processEvents();
    }

    if (key == "MODE" && !MSS_toUpdate)
    {
      string curval = sdata;
      if (MOOS_SHIP_STATUS != curval || MSS_first)
      {
        Console::Debug("MOOS_SHIP_STATUS(): " + QString::fromStdString(curval));
        QString msgnet = QString("MOOS_SHIP_STATUS") + _sep +
                         QString::fromStdString(curval);
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);

        MOOS_SHIP_STATUS = curval;
        MSS_first = false;
      }
      QCoreApplication::processEvents();
    }

    else if (key == "RETURN" && !MR_toUpdate)
    {
      bool curval = (sdata == "true");
      if (MOOS_RETURN != curval || MR_first)
      {
        Console::Debug("MOOS_RETURN(): " + QString((curval ? "true" : "false")));
        QString msgnet = QString("MOOS_RET") + _sep +
                         QString((curval ? "true" : "false"));
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
        MOOS_RETURN = curval;
        MR_first = false;
      }
      QCoreApplication::processEvents();
    }

    else if (key == "END" && !ME_toUpdate)
    {
      bool curval = (sdata == "true");
      if (MOOS_END != curval || ME_first)
      {
        Console::Debug("MOOS_END(): " + QString((curval ? "true" : "false")));
        QString msgnet = QString("MOOS_END") + _sep +
                         QString((curval ? "true" : "false"));
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
        MOOS_END = curval;
        ME_first = false;
      }
      QCoreApplication::processEvents();
    }

    else if (key == "DEPLOY" && !MD_toUpdate)
    {
      bool curval = (sdata == "true");
      if (MOOS_DEPLOY != curval || MD_first)
      {
        Console::Debug("MOOS_DEPLOY(): " + QString((curval ? "true" : "false")));
        QString msgnet = QString("MOOS_DLY") + _sep +
                         QString((curval ? "true" : "false"));
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
        MOOS_DEPLOY = curval;
        MD_first = false;
      }
      QCoreApplication::processEvents();
    }

    else if (key == "MOOS_MANUAL_OVERRIDE" && !MMO_toUpdate)
    {
      bool curval = (sdata == "true");
      if (MOOS_MANUAL_OVERRIDE != curval || MMO_first)
      {
        Console::Debug("MOOS_MANUAL_OVERRIDE(): " + QString((curval ? "true" : "false")));
        QString msgnet = QString("MOOS_MO") + _sep +
                         QString((curval ? "true" : "false"));
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
        MOOS_MANUAL_OVERRIDE = curval;
        MMO_first = false;
      }
      QCoreApplication::processEvents();
    }

    else if (key == "IVPHELM_ALLSTOP")
    {
      IVPHELM_ALLSTOP = sdata;
      Console::Debug("IVPHELM_ALLSTOP(): " + QString::fromStdString(IVPHELM_ALLSTOP));

      if (IVPHELM_ALLSTOP != "clear")
      {
        QString msgnet = QString("MOOS_SHIP_STATUS") + _sep +
                         QString::fromStdString(IVPHELM_ALLSTOP);
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
      }
      else
      {
        QString msgnet = QString("MOOS_SHIP_STATUS") + _sep +
                         QString::fromStdString(MOOS_SHIP_STATUS);
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
      }
      QCoreApplication::processEvents();
    }

    else if (key == "REGION_INFO")
    {
      string a = tokStringParse(sdata, "lat_datum", ',', '=');
      string b = tokStringParse(sdata, "lon_datum", ',', '=');
      Console::Debug("REGION_INFO():  lat_datum=" + QString::fromStdString(a) + " / lon_datum=" + QString::fromStdString(b));
      lat_datum = stod(a);
      lon_datum = stod(b);
      // m_geodesy.Initialise(lat_datum, lon_datum);
      QCoreApplication::processEvents();
    }

    else if (key == "DESIRED_GEAR")
    {
      Console::Debug("DESIRED_GEAR(): " + QString::number(DESIRED_GEAR));
      DESIRED_GEAR = ddata;
      QCoreApplication::processEvents();
    }

    else if (key == "DESIRED_RUDDER")
    {

      // Console::Debug("DESIRED_RUDDER(): " + QString::fromStdString(DESIRED_RUDDER));

      DESIRED_RUDDER = ddata;
      QString msgnet = QString("CRUDDER") + _sep +
                       QString::number(DESIRED_RUDDER);
      SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
      QCoreApplication::processEvents();
    }

    // TODO: passar controle pra HANDLE ao iniciar o pSimpass
    else if (key == "DESIRED_THRUST")
    {

      // Console::Debug("DESIRED_THRUST(): " + QString::fromStdString(DESIRED_THRUST));
      DESIRED_THRUST = ddata;
      QString msgnet = QString("CMACHINE_PERCENTAGE") + _sep +
                       QString::number((DESIRED_GEAR == 0 ? 0 : (DESIRED_GEAR == 1 ? DESIRED_THRUST * -1 : DESIRED_THRUST * 1))) + _sep +
                       QString::number((DESIRED_GEAR == 0 ? 0 : (DESIRED_GEAR == 1 ? DESIRED_THRUST * -1 : DESIRED_THRUST * 1)));
      SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
      QCoreApplication::processEvents();
    }

    else if (key == "CAMERA_CNTRL_H")
    {
      // Console::Debug("CAMERA_CNTRL_H()_ANT: " + QString::number(CAMERA_CNTRL_H));
      Console::Debug("CAMERA_CNTRL_H(): " + QString::number(ddata));
      CAMERA_CNTRL_H = ddata; 
      // Console::Debug("CAMERA_CNTRL_H()_NEW: " + QString::number(CAMERA_CNTRL_H));
      // Console::Debug("CAMERA_CNTRL_H()_int: " + QString::number(camera_cntrl_h_int));
    }

    else if (key == "CAMERA_CNTRL_V")
    {
      Console::Debug("CAMERA_CNTRL_V(): " + QString::number(ddata));
      CAMERA_CNTRL_V = ddata;
    }

    else if (key == "CAMERA_CNTRL_Z")
    {
      Console::Debug("CAMERA_CNTRL_Z(): " + QString::number(ddata));
      CAMERA_CNTRL_Z = ddata;
    }

    // else if (key == "MOV_CAMERA_CNTRL_H")
    // {
    //   Console::Debug("MOV_CAMERA_CNTRL_H(): " + QString::number(MOV_CAMERA_CNTRL_H));
    //   CAMERA_CNTRL_H += ddata;
    //   CAMERA_CNTRL_H = (CAMERA_CNTRL_H < 360 ? 0 : (CAMERA_CNTRL_H > 360 ? 0 : CAMERA_CNTRL_H));
    // }

    // else if (key == "MOV_CAMERA_CNTRL_V")
    // {
    //   Console::Debug("MOV_CAMERA_CNTRL_V(): " + QString::number(MOV_CAMERA_CNTRL_V));
    //   CAMERA_CNTRL_V += ddata;
    //   CAMERA_CNTRL_V = (CAMERA_CNTRL_V < 360 ? 0 : (CAMERA_CNTRL_V > 360 ? 0 : CAMERA_CNTRL_V));
    // }

    // else if (key == "MOV_CAMERA_CNTRL_Z")
    // {
    //   Console::Debug("MOV_CAMERA_CNTRL_Z(): " + QString::number(MOV_CAMERA_CNTRL_Z));
    //   CAMERA_CNTRL_Z += ddata;
    //   CAMERA_CNTRL_Z = (CAMERA_CNTRL_Z < 0 ? 0 : (CAMERA_CNTRL_Z > 100 ? 100 : CAMERA_CNTRL_Z));;
    // }

    if (camera_cntrl_h_int != CAMERA_CNTRL_H || camera_cntrl_v_int != CAMERA_CNTRL_V || camera_cntrl_z_int != CAMERA_CNTRL_Z)
    {
      Console::Debug("SEND CAMERA_CONTROL()");
      QString msgnet = QString("CAMERA_CONTROL") + _sep +
                      QString::number(CAMERA_CNTRL_V) + _sep +
                      QString::number(CAMERA_CNTRL_H) + _sep +
                      QString::number(CAMERA_CNTRL_Z);
      SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
camera_cntrl_h_int = CAMERA_CNTRL_H ;
camera_cntrl_v_int = CAMERA_CNTRL_V ;
camera_cntrl_z_int = CAMERA_CNTRL_Z ;
      QCoreApplication::processEvents();
    }
  }


    // if (key == "NAV_HEADING")
    // {
    //   QString msgnet = QString("HEADING") + _sep +
    //                    QString("0") + _sep +           // Id (ownship == 0)
    //                    QString::number(ddata) + _sep + // Heading
    //                    QString::number(ddata) + _sep + // HeadingErr (exibido pelo Instruments)
    //                    QString("0");                   // Deviation Calculated
    //   SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
    //   QCoreApplication::processEvents();
    // }

    // if (key == "NAV_YAW")
    // {
    //   QString msgnet = QString("RRUDDER") + _sep +
    //                    QString::number(ddata);
    //   SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
    //   QCoreApplication::processEvents();
    // }

    // if (key == "NAV_SPEED")
    // {
    //   QString msgnet = QString("SPEED") + _sep +
    //                    QString("0") + _sep +           // Id (ownship == 0)
    //                    QString::number(ddata) + _sep + // STW
    //                    QString::number(ddata);         // SOG
    //   SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
    //   QCoreApplication::processEvents();
    // }

    // if (key == "NAV_LAT")
    // {
    //   latlong->SetLat(ddata);
    //   latlong_updated = true;
    // }
    // if (key == "NAV_LONG")
    // {
    //   latlong->SetLong(ddata);
    //   latlong_updated = true;
    // }

    // //LATLONG
    // if (latlong_updated)
    // {
    //    QString msgnet = QString("LATLONG") + _sep +
    //                    QString("0") + _sep +                     // Id (ownship == 0)
    //                    latlong->LatToNMEAString() + _sep +
    //                    latlong->LongToNMEAString();
    //   SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
    //   QCoreApplication::processEvents();
    //   latlong_updated = false;
    // }

  return (true);
}

bool MoosConnect::OnConnectToServer()
{
  Register("DESIRED_RUDDER", 0.0);
  Register("DESIRED_THRUST", 0.0);
  Register("DESIRED_GEAR", 0);
  Register("MOOS_MANUAL_OVERRIDE", 0.0);
  // Register("NAV_HEADING", 0.0);
  // Register("NAV_YAW", 0.0);
  // Register("NAV_LAT", 0.0);
  // Register("NAV_LONG", 0.0);
  // Register("NAV_X", 0.0);
  // Register("NAV_Y", 0.0);
  // Register("NAV_SPEED", 0.0);
  Register("REGION_INFO", 0.0);
  Register("WPT_UPDATE", 0.0);

  Register("RETURN", 0.0);
  Register("END", 0.0);
  Register("DEPLOY", 0.0);
  Register("MODE", 0.0);

  Register("IVPHELM_ALLSTOP", 0.0);


  Register("CAMERA_CNTRL_H", 0.0);
  Register("CAMERA_CNTRL_V", 0.0);
  Register("CAMERA_CNTRL_Z", 0.0);

  // Register("DESIRED_CAMERA_CNTRL_H", 0.0);
  // Register("DESIRED_CAMERA_CNTRL_V", 0.0);
  // Register("DESIRED_CAMERA_CNTRL_Z", 0.0);
  Register("MOV_CAMERA_CNTRL_H", 0.0);
  Register("MOV_CAMERA_CNTRL_V", 0.0);
  Register("MOV_CAMERA_CNTRL_Z", 0.0);

  return (true);
}

void MoosConnect::ProcessLatLong(const QStringList &msg)
{
  int id = msg.at(1).toInt();
  // Rio de Janeiro
  double lat_origin = -22.93335; // ALTERAR AQUI SE MUDAR A CARTA NÁUTICA !!!
  double lon_origin = -43.136666665;
  double _nav_x = 0;
  double _nav_y = 0;

  if (id == 0) // Ownship
  {
    float latitude = LatLong::NMEAStringToCoord(msg.at(2), msg.at(3));
    float longitude = LatLong::NMEAStringToCoord(msg.at(4), msg.at(5));

    // if (latitude != NAV_LAT || longitude != NAV_LONG)
    //   Console::Debug("ProcessLatLong():" + msg.join(";"));

    NAV_LAT = latitude;
    NAV_LONG = longitude;

    m_geodesy.Initialise(lat_origin, lon_origin);
    m_geodesy.LatLong2LocalGrid(latitude, longitude, _nav_y, _nav_x);

    NAV_Y = _nav_y;
    NAV_X = _nav_x;
  }
}

void MoosConnect::ProcessHeading(const QStringList &msg)
{
  int id = msg.at(1).toInt();

  if (id == 0) // Ownship
  {
    float heading = msg.at(2).toFloat();

    if (heading != NAV_HEADING)
      // Console::Debug("ProcessHeading():" + msg.join(";"));

    NAV_HEADING = heading;
  }
}

void MoosConnect::ProcessRRudder(const QStringList &msg)
{
  float angle = msg.at(1).toFloat();

  // if (angle != NAV_YAW)
    // Console::Debug("ProcessRRudder():" + msg.join(";"));

  NAV_YAW = angle;
}

void MoosConnect::ProcessSpeed(const QStringList &msg)
{
  int id = msg.at(1).toInt();

  if (id == 0) // Ownship
  {
    float speed = msg.at(2).toFloat();

    // if (speed != NAV_SPEED)
    //   Console::Debug("ProcessSpeed():" + msg.join(";"));

    NAV_SPEED = speed;
  }
}

bool MoosConnect::Iterate()
{
  // std :: vector<unsigned char> X(100) ;
  if (!MMO_first)
    Notify("MOOS_MANUAL_OVERRIDE", (MOOS_MANUAL_OVERRIDE ? "true" : "false"));

  Notify("DESIRED_RUDDER", DESIRED_RUDDER);
  Notify("DESIRED_THRUST", DESIRED_THRUST);
  // if (!DG_first)
  // {
  //   DG_first = true;
  //   Console::Debug("NOTIFY DESIRED_GEAR" );
  //   Notify("DESIRED_GEAR", DESIRED_GEAR);
  // }

  Notify("NAV_LAT", NAV_LAT);
  Notify("NAV_LONG", NAV_LONG);
  Notify("NAV_X", NAV_X);
  Notify("NAV_Y", NAV_Y);
  Notify("NAV_SPEED", NAV_SPEED);
  Notify("NAV_YAW", NAV_YAW);
  Notify("NAV_HEADING", NAV_HEADING);
  Notify("WPT_UPDATE", WPT_UPDATE);
  //   if (camera_cntrl_h_int != CAMERA_CNTRL_H || camera_cntrl_v_int != CAMERA_CNTRL_V || camera_cntrl_z_int != CAMERA_CNTRL_Z)
  //   {
  //     Console::Debug("CAMERA NOTIFY()");
  // // Notify("CAMERA_CNTRL_H", CAMERA_CNTRL_H);
  // // Notify("CAMERA_CNTRL_V", CAMERA_CNTRL_V);
  // // Notify("CAMERA_CNTRL_Z", CAMERA_CNTRL_Z);
  // //   Notify("MOV_CAMERA_CNTRL_H", MOV_CAMERA_CNTRL_H);
  // // Notify("MOV_CAMERA_CNTRL_V", MOV_CAMERA_CNTRL_V);
  // // Notify("MOV_CAMERA_CNTRL_Z", MOV_CAMERA_CNTRL_Z);

  //   }
  // Notify("DESIRED_CAMERA_CNTRL_H", DESIRED_CAMERA_CNTRL_H);
  // Notify("DESIRED_CAMERA_CNTRL_V", DESIRED_CAMERA_CNTRL_V);
  // Notify("DESIRED_CAMERA_CNTRL_Z", DESIRED_CAMERA_CNTRL_Z);



  if (!MR_first)
    Notify("RETURN", (MOOS_RETURN ? "true" : "false"));

  if (!ME_first)
    Notify("END", (MOOS_END ? "true" : "false"));

  if (!MD_first)
    Notify("DEPLOY", (MOOS_DEPLOY ? "true" : "false"));

  if (!MSS_first)
    Notify("MODE", MOOS_SHIP_STATUS);

  MD_toUpdate = false;
  MR_toUpdate = false;
  ME_toUpdate = false;
  MSS_toUpdate = false;
  MMO_toUpdate = false;

  return true;
}

void MoosConnect::ProcessSET_MOOS_DEPLOY(const QStringList &msg)
{
  if ((msg.at(1) == "true") != MOOS_DEPLOY)
    Console::Debug("MOOS_DEPLOY():" + msg.join(";"));
  MOOS_DEPLOY = (msg.at(1) == "true");
  QString msgnet = QString("MOOS_DLY") + _sep +
                   QString((MOOS_DEPLOY ? "true" : "false"));
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  MD_toUpdate = true;
  QCoreApplication::processEvents();

  if (!MOOS_DEPLOY)
  {
    MOOS_RETURN = false;
    MOOS_END = false;

    MR_toUpdate = true;
    ME_toUpdate = true;

    Console::Debug("ProcessSET_MOOS_MANUAL_OVERRIDE():MOOS_END(): " + QString((MOOS_END ? "true" : "false")));
    QString msgnete = QString("MOOS_END") + _sep +
                      QString((MOOS_END ? "true" : "false"));
    SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnete, true);
    QCoreApplication::processEvents();

    Console::Debug("ProcessSET_MOOS_MANUAL_OVERRIDE():MOOS_RETURN(): " + QString((MOOS_RETURN ? "true" : "false")));
    QString msgnetr = QString("MOOS_RET") + _sep +
                      QString((MOOS_RETURN ? "true" : "false"));
    SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnetr, true);
    QCoreApplication::processEvents();
  }
}

void MoosConnect::ProcessMOOS_DEPLOY_REQUEST(const QStringList &msg)
{
  Console::Debug("ProcessMOOS_DEPLOY_REQUEST():" + msg.join(";"));
  QString msgnet = QString("MOOS_DLY") + _sep +
                   QString((MOOS_DEPLOY ? "true" : "false"));
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  QCoreApplication::processEvents();
}

void MoosConnect::ProcessSET_MOOS_RETURN(const QStringList &msg)
{
  if ((msg.at(1) == "true") != MOOS_RETURN)
    Console::Debug("MOOS_RETURN():" + msg.join(";"));
  MOOS_RETURN = (msg.at(1) == "true");
  QString msgnet = QString("MOOS_RET") + _sep +
                   QString((MOOS_RETURN ? "true" : "false"));
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  MR_toUpdate = true;
  QCoreApplication::processEvents();
}

void MoosConnect::ProcessMOOS_RETURN_REQUEST(const QStringList &msg)
{
  Console::Debug("ProcessMOOS_RETURN_REQUEST():" + msg.join(";"));
  QString msgnet = QString("MOOS_RET") + _sep +
                   QString((MOOS_RETURN ? "true" : "false"));
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  QCoreApplication::processEvents();
}

void MoosConnect::ProcessSET_MOOS_END(const QStringList &msg)
{
  if ((msg.at(1) == "true") != MOOS_END)
    Console::Debug("ProcessSET_MOOS_END():" + msg.join(";"));
  MOOS_END = (msg.at(1) == "true");
  QString msgnet = QString("MOOS_END") + _sep +
                   QString((MOOS_END ? "true" : "false"));
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  ME_toUpdate = true;
  QCoreApplication::processEvents();
}

void MoosConnect::ProcessMOOS_END_REQUEST(const QStringList &msg)
{
  Console::Debug("ProcessMOOS_END_REQUEST():" + msg.join(";"));
  QString msgnet = QString("MOOS_END") + _sep +
                   QString((MOOS_END ? "true" : "false"));
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  QCoreApplication::processEvents();
}