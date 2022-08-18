#ifndef __VOLUME_H_
#define __VOLUME_H_

#include "vector3.h"
#include <vector>

double triangle_area(std::vector<Vector3> & points);
double quad_area(std::vector<Vector3> & points);
double line_area(std::vector<Vector3> & points);

#endif
