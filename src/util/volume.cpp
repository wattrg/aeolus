
#include "volume.h"

double quad_area(std::vector<Vector3> & points){
    if (points.size() != 4) { throw std::runtime_error("Shape doesn't have 4 points"); }
    for (Vector3 point : points){
        if (point.z != 0.0) { 
            throw std::runtime_error("Non zero z coordinate not supported yet"); 
        }
    }

    // use the shoelace formula applied to a quad
    //https://math.stackexchange.com/questions/1259094/coordinate-geometry-area-of-a-quadrilateral
    Vector3 a = points[0];
    Vector3 b = points[1];
    Vector3 c = points[2];
    Vector3 d = points[3];
    double tmp_plus = a.x*b.y + b.x*c.y + c.x*d.y + d.x*a.y;
    double tmp_minus = b.x*a.y + c.x*b.y + d.x*c.y + a.x*d.y;
    return 0.5 * fabs(tmp_plus - tmp_minus);
}
