
#include "volume.h"

double triangle_area(std::vector<Vector3> & points){
    if (points.size() != 3) { throw std::runtime_error("Shape doesn't have 3 points"); }
    for (Vector3 point : points){
        if (fabs(point.z) > 1e-14) { 
            throw std::runtime_error("Non zero z coordinate not supported yet"); 
        }
    }

    Vector3 & a = points[0];
    Vector3 & b = points[1];
    Vector3 & c = points[2];
    double tmp = a.x * (b.y - c.y) + b.x*(c.y - a.y) + c.x*(a.y - b.y);
    return 0.5 * fabs(tmp);
}

double quad_area(std::vector<Vector3> & points){
    if (points.size() != 4) { throw std::runtime_error("Shape doesn't have 4 points"); }
    for (Vector3 point : points){
        if (fabs(point.z) > 1e-14) { 
            throw std::runtime_error("Non zero z coordinate not supported yet"); 
        }
    }

    // use the shoelace formula applied to a quad
    //https://math.stackexchange.com/questions/1259094/coordinate-geometry-area-of-a-quadrilateral
    Vector3 & a = points[0];
    Vector3 & b = points[1];
    Vector3 & c = points[2];
    Vector3 & d = points[3];
    double tmp_plus = a.x*b.y + b.x*c.y + c.x*d.y + d.x*a.y;
    double tmp_minus = b.x*a.y + c.x*b.y + d.x*c.y + a.x*d.y;
    return 0.5 * fabs(tmp_plus - tmp_minus);
}

double line_area(std::vector<Vector3> & points){
    if (points.size() != 2)
        throw std::runtime_error("Shape should only have 2 points");
    Vector3 a = points[0];
    Vector3 b= points[1];
    return sqrt( (b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y) + (b.z-a.z)*(b.z-a.z) );
}
