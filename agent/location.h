#ifndef LOCATION_H
#define LOCATION_H

#include<QObject>
class Location
{
public:
    double lat;
    double lon;
    Location(double lat, double lon);
    Location();
    void setLocation(double lat, double lon);
    static double distance(Location &a,Location &b);
    QByteArray serialize();
    void deserialize(QByteArray);
};

#endif // LOCATION_H
