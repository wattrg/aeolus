#include "grid_vertex.h"

bool operator == (const Grid::Vertex & left, const Grid::Vertex & right){
    return (left.id() == right.id());
}

bool operator != (const Grid::Vertex & left, const Grid::Vertex & right){
    return (left.id() != right.id());
}
