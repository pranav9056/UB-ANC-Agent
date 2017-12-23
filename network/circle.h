#ifndef CIRCLE_H
#define CIRCLE_H


#include "mercatorprojection.h"

class Circle
{

    // radius
    double r;
    // center
    core::Point c;

public:
    /**
     * @param R - radius
     * @param C - center
     */
    Circle(double R, core::Point point);

    core::Point  getC() const { return c; }
    double getR() const { return r; }

    size_t intersect(const Circle& C2, core::Point& i1, core::Point& i2);
    core::Point closestCirclePointFromPoint(core::Point target);
    bool checkPoint(core::Point pix);



};

#endif // CIRCLE_H
