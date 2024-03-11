#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "NetworkClient.h"

#include <QTime>

#include <iostream>       // std::cerr
#include <stdexcept>      // std::out_of_range
#include <vector>         // std::vector

class NetworkRequestManager;

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    NetworkManager( QObject* parent = 0 );
    NetworkManager( const QString& hostname, unsigned short tcpPort, unsigned short udpPort, QObject* parent = 0 );
    virtual ~NetworkManager();

    bool Connect( const QString& hostname, unsigned short tcpPort, unsigned short udpPort );
    bool Connect();
    void Disconnect();
    void Flush();

    bool IsConnected();
    bool IsConnecting();

    void EnableSend( bool b );
    bool IsSendEnabled();
    void EnableReceive( bool b );
    bool IsReceiveEnabled();

    void SendMessage( const QString& msg, bool force = false );
    void SendUDPMessage( const QString& msg, bool force = false );

    void SendAppId( const QString& name);

    bool IsServerShipProcessLocal();

signals:
    void Connected();
    void Disconnected();
    void NetworkError();
    void UnhandledMessage( const QStringList& msg );

	//MessageError
    void ReceiveMessageError( const QString& paMessage );

protected slots:
    void ClientConnected();
    void ProcessMessage( const QStringList& msg );
    void ProcessMessageTest( const QString& type, const std::vector<QString>& msg );

protected:
    void PrintReceivedMessage( const QStringList& msg );
    void PrintReceivedMessageTest( const std::vector<QString>& msg );
    void PrintReceivedMessageErrorTest( const std::vector<QString>& msg, const QString& error = "" );

    void Init();
    void SubscribeMessages();
    void UnsubscribeMessages();
    

    // Ship
    void ProcessOwnShipMachineStateCommand( const std::vector<QString>& msg );
    void ProcessOwnShipMachinePercentageCommand( const std::vector<QString>& msg );
    void ProcessOwnShipRudderCommand( const std::vector<QString>& msg );
    void ProcessOpenCPNGPSRouteCommand( const std::vector<QString>& msg );
    void ProcessShipPosition( const std::vector<QString>& msg );
    void ProcessShipHeading(const std::vector<QString> &msg);
    void ProcessOwnShipRudderAngle(const std::vector<QString> &msg);
    void ProcessShipSpeed(const std::vector<QString> &msg);



    //
    void ProcessMOOS_MANUAL_OVERRIDE_REQUESTCommand( const std::vector<QString>& msg );
    void ProcessSET_MOOS_MANUAL_OVERRIDECommand( const std::vector<QString>& msg );

    //
    void ProcessMOOS_SHIP_STATUS_REQUESTCommand( const std::vector<QString>& msg );

    //
    void ProcessMOOS_DEPLOY_REQUESTCommand( const std::vector<QString>& msg );
    void ProcessSET_MOOS_DEPLOYCommand( const std::vector<QString>& msg );

        //
    void ProcessMOOS_RETURN_REQUESTCommand( const std::vector<QString>& msg );
    void ProcessSET_MOOS_RETURNCommand( const std::vector<QString>& msg );

        //
    void ProcessMOOS_END_REQUESTCommand( const std::vector<QString>& msg );
    void ProcessSET_MOOS_ENDCommand( const std::vector<QString>& msg );

    friend class NetworkClient;
    NetworkClient* GetNetworkClient();

private:
    NetworkClient* _netclient;
    NetworkRequestManager* _networkRequestManager;

    bool _sendEnabled;
    bool _recvEnabled;

    bool _printSentMessages;
    bool _printReceivedMessages;    
    bool _printReceivedMessagesError;
    bool _printReceivedMessagesFilterEnabled;
    QSet< QString > _printReceivedMessagesFilter;


    static const char _sep = ';';

    bool _isMaster;

private:
    QStringList ConvertStdVectorInQStringList( std::vector<QString>&msg );
    QStringList ProcessNewMessage( const std::vector<QString>&msg, int numMim );
    void ValidFloat( const QString& msg, QString &error, QString &msgError, int &numCurrent, int &numCurrentErro, int numMaxError = 1 );
    void ValidInt( const QString& msg, QString &error, QString &msgError, int &numCurrent, int &numCurrentErro, int numMaxError = 1 );
    void ValidChar( const QString& msg, QString &error, QString &msgError, int &numCurrent, int &numCurrentErro, int numMaxError = 1 );
    void ValidBool( const QString& msg, QString &error, QString &msgError, int &numCurrent, int &numCurrentErro, int numMaxError = 1 );
    void ValidString( const QString& msg, QString &error, QString &msgError, int &numCurrent, int &numCurrentErro, int numMaxError = 1 );
};

#endif // GEOSERVERCLIENT_H
