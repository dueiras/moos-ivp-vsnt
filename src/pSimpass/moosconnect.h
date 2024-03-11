#ifndef MOOSCONNECT_H
#define MOOSCONNECT_H

#include <QObject>
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "SIMPASS.h"
#include <QCoreApplication>
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"
#include "Console.h"

class MoosConnect : public CMOOSApp, public Singleton<MoosConnect>
{

private:
    CMOOSGeodesy m_geodesy;

public:
    MoosConnect();
    bool OnStartUp();
    bool OnConnectToServer();
    bool Iterate ();
    bool OnNewMail (MOOSMSG_LIST & Mail);

    //Simpass
    void ProcessEngine(const QStringList &msg);
    void ProcessLatLong(const QStringList &msg);
    void ProcessRudder(const QStringList &msg);
    void ProcessOpenCPNGPSRoute(const QStringList &msg);
    void ProcessSET_MOOS_MANUAL_OVERRIDE(const QStringList &msg);
    void ProcessMOOS_MANUAL_OVERRIDE_REQUEST(const QStringList &msg);

    void ProcessMOOS_SHIP_STATUS_REQUEST(const QStringList &msg);    

    void ProcessSET_MOOS_DEPLOY(const QStringList &msg);
    void ProcessMOOS_DEPLOY_REQUEST(const QStringList &msg); 

    void ProcessSET_MOOS_RETURN(const QStringList &msg);
    void ProcessMOOS_RETURN_REQUEST(const QStringList &msg); 

    void ProcessSET_MOOS_END(const QStringList &msg);
    void ProcessMOOS_END_REQUEST(const QStringList &msg); 

    std::vector<std::string> split(const std::string& s, char seperator);

    void ProcessHeading(const QStringList &msg);
    void ProcessRRudder(const QStringList &msg);
    void ProcessSpeed(const QStringList &msg);

};


#endif // MOOSCONNECT_H
