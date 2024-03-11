#include "Console.h"
#include "QDebug"
#include "QtGlobal"


Console::Console()
{

}

Console::~Console()
{
}

void Console::Print( const QString& text )
{
    Print( NORMAL, text );
}

void Console::Info( const QString& text )
{
    Print( INFO, text );
}

void Console::Warning( const QString& text )
{
    Print( WARNING, text );
}

void Console::Error( const QString& text )
{
    Print( ERROR, text );
}

void Console::Debug( const QString& text )
{
    Print( DEBUG, text );
}

void Console::Print( OutputType type, const QString& text )
{
    const std::string red = "\033[31m";
    const std::string green = "\033[32m";
    const std::string yellow = "\033[33m";
    const std::string cyan = "\033[36m";
    const std::string magenta = "\033[35m";
    const std::string white = "\033[97m";
    const std::string gray = "\033[37m";
    const std::string reset = "\033[0m";

    QString msgType;
    std::string color;
    switch( type )
    {
        case NORMAL:
            msgType = "" + text;
            qDebug().nospace() << "\033[0m"  << qPrintable(msgType.trimmed()) << "\033[0m";
            break;
        case INFO:
            msgType = "[INFO] " + text;
            qDebug().nospace() << "\033[0m"  << qPrintable(msgType.trimmed()) << "\033[0m";
            break;
        case WARNING:
            msgType = "[WARNING] " + text;
            qDebug().nospace() << "\033[33m" << qPrintable(msgType.trimmed()) << "\033[0m";
            break;
        case DEBUG:
            msgType = "[DEBUG] " + text;
            qDebug().nospace() << "\033[32m" << qPrintable(msgType.trimmed()) << "\033[0m";
            break;
        case ERROR:
            msgType = "[ERROR] " + text;
            qDebug().nospace() << "\033[31m" << qPrintable(msgType.trimmed()) << "\033[0m";
            break;
    }
 }

