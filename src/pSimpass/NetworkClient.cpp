#include "NetworkClient.h"

#include <QTcpSocket>
#include <QUdpSocket>
#include <QtEndian>

#include "Console.h"
#include "SIMPASS.h"



NetworkClient::NetworkClient( QObject* parent ) :
    QObject( parent ), _printReceivedData( false ),_printSentData( false ), _udpConnected( false )
{
    this->Init();    
//    receiveMessages.open("recv.txt");
//    sendMessages.open("send.txt");
//    allMessages.open("all.txt");
}

NetworkClient::NetworkClient( const QString& host, unsigned short tcpPort, unsigned short udpPort, QObject* parent ) :
    QObject( parent ), _printReceivedData( false ), _udpConnected( false )
{
    this->Init();
    this->Connect( host, tcpPort, udpPort );
//    receiveMessages.open("recv.txt");
//    sendMessages.open("send.txt");
//    allMessages.open("all.txt");
}

NetworkClient::~NetworkClient()
{
    this->Disconnect();
}

bool NetworkClient::Connect( const QString &host, unsigned short tcpPort, unsigned short udpPort )
{
    return this->UDPConnect( host, udpPort ) && this->TCPConnect( host, tcpPort );
}

bool NetworkClient::TCPConnect( const QString& host, unsigned short port )
{
    bool flag = false;
    while (!flag)
    {
        try
        {
            _tcpSocket->connectToHost( host, port );
            flag = true;
        }
        catch(std::exception& e)
        {
            //return false;
            flag = false;
        }
    }
    return true;
}

bool NetworkClient::UDPConnect( const QString& host, unsigned short port )
{
    bool flag = false;
    while (!flag)
    {
        _host = QHostAddress( host );
        _udpPort = port;

        Console::Info( "Establishing UDP channel with server (" + host + ")" );
        _udpSocket->bind( 0 );
        quint16 localPort = _udpSocket->localPort();
        _udpSocket->writeDatagram( QString::number( localPort ).toUtf8(), QHostAddress( host ), port );

        _udpSocket->waitForReadyRead( 10000 ); //Wait 10 seconds
        qint64 size = _udpSocket->pendingDatagramSize();

        if( size > 0 )
        {
            char* cRemotePort = new char[ size + 1 ];
            cRemotePort[ size ] = 0;
            _udpSocket->readDatagram( cRemotePort, size );
            QString sRemotePort( cRemotePort );
            delete cRemotePort;
            _udpPort = sRemotePort.toUShort();
            localPort = _udpSocket->localPort();
            Console::Info( "UDP channel established to server port " + QString::number( _udpPort ) );

            _udpConnected = true;

            flag = true;
            connect( _udpSocket, SIGNAL( readyRead() ), this, SLOT( ReceiveUDPData() ) );

        }
        else
        {
            //Console::Error( "  Fail to establish UDP channel with network server" );
            this->Disconnect();
            //return false;
            flag = false;
        }
    }
    return true;

}
void NetworkClient::Disconnect()
{
    _udpSocket->disconnectFromHost();
    _udpConnected = false;
    _tcpSocket->disconnectFromHost();

//    receiveMessages << "Disconnect NetworkClient" << std::endl;
//    sendMessages << "Disconnect NetworkClient" << std::endl;
//    allMessages << "Disconnect NetworkClient" << std::endl;

//    receiveMessages.close();
//    sendMessages.close();
//    allMessages.close();

}

bool NetworkClient::IsConnected()
{
    return ( _tcpSocket->state() == QTcpSocket::ConnectedState ) && _udpConnected;
}

bool NetworkClient::IsConnecting()
{
    return ( _tcpSocket->state() == QTcpSocket::ConnectingState || _tcpSocket->state() == QTcpSocket::HostLookupState );
}

void NetworkClient::SendMessage( const QString& msg )
{
    this->SendTCPMessage( msg );
}

void NetworkClient::SendTCPMessage( const QString& msg )
{
    this->SendMessage( msg, _tcpSocket );
}

void NetworkClient::SendUDPMessage( const QString& msg )
{
    this->SendMessage( msg, _udpSocket );
}

void NetworkClient::SendMessage( const QString &umsg, QAbstractSocket* socket  )
{
    if( !this->IsConnected() )
    {
        emit NetworkError();
        return;
    }

    // Message Format in TCP:
    // 0;<ticks>;<msg>\n

    QString header = _separator + QString::number( _timer.elapsed() ) + _separator;

    unsigned int size = header.size() + umsg.size() + sizeof( int ) + 2;

    QByteArray msg;
    msg.reserve( size );

//    unsigned int size2 = qToLittleEndian( size );

//    msg.append( reinterpret_cast< char* >( &size2 ), sizeof( size2 ) );
    msg.append( "0000" ); //TODO enivar tamanho quando o networkserver e clients estiverem funcionando corretamente

    msg.append( header );
    msg.append( umsg );

    int n = 0;
    do
    {
        int w;
        if( socket->socketType() == QAbstractSocket::TcpSocket )
        {
            msg.append( "\r\n" );
            w = socket->write( msg.data() + n, msg.size() - n );
        }
        else if( socket->socketType() == QAbstractSocket::UdpSocket )
        {
            QUdpSocket* udpSock = static_cast< QUdpSocket* >( socket );
            w = udpSock->writeDatagram( msg.data() + n, msg.size() - n, _host, _udpPort );
        }

        if( w < 0 )
        {
            emit NetworkError();
            return;
        }

        n += w;



    } while( n < msg.size() );

    QString consoleMsg;

    if( socket->socketType() == QAbstractSocket::TcpSocket )
        consoleMsg = "<TCP Send> ";
    else
        consoleMsg = "<UDP Send> ";

    consoleMsg.append( msg );

    if( _printSentData )
    {
        //Console::Print( consoleMsg );
        std::cout << consoleMsg.toStdString() << std::endl;
    }

    //Console::Info( /*"[Network] Sent: " + */consoleMsg );
}

void NetworkClient::SendMessage( const QStringList &msg )
{
    this->SendMessage( msg.join( QString( _separator ) ) );
}

void NetworkClient::Subscribe( const QString &type )
{
    QString msg = QString( "SUBS" ) + _separator + type;
    this->SendTCPMessage( msg );
    this->SendUDPMessage( msg );
}

void NetworkClient::Subscribe( const QStringList &types )
{
    this->Subscribe( types.join( QString( _separator ) ) );
}

void NetworkClient::Unsubscribe( const QString & )
{}

void NetworkClient::Unsubscribe( const QStringList & )
{}

void NetworkClient::ReceiveTCPData()
{
    if( !this->IsConnected() )
    {
        emit NetworkError();
        return;
    }

    int len = _tcpSocket->bytesAvailable();

    do {
        char c;

        if( !_tcpSocket->getChar( &c ) )
            return;

        if( c == '\n' )
        {
//            receiveMessages << "<TCP Receive> " << _tcpRecvbuff.toStdString() << std::endl;

//            allMessages << "<TCP Receive> " << _tcpRecvbuff.toStdString() << std::endl;

            _tcpRecvmsg = _tcpRecvbuff.trimmed().split( ';' );
            _tcpRecvmsg.erase( _tcpRecvmsg.begin(), _tcpRecvmsg.begin() + 2 );

            vaTcpRecvmsg.clear();

            foreach (QString vaItem, _tcpRecvmsg)
            {
                //
                vaTcpRecvmsg.push_back( vaItem );
                //
            }

            if( _printReceivedData )
            {
                QString consoleMsg;
                consoleMsg = "<TCP> ";

                consoleMsg.append( _tcpRecvbuff );

//                Console::Print( consoleMsg );
                std::cout << consoleMsg.toStdString() << std::endl;
            }

            _tcpRecvbuff.clear();

            emit MessageReceived( _tcpRecvmsg );
            emit MessageReceivedTest( "TCP", vaTcpRecvmsg );
        }
        else
            _tcpRecvbuff.append( c );

    } while ( --len > 0 );
}

void NetworkClient::ReceiveUDPData()
{
//    do
//    {
        qint64 len = _udpSocket->pendingDatagramSize();

        if( len <= 0 )
            return;

        QByteArray data;
        data.resize( len );

        QHostAddress addr;
        quint16 port;

        _udpSocket->readDatagram( data.data(), len, &addr, &port );


        if( addr == _host )
        {
            //unsigned int sizeField = *( reinterpret_cast< unsigned int* >( data.data() ) );
            data.remove( 0, 5 );

//            Console::Debug( "Size Field: " + QString::number( sizeField ) );

            QString sData( data );

//            receiveMessages << "<UDP Receive> " << sData.toStdString() << "\n" << std::endl;;

//            allMessages << "<UDP Receive> " << sData.toStdString() << "\n" << std::endl;;

            _udpRecvmsg = sData.trimmed().split( ';' );
            //FIXME fazer a verificacao de tamanho com o cabecalho da msg
            _udpRecvmsg.erase( _udpRecvmsg.begin(), _udpRecvmsg.begin() + 1 );

            vaUdpRecvmsg.clear();

            std::copy(_udpRecvmsg.begin(), _udpRecvmsg.end(), std::back_inserter(vaUdpRecvmsg));

            emit MessageReceived( _udpRecvmsg );
            emit MessageReceivedTest( "UDP", vaUdpRecvmsg );
        }
//    }while( _udpSocket->hasPendingDatagrams() );


//    this->ReceiveData( _udpSocket, _udpRecvbuff, _udpRecvmsg );
}

void NetworkClient::Init()
{
    _tcpSocket = new QTcpSocket(this);
    _udpSocket = new QUdpSocket(this);
    _timer.start();

    connect( _tcpSocket, SIGNAL( connected() ), this, SIGNAL( Connected() ) );
    connect( _tcpSocket, SIGNAL( disconnected() ), this, SIGNAL( Disconnected() ) );
    connect( _tcpSocket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SIGNAL( NetworkError() ) );
    connect( _tcpSocket, SIGNAL( readyRead() ), this, SLOT( ReceiveTCPData() ) );

//    connect( _udpSocket, SIGNAL( connected() ), this, SIGNAL( Connected() ) );
    connect( _udpSocket, SIGNAL( disconnected() ), this, SIGNAL( Disconnected() ) );
    connect( _udpSocket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SIGNAL( NetworkError() ) );
}

void NetworkClient::Flush()
{
    _tcpSocket->flush();
    _udpSocket->flush();
}

const char NetworkClient::_separator = ';';

void NetworkClient::TogglePrintReceivedDataCommand( const QStringList&, const QString& )
{
    NetworkClient* nc = SIMPASS::getInstancePtr()->GetNetworkManager()->GetNetworkClient();
    nc->_printReceivedData = !nc->_printReceivedData;

    QString onoff;

    if( nc->_printReceivedData )
        onoff = "ON";
    else
        onoff = "OFF";

    Console::Print( "Printing network received data is " + onoff );
}

void NetworkClient::TogglePrintSentDataCommand( const QStringList&, const QString& )
{
    NetworkClient* nc = SIMPASS::getInstancePtr()->GetNetworkManager()->GetNetworkClient();
    nc->_printSentData = !nc->_printSentData;

    QString onoff;

    if( nc->_printSentData )
        onoff = "ON";
    else
        onoff = "OFF";

    Console::Print( "Printing network sent data is " + onoff );
}
