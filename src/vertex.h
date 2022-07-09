#ifndef __VERTEX_H_
#define __VERTEX_H_

#include <ostream>
#include <cmath>
#include "vector.h"

class Vertex {
public:
    Vertex(Vector3 x, int id);

    // get the position
    Vector3 & get_pos();

    // calculate distance to another vertex
    double distance_to(Vertex & other);

    // check if two vertices are close to one another
    bool is(Vertex & other);

    std::string to_string() const;
    friend std::ostream& operator << (std::ostream& os, const Vertex vertex);

private:
    Vector3 _pos;
    int _id;
};


#endif // __VERTEX_H_
