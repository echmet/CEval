CEval
===

Introduction
---

CEval (Capillary Electrophoresis Evaluator) is a multiplatform open-source tool to aid with evaluation of experimental results obtained by Capillary Zone Electrophoresis with specific aim at Affinity Capillary Electrophoresis. CEval can perform basic evaluation of electropherograms and calculate some common parameters of peaks. Additionally, CEval can fit peaks with Haaroff - van der Linde function to allow for better estimation of migration times.

In ACE mode CEval can assist with evaluation of electrophoretic mobilities and complexation constants of analytes with selectors by the means of nonlinear regression. Statistical evaluation of calculated results is also provided in graphical form.

Dependencies
---

CEval makes use of the following tools

- [Qt 5 toolkit](https://www.qt.io/)
- [Boost](http://www.boost.org/)
- [Qwt](http://qwt.sourceforge.net/)
- [libHPCS](https://github.com/echmet/libHPCS)
- [HVL_MT](https://github.com/echmet/HVL_MT)

Qt 5 toolkit, Boost, Qwt and libHPCS are required to build CEval. HVL_MT is an optional dependency which is loaded at runtime. HVL_MT is required for fitting peaks with HVL function.

Build
---

Prior to building CEval, all hard dependencies have to be available it a binary form, either by building them from source or by obtaining an appropriate pre-built package. Refer to web presentations of the respective projects for further instructions.

1. Setting up paths to build dependencies

  Paths to build dependencies can be set in **CEval.pri** file. The variables to set are following:

  - HPCSPATH - Path to the directory which contains both **libHPCS** shared library and **libHPCS.h** public header file
  - BOOSTPATH - Path to the directory with the Boost library headers
  - QWTPATH - Path to the **features/qwt.prf** file

2. Building

  ### Linux/UNIX

  After you have set up the paths as instructed above, cd to the directory with CEval sources and run the following:

       mkdir build
       cd build
       qmake "CONFIG+=Release" ../CEval.pro
       make

  ### Windows

  It is recommended to install [QtCreator](https://www.qt.io/ide/) and use its user interface to set the project up and build it. While it might be possible to use other compilers, the authors recommend to use the [MinGW toolchain](http://www.mingw.org/) to build CEval.

  Remark: Linux is the only UNIX-based system so far that has been tested to build and run CEval.

3. Run

  In order for CEval to use the **HVL_MT** library, the hvl_mt library (usually named **hvl_mt.so, hvl_mt.dll, hvl_mt.dylib**) must be placed in the same directory as CEval's executable.

Pre-build packages

Pre-build packages are available for both Windows and Linux and can be downloaded from here:

<- TODO: Přidat link ->

Usage
---
<- TODO: Asi link na ECHMETí web, až tam budou tutoriály atp. ->

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
