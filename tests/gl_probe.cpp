#include <GL/glew.h>
#include <QApplication>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QTimer>
#include <QDebug>

class Probe : public QOpenGLWidget {
    bool checked = false;
    void initializeGL() override {
        const GLenum status = glewInit();
        qInfo() << "Qt" << qVersion() << "GL" << reinterpret_cast<const char*>(glGetString(GL_VERSION))
                << "format" << format() << "GLEW" << status;
        if (status != GLEW_OK || !GLEW_EXT_framebuffer_object || format().stencilBufferSize() < 8) {
            QTimer::singleShot(0, qApp, [] { QCoreApplication::exit(1); });
        }
    }
    void paintGL() override {
        while (glGetError() != GL_NO_ERROR) {}
        glDisable(GL_SCISSOR_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor3f(1, 0, 0);
        glBegin(GL_QUADS);
        glVertex2f(-1,-1); glVertex2f(1,-1); glVertex2f(1,1); glVertex2f(-1,1);
        glEnd();
        const GLenum error = glGetError();
        if (checked) return;
        checked = true;
        qInfo() << "Legacy drawing error" << error << "widget framebuffer" << defaultFramebufferObject()
                << "device pixel ratio" << devicePixelRatioF();
        QTimer::singleShot(100, this, [this, error] {
            const QImage result = grabFramebuffer();
            const QColor center = result.pixelColor(result.width()/2, result.height()/2);
            qInfo() << "Framebuffer center:" << center;
            QCoreApplication::exit(error == GL_NO_ERROR && center.red() > 240 && center.green() < 10 ? 0 : 1);
        });
    }
};

int main(int argc, char **argv) {
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(2,1);
    format.setProfile(QSurfaceFormat::NoProfile);
    format.setStencilBufferSize(8);
    format.setDepthBufferSize(24);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);
    QApplication app(argc, argv);
    Probe probe;
    probe.resize(200,200);
    probe.show();
    QTimer::singleShot(10000, &app, [] { QCoreApplication::exit(2); });
    return app.exec();
}
