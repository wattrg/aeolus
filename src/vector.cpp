#include "vector.h"

Vector2::Vector2(){}
Vector2::Vector2(double x, double y) : x(x), y(y) {}

std::ostream& operator << (std::ostream& os, const Vector2 vec){
    os << "Vector2(";
    os << "x = " << vec.x << ", ";
    os << "y = " << vec.y << ")";
    return os;
}

double Vector2::distance_to(Vector2 & other){
    double dx = this->x - other.x;
    double dy = this->y - other.y;
    return std::sqrt(dx*dx + dy*dy);
}

bool Vector2::is_close(Vector2 & other){
    return this->distance_to(other) < TOL;
}
