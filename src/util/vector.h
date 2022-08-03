#ifndef __UTIL_H_
#define __UTIL_H_

#include <cmath>
#include <iostream>

const double TOL=1e-10;

class Vector3 {
public:
    Vector3();
    Vector3(double x);
    Vector3(double x, double y);
    Vector3(double x, double y, double z);
    double x, y, z;

    double distance_to(Vector3 & other);
    bool is_close(Vector3 & other);

    void copy(Vector3 & other);

    std::string to_string () const;
    friend std::ostream& operator << (std::ostream& os, const Vector3 vec);
    
    Vector3 operator + (const Vector3& other);
    Vector3 operator - (const Vector3& other);
    Vector3 operator * (const double c);
    Vector3 operator / (const double c);

    // calculate the length of the vector
    double length();

    // scale the vector by some scalar
    void scale(double scale);

    // cross product
    Vector3 cross(const Vector3& other);

    // dot product
    double dot(const Vector3& other);

    // transform vector to the a coordinate system defined by three vectors
    void transform_to_local_frame(const Vector3 & n, const Vector3 & t1, const Vector3 & t2);

    // transform a vector in a coordinate system defined by three vectors
    // back into the global frame
    void transform_to_global_frame(const Vector3 & n, const Vector3 & t1, const Vector3 & t2);
};

#endif // __UTIL_H_
