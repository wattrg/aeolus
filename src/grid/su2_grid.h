#include "grid.h"

namespace Grid {

class Su2Grid : public Grid {
    Grid();
    ~GridInput();
    void read_grid(const char * file_name,
                   std::map<std::string, BoundaryCondition *> & bc_map);
};

}
