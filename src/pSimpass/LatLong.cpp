#include "LatLong.h"

#include <QStringList>
#include <QtCore/qmath.h>
#include "MathUtils.h"

QString LatLong::CoordToString( qreal value, bool latitude )
{
    int deg = ::abs( ( int )value );
    float minute = ( ::abs( value ) - ( float )deg ) * 60.f;
    int minuteDec = ( minute - ( int )minute ) * 10000;

    char ns;
    int numZerosDeg;

    if( latitude )
    {
        ns = ( value >= 0 )?'N':'S';
        numZerosDeg = 2;
    }
    else
    {
        ns = ( value >= 0 )?'E':'W';
        numZerosDeg = 3;
    }

    QString Str = QString( "%1%2%3.%4'%5" ).arg( deg, numZerosDeg, 10, QChar('0') ).arg( QChar( 0x00b0 ) ).arg( ( int )minute, 2, 10, QChar( '0' ) ).arg( minuteDec, 4, 10, QChar( '0' ) ).arg( ns );

    return Str;
}

QString LatLong::CoordToNMEAString( float value, bool latitude )
{
    char hem;
    int degSize;

    if( latitude )
    {
        degSize = 2;
        hem = ( value  < 0 ? 'S' : 'N' );
    }
    else
    {
        degSize = 3;
        hem = ( value  < 0 ? 'W' : 'E' );
    }

    value  = ::abs( value );

    int deg = ( int )value;
    float min = ( value - deg ) * 60;

    int min_int = ( int )min;
    int min_frac = ( min - min_int ) * 10000;

    return QString( "%1%2.%3;%4" ).arg( deg, degSize, 10, QChar( '0' ) ).arg( min_int, 2, 10, QChar('0') ).arg( min_frac, 4, 10, QChar('0') ).arg( hem );
}



float LatLong::NMEAStringToCoord( const QString& str )
{
    QStringList strList = str.split( QChar( ';' ) );
    return NMEAStringToCoord( strList[ 0 ], strList[ 1 ] );
}

float LatLong::NMEAStringToCoord( const QString& coord, const QString& hemisphere )
{
    float sign = ( hemisphere == "S" || hemisphere == "W" )?-1.0:1.0;

    int degSize = coord.indexOf( '.' ) - 2;
    int deg = coord.mid( 0, degSize ).toInt();

    float min = coord.mid( degSize ).toFloat();

    float value = sign * ( ( float )deg + min / 60.0 );

    return value;
}

LatLong LatLong::FromNMEAString( const QString& latitude, const QString& latHem, const QString& longitude, const QString& longHem )
{
    float lat = NMEAStringToCoord( latitude, latHem );
    float longi = NMEAStringToCoord( longitude, longHem );

    return LatLong( lat, longi );
}

LatLong LatLong::FromNMEAString( const QString& latitude, const QString& longitude )
{
    float lat = NMEAStringToCoord( latitude );
    float longi = NMEAStringToCoord( longitude );

    return LatLong( lat, longi );
}

float LatLong::DistanceInKm( const LatLong& start, const LatLong& end, const float latReference )
{

    //
    float medianLat = latReference;

    float p = PARALLEL( medianLat );

    float a = ( (end.x() - start.x()) * 60 ) * p;

    float m = MERIDIAN( medianLat );

    float b = ( ( end.y() - start.y() ) * 60 ) * m;

    //Meters
    float d = sqrt( pow( a, 2 ) + pow( b, 2 ) )/1000;
    //

    return d;
}



float LatLong::DistanceInNm( const LatLong& start, const LatLong& end, const float latReference )
{
    return DistanceInKm( start, end, latReference ) / 1.852;
}

float LatLong::DistanceInYd( const LatLong& start, const LatLong& end, const float latReference )
{
    return DistanceInKm( start, end, latReference ) * 1093.6;
}

float LatLong::DistanceInFt( const LatLong& start, const LatLong& end, const float latReference )
{
    return DistanceInKm( start, end, latReference ) * 3280.8432102975721136;
}

float LatLong::Bearing( const LatLong& start, const LatLong& end )
{
    float startLatRad = DEGREESTORADIANS( start.y() );
    float endLatRad = DEGREESTORADIANS( end.y() );
    float distanceLongitude = DEGREESTORADIANS((end.x() - start.x()));

    float angRad = qAtan2( qSin( distanceLongitude ) * qCos( endLatRad ), qCos( startLatRad ) * qSin( endLatRad ) - qSin( startLatRad ) * qCos( endLatRad ) * qCos( distanceLongitude ) );
    float angDeg = RADIANSTODEGREES( angRad );

    if( angDeg < 0.f )
        angDeg = 360.f + angDeg;

    return angDeg;
}

LatLong LatLong::AddDistanceInNm( const LatLong& start, float distance, float bearing )
{
    float bearingRad = DEGREESTORADIANS( bearing );

    float dX = distance * qSin( bearingRad );
    float dY = distance * qCos( bearingRad );

    float dLong = dX / ( qCos( DEGREESTORADIANS( start.y() ) ) * 60.f );
    float dLat = dY / 60.f;

    return LatLong( start.Lat() + dLat, start.Long() + dLong );
}

QPointF LatLong::FromAngleToVector( float bearing )
{
    float bearingRad = DEGREESTORADIANS( bearing );

    float x = qSin( bearingRad );
    float y = qCos( bearingRad );

    return QPointF( x, y );
}

LatLong LatLong::AddDistanceInKm( const LatLong& start, float distance, float bearing )
{
    return AddDistanceInNm( start, MathUtils::KmToNm( distance ), bearing );
}

LatLong LatLong::AddDistanceInYd( const LatLong& start, float distance, float bearing )
{
    return AddDistanceInNm( start, MathUtils::YdToNm( distance ), bearing );
}

LatLong LatLong::AddDistanceInFt( const LatLong& start, float distance, float bearing )
{
    return AddDistanceInNm( start, MathUtils::FtToNm( distance ), bearing );
}

bool LatLong::IsPointInPolygon( std::vector<LatLong*> const& polygon, const LatLong& point)
{
    size_t i, j;
    bool isInside(false);
    for (i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++)
    {
        bool sentenceOne = (((polygon[i]->Lat() <= point.Lat()) && (point.Lat() < polygon[j]->Lat()))
                            || ((polygon[j]->Lat() <= point.Lat()) && (point.Lat() < polygon[i]->Lat())));

        bool sentenceTwo = (point.Long() < (polygon[j]->Long() - polygon[i]->Long()) * (point.Lat() - polygon[i]->Lat())
                            / (polygon[j]->Lat() - polygon[i]->Lat()) + polygon[i]->Long());

        if (sentenceOne && sentenceTwo)
        {
            isInside = !isInside;
        }
    }
    return isInside;
}

/// \brief Given a line defined by two points (P1 and P2) and a point, the function calculates the
/// Euclidian Distance from the point to the line. In other words, it calculates the length of the line segment
/// which joins the point to the line and is perpendicular to the line. The result distance is given in nautical miles.
/// \param lineP1 the first point (P1) that defines the line.
/// \param lineP2 the second point (P2) that defines the line.
/// \param point the point from which to measure the distance to the line.
/// \return the distance in NM (Nautical Miles). The distance is always positive.
float LatLong::DistanceFromPointToLine( const LatLong& lineP1, const LatLong& lineP2, const LatLong& point )
{
    float x1, y1, x2, y2, x0, y0;
    x1 = lineP1.Long();
    y1 = lineP1.Lat();
    x2 = lineP2.Long();
    y2 = lineP2.Lat();
    x0 = point.Long();
    y0 = point.Lat();

    return abs( (y2 - y1)*x0 - (x2 - x1)*y0 + x2*y1 - y2*x1 ) /
              ( sqrt( pow((y2 - y1),2) + pow((x2 - x1),2) ) );
}
