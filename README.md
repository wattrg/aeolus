# Aeolus
Simple finite volume CFD solver

## Installation
You can get a copy of the code with

```
git clone git@github.com:wattrg/aeolus.git
```

From the folder in which you want the repository to land.

### Dependancies
To install Aeolus, you first need the following dependancies:
1. `Python`  
2. `pybind11` Can be installed via pip or conda
   
### Set your environment variables
When you install Aeolus, it will be installed to the directory `inst`, within the repository.
You'll want to add the following lines to you `.bashrc` to tell your computer to look in that directory
```
export PATH=$PATH:/dir/to/repo/inst/bin
export PYTHONPATH=${PYTHONPATH}:/dir/to/rep/inst/lib
```

### Compile and install
To only compile (i.e. not copy the completed files to the install directory), you can type `make` or `make build` from the root of the repository.

To install (i.e. copy the completes files to the install directory), you can type `make install` from the root of the respository. This re-build any components which have changed as well.

#### Dependancies
```
sudo apt install gcc-offload-nvptx
```
   
## Using the Python library
Once you have successfully installed `Aeolus`, you should be able to use it through `Python` scripts.
Simply put 
```
import aeolus
```
in your imports.
