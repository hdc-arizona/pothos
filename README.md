# Pothos

This repository used to be called "nanocubes2". We've renamed it
pothos to move away from the nanocubes name, and because the internals
of the data structure look like a tangle of vines. It seemed fitting.


# Dependencies

This project uses CMake and a number of open-source libraries:

## Bundled with the source
* [Rang](https://github.com/agauniyal/rang/)
* [gperftools](https://github.com/gperftools/gperftools) (for tcmalloc)

## Assumed to be available in the environment

* Boost
* Apache Arrow

# Compiling

Compiling the code should just be a matter of running cmake:

    $ md build
	$ cd build
	$ cmake ..
	$ make -j16
	

# Acknowledgment

This material is based upon work supported or partially supported by the National Science Foundation under Grant Number 1815238, project titled "III: Small: An end-to-end pipeline for interactive visual analysis of big data"

Any opinions, findings, and conclusions or recommendations expressed in this project are those of author(s) and do not necessarily reflect the views of the National Science Foundation.
