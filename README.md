# Handle shim library
Prototype implementation of a shim layer wrapping selected MPI handles. 

## Configuration of the handle shim library
The handle shim library wraps selected MPI handles into tool-defined handles.
The defines in tracking.h:20 and following select the handles to be wrapped. At the moment, wrapping type or op will generate a compiler error, because predefined handles for these handles are not defined in the prototype.

The define `REAL_DATAPOOL` selects the memory pool implementation for the tool handles. Removing the define will replace the memory pool implementation by C++ `new`/`delete`.

## Build using the Makefile
The Makefile accepts several configuration variables to be overwritten:
- MPI defaults to `openmpi`. This will be used as infix in most generated files
- MPICC defaults to `mpicc.$(MPI)`. In combination with the MPI variable this allows easy switching of MPI on Ubuntu
- MPICXX defaults to `mpicxx.$(MPI)`. In combination with the MPI variable this allows easy switching of MPI on Ubuntu
- MPIRUN defaults to `mpirun.$(MPI)`. In combination with the MPI variable this allows easy switching of MPI on Ubuntu

Using different settings for `MPI` allows to maintain builds for different MPI implementations in the same directory.

### Examples
```lang=BASH
# Building for MPICH on Ubuntu
MPI=mpich make all
# Building for IntelMPI and running on a cluster with SLURM
MPICC=mpiicc MPICXX=mpiicpc MPIRUN=srun MPI=intelmpi make all run-test
```
The run-test targets probably need adjustment to allocate multiple cores per (SLURM-) task for the multi-threaded execution.

## Files
### MPI Function Wrappers
- completion-wrappers.cpp  - Request completion calls, included by gen-nb-wrappers.cpp / nb-wrappers.cpp
- gen-nb-wrappers.cpp  - Generated non-blocking communication calls (manually modified to mark persistent requests)
- man-wrappers.cpp  - Manually modified MPI wrappers with special semantics regarding handles
- gen-wrappers.cpp  - Generated MPI wrappers with simple IN or OUT semantics regarding handles
- tracking.cpp  - Implementation of selected class functions defined in tracking.h

### wrap.py Templates
- gen-nb-wrappers.w
- gen-wrappers.w

```
wrap.py -n -s gen-wrappers.w -o gen-wrappers.cpp
```