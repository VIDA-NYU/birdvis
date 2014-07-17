BirdVis
=======

Source code for the BirdVis project, for more information visit www.birdvis.org

1) Compilation Instructions

   This code has the following dependencies: Qt4, Boost, R, Rcpp, RInside, freeimage and gdal.
In linux, one can install Qt4, Boost, R, freeimage and gdal by apt-get. To install Rcpp and RInside,
run R from your terminal and use install.packages("Rcpp") and install.packages("RInside"). After
all dependencies have been installed simply run 
  > qmake birdvis.pro
  > make
