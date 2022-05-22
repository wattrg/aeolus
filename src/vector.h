#ifndef __UTIL_H_
#define __UTIL_H_

#include <iostream>

class Vector2 {
public:
    Vector2();
    Vector2(double x, double y);
    double x;
    double y;
    friend std::ostream& operator << (std::ostream& os, const Vector2 vec);
};

#endif // __UTIL_H_
