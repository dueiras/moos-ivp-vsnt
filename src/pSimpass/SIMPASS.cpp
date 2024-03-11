#include "SIMPASS.h"
#include <QFileInfo>

SIMPASS::SIMPASS() : _networkManager(NULL), _settings( NULL )
{
    _settings = new QSettings( "pSimpass.ini", QSettings::IniFormat );

}

void SIMPASS::OpenOtherIni(QString _file)
{
    if (QFileInfo(_file).exists())
    {
        _settings->deleteLater();
        _settings = new QSettings( _file, QSettings::IniFormat );
    }
}

SIMPASS::~SIMPASS()
{
    _settings->deleteLater();


    if( _networkManager != NULL )
        _networkManager->deleteLater();

}


NetworkManager* SIMPASS::GetNetworkManager()
{
    if(_networkManager == NULL){
        _networkManager = new NetworkManager();
    }
    return _networkManager;
}


QSettings* SIMPASS::GetSettings()
{
    return _settings;
}



