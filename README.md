CEval
===

Introduction
---

CEval (Capillary Electrophoresis Evaluator) is a multiplatform open-source tool to aid with evaluation of experimental results obtained by Capillary Zone Electrophoresis with specific aim at Affinity Capillary Electrophoresis. CEval can perform basic evaluation of electrophoregrams and calculate some common parameters of peaks. Additionally, CEval can fit peaks with Haaroff - van der Linde function to allow for better estimation of migration times.

In ACE mode CEval can assist with evaluation of electrophoretic mobilities and complexation constants of analytes with selectors by the means of nonlinear regression. Statistical evaluation of calculated results is also provided in graphical form.

Dependencies
---

CEval makes use of the following toolkits:

- [Qt 5 toolkit](http://www.qt.io/)
- [Boost](http://www.boost.org/)
- [Qwt](http://qwt.sourceforge.net/)
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
- [HVL_MT](https://github.com/echmet/HVL_MT)
- [ECHMET Data Import Infrastructure](https://github.com/echmet/EDII)

All of the libraries and packages above are hard dependencies. CEval cannot be built or run without them.

Build
---

Prior to building CEval, all hard dependencies have to be available in binary form, either by building them from source or by obtaining an appropriate pre-built package (the binary form requirement does not apply to the Boost and Eigen libraries). Please refer to web presentations of the respective projects for further instructions.

1. Setting up paths to build dependencies

  Paths to build dependencies can be set in **CEval.pri** file. The variables to set are following:

  - `HVLPATH` - Path to the directory which contains installation of the **HVL_MT** library.
  - `BOOSTPATH` - Path to the directory with the Boost library headers.
  - `QWTPATH` - Path to the **features/qwt.prf** file.
  - `EIGENPATH` - Path to the directory where the Eigen library is located.
  - `EDIIPATH` - Path containing the EDII public API header files

**Hint:** If you intend to modify CEval and commit your changes to git, we suggest you run

`git update-index --assume-unchanged CEval.pri`

prior to making any changes to the `CEval.pri` file. This will make git exclude any changes made to the file while still being aware of any upstream changes to that file.

2. Building

  ### Linux/UNIX

  After you have set up the paths as instructed above, `cd` to the directory with CEval sources and run the following:

       mkdir build
       cd build
       qmake "CONFIG+=Release" ../CEval.pro
       make

  **Remark:** Linux is the only UNIX-based system so far that has been tested to build and run CEval.

  ### Windows

  It is recommended to install [QtCreator](https://www.qt.io/ide/) and use its user interface to set the project up and build it. CEval has been tested to build with both [MinGW toolchain](http://www.mingw.org/) and Microsoft VC++14 (part of [Microsoft Visual Studio 2015](https://www.visualstudio.com/)). Microsoft VC++14 is the preferred compiler on Windows. Development package for Windows downloadable from the [Qt project's](http://qt.io) website ships Qt 5 binaries linkable against MinGW and VC++14 binaries and the MinGW compiler, Microsoft VC++14 compiler has to be obtained separately.


3. Run

  If the `Qwt`, `libHPCS` and `libhvl_mt` libraries were built to be linked dynamically (the default), it is necessary to make the libraries (usually named **qwt.so, qwt.dll, qwt.dylib**; **libHPCS.so, libHPCS.dll, libHPCS.dylib** and **libhvl_mt.so, libhvl_mt.dll, libhvl_mt.dylib**) available to the operating system's dynamic linker. This can be achieved by various ways. The libraries can be copied to a directory where the linker is set up to look (i.e. **/usr/lib, C:\Windows\system32**) or to the directory which contains the CEval binary. On Linux systems the **LD_LIBRARY_PATH** environment variable can be adjusted to include paths to the necessary libraries, Windows users may adjust the **PATH** variable in the same manner.

As CEval cannot function without the EDII service it prompts the user for a path to EDII service binary when the path is either not set or seems invalid. CEval will attempt to start the EDII service only when the service is not already running.

Pre-built binaries
---
Pre-built binaries for commonly used platforms can be downloaded from the link below. Please note that the binaries may be considerably out of date.

[ECHMET website](https://echmet.natur.cuni.cz/software/download#ceval)

Usage
---
Tutorial for CEval can be obtained from here. Please keep in mind that the tutorial corresponds to the binary version that is available for download at the ECHMET website and may not reflect the state of the latest development code.

[ECHMET website](https://echmet.natur.cuni.cz/software/download#ceval)

Licensing
---

CEval is distributed under the terms of [The GNU General Public License v3](https://www.gnu.org/licenses/gpl.html) (GNU GPLv3).

As permitted by section 7. *Additional Terms* of The GNU GPLv3 license, the authors require that any derivative work based on CEval clearly refers to the origin of the software and its authors. Such reference must include the address of this source code repository (https://github.com/echmet/CEval) and names of all authors and their affiliation stated in section [Authors](#Authors) of this README file.

<a name="Authors"></a>
Authors
---

Pavel Dubský  
Magda Dovhunová  
Michal Malý  

Group of Electromigration and Chromatographic Methods (http://echmet.natur.cuni.cz)

Department of Physical and Macromolecular Chemistry  
Faculty of Science, Charles University in Prague, Czech Republic
