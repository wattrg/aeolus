#ifndef __VERTEX_H_
#define __VERTEX_H_

#include <ostream>
#include <cmath>
#include "../grid/grid_vertex.h"

class Vertex {
public:
    Vertex () {}

    //Vertex(Vector3 x, int id);
    Vertex(Grid::Vertex &);

    // get the position
    const Vector3 & get_pos() const;

    // calculate distance to another vertex
    double distance_to(Vertex & other);

    // check if two vertices are close to one another
    bool is(Vertex & other);

    std::string to_string() const;
    friend std::ostream& operator << (std::ostream& os, const Vertex vertex);

    int id() const;

private:
    Vector3 _pos;
    int _id;
};


#endif // __VERTEX_H_
