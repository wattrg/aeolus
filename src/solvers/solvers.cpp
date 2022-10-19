#include "solvers.h"

Solver::Solver(Simulation & config) : _config(config) {}

void Solver::solve(){
    double dt;
    // Give each fluid block a local copy of some data
    for (FluidBlock * fluid_block : this->_config.fluid_blocks()){
        fluid_block->set_flux_calculator(this->_config.flux_calculator());
        fluid_block->set_gas_model(this->_config.gas_model());
    }

// maybe move fluid block to the gpu
#ifdef GPU
    Vertex * vertices = this->_config.fluid_blocks()[0]->vertices_ptr();
    unsigned int num_verts = this->_config.fluid_blocks()[0]->number_vertices();
    Interface * interfaces = this->_config.fluid_blocks()[0]->interfaces_ptr();
    unsigned int num_interfaces = this->_config.fluid_blocks()[0]->number_interfaces();
    Cell * cells = this->_config.fluid_blocks()[0]->cells_ptr();
    unsigned int num_cells = this->_config.fluid_blocks()[0]->number_cells();
#pragma omp target enter data map(to: vertices[:num_verts]) map(to: interfaces[:num_interfaces]), map(to: cells[:num_cells])
#endif

    // begin the time stepping
    this->_config.log.info("Beginning stepping with " + this->_name() + " solver.");
    for (unsigned int step=0; step < this->_max_step; step++){
        // try to take a step
        try{
            dt = this->_step(); 
        }
        catch (GasModelException & e) {
            // if there's an error, write a solution
            // and bail out
            this->_config.write_fluid_blocks();
            this->_config.log.error("Some went wrong :(");
            throw e;
        }
        this->_config.add_time_increment(dt);

        if (step % this->_plot_step == 0){
            this->_config.write_fluid_blocks();
        }

        if (step % this->_print_step == 0){
            this->_config.log.info(
                "   step " + std::to_string(step) + 
                " time = " + std::to_string(this->_config.time()) + 
                " dt = " + std::to_string(dt)
            );
        }
    }
    this->_config.log.info("Finished stepping with " + this->_name() + " solver.");

}
