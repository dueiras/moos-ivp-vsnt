#include "MathUtils.h"
#include <QtCore/qmath.h>

float MathUtils::KmToNm(float distance)
{
    return (distance * 0.539956804);
}

float MathUtils::NmToKm(float distance)
{
    return (distance * 1.852);
}

float MathUtils::YdToNm(float distance)
{
    return (distance * 0.0004937);
}

float MathUtils::NmToYd(float distance)
{
    return (distance * 2025.37183);
}

float MathUtils::FtToNm(float distance)
{
    return (distance * 0.000164579);
}

float MathUtils::NmToft(float distance)
{
    return (distance * 6076.12162547110347);
}


void MathUtils::CalculateIntersectionNN( const QPointF& p1, const QVector2D& d1, const QPointF& p2, const QVector2D& d2, qreal& t1, qreal& t2 )
{
    QVector2D d1n = d1.normalized();
    QVector2D d2n = d2.normalized();

    QPointF diff( p1 - p2 );

    t1 = ( diff.y() * d2n.x() - diff.x() * d2n.y() ) / ( d1n.x() * d2n.y() - d2n.x() * d1n.y() );

    if( d2n.x() != 0 )
        t2 = ( diff.x() + t1 * d1n.x() ) / d2n.x();
    else
        t2 = ( diff.y() + t1 * d1n.y() ) / d2n.y();
}

QVector2D MathUtils::CalculateElipseTangentPoint( qreal a, qreal b, const QVector2D& line )
{
    QVector2D d = line.normalized();

    qreal adivbsqr = ( a / b ) * ( a / b );

    qreal y = 0.0;
    qreal x = -1.0 * a;

    if( d.x() != 0 )
    {
        qreal k = -1.0 * d.y() / d.x();
        y = qSqrt( ( b * b ) / ( k * k * adivbsqr + 1 ) );

        x = y * k * adivbsqr;
    }

    return QVector2D( x, y );
}

