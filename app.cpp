/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// Copyright 2011-2013 NYU Polytechnic School of Engineering.                      //
// Copyright 2010-2011 University of Utah.                                         //
//                                                                                 //
// Permission is hereby granted, free of charge, to any person obtaining a copy    //
// of this software and associated documentation files (the "Software"), to dea    //
// in the Software without restriction, including without limitation the rights    //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       //
// copies of the Software, and to permit persons to whom the Software is           //
// furnished to do so, subject to the following conditions:		           //
// 										   //
// The above copyright notice and this permission notice shall be included in	   //
// all copies or substantial portions of the Software.			           //
// 										   //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR	   //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,	   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          // 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
// THE SOFTWARE.                                                                   //
//                                                                                 // 
/////////////////////////////////////////////////////////////////////////////////////

#include <QApplication>
#include <QDesktopWidget>
#include <cstdlib>

#include "GLContext.hpp"    
#include "Labeling.hpp"
#include "Colormap1d.hpp"

int 
main(int argc, char *argv[])
{
    ////////////////////
    int numColors = 256;
    Colormap1d* colormap = Colormap1d::seqWhiteRedCont();
    
    float step = 1.0/numColors;

    for(int i = 0 ; i <= numColors ; ++i)
    {
	Color c = colormap->getColor(i*step);
	cout << c.r << "," << c.g << "," << c.b << endl;
    }



    ////////////
    Labeling lbl;
    double Q[] = {1,5,2,2.5,4,3};
    double W[] = {0.25,0.2,0.5,0.05}; 
    lbl.labelExtended(0,365,1,Q,6,true,W);
    lbl.labelExtended(-105.92159271240234,
		      259.07846069335938,
		      1,Q,6,true,W);

    // change environment R_HOME to be internal to the app folder
    char *pathvar = getenv("R_HOME");
    cout << "R_HOME = " << pathvar << endl;

    QApplication app(argc, argv);

    GLContext window;
    window.resize(window.sizeHint());
    

    int desktopArea = QApplication::desktop()->width() *
      QApplication::desktop()->height();
    int widgetArea = window.width() * window.height();
    if (((float)widgetArea / (float)desktopArea) < 0.75f)
      window.show();
    else
      window.showMaximized();
    return app.exec();
}
