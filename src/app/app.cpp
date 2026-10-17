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

#include "GLContext.hpp"
#include "BirdVisApplication.hpp"
#include <QScreen>
#include <QSurfaceFormat>
#include <QDir>
#include <QIcon>
#include <QFileInfo>
#include <QDebug>
#include <exception>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(2, 1);
    format.setProfile(QSurfaceFormat::NoProfile);
    format.setStencilBufferSize(8);
    format.setDepthBufferSize(24);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);
    if (qEnvironmentVariableIsEmpty("R_HOME"))
        qputenv("R_HOME", BIRDVIS_R_HOME);
    if (qEnvironmentVariableIsEmpty("R_LIBS_USER"))
        qputenv("R_LIBS_USER", BIRDVIS_R_LIBS);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    try {
        const QString resources = qEnvironmentVariable("BIRDVIS_RESOURCES", BIRDVIS_RESOURCE_DIR);
        QStringList required = {"bvlogo.png", "usa_st.shp", "usa_st.shx", "usa_st.dbf",
                                "counties.shp", "counties.shx", "counties.dbf"};
        for (int size : {7,8,9,10,11,12,13,14,15,16,17,18,19,200}) {
            required << QString("fonts/arial_%1.description").arg(size)
                     << QString("fonts/arial_%1.sgi").arg(size);
        }
        QStringList missing;
        for (const auto &file : required)
            if (const QFileInfo info(QDir(resources).filePath(file));
                !info.isFile() || !info.isReadable()) missing << file;
        if (!missing.isEmpty()) {
            qCritical().noquote() << "Missing BirdVis resources in" << resources
                << "\nSet BIRDVIS_RESOURCES to the original resource directory.\n"
                << missing.join("\n");
            return 1;
        }
        BirdVisApplication app(argc, argv);
        app.setWindowIcon(QIcon(QDir(resources).filePath("birdvis_logo.icns")));
        GLContext window;
        window.resize(1440, 900);
        if (window.screen()) {
            const QSize available = window.screen()->availableGeometry().size();
            window.resize(window.size().boundedTo(available));
        }
        window.show();
        return app.exec();
    } catch (const std::exception &error) {
        qCritical() << "BirdVis startup failed:" << error.what();
        return 1;
    }
}
