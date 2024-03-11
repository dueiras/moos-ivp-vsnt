#ifndef SIMPASS_H
#define SIMPASS_H

#include <QSettings>

#include "Singleton.h"
#include "NetworkManager.h"


class SIMPASS : public Singleton<SIMPASS>
{
public:
    SIMPASS();
    ~SIMPASS();

    NetworkManager* GetNetworkManager();
    QSettings* GetSettings();

    void OpenOtherIni(QString _file);


private:
    NetworkManager* _networkManager;
    QSettings* _settings;


};

#endif // SIMPASS_H

