#ifndef __GRID_VERTEX_H_
#define __GRID_VERTEX_H_

#include "../util/vector3.h"

namespace Grid {

class Vertex{
public:
    Vertex(Vector3 pos, unsigned int id) : _pos(pos), _id(id) {}
    int id() const {return this->_id;}
    Vector3 & get_pos() { return this->_pos; }
    std::string to_string() const { 
        return std::string("Vertex(x = " + std::to_string(this->_pos.x) + 
                                  "y = " + std::to_string(this->_pos.y) + 
                                  "z = " + std::to_string(this->_pos.z) + ")");
    }
private:
    Vector3 _pos;
    int _id;
};

}

bool operator == (const Grid::Vertex & left, const Grid::Vertex & right);

#endif
