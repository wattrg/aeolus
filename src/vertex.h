#ifndef __VERTEX_H_
#define __VERTEX_H_

#include <ostream>
#include <cmath>
#include "vector.h"

class Vertex {
public:
    Vertex(Vector2 x);

    // get the position
    Vector2 & get_pos();

    // calculate distance to another vertex
    double distance_to(Vertex & other);

    // check if two vertices are close to one another
    bool is_close(Vertex & other);

    friend std::ostream& operator << (std::ostream& os, const Vertex vertex);

private:
    Vector2 _pos;
};


#endif // __VERTEX_H_
