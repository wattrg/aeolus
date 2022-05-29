#ifndef __UTIL_H_
#define __UTIL_H_

#include <cmath>
#include <iostream>

const double TOL=1e-10;

class Vector2 {
public:
    Vector2();
    Vector2(double x, double y);
    double x;
    double y;
    double distance_to(Vector2 & other);
    bool is_close(Vector2 & other);
    friend std::ostream& operator << (std::ostream& os, const Vector2 vec);

};

#endif // __UTIL_H_
