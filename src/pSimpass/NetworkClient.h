#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QElapsedTimer>
#include <QString>
#include <QStringList>
#include <QHostAddress>
#include <iostream>
#include <fstream>

class QTcpSocket;
class QUdpSocket;
class QAbstractSocket;

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    NetworkClient( QObject* parent = 0 );
    NetworkClient( const QString& hostname, unsigned short tcpPort, unsigned short udpPort, QObject* parent = 0 );
    virtual ~NetworkClient();

    bool Connect( const QString& hostname, unsigned short tcpPort, unsigned short udpPort );
    void Disconnect();

    bool IsConnected();
    bool IsConnecting();

    void SendMessage( const QString& msg );
    void SendMessage( const QStringList& msg);

    void SendTCPMessage( const QString& msg );
    void SendUDPMessage( const QString& msg );

    void Subscribe( const QString& type );
    void Subscribe( const QStringList& types );
    void Unsubscribe( const QString& type );
    void Unsubscribe( const QStringList& types );

    void Flush();

signals:
    void MessageReceived( const QStringList& msg );
    void MessageReceivedTest( const QString& type, const std::vector<QString>& msg );
    void Connected();
    void Disconnected();
    void NetworkError();

protected slots:
    void ReceiveTCPData();
    void ReceiveUDPData();

protected:
    void Init();

    void ReceiveData( QAbstractSocket* socket, QString& recvbuff, QStringList& recvmsg );
    bool TCPConnect( const QString& host, unsigned short port );
    bool UDPConnect( const QString& host, unsigned short port );
    void SendMessage( const QString& msg, QAbstractSocket* socket );

    static void TogglePrintReceivedDataCommand( const QStringList& params, const QString& commandline );
    static void TogglePrintSentDataCommand( const QStringList&, const QString& );

private:
    QTcpSocket* _tcpSocket;
    QUdpSocket* _udpSocket;
    QElapsedTimer _timer;
    QString _tcpRecvbuff;
    QStringList _tcpRecvmsg;
    std::vector<QString> vaTcpRecvmsg;
    QString _udpRecvbuff;
    QStringList _udpRecvmsg;
    std::vector<QString> vaUdpRecvmsg;
    QHostAddress _host;
    unsigned short _udpPort;
    bool _udpConnected;

    bool _printReceivedData;
    bool _printSentData;

    static const char _separator;

//    std::ofstream receiveMessages;
//    std::ofstream sendMessages;
//    std::ofstream allMessages;
};

#endif // GEOSERVERCLIENT_H
