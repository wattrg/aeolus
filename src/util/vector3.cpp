#include "vector3.h"


Vector3::Vector3() : x(0.0), y(0.0), z(0.0){}
Vector3::Vector3(double x) : x(x), y(0.0), z(0.0) {}
Vector3::Vector3(double x, double y) : x(x), y(y), z(0.0) {}
Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

void Vector3::copy(const Vector3 & other){
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
}

Vector3 Vector3::operator + (const Vector3& other) const {
    return Vector3(this->x + other.x, this->y + other.y, this->z + other.z);    
}

Vector3 Vector3::operator - (const Vector3& other) const{
    return Vector3(this->x - other.x, this->y - other.y, this->z - other.z);    
}

Vector3 Vector3::operator * (const double c) const {
    return Vector3(c*this->x, c*this->y, c*this->z);    
}

Vector3 Vector3::operator / (const double c) const {
    return Vector3(this->x/c, this->y/c, this->z/c);    
}

double Vector3::length(){
   return sqrt(this->x*this->x + this->y*this->y + this->z*this->z); 
}

void Vector3::scale(double scale){
    this->x *= scale;
    this->y *= scale;
    this->z *= scale;
}

Vector3 Vector3::cross(const Vector3& other){
    double x = this->y*other.z - this->z*other.y;
    double y = this->z*other.x - this->x*other.z;
    double z = this->x*other.y - this->y*other.x;
    return Vector3(x, y, z);
}

double Vector3::dot(const Vector3 & other){
    return this->x*other.x + this->y*other.y + this->z*other.z;
}

void Vector3::transform_to_local_frame(const Vector3 & n, const Vector3 & t1, const Vector3 & t2){
    double x = this->dot(n);
    double y = this->dot(t1);
    double z = this->dot(t2);
    this->x = x;
    this->y = y;
    this->z = z;
}

void Vector3::transform_to_global_frame(const Vector3 & n, const Vector3 & t1, const Vector3 & t2){
    double x = this->x*n.x + this->y*t1.x + this->z*t2.x;
    double y = this->x*n.y + this->y*t1.y + this->z*t2.y;
    double z = this->x*n.z + this->y*t1.z + this->z*t2.z;
    this->x = x;
    this->y = y;
    this->z = z;
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
