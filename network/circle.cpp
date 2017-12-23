#include <cstdlib>
#include <iostream>
#include <cmath>

#include "circle.h"
#include "mercatorprojection.h"
#include <algorithm>

double norm(core::Point a,core::Point b){
    return sqrt(pow(a.X()-b.X(),2)+pow(a.Y()-b.Y(),2));
}

Circle::Circle(double rad,core::Point point)
{
    r = rad;
    c = point;
}


size_t Circle::intersect(const Circle& C2, core::Point& i1, core::Point& i2) {
    // distance between the centers
    double d = norm(c,C2.c);
    // find number of solutions
    if(d > r + C2.r) // circles are too far apart, no solution(s)
    {
        //std::cout << "Circles are too far apart\n";
        return 0;
    }
    else if(d == 0 && r == C2.r) // circles coincide
    {
        //std::cout << "Circles coincide\n";
        return 0;
    }
    // one circle contains the other
    else if(d + std::min(r, C2.r) < std::max(r, C2.r))
    {
        //std::cout << "One circle contains the other\n";
        return 0;
    }
    else
    {
        double a = (r*r - C2.r*C2.r + d*d)/ (2.0*d);
        double h = sqrt(r*r - a*a);

        // find p2
        core::Point p2( c.X() + (a * (C2.c.X() - c.X())) / d,
                c.Y() + (a * (C2.c.Y() - c.Y())) / d);

        // find intersection points p3
        i1.SetX(p2.X() + (h * (C2.c.Y() - c.Y())/ d));
        i1.SetY(p2.Y() - (h * (C2.c.X() - c.X())/ d));

        i2.SetX(p2.X() - (h * (C2.c.Y() - c.Y())/ d));
        i2.SetY(p2.Y() + (h * (C2.c.X() - c.X())/ d));

        if(d == r + C2.r)
            return 1;
        return 2;
    }
}

core::Point Circle::closestCirclePointFromPoint(core::Point target){
    core::Point i1,i2;
    if (c.X() - target.X() == 0){
        i1.SetX(c.X());
        i1.SetY(c.Y()+r);
        i2.SetX(c.X());
        i2.SetY(c.Y()-r);
    }
    else{
        double m = (c.Y()-target.Y())/(c.X()-target.X());
        double a = 1+ pow(m,2);
        double yIntercept = c.Y()-(m*c.X());
        double b = (-2*c.X())+(2*m*yIntercept) +(-2*m+c.Y());
        double cons = pow(c.X(),2)+pow(c.Y(),2)+pow(yIntercept,2) + (-2*yIntercept*c.Y()) - pow(r,2);
        double d = sqrt((pow(b,2)-(4*a*cons)));
        double x1 = (-1*b + d)/(4*a);
        double x2 = (-1*b - d)/(4*a);
        i1.SetX(x1);
        i2.SetX(x2);
        i1.SetY(m*x1+yIntercept);
        i2.SetY(m*x2+yIntercept);

    }
    if(norm(target,i1)<norm(target,i2))
        return i1;
    else
        return i2;


}
bool Circle::checkPoint(core::Point pix){
    if (pow(pix.X()-c.X(),2)+pow(pix.Y()-c.Y(),2)<=pow(r,2))
        return true;
    return false;
}
