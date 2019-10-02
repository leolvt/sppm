# SPPM - Spatial Product Partition Model

This is the Spatial PPM - a spatial version of the Product Partition
Model, a well known model for cluster analysis in temporal series.

## Citation

If you find this code helpful and use it in your work, please cite [our JMLR
paper](http://www.jmlr.org//papers/v20/16-615.html):

> Teixeira, L. V., Assunção, R. M., & Loschi, R. H. (2019). Bayesian Space-Time
> Partitioning by Sampling and Pruning Spanning Trees. Journal of Machine
> Learning Research, 20(85), 1-35.

```bibtex
@article{teixeira2019bayesian,
    title={Bayesian Space-Time Partitioning by Sampling and Pruning Spanning Trees},
    author={Teixeira, Leonardo V. and Assun{\c{c}}{\~a}o, Renato M. and Loschi, Rosangela H.},
    journal={Journal of Machine Learning Research},
    volume={20},
    number={85},
    pages={1--35},
    year={2019}
}
```

This code was also used for the [ICMD version](https://doi.org/10.1109/ICDM.2015.106):

> Teixeira, L. V., Assuncao, R. M., & Loschi, R. H. (2015, November). A
> generative spatial clustering model for random data through spanning trees. In
> 2015 IEEE International Conference on Data Mining (pp. 997-1002). IEEE.

```bitex
@inproceedings{teixeira2015generative,
    title={A generative spatial clustering model for random data through spanning trees},
    author={Teixeira, Leonardo V. and Assuncao, Renato M. and Loschi, Rosangela H.},
    booktitle={2015 IEEE International Conference on Data Mining},
    pages={997--1002},
    year={2015},
    organization={IEEE}
}
```

## Dependencies

This code uses C++11. The external dependencies are listed below:

* [LEMON][1] - a C++ template graph library
* [gflags][2] - command line flags processing library (formerly Google Commandline Flags)
* [RapidJSON][3] - a JSON parsing library, used to read data in GeoJSON
* [Easylogging++][4] - a single header C++ logging library

The library Easylogging++ is included with this source code. The others must be
installed in the build environment. If your system provides packages to install
them, use that. Otherwise, build them from source and install them to the proper
subdirectory in the 'deps' folder of this repository (e.g. install LEMON library
to /path/to/this/repository/deps/lemon, so that inside that folder there will be
the include and lib folders for the LEMON library).

[1]: http://lemon.cs.elte.hu/trac/lemon "LEMON"
[2]: https://github.com/gflags/gflags "gflags"
[3]: https://github.com/miloyip/rapidjson "RapidJSON"
[4]: https://github.com/muflihun/easyloggingpp "Easylogging++"


## Building

This project uses CMake build system. To build it from source, create a build
directory, change to it, call the cmake tool and then call the make tool.

Example:

	mkdir build
	cd build
	cmake ../
	make

The binary 'sppm' will be available under the 'src' subdir, inside the build
dir.

## Usage

	sppm [options] [--] normal <GeoJSON> <attr> <r> <s> <m> <v> <a> <b>
	sppm [options] [--] poisson <GeoJSON> <attr_Yi> <attr_Ei> <r> <s> <a> <b>

Example:
	./src/sppm normal /path/to/Data.GeoJSON MY_ATTR 1 9 5 4 1 2

The data is expected to be in a GeoJSON file. It is also expected to have the
graph neighborhood information present in the GeoJSON file.
