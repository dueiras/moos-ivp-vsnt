#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <QPointF>
#include "LatLong.h"
#include <QVector2D>

#define DEGREESTORADIANS(x) x*3.141594f/180.0f
#define RADIANSTODEGREES(x) x*180.0f/3.141594f
#define EARTHRADIUS 6371.0f
#define MERIDIAN(y) (-0.00006*pow(y,3)) + (0.007*pow(y,2)) - (0.023*y) + 1842.0
#define PARALLEL(y) (0.001*pow(y,3)) - (0.326*pow(y,2)) + (0.677*y) + 1854.0

class MathUtils{
public:
    static float KmToNm(float distance);
    static float NmToKm(float distance);

    static float YdToNm(float distance);
    static float NmToYd(float distance);

    static float FtToNm(float distance);
    static float NmToft(float distance);

    static void CalculateIntersectionNN( const QPointF& p1, const QVector2D& d1, const QPointF& p2, const QVector2D& d2, qreal& t1, qreal& t2 );

    static QVector2D CalculateElipseTangentPoint( qreal a, qreal b, const QVector2D& line );

};

#endif // MATHUTILS_H
