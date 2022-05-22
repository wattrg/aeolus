#ifndef __VERTEX_H_
#define __VERTEX_H_

#include <ostream>
#include "vector.h"

class Vertex {
public:
    Vertex(Vector2 x);

    Vector2 pos;

    friend std::ostream& operator << (std::ostream& os, const Vertex vertex);
};


#endif // __VERTEX_H_
