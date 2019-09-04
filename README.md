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

# Compiling

Compiling the code should just be a matter of running cmake:

    $ md build
	$ cd build
	$ ccmake ..
	
	$ make -j16
	

