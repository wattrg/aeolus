#include "vector.h"

Vector2::Vector2(){}
Vector2::Vector2(double x, double y) : x(x), y(y) {}

std::ostream& operator << (std::ostream& os, const Vector2 vec){
    os << "Vector2(";
    os << "x = " << vec.x << ", ";
    os << "y = " << vec.y << ")";
    return os;
}
