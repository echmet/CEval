CEval
===

Introduction
---

CEval (Capillary Electrophoresis Evaluator) is a multiplatform open-source tool to aid with evaluation of experimental results obtained by Capillary Zone Electrophoresis with specific aim at Affinity Capillary Electrophoresis. CEval can perform basic evaluation of electrophoregrams and calculate some common parameters of peaks. Additionally, CEval can fit peaks with Haaroff - van der Linde function to allow for better estimation of migration times.

In ACE mode CEval can assist with evaluation of electrophoretic mobilities and complexation constants of analytes with selectors by the means of nonlinear regression. Statistical evaluation of calculated results is also provided in graphical form.

CEval binaries and a user guide can be downloaded from the [ECHMET research group website](https://echmet.natur.cuni.cz/#portfolio).

In case the primary ECHMET group site is down you may download CEval binaries from a [backup site](https://devoid-pointer.net/echmet_fallback)

Dependencies
---

CEval makes use of the following toolkits:

- [Qt 5 toolkit](http://www.qt.io/)
- [Boost](http://www.boost.org/)
- [Qwt](http://qwt.sourceforge.net/)
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
- [ECHMET Data Import Infrastructure](https://github.com/echmet/EDII) (\*)
- [ECHMETUpdateCheck](https://github.com/echmet/ECHMETUpdateCheck)

All of the libraries and packages above are hard dependencies. CEval cannot be built or run without them.

\* EDII is a separate program that is supposed to run as a background service that CEval uses to load data. As such, CEval does not need to link against EDII.

Build
---

Prior to building CEval, all hard dependencies have to be available in binary form, either by building them from source or by obtaining an appropriate pre-built package (the binary form requirement does not apply to the Boost and Eigen libraries). Please refer to web presentations of the respective projects for further instructions.

CEval uses the [CMake](https://cmake.org) build system. To generate Makefiles on a UNIX system, `cd` into the directory with CEval source code and issue the following commands

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DQWT_DIR=<path_to_where_qwt_is_installed>> -DECHMETUPDATECHECK_DIR=<path_to_where_ECHMETUpdateCheck_is_installed> -DEDII_DIR=<path_to_where_EDII_is_installed> -DEIGEN_DIR=<path_to_Eigen_headers> -DBOOST_DIR=<path_to_Boost_headers>
    make

If any of CEval's dependencies are installed as system-wide dependencies, their installation path does not need to be set explicitly.

On a Windows system you may use the CMake GUI tool to set up the project variables and generate Makefiles or Microsoft Visual Studio project files. Note that CEval expects at least GCC 4.9.2 or MSVC14 (part of Microsoft Visual Studio 2015).

Usage
---
Tutorial for CEval can be obtained from [here](https://echmet.natur.cuni.cz/assets/files/CEval_Manual.pdf). Please keep in mind that the tutorial corresponds to the binary version that is available for download at the ECHMET website and may not reflect the state of the latest development code.

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
