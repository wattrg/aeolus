#ifndef __GRID_UTIL_H_
#define __GRID_UTIL_H_

#include "grid.h"

namespace Grid {

enum class ElementShape { Triangle, Quad, Line, };
enum class CellShape { Triangle, Quad, };

struct Element {

    ElementShape shape;
    std::vector<int> vertices;

    Element(ElementShape shape, std::vector<int> vertices)
        : shape(shape), vertices(vertices)
    {};
};

}

#endif
