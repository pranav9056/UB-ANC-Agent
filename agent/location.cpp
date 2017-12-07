#include "location.h"
#include "mercatorprojection.h"
#include "config.h"
#include "QGCGeo.h"
#include <QtMath>
#include<QObject>
#include<QsLog.h>
Location::Location(double lat, double lon)
{
    this->lat = lat;
    this->lon = lon;
}
Location::Location(){
    this->lat=0;
    this->lon=0;
}
void Location::setLocation(double lat, double lon){
    this->lat = lat;
    this->lon = lon;
}
double Location::distance(Location &a,Location &b){


    projections::MercatorProjection proj;
    core::Point pix1 = proj.FromLatLngToPixel(a.lat, a.lon, GND_RES);
    core::Point pix2 = proj.FromLatLngToPixel(b.lat, b.lon, GND_RES);

    Vector3d v1(pix1.X(), pix1.Y(),0);
    Vector3d v2(pix2.X(), pix2.Y(),0);

    Vector3d v = v1-v2;

    return v.length();
}

QByteArray Location::serialize(){
    QByteArray data;
    if(this->lat<0){
        data = QByteArray::number(-1*this->lat);
        data=data.rightJustified(10,'0');
        data=data.replace(1,1,"-");
    }else{
        data = QByteArray::number(this->lat);
        data=data.rightJustified(10,'0');
    }
    char *data2 = new char[data.size() + 1];
    strcpy(data2, data.data());
    //QLOG_INFO()<<"aaaaaaaaaaaaaaa"<<data2;
    QByteArray temp;
    if(this->lon<0){
        temp = QByteArray::number(-1*this->lon);
        temp=temp.rightJustified(10,'0');
        temp=temp.replace(0,1,"-");
    }else{
        temp = QByteArray::number(this->lon);
        temp=temp.rightJustified(10,'0');
    }
    char *data3 = new char[temp.size() + 1];
    strcpy(data3, temp.data());
    //QLOG_INFO()<<"bbbbbbbbbbbbbbb"<<data3;
    data.append(temp);
    char *data4 = new char[data.size() + 1];
    strcpy(data4, data.data());
    //QLOG_INFO()<<"cccccccccccccc"<<data4;
    data.prepend(QByteArray("B:L"));
    char *data1 = new char[data.size() + 1];
    strcpy(data1, data.data());
    //QLOG_INFO()<<"ddddddddddddddddddddd"<<data1;
    return data;
}

void Location::deserialize(QByteArray data){
    this->lat=data.mid(3,10).toDouble();
    this->lon=data.mid(13,10).toDouble();

}
