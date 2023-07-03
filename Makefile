#CXXFLAGS=-g -fopenmp -O3 -std=c++17 -fPIC -DFORTRAN_SUPPORT -DPRINT_DEBUG
CXXFLAGS=-g -fopenmp -O3 -std=c++17 -fPIC -flto -DOMPI_SKIP_MPICXX=true -I$(PWD)
#-DPRINT_DEBUG
#CXXFLAGS=-g -fopenmp -O3 -std=c++17 -fPIC -fsanitize=address
#MPI=mpich
MPI?=openmpi
WRAP?=external/wrap/wrap.py
MPICXX?=time mpicxx.$(MPI)
MPICC?=mpicc.$(MPI)
MPIRUN?=mpirun.$(MPI)

ifeq ($(MPI), mpich)
CXXFLAGS+= -DHAVE_COUNT -DHAVE_SESSION
endif

.PHONY: all lib maplib test
all: test.$(MPI) libHandleShim.$(MPI).so
lib: libHandleShim.$(MPI).so
maplib: libHandleMap.$(MPI).so
test: test.$(MPI)

.PRECIOUS: %.$(MPI).cpp

%.$(MPI).cpp: %.w
	$(WRAP) -s -c $(MPICC) $< -o $@
	sed -i -e '/ = PMPI_[^(]*_init_\?c\?(/{n;s/postRequest/postRequestInit/}' -e 's/^_EXTERN_C_ //' $@
	if (which clang-format > /dev/null 2>&1); then clang-format -i $@; fi

%.shim.$(MPI).o : %.cpp tracking.h Makefile
	$(MPICXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

%.map.$(MPI).o : %.cpp tracking.h Makefile
	$(MPICXX) -c $(CXXFLAGS) $(CPPFLAGS) -DHANDLE_MAP $< -o $@

libHandleShim.$(MPI).so: tracking.shim.$(MPI).o gen-nb-wrappers.$(MPI).shim.$(MPI).o gen-wrappers.$(MPI).shim.$(MPI).o man-wrappers.shim.$(MPI).o
	$(MPICXX) -shared $(CXXFLAGS) $(CPPFLAGS) $^ -o $@

libHandleShim.$(MPI).patched.so: libHandleShim.$(MPI).so
	sed -e 's/PMPI_Init/XMPI_Init/g' -e 's/PMPI_Finalize/XMPI_Finalize/g' $< > $@

libHandleMap.$(MPI).so: tracking.map.$(MPI).o gen-nb-wrappers.$(MPI).map.$(MPI).o gen-wrappers.$(MPI).map.$(MPI).o man-wrappers.map.$(MPI).o
	$(MPICXX) -shared $(CXXFLAGS) $(CPPFLAGS) $^ -o $@
#	sed -i -e 's/PMPI_Init/XMPI_Init/g' -e 's/PMPI_Finalize/XMPI_Finalize/g' $@

test.$(MPI) : tests/test.shim.$(MPI).o 
#tracking.$(MPI).o gen-nb-wrappers.$(MPI).$(MPI).o gen-wrappers.$(MPI).$(MPI).o man-wrappers.$(MPI).o
	$(MPICXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $@

test-grp.$(MPI) : tests/test-group.shim.$(MPI).o 
#tracking.$(MPI).o gen-nb-wrappers.$(MPI).$(MPI).o gen-wrappers.$(MPI).$(MPI).o man-wrappers.$(MPI).o
	$(MPICXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $@

run-test: test.$(MPI) lib maplib
	$(MPIRUN) -n 2 env OMP_NUM_THREADS=1 LD_PRELOAD=./libHandleShim.$(MPI).so ./test.$(MPI)
	$(MPIRUN) -n 2 env OMP_NUM_THREADS=2 LD_PRELOAD=./libHandleShim.$(MPI).so ./test.$(MPI)
	$(MPIRUN) -n 2 env OMP_NUM_THREADS=3 LD_PRELOAD=./libHandleShim.$(MPI).so ./test.$(MPI)
	$(MPIRUN) -n 2 env OMP_NUM_THREADS=1 LD_PRELOAD=./libHandleMap.$(MPI).so ./test.$(MPI)
	$(MPIRUN) -n 2 env OMP_NUM_THREADS=2 LD_PRELOAD=./libHandleMap.$(MPI).so ./test.$(MPI)
	$(MPIRUN) -n 2 env OMP_NUM_THREADS=3 LD_PRELOAD=./libHandleMap.$(MPI).so ./test.$(MPI)

gen-nb-wrappers.$(MPI).$(MPI).o : completion-wrappers.cpp

clean:
	rm -f *.o
mrproper: clean
	rm -f *.mpich *.openmpi *.so *~
