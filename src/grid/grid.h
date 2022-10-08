#ifndef __GRID_H_
#define __GRID_H_

#include <vector>
#include <map>
#include <string>
#include "grid_vertex.h"
#include "grid_interface.h"
#include "su2.h"

namespace Grid {

enum class GridFormats{
    su2,
    none,
};

GridFormats extension_to_grid_format(std::string);

class Grid {
public: 
    Grid(std::string file_name);
    virtual ~Grid() {}
    //virtual void read_grid(const char * file_name) {}

    std::vector<Vertex *> vertices() {return this->_vertices;}
    std::vector<Interface *> interfaces () {return this->_interfaces;}
    std::vector<Cell *> cells() {return this->_cells;}
    std::map<std::string, std::vector<Interface *>> & bcs() {return this->_bcs;}
    unsigned int number_boundaries() const {return this->_bcs.size();}
    void set_grid(std::vector<Vertex *>, 
                  std::vector<Interface *>, 
                  std::vector<Cell *>,
                  std::map<std::string, std::vector<Interface *>>);
    std::string to_string();

private:
    std::vector<Vertex *> _vertices;
    std::vector<Interface *> _interfaces;
    std::vector<Cell *> _cells;
    std::map<std::string, std::vector<Interface *>> _bcs;
};

}

#endif
