CEval
===

Introduction
---

CEval (Capillary Electrophoresis Evaluator) is a multiplatform open-source tool to aid with evaluation of experimental results obtained by Capillary Zone Electrophoresis with specific aim at Affinity Capillary Electrophoresis. CEval can perform basic evaluation of electrophoregrams and calculate some common parameters of peaks. Additionally, CEval can fit peaks with Haaroff - van der Linde function to allow for better estimation of migration times.

In ACE mode CEval can assist with evaluation of electrophoretic mobilities and complexation constants of analytes with selectors by the means of nonlinear regression. Statistical evaluation of calculated results is also provided in graphical form.

Dependencies
---

CEval makes use of the following tools:

- [Qt 5 toolkit](http://www.qt.io/)
- [Armadillo](http://arma.sourceforge.net/)
- [BLAS](http://www.netlib.org/blas/)
- [LAPACK](http://www.netlib.org/lapack/)
- [Boost](http://www.boost.org/)
- [Qwt](http://qwt.sourceforge.net/)
- [libHPCS](https://github.com/echmet/libHPCS)
- [HVL_MT](https://github.com/echmet/HVL_MT)

All of the libraries and packages above are hard dependencies. CEval cannot be built or run without them.

Build
---

Prior to building CEval, all hard dependencies have to be available in binary form, either by building them from source or by obtaining an appropriate pre-built package (The binary form requirement does not apply for the Boost libraries). Refer to web presentations of the respective projects for further instructions.

CEval is configured to use the Armadillo library only as a wrapper at the source code level, it links directly against BLAS and LAPACK. When building Armadillo, set the variable `ARMA_USE_WRAPPER` to `false` in its `CMakeLists.txt` file.

It is possible to build Armadillo as a binary wrapper and/or link against other LAPACK and BLAS implementations supported by the Armadillo library. However, note that such configurations have not been tested by CEval developers. Should you wish to try, note that the `LIBS` variable in `CEval.pri` has to be adjusted accordingly.

1. Setting up paths to build dependencies

  Paths to build dependencies can be set in **CEval.pri** file. The variables to set are following:

  - `HVLPATH` - Path to the directory which contains both **hvl_mt** shared library and **hvl.h** header file
  - `HPCSPATH` - Path to the directory which contains both **libHPCS** shared library and **libHPCS.h** public header file
  - `BOOSTPATH` - Path to the directory with the Boost library headers
  - `QWTPATH` - Path to the **features/qwt.prf** file
  - `ARMAPATH` - Path to the directory where the Armadillo library is installed. This path must include the **/lib** and **/include** subdirectories.
  - Set the `LIBS` QMake variable in `CEval.pri` to point to your **BLAS** and **LAPACK** libraries locations or use `-lblas` and  `-llapack` if you have a system-wide installation of these libraries.

2. Building

  ### Linux/UNIX

  After you have set up the paths as instructed above, `cd` to the directory with CEval sources and run the following:

       mkdir build
       cd build
       qmake "CONFIG+=Release" ../CEval.pro
       make

  **Remark:** Linux is the only UNIX-based system so far that has been tested to build and run CEval.

  ### Windows

  It is recommended to install [QtCreator](https://www.qt.io/ide/) and use its user interface to set the project up and build it. While it might be possible to use other compilers, the authors recommend to use the [MinGW toolchain](http://www.mingw.org/) to build CEval. Development package for Windows downloadable from the [Qt project's](http://qt.io) website ships both Qt 5 and MinGW compiler.

3. Run

  If the Qwt, libHPCS and libhvl_mt libraries were built to be linked dynamically (the default), it is necessary to make the libraries (usually named **qwt.so, qwt.dll, qwt.dylib**; **libHPCS.so, libHPCS.dll, libHPCS.dylib** and **libhvl_mt.so, libhvl_mt.dll, libhvl_mt.dylib**) available to the operating system's dynamic linker. This can be achieved by various ways. The libraries can be copied to a directory where the linker is set up to look (i.e. **/usr/lib, C:\Windows\system32**) or to the same directory as CEval binary. On Linux systems the **LD_LIBRARY_PATH** environment variable can be adjusted to include paths to the necessary libraries.

Pre-built binaries
---
Pre-built binaries for commonly used platforms can be downloaded from here:  
[ECHMET website](http://echmet.natur.cuni.cz/ceval#block-block-17)

Usage
---
Tutorial for CEval can be obtained from here:  
[ECHMET website](http://echmet.natur.cuni.cz/ceval#block-block-17)

Licensing
---

CEval is distributed under the terms of [The GNU General Public License v3](https://www.gnu.org/licenses/gpl.html) (GNU GPLv3).

As permitted by section 7. *Additional Terms* of The GNU GPLv3 license, the authors require that any derivative work based on CEval clearly refers to the origin of the software and its authors. Such reference must include the address of this source code repository (https://github.com/echmet/CEval) and names of all authors and their affiliation stated in section [Authors](#Authors) of this README file.

<a name="Authors"></a>
Authors
---

Pavel Dubský  
Magda Ördögová  
Michal Malý  

Group of Electromigration and Chromatographic Methods (http://echmet.natur.cuni.cz)

Department of Physical and Macromolecular Chemistry  
Faculty of Science, Charles University in Prague, Czech Republic
