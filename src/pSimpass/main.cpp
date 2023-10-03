#include <QCoreApplication>
#include <SIMPASS.h>
#include <moosconnect.h>

int main(int argc, char *argv[])
{
     QCoreApplication a(argc, argv);

     bool connected = SIMPASS::getInstancePtr()->GetNetworkManager()->Connect();

    MoosConnect App;
    App.Run("pSimpass", "alpha.moos");
   

     return a.exec();
    //return (0);
}
