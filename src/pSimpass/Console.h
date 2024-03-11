#ifndef CONSOLE_H
#define CONSOLE_H

#include <QString>


class Console
{


public:
    Console();
    virtual ~Console();

    static void Print( const QString& text );

    static void Info( const QString& text );
    static void Warning( const QString& text );
    static void Error( const QString& text );
    static void Debug( const QString& text );

protected:
    enum OutputType
    {
        NORMAL = 0,
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    static void Print( OutputType type, const QString& text );


};

#endif // CONSOLE_H
