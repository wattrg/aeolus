#include "solvers.h"

Solver::Solver(Simulation & config) : _config(config) {}

void Solver::solve(){
    double dt;
    // set the flux calculator
    for (FluidBlock * fluid_block : this->_config.fluid_blocks()){
        fluid_block->set_flux_calculator(this->_config.flux_calculator());
    }

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
            this->_config.log.error("Some went wrong");
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
