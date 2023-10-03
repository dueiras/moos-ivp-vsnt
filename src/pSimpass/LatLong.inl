inline LatLong::LatLong( qreal latitude, qreal longitude )
    : QPointF( longitude, latitude )
{
}

inline LatLong::LatLong( const QPointF& point )
    : QPointF( point )
{
}

inline qreal LatLong::Lat() const
{
    return y();
}

inline qreal LatLong::Long() const
{
    return x();
}

inline void LatLong::SetLat( qreal latitude )
{
    setY( latitude );
}

inline void LatLong::SetLong( qreal longitude )
{
    setX( longitude );
}


inline float LatLong::DistanceInKm( const LatLong& to, const float latReference ) const
{
    return DistanceInKm( *this, to, latReference );
}

inline float LatLong::DistanceInNm( const LatLong& to, const float latReference ) const
{
    return DistanceInNm( *this, to, latReference );
}

inline float LatLong::BearingTo( const LatLong& to ) const
{
    return Bearing( *this, to );
}

inline LatLong LatLong::AddDistanceInKm( float distance, float bearing ) const
{
    return AddDistanceInKm( *this, distance, bearing );
}

inline LatLong LatLong::AddDistanceInNm( float distance, float bearing ) const
{
    return AddDistanceInNm( *this, distance, bearing );
}

inline QString LatLong::LatToString() const
{
    return CoordToString( Lat(), true );
}

inline QString LatLong::LongToString() const
{
    return CoordToString( Long(), false );
}

inline QString LatLong::toString() const
{
    return QString( "%1, %2" ).arg( LatToString(), LongToString() );
}

inline QString LatLong::LatToNMEAString() const
{
    return CoordToNMEAString( Lat(), true );
}

inline QString LatLong::LongToNMEAString() const
{
    return CoordToNMEAString( Long(), false );
}
