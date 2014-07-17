#####################################################################################
##                                                                                 ##
## Copyright 2011-2013 NYU Polytechnic School of Engineering.                      ##
## Copyright 2010-2011 University of Utah.                                         ##
##                                                                                 ##
## Permission is hereby granted, free of charge, to any person obtaining a copy    ##
## of this software and associated documentation files (the "Software"), to dea    ##
## in the Software without restriction, including without limitation the rights    ##
## to use, copy, modify, merge, publish, distribute, sublicense, and#or sell       ##
## copies of the Software, and to permit persons to whom the Software is           ##
## furnished to do so, subject to the following conditions:		           ##
## 										   ##
## The above copyright notice and this permission notice shall be included in	   ##
## all copies or substantial portions of the Software.			           ##
## 										   ##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR	   ##
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,	   ##
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     ##
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          ## 
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   ##
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       ##
## THE SOFTWARE.                                                                   ##
##                                                                                 ## 
#####################################################################################


message()
message(BirdVis qmake)

HEADERS = \
        Array.hpp \
	Curve.hpp \
	DataSet.hpp \
	Dataflow.hpp \
	DataflowTypes.hpp \
        GLContext.hpp \
	Global.hpp \
	Labeling.hpp \
	Locations.hpp \
	MeasureSet.hpp \
	RenderSystem.hpp \
	PlotModules.hpp \
	Species.hpp \
	Spreadsheet.hpp \
	Environment.hpp \
	SimpleEnvironment.hpp \
	BasicModules.hpp \
	TagCloudModules.hpp \
	ColormapModules.hpp \
        RectangleLayout.hpp \
        EventSystem.hpp \
	Tokenizer.hpp \
        Regions.hpp \
	Vec3.hpp \
	View.hpp \
        Matrix.hpp \
        Vector.hpp \
        Rotation.hpp \
        Color.hpp \
        ViewRect.hpp \
        Colormap1d.hpp \
	csv2grid.hpp \
	datatypes.hpp \
	bvdata.hpp \
	font.h \
	gl_util.hpp \
	image.h \
	modules.hpp \
	Texture.hpp \
	readrgb.h \
	Triangulator.h \
        Util.hpp

SOURCES = \
	Array.cpp \
	Curve.cpp \
	DataSet.cpp \
	Dataflow.cpp \
	DataflowTypes.cpp \
        GLContext.cpp \
        Global.cpp \
	Labeling.cpp \
        RectangleLayout.cpp \
	Spreadsheet.cpp \
	Environment.cpp \
	SimpleEnvironment.cpp \
        Matrix.cpp \
        Vector.cpp \
	ColormapModules.cpp \
	BasicModules.cpp \
        modules.cpp \
        Rotation.cpp \
        Regions.cpp \
	PlotModules.cpp \
        EventSystem.cpp \
	Locations.cpp \
	MeasureSet.cpp \
	RenderSystem.cpp \
	Species.cpp \
	TagCloudModules.cpp \
	Tokenizer.cpp \
	View.cpp \
        Color.cpp \
        ViewRect.cpp \
        Colormap1d.cpp \
	gl_util.cpp \
	bvdata.cpp \
	app.cpp \
	csv2grid.cpp \
	datatypes.cpp \
	Texture.cpp \
	font.c \
	image.c \
	readrgb.c \
	Triangulator.cpp \
        Util.cpp

QT += opengl 

# using pkg-config packages
CONFIG    += link_pkgconfig debug

# R, Rcpp, and Rinside are included by hand
message(RHOME    $${R_HOME})
RLDFLAGS     = $$system(R CMD config --ldflags)
RBLAS        = $$system(R CMD config BLAS_LIBS)
RLAPACK      = $$system(R CMD config LAPACK_LIBS)
RCPPLIBS     = $$system("echo 'Rcpp:::LdFlags()'     | R --vanilla --slave")
RINSIDELIBS  = $$system("echo 'RInside:::LdFlags()'  | R --vanilla --slave")

RCPPFLAGS    = $$system(R CMD config --cppflags)
RCPPFLAGS    = $$replace(RCPPFLAGS,"-I","")  
RCPPINCL     = $$system("echo 'Rcpp:::CxxFlags()'    | R --vanilla --slave")
RCPPINCL     = $$replace(RCPPINCL,"-I","")   
RINSIDEINCL  = $$system("echo 'RInside:::CxxFlags()' | R --vanilla --slave")
RINSIDEINCL  = $$replace(RINSIDEINCL,"-I","")

# message(RCPPFLAGS    $${RCPPFLAGS})
# message(RCPPINCL     $${RCPPINCL})
# message(RINSIDEINCL  $${RINSIDEINCL})

QMAKE_LIBS = \
   $${RLDFLAGS} \
   $${RLAPACK} \
   $${RCPPLIBS} \
   $${RINSIDELIBS}

INCLUDEPATH += \
    $${RCPPFLAGS} \
    $${RCPPINCL} \
    $${RINSIDEINCL} \
    /usr/include/gdal

LIBS += -lgdal \
        -lfreeimage \
        -lGLU \
        -lGLEW

message(directory $${INCLUDEPATH})

# install
target.path   = .
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS birdvis.pro
sources.path  = .
INSTALLS     += target sources
