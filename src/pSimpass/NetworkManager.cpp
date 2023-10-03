#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include "NetworkManager.h"
#include "SIMPASS.h"
#include "moosconnect.h"

#include "Console.h"

#include <QSettings>
#include <QTime>


NetworkManager::NetworkManager( QObject* parent ) :
    QObject( parent ), _printSentMessages( false ), _printReceivedMessages( false ), _printReceivedMessagesFilterEnabled( false ), _printReceivedMessagesError( false )
{
    this->Init();
}

NetworkManager::NetworkManager( const QString &host, unsigned short tcpPort, unsigned short udpPort, QObject* parent ) :
    QObject( parent ), _printSentMessages( false ), _printReceivedMessages( false ), _printReceivedMessagesError( false )
{
    this->Init();
    this->Connect( host, tcpPort, udpPort );
}

NetworkManager::~NetworkManager()
{
    this->Disconnect();
}

bool NetworkManager::Connect( const QString &host, unsigned short tcpPort, unsigned short udpPort )
{
    return _netclient->Connect( host, tcpPort, udpPort );
}

bool NetworkManager::Connect()
{
    QSettings* settings = SIMPASS::getInstancePtr()->GetSettings();

    QString host = settings->value( "simulationserver/ip" ).toString();
    int tcpPort = settings->value( "simulationserver/tcpPort" ).toInt();
    int udpPort = settings->value( "simulationserver/udpPort" ).toInt();

    Console::Info( "ini ( ip: " + host + " / tcp: " + settings->value( "simulationserver/tcpPort" ).toString() +
                   " udp: " + settings->value( "simulationserver/udpPort" ).toString() + ")" );

    return _netclient->Connect( host, tcpPort, udpPort );
}

void NetworkManager::Disconnect()
{
    _netclient->Disconnect();
}

void NetworkManager::Flush()
{
    _netclient->Flush();
}

bool NetworkManager::IsConnected()
{
    return _netclient->IsConnected();
}

bool NetworkManager::IsConnecting()
{
    return _netclient->IsConnecting();
}

void NetworkManager::EnableSend( bool b )
{
    _sendEnabled = b;
}

bool NetworkManager::IsSendEnabled()
{
    return _sendEnabled;
}

void NetworkManager::EnableReceive( bool b )
{
    _recvEnabled = b;
}

bool NetworkManager::IsReceiveEnabled()
{
    return _recvEnabled;
}

void NetworkManager::SendMessage( const QString &msg, bool force )
{
    if( _sendEnabled || force ){
        _netclient->SendMessage( msg );
    }
}


void NetworkManager::SendUDPMessage( const QString& msg, bool force )
{
    if( _sendEnabled || force ){
        _netclient->SendUDPMessage( msg );
    }
}

void NetworkManager::PrintReceivedMessage( const QStringList& msg )
{
    if( !_printReceivedMessages )
        return;

    if( !_printReceivedMessagesFilterEnabled || _printReceivedMessagesFilter.find( msg[ 0 ] ) != _printReceivedMessagesFilter.end() )
        Console::Info( "[Network] recv: " + msg.join( QString( _sep ) ) );
}

void NetworkManager::PrintReceivedMessageTest( const std::vector<QString>& msg )
{
    if( !_printReceivedMessages )
        return;

    if( !_printReceivedMessagesFilterEnabled || _printReceivedMessagesFilter.find( msg[ 0 ] ) != _printReceivedMessagesFilter.end() )
    {
        //
        QString* ss = new QString();

        ss->append("[Network] recv: ");

        for(size_t i = 0; i < msg.size(); ++i)
        {
            if(i != 0)
            {
                ss->append( _sep );
            }

            ss->append( msg[i] );
        }


        Console::Info( *ss );
        //
    }
}

void NetworkManager::PrintReceivedMessageErrorTest( const std::vector<QString>& msg, const QString& error )
{
    if( !_printReceivedMessagesError )
        return;

    if( !_printReceivedMessagesFilterEnabled || _printReceivedMessagesFilter.find( msg[ 0 ] ) != _printReceivedMessagesFilter.end() )
    {
        //
        QString* ss = new QString();

        ss->append("[Network] recv: ");

        for(size_t i = 0; i < msg.size(); ++i)
        {
            if(i != 0)
            {
                ss->append( _sep );
            }

            ss->append( msg[i] );
        }

        ss->append(". Message error, " + error);

        Console::Error( *ss );
        //
    }
}


QStringList NetworkManager::ConvertStdVectorInQStringList( std::vector<QString>&msg )
{
    //
    QStringList listMsg = QStringList::fromVector(QVector<QString>::fromStdVector(msg));

    std::vector<QString>::iterator it = msg.begin();



//    while (it != msg.end())
//    {
//        //
//        QString* va = (QString*)it._Ptr;

//        listMsg.push_back(*va);

//        it = msg.erase(it);
//        //
//    }

    return listMsg;
    //
}

QStringList NetworkManager::ProcessNewMessage( const std::vector<QString>&msg, int numMim )
{
    //
    if( msg.size() < numMim )
        throw "";

    std::vector<QString> va = msg;

    QStringList vaMsg = this->ConvertStdVectorInQStringList( va );

    vaMsg.removeFirst();

    return vaMsg;
    //
}

void NetworkManager::ProcessMessageTest( const QString& type, const std::vector<QString>&msg )
{
    try {
        //
        const QString& messageName = msg[ 0 ];

        //Special Messages
        if( messageName == "CMACHINE_PERCENTAGE" )
            this->ProcessOwnShipMachinePercentageCommand( msg );
        // else if( messageName == "CMACHINE_STATE" )
        //     this->ProcessOwnShipMachineStateCommand( msg );            
        else if( messageName == "CRUDDER" )
            this->ProcessOwnShipRudderCommand( msg );            
        else if( messageName == "OPENCPN_GPS_ROUTE" )
            this->ProcessOpenCPNGPSRouteCommand( msg );  
        else if( messageName == "SET_MOOS_MO" )
            this->ProcessSET_MOOS_MANUAL_OVERRIDECommand( msg ); 
        else if( messageName == "MOOS_MO_REQUEST" )            
            this->ProcessMOOS_MANUAL_OVERRIDE_REQUESTCommand( msg ); 
        else if( messageName == "MOOS_SHIP_STATUS_REQUEST" )            
            this->ProcessMOOS_SHIP_STATUS_REQUESTCommand( msg ); 
        else if( messageName == "SET_MOOS_DLY" )
            this->ProcessSET_MOOS_DEPLOYCommand( msg ); 
        else if( messageName == "MOOS_DLY_REQUEST" )            
            this->ProcessMOOS_DEPLOY_REQUESTCommand( msg ); 
        else if( messageName == "SET_MOOS_RET" )
            this->ProcessSET_MOOS_RETURNCommand( msg ); 
        else if( messageName == "MOOS_RET_REQUEST" )            
            this->ProcessMOOS_RETURN_REQUESTCommand( msg ); 
        else if( messageName == "SET_MOOS_END" )
            this->ProcessSET_MOOS_ENDCommand( msg ); 
        else if( messageName == "MOOS_END_REQUEST" )            
            this->ProcessMOOS_END_REQUESTCommand( msg );                         

        if( !_recvEnabled )
            return;



        if( _printReceivedMessages )
            this->PrintReceivedMessageTest( msg );
        //
    } catch ( const std::exception& oor ) {
        //

        char buffer[2000];

        QString vaBuffer;

        int vaMsgSize = msg.size();

        for(int i=0; i < vaMsgSize; i++)
        {
            //
            vaBuffer.append(msg[i]);

            if( i+1 < vaMsgSize )
            {
                //
                vaBuffer.append(";");
                //
            }
            //
        }

        sprintf (buffer, "%s -> This message '%s' of the %s type contains errors, for more details enable NetworkTogglePrintReceivedMessages in console.",oor.what(), vaBuffer.toUtf8().data(),(type.toUtf8()).data() );
        //Console::Debug(QString("****DEBUG ERROR->"));
        QString vaString =  QString::fromUtf8(buffer);

        if( _printReceivedMessagesError )
        {

            this->PrintReceivedMessageErrorTest( msg, tr(oor.what()) );
        }

        emit ReceiveMessageError( vaString );
        //
    }


}

void NetworkManager::ProcessMOOS_MANUAL_OVERRIDE_REQUESTCommand( const std::vector<QString>& msg )
{
   //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
   Console::Info( "ProcessMOOS_MANUAL_OVERRIDE_REQUESTCommand()" );
   std::vector<QString> vaMsg = msg;
   MoosConnect::getInstancePtr()->ProcessMOOS_MANUAL_OVERRIDE_REQUEST(this->ConvertStdVectorInQStringList( vaMsg ));
}

//ProcessMOOS_SHIP_STATUS_REQUESTCommand
void NetworkManager::ProcessMOOS_SHIP_STATUS_REQUESTCommand( const std::vector<QString>& msg )
{
   //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
   Console::Info( "ProcessMOOS_SHIP_STATUS_REQUESTCommand()" );
   std::vector<QString> vaMsg = msg;
   MoosConnect::getInstancePtr()->ProcessMOOS_SHIP_STATUS_REQUEST(this->ConvertStdVectorInQStringList( vaMsg ));
}

void NetworkManager::ProcessSET_MOOS_MANUAL_OVERRIDECommand( const std::vector<QString>& msg )
{
   //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
   std::vector<QString> vaMsg = msg;
   MoosConnect::getInstancePtr()->ProcessSET_MOOS_MANUAL_OVERRIDE(this->ConvertStdVectorInQStringList( vaMsg ));
}

    void NetworkManager::ProcessMOOS_DEPLOY_REQUESTCommand( const std::vector<QString>& msg )
    {
        //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
        Console::Info( "ProcessMOOS_DEPLOY_REQUESTCommand()" );
        std::vector<QString> vaMsg = msg;
        MoosConnect::getInstancePtr()->ProcessMOOS_DEPLOY_REQUEST(this->ConvertStdVectorInQStringList( vaMsg ));
    }

    void NetworkManager::ProcessSET_MOOS_DEPLOYCommand( const std::vector<QString>& msg )
    {
        //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
        std::vector<QString> vaMsg = msg;
        MoosConnect::getInstancePtr()->ProcessSET_MOOS_DEPLOY(this->ConvertStdVectorInQStringList( vaMsg ));
    }

    //
    void NetworkManager::ProcessMOOS_RETURN_REQUESTCommand( const std::vector<QString>& msg )
    {
        //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
        Console::Info( "ProcessMOOS_RETURN_REQUESTCommand()" );
        std::vector<QString> vaMsg = msg;
        MoosConnect::getInstancePtr()->ProcessMOOS_RETURN_REQUEST(this->ConvertStdVectorInQStringList( vaMsg ));
    }

    void NetworkManager::ProcessSET_MOOS_RETURNCommand( const std::vector<QString>& msg )
    {
        //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
        std::vector<QString> vaMsg = msg;
        MoosConnect::getInstancePtr()->ProcessSET_MOOS_RETURN(this->ConvertStdVectorInQStringList( vaMsg ));
    }

    //
    void NetworkManager::ProcessMOOS_END_REQUESTCommand( const std::vector<QString>& msg )
    {
        //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
        Console::Info( "ProcessMOOS_END_REQUESTCommand()" );
        std::vector<QString> vaMsg = msg;
        MoosConnect::getInstancePtr()->ProcessMOOS_END_REQUEST(this->ConvertStdVectorInQStringList( vaMsg ));
    }

    void NetworkManager::ProcessSET_MOOS_ENDCommand( const std::vector<QString>& msg )
    {
        //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
        std::vector<QString> vaMsg = msg;
        MoosConnect::getInstancePtr()->ProcessSET_MOOS_END(this->ConvertStdVectorInQStringList( vaMsg ));
    }

void NetworkManager::ProcessMessage( const QStringList &msg )
{
    if( _printReceivedMessages )
        this->PrintReceivedMessage( msg );

    if( !_recvEnabled )
        return;

    const QString& messageName = msg[ 0 ];
}

void NetworkManager::Init()
{
    _recvEnabled = false;
    _sendEnabled = false;

    _isMaster = true;

    _netclient = new NetworkClient( this );

    connect( _netclient, SIGNAL( Connected() ), this, SLOT( ClientConnected() ) );
    connect( _netclient, SIGNAL( Disconnected() ), this, SIGNAL( Disconnected() ) );
    connect( _netclient, SIGNAL( NetworkError() ), this, SIGNAL( NetworkError() ) );
    connect( _netclient, SIGNAL( MessageReceived( const QStringList& ) ), this, SLOT( ProcessMessage( const QStringList& ) ) );
    connect( _netclient, SIGNAL( MessageReceivedTest( const QString&, const std::vector<QString>&)) , this, SLOT( ProcessMessageTest( const QString&, const std::vector<QString>&) ) );

    //connect (_netclient, SIGNAL(ServerShipProcessChanged(QString)), this, SLOT (ChangeServerShipProcess(QString)));
}

void NetworkManager::SendAppId( const QString& name)
{
    QString msg = QString( "SEND_APP_ID" ) + _sep + name;
    this->SendMessage( msg , true);
}

void NetworkManager::SubscribeMessages()
{
    static QStringList types = QStringList() << "CMACHINE_STATE"
                                             << "CMACHINE_PERCENTAGE"
                                             << "CRUDDER" 
                                             << "OPENCPN_GPS_ROUTE"
                                             << "SET_MOOS_MO"
                                             << "MOOS_MO_REQUEST"
                                             << "SET_MOOS_DLY"
                                             << "MOOS_DLY_REQUEST"
                                             << "SET_MOOS_RET"
                                             << "MOOS_RET_REQUEST"
                                             << "SET_MOOS_END"
                                             << "MOOS_END_REQUEST"
                                             << "MOOS_SHIP_STATUS_REQUEST";

    _netclient->Subscribe( types );
}

void NetworkManager::UnsubscribeMessages()
{
}




NetworkClient* NetworkManager::GetNetworkClient()
{
    return _netclient;
}



void NetworkManager::ValidFloat( const QString& msg, QString &error, QString &msgError, int &numCurrent, int &numCurrentErro, int numMaxError )
{
    //
    QRegExp re("^[0-9.,-E]+$");

    if( re.indexIn( msg ) == -1 )
    {
        //
        msgError.append( error );

        numCurrentErro++;
        //
    }

    numCurrent++;

    if( numMaxError == numCurrent && numCurrentErro >= 1 )
    {
        //
        throw msgError.toUtf8() + "paramter(s) is incorrect(s).";
        //
    }
    //
}

void NetworkManager::ValidInt( const QString& msg, QString &error, QString &msgError, int &numCurrent, int &numCurrentErro, int numMaxError )
{
    //
    QRegExp re("^[0-9-]+$");

    if( re.indexIn( msg ) == -1 )
    {
        //
        msgError.append( error );

        numCurrentErro++;
        //
    }

    numCurrent++;

    if( numMaxError == numCurrent && numCurrentErro >= 1 )
    {
        //
        throw msgError.toUtf8() + "paramter(s) is incorrect(s)." ;
        //
    }
    //
}

void NetworkManager::ValidChar( const QString& msg, QString &error, QString &msgError, int &numCurrent, int &numCurrentErro, int numMaxError )
{
    //
    QRegExp re("^[a-zA-Z]+$");

    if( re.indexIn( msg ) == -1 )
    {
        //
        msgError.append( error );

        numCurrentErro++;
        //
    }

    numCurrent++;

    if( numMaxError == numCurrent && numCurrentErro >= 1 )
    {
        //
        throw msgError.toUtf8() + "paramter(s) is incorrect(s)." ;
        //
    }
    //
}

void NetworkManager::ValidBool( const QString& msg, QString &error, QString &msgError, int &numCurrent, int &numCurrentErro, int numMaxError )
{
    //
    QRegExp re("^[false|true|FALSE|TRUE]{1,}$");

    if( re.indexIn( msg ) == -1 )
    {
        //
        msgError.append( error );

        numCurrentErro++;
        //
    }

    numCurrent++;

    if( numMaxError == numCurrent && numCurrentErro >= 1 )
    {
        //
        throw  msgError.toUtf8() + "paramter(s) is incorrect(s)." ;
        //
    }
    //
}

void NetworkManager::ValidString( const QString& msg, QString &error, QString &msgError, int &numCurrent, int &numCurrentErro, int numMaxError )
{
    //
    QRegExp re("^[\\w+\\s+-_\\/()*&%$#@!]+$");

    if( re.indexIn( msg ) == -1 )
    {
        //
        msgError.append( error );

        numCurrentErro++;
        //
    }

    numCurrent++;

    if( numMaxError == numCurrent && numCurrentErro >= 1 )
    {
        //
        throw  msgError.toUtf8() + "paramter(s) is incorrect(s)." ;
        //
    }
    //
}

bool NetworkManager::IsServerShipProcessLocal()
{
    return _isMaster;
}

void NetworkManager::ClientConnected()
{
    this->SendAppId("pSimpass");
    this->SubscribeMessages();
    //this->SendDbIsConnected();
    //emit Connected();
}

void NetworkManager::ProcessOwnShipMachineStateCommand( const std::vector<QString>& msg )
{
   //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
   std::vector<QString> vaMsg = msg;
   MoosConnect::getInstancePtr()->ProcessEngine(this->ConvertStdVectorInQStringList( vaMsg ));
}

void NetworkManager::ProcessOwnShipRudderCommand( const std::vector<QString>& msg )
{
   //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
   std::vector<QString> vaMsg = msg;
   MoosConnect::getInstancePtr()->ProcessRudder(this->ConvertStdVectorInQStringList( vaMsg ));
}

void NetworkManager::ProcessOpenCPNGPSRouteCommand( const std::vector<QString>& msg )
{
   //emit OwnShipMachineStateCommandChanged( ProcessNewMessage(msg,3) );
   std::vector<QString> vaMsg = msg;
   MoosConnect::getInstancePtr()->ProcessOpenCPNGPSRoute(this->ConvertStdVectorInQStringList( vaMsg ));
}

void NetworkManager::ProcessOwnShipMachinePercentageCommand( const std::vector<QString>& msg )
{
    // int vaCurrent = 0;

    // int vaCurrentErro = 0;

    // QString vaMsgErro("");

    // int vaMax = 2;

    // QString vaMsgFirst = msg.at( 1 );

    // ValidInt(vaMsgFirst,vaMsgErro/*tr("first, ")*/,vaMsgErro,vaCurrent,vaCurrentErro,vaMax);

    // QString vaMsgSecond = msg.at( 2 );

    // ValidInt(vaMsgSecond,vaMsgErro/*tr("second, ")*/,vaMsgErro,vaCurrent,vaCurrentErro,vaMax);

    // std::vector<QString> vaMsg = msg;

    // vaMsg.erase(vaMsg.begin());

    // emit OwnShipMachinePercentageCommandChanged( vaMsg );
    std::vector<QString> vaMsg = msg;
    MoosConnect::getInstancePtr()->ProcessEngine(this->ConvertStdVectorInQStringList( vaMsg ));
}