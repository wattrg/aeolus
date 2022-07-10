#include "vector.h"


Vector3::Vector3() : x(0.0), y(0.0), z(0.0){}
Vector3::Vector3(double x) : x(x), y(0.0), z(0.0) {}
Vector3::Vector3(double x, double y) : x(x), y(y), z(0.0) {}
Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

void Vector3::copy(Vector3 & other){
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
}

std::string Vector3::to_string() const {
    std::string str = "Vector3(";
    str.append("x = ");
    str.append(std::to_string(this->x));
    str.append(", y = ");
    str.append(std::to_string(this->y));
    str.append(", z = ");
    str.append(std::to_string(this->z));
    str.append(")");
    return str;
}

std::ostream& operator << (std::ostream& os, const Vector3 vec){
    os << vec.to_string();
    return os;
}

double Vector3::distance_to(Vector3 & other){
    double dx = this->x - other.x;
    double dy = this->y - other.y;
    double dz = this->x - other.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

bool Vector3::is_close(Vector3 & other){
    return this->distance_to(other) < TOL;
}
