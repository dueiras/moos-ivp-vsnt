#ifndef LATLONGG_H
#define LATLONGG_H

#include <QPointF>
#include <QString>

#include <vector> // std::vector

class LatLong : public QPointF
{

public:
    LatLong( qreal latitude = 0.0, qreal longitude = 0.0 );
    LatLong( const QPointF& point );

    qreal Lat() const;
    qreal Long() const;

    void SetLat( qreal latitude );
    void SetLong( qreal longitude );

    float DistanceInKm( const LatLong& to, const float latReference ) const;
    float DistanceInNm( const LatLong& to, const float latReference ) const;

    float BearingTo( const LatLong& to ) const;

    LatLong AddDistanceInKm( float distance, float bearing ) const;
    LatLong AddDistanceInNm( float distance, float bearing ) const;


    //String conversions
    QString LatToString() const;
    QString LongToString() const;
    QString toString() const;

    QString LatToNMEAString() const;
    QString LongToNMEAString() const;

    static LatLong FromNMEAString( const QString& latitude, const QString& longitude );
    static LatLong FromNMEAString( const QString& latitude, const QString& latHem, const QString& longitude, const QString& longHem );

    static float NMEAStringToCoord( const QString& str );
    static float NMEAStringToCoord( const QString& coord, const QString& hemisphere );

    static QString CoordToString( qreal value, bool latitude );
    static QString CoordToNMEAString( float value, bool latitude );

    static QPointF FromAngleToVector( float bearing );
    static float DistanceInKm( const LatLong& start, const LatLong& end, const float latReference );
    static float DistanceInNm( const LatLong& start, const LatLong& end, const float latReference );
    static float DistanceInYd( const LatLong& start, const LatLong& end, const float latReference );
    static float DistanceInFt( const LatLong& start, const LatLong& end, const float latReference );

    static float Bearing( const LatLong& start, const LatLong& end );

    static LatLong AddDistanceInNm( const LatLong& start, float distance, float bearing );
    static LatLong AddDistanceInKm( const LatLong& start, float distance, float bearing );
    static LatLong AddDistanceInYd( const LatLong& start, float distance, float bearing );
    static LatLong AddDistanceInFt( const LatLong& start, float distance, float bearing );

    static bool IsPointInPolygon( std::vector<LatLong*> const& polygon, const LatLong& point);
    static float DistanceFromPointToLine( const LatLong& lineP1, const LatLong& lineP2, const LatLong& point );
};

#include "LatLong.inl"

#endif // LATLONGG_H
