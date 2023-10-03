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
LatLong* latlong = new LatLong();

bool MOOS_MANUAL_OVERRIDE=false;
bool MMO_toUpdate = false;
bool MMO_first = false;

bool MOOS_DEPLOY=false;
bool MD_toUpdate = false;
bool MD_first = true;

bool MOOS_RETURN=false;
bool MR_toUpdate = false;
bool MR_first = true;

bool MOOS_END=false;
bool ME_toUpdate = false;
bool ME_first = true;

string MOOS_SHIP_STATUS = "";
bool MSS_toUpdate = false;
bool MSS_first = true;


string IVPHELM_ALLSTOP = "";

MoosConnect::MoosConnect()
{
}


bool MoosConnect::OnStartUp()
{
  MOOS_MANUAL_OVERRIDE = SIMPASS::getInstancePtr()->GetSettings()->value( "configuration/moos_manual_override" ).toBool();
  route_label = SIMPASS::getInstancePtr()->GetSettings()->value( "route/label" ).toString();
  route_label_color = SIMPASS::getInstancePtr()->GetSettings()->value( "route/label_color" ).toString();
  route_edge_color = SIMPASS::getInstancePtr()->GetSettings()->value( "route/edge_color" ).toString();
  route_vertex_color = SIMPASS::getInstancePtr()->GetSettings()->value( "route/vertex_color" ).toString();
  route_vertex_size = SIMPASS::getInstancePtr()->GetSettings()->value( "route/vertex_size" ).toString();
  route_edge_size = SIMPASS::getInstancePtr()->GetSettings()->value( "route/edge_size" ).toString();
  QCoreApplication::processEvents();
  Console::Debug("configuration/moos_manual_override: " + QString(MOOS_MANUAL_OVERRIDE?"true":"false"));
  MMO_first = false;
  return true;
}

void MoosConnect::ProcessEngine(const QStringList &msg)
{
  if (msg.at(2).toDouble() != DESIRED_THRUST)
    Console::Debug("ProcessEngine():" + msg.join(";"));
  DESIRED_THRUST = msg.at(2).toDouble();  
}

void MoosConnect::ProcessRudder(const QStringList &msg)
{
  if (msg.at(1).toDouble() != DESIRED_RUDDER)
    Console::Debug("ProcessRudder():" + msg.join(";"));
  DESIRED_RUDDER = msg.at(1).toDouble();
}

void MoosConnect::ProcessOpenCPNGPSRoute(const QStringList &msg)
{
  
  int i = 1;
  QString msgnet = "";
  bool first = true;
  
  double n_x = 0;
  double n_y = 0;

  
  while (i<msg.size())
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
    m_geodesy.LatLong2LocalGrid(msg.at(i).toDouble() , msg.at(i+1).toDouble(), n_y, n_x);

    msgnet += QString::number(n_x)+ "," + QString::number(n_y);
    i=i+2;
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
  
  Console::Debug("ProcessOpenCPNGPSRoute():  " + msgnet); 
  WPT_UPDATE = msgnet.toStdString();
  //Console::Debug("ProcessOpenCPNGPSRoute():" + msg.join(";"));
}

std::vector<std::string> MoosConnect::split(const std::string& s, char seperator)
{
   std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word

    return output;
}

void MoosConnect::ProcessSET_MOOS_MANUAL_OVERRIDE(const QStringList &msg)
{
  if ((msg.at(1)=="true") != MOOS_MANUAL_OVERRIDE)
  {
     Console::Debug("ProcessSET_MOOS_MANUAL_OVERRIDE():" + msg.join(";"));
  MOOS_MANUAL_OVERRIDE = (msg.at(1)=="true");
   QString msgnet = QString("MOOS_MO") + _sep +
                     QString((MOOS_MANUAL_OVERRIDE?"true":"false"));                             
   SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);  
   QCoreApplication::processEvents();
  MMO_toUpdate = true;
  }
}

void MoosConnect::ProcessMOOS_MANUAL_OVERRIDE_REQUEST(const QStringList &msg)
{
  Console::Debug("ProcessMOOS_MANUAL_OVERRIDE_REQUEST():" + msg.join(";"));
  QString msgnet = QString("MOOS_MO") + _sep +
                   QString((MOOS_MANUAL_OVERRIDE?"true":"false"));                             
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

    string key   = msg.m_sKey;
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

    if (key == "RETURN" && !MR_toUpdate)
    {
      bool curval = (sdata=="true");
      if (MOOS_RETURN != curval || MR_first)
      {
        Console::Debug("MOOS_RETURN(): " + QString((curval?"true":"false")));
        QString msgnet = QString("MOOS_RET") + _sep +
                         QString((curval?"true":"false"));                             
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
        MOOS_RETURN = curval;
        MR_first = false;
      }
      QCoreApplication::processEvents();
    }

    if (key == "END" && !ME_toUpdate)
    {
      bool curval = (sdata=="true");
      if (MOOS_END != curval || ME_first)
      {
        Console::Debug("MOOS_END(): " + QString((curval?"true":"false")));
        QString msgnet = QString("MOOS_END") + _sep +
                         QString((curval?"true":"false"));                             
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
        MOOS_END = curval;
        ME_first = false;
      }
      QCoreApplication::processEvents();
    }

    if (key == "DEPLOY" && !MD_toUpdate)
    {
      bool curval = (sdata=="true");
      if (MOOS_DEPLOY != curval || MD_first)
      {
        Console::Debug("MOOS_DEPLOY(): " + QString((curval?"true":"false")));
        QString msgnet = QString("MOOS_DLY") + _sep +
                         QString((curval?"true":"false"));                             
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
        MOOS_DEPLOY = curval;
        MD_first = false;
      }
      QCoreApplication::processEvents();
    }

    if (key == "MOOS_MANUAL_OVERRIDE" && !MMO_toUpdate) 
    {
      bool curval = (sdata=="true");
      if (MOOS_MANUAL_OVERRIDE != curval || MMO_first)
      {
        Console::Debug("MOOS_MANUAL_OVERRIDE(): " + QString((curval?"true":"false")));
        QString msgnet = QString("MOOS_MO") + _sep +
                         QString((curval?"true":"false"));                             
        SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
        MOOS_MANUAL_OVERRIDE = curval;
        MMO_first = false;
      }
      QCoreApplication::processEvents();
    }

    if (key == "IVPHELM_ALLSTOP")
    {
      IVPHELM_ALLSTOP = sdata;
      Console::Debug("IVPHELM_ALLSTOP(): " + QString::fromStdString(IVPHELM_ALLSTOP));

      if (IVPHELM_ALLSTOP!="clear")
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

    if (key == "REGION_INFO")
    {
      string a = tokStringParse(sdata, "lat_datum", ',', '=');
      string b = tokStringParse(sdata, "lon_datum", ',', '=');
      Console::Debug("REGION_INFO():  lat_datum=" + QString::fromStdString(a) + " / lon_datum=" + QString::fromStdString(b));
      lat_datum = stod(a);
      lon_datum = stod(b);
      //m_geodesy.Initialise(lat_datum, lon_datum);
      QCoreApplication::processEvents();
    }

    if (key== "NAV_HEADING")
    {
      QString msgnet = QString("HEADING") + _sep +
                       QString("0") + _sep +                     // Id (ownship == 0)
                       QString::number(ddata) + _sep + // Heading
                       QString::number(ddata) + _sep + // HeadingErr (exibido pelo Instruments)
                       QString("0");                             // Deviation Calculated
      SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
      QCoreApplication::processEvents();
    }

    if (key == "NAV_YAW")
    {
      QString msgnet = QString("RRUDDER") + _sep +
                       QString::number(ddata); 
      SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
      QCoreApplication::processEvents();
    }

    if (key == "NAV_SPEED")
    {
      QString msgnet = QString("SPEED") + _sep +
                        QString("0") + _sep +                     // Id (ownship == 0)      
                        QString::number(ddata)  + _sep +  //STW
                        QString::number(ddata);           //SOG
      SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
      QCoreApplication::processEvents();
    }

    if (key == "NAV_LAT")
    {
      latlong->SetLat(ddata);
      latlong_updated = true;
    }
    if (key == "NAV_LONG")
    {
      latlong->SetLong(ddata);
      latlong_updated = true;
    }

    //LATLONG
    if (latlong_updated)
    {
       QString msgnet = QString("LATLONG") + _sep +
                       QString("0") + _sep +                     // Id (ownship == 0)
                       latlong->LatToNMEAString() + _sep +
                       latlong->LongToNMEAString(); 
      SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
      QCoreApplication::processEvents();
      latlong_updated = false;
    }
  }
  return (true);
}

bool MoosConnect::OnConnectToServer()
{
  Register("DESIRED_RUDDER", 0.0);
  Register("DESIRED_THRUST", 0.0);
  Register("MOOS_MANUAL_OVERRIDE", 0.0); 
  Register("NAV_HEADING", 0.0);
  Register("NAV_YAW", 0.0);
  Register("NAV_LAT", 0.0);
  Register("NAV_LONG", 0.0);
  Register("NAV_SPEED", 0.0);
  Register("REGION_INFO", 0.0);
  Register("WPT_UPDATE", 0.0);

  Register("RETURN", 0.0);
  Register("END", 0.0);
  Register("DEPLOY", 0.0);
  Register("MODE", 0.0);

  Register("IVPHELM_ALLSTOP", 0.0);
  return (true);
}

bool MoosConnect::Iterate()
{
  // std :: vector<unsigned char> X(100) ;
  if (!MMO_first)
    Notify("MOOS_MANUAL_OVERRIDE",(MOOS_MANUAL_OVERRIDE?"true":"false")); 

  Notify("DESIRED_RUDDER" , DESIRED_RUDDER) ;
  Notify("DESIRED_THRUST" , DESIRED_THRUST) ;
  Notify("WPT_UPDATE" , WPT_UPDATE) ;
  
  if (!MR_first)
    Notify("RETURN", (MOOS_RETURN?"true":"false"));

  if (!ME_first)  
    Notify("END", (MOOS_END?"true":"false"));

  if (!MD_first)  
    Notify("DEPLOY", (MOOS_DEPLOY?"true":"false"));

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
  if ((msg.at(1)=="true") != MOOS_DEPLOY)
    Console::Debug("MOOS_DEPLOY():" + msg.join(";"));
  MOOS_DEPLOY = (msg.at(1)=="true");
  QString msgnet = QString("MOOS_DLY") + _sep +
                    QString((MOOS_DEPLOY?"true":"false"));                             
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);  
  MD_toUpdate = true;
  QCoreApplication::processEvents();

  if (!MOOS_DEPLOY)
  {
    MOOS_RETURN = false;
    MOOS_END = false;

    MR_toUpdate = true;
    ME_toUpdate = true;

    Console::Debug("ProcessSET_MOOS_MANUAL_OVERRIDE():MOOS_END(): " + QString((MOOS_END?"true":"false")));
    QString msgnete = QString("MOOS_END") + _sep +
                      QString((MOOS_END?"true":"false"));                             
    SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnete, true);
    QCoreApplication::processEvents();

    Console::Debug("ProcessSET_MOOS_MANUAL_OVERRIDE():MOOS_RETURN(): " + QString((MOOS_RETURN?"true":"false")));
    QString msgnetr = QString("MOOS_RET") + _sep +
                      QString((MOOS_RETURN?"true":"false"));                             
    SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnetr, true);
    QCoreApplication::processEvents();
  }
}

void MoosConnect::ProcessMOOS_DEPLOY_REQUEST(const QStringList &msg)
{
  Console::Debug("ProcessMOOS_DEPLOY_REQUEST():" + msg.join(";"));
  QString msgnet = QString("MOOS_DLY") + _sep +
                  QString((MOOS_DEPLOY?"true":"false"));                             
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  QCoreApplication::processEvents();
}

void MoosConnect::ProcessSET_MOOS_RETURN(const QStringList &msg)
{
  if ((msg.at(1)=="true") != MOOS_RETURN)
    Console::Debug("MOOS_RETURN():" + msg.join(";"));
  MOOS_RETURN = (msg.at(1)=="true");
  QString msgnet = QString("MOOS_RET") + _sep +
                    QString((MOOS_RETURN?"true":"false"));                             
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);  
  MR_toUpdate = true;
  QCoreApplication::processEvents();
}

void MoosConnect::ProcessMOOS_RETURN_REQUEST(const QStringList &msg)
{
  Console::Debug("ProcessMOOS_RETURN_REQUEST():" + msg.join(";"));
  QString msgnet = QString("MOOS_RET") + _sep +
                  QString((MOOS_RETURN?"true":"false"));                             
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  QCoreApplication::processEvents();
}

void MoosConnect::ProcessSET_MOOS_END(const QStringList &msg)
{
  if ((msg.at(1)=="true") != MOOS_END)
    Console::Debug("ProcessSET_MOOS_END():" + msg.join(";"));
  MOOS_END = (msg.at(1)=="true");
  QString msgnet = QString("MOOS_END") + _sep +
                    QString((MOOS_END?"true":"false"));                             
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);  
  ME_toUpdate = true;
  QCoreApplication::processEvents();
}

void MoosConnect::ProcessMOOS_END_REQUEST(const QStringList &msg)
{
  Console::Debug("ProcessMOOS_END_REQUEST():" + msg.join(";"));
  QString msgnet = QString("MOOS_END") + _sep +
                  QString((MOOS_END?"true":"false"));                             
  SIMPASS::getInstancePtr()->GetNetworkManager()->SendMessage(msgnet, true);
  QCoreApplication::processEvents();
}