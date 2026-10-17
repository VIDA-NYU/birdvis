#include "GLContext.hpp"
#include "AreaEstimator.hpp"
#include "BirdVisApplication.hpp"
#include <QTimer>
#include <QDir>
#include <QDebug>
#include <QTest>
#include <QInputDialog>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QColorDialog>
#include <memory>
#include <stdexcept>
#include <functional>

template<class Dialog>
void interactDialog(const std::function<void()> &trigger, const std::function<void(Dialog*)> &respond) {
    bool reached=false;
    QTimer poll;
    QObject::connect(&poll,&QTimer::timeout,[&]{
        for(QWidget *widget:QApplication::topLevelWidgets()) {
            auto *dialog=qobject_cast<Dialog*>(widget);
            if(!dialog || !dialog->isVisible()) continue;
            reached=true; poll.stop(); respond(dialog); break;
        }
    });
    poll.start(25);
    trigger();
    poll.stop();
    if(!reached) throw std::runtime_error("Expected dialog was not reached");
}

int main(int argc, char **argv) {
    qputenv("R_HOME", BIRDVIS_R_HOME); qputenv("R_LIBS_USER", BIRDVIS_R_LIBS);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
    QSurfaceFormat f; f.setVersion(2,1); f.setProfile(QSurfaceFormat::NoProfile);
    f.setStencilBufferSize(8); f.setDepthBufferSize(24); QSurfaceFormat::setDefaultFormat(f);
    BirdVisApplication app(argc,argv);
    QDir::setCurrent(QDir::tempPath()); // Verify source-configured paths outside the checkout.
    auto holder=std::make_unique<GLContext>();
    GLContext &window=*holder; window.resize(1200,800); window.show();
    QTimer::singleShot(500, &window, [&] {
        try {
            window.makeCurrent();
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,window.defaultFramebufferObject());
            glViewport(3,4,99,101); glEnable(GL_SCISSOR_TEST); glScissor(1,2,3,4);
            GLint original=0; glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT,&original);
            const GLuint list=glGenLists(1); glNewList(list,GL_COMPILE);
            glBegin(GL_QUADS); glVertex2f(0,0); glVertex2f(0.5,0); glVertex2f(0.5,1); glVertex2f(0,1); glEnd(); glEndList();
            const double fraction=estimateAreaFraction(list,0,0,1,1);
            GLint restored=0, viewport[4]; glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT,&restored); glGetIntegerv(GL_VIEWPORT,viewport);
            if(std::abs(fraction-0.5)>0.002 || restored!=original || viewport[0]!=3 || viewport[2]!=99 || !glIsEnabled(GL_SCISSOR_TEST))
                throw std::runtime_error("Area calculation or state restoration failed");
            glDeleteLists(list,1); glDisable(GL_SCISSOR_TEST);
            window.update(); QTest::qWait(100);
            const QString output=argc>1 ? QString::fromLocal8Bit(argv[1]) : "native-initial.png";
            if(!window.grabFramebuffer().save(output)) throw std::runtime_error("Cannot save screenshot");
            auto drag = [&](QPoint from, QPoint to, Qt::MouseButton button = Qt::LeftButton) {
                QTest::mouseMove(&window, from); QTest::qWait(50); QTest::mousePress(&window, button, Qt::NoModifier, from);
                QTest::mouseMove(&window, from+QPoint(10,5), 30);
                QMouseEvent move(QEvent::MouseMove, to, window.mapToGlobal(to), Qt::NoButton, button, Qt::NoModifier);
                QApplication::sendEvent(&window, &move);
                QTest::mouseRelease(&window, button, Qt::NoModifier, to);
                QTest::qWait(200);
            };
            interactDialog<QInputDialog>([&]{ drag(QPoint(50,40),QPoint(500,300)); },
                [](QInputDialog *dialog){ dialog->accept(); });
            drag(QPoint(50,201), QPoint(500,300));
            window.grabFramebuffer().save(output+"-single.png");
            auto snapshot = [&](const QString &name) {
                window.update(); QTest::qWait(100);
                if (!window.grabFramebuffer().save(output+"-"+name+".png")) throw std::runtime_error("Screenshot failed");
            };
            if (!Global::instance()->getRepository()->getFile(0)->loaded)
                throw std::runtime_error("Species drop did not load a dataset");
            drag(QPoint(50,421), QPoint(500,400)); // Bobolink, from the original data set.
            if (!Global::instance()->getRepository()->getFile(13)->loaded)
                throw std::runtime_error("Replacement species drop failed");
            for (Qt::Key key : {Qt::Key_Right, Qt::Key_Left, Qt::Key_M, Qt::Key_I, Qt::Key_S, Qt::Key_S})
                QTest::keyClick(&window,key);
            drag(QPoint(500,400),QPoint(550,420)); // Pan.
            QWheelEvent wheel(QPointF(550,420), window.mapToGlobal(QPoint(550,420)), QPoint(), QPoint(0,120), Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
            QApplication::sendEvent(&window,&wheel);
            QWheelEvent trackpad(QPointF(550,420),window.mapToGlobal(QPoint(550,420)),QPoint(0,-15),QPoint(),Qt::NoButton,Qt::NoModifier,Qt::ScrollUpdate,false);
            QApplication::sendEvent(&window,&trackpad);
            drag(QPoint(500,400),QPoint(650,500),Qt::RightButton);
            snapshot("selection");
            QMouseEvent selectionMove(QEvent::MouseMove,QPoint(570,450),window.mapToGlobal(QPoint(570,450)),Qt::NoButton,Qt::NoButton,Qt::NoModifier);
            QApplication::sendEvent(&window,&selectionMove);
            QTest::keyClick(&window,Qt::Key_Delete);
            window.update(); QTest::qWait(100);
            const QImage deleted=window.grabFramebuffer();
            const QColor selectionPixel=deleted.pixelColor(570*deleted.width()/window.width(),450*deleted.height()/window.height());
            if(selectionPixel.blue()>selectionPixel.red()) throw std::runtime_error("Selection deletion failed");
            auto mode = [&](int y, const QString &dimensions) {
                interactDialog<QInputDialog>([&]{drag(QPoint(50,y),QPoint(500,300));},
                    [&](QInputDialog *dialog){dialog->setTextValue(dimensions);dialog->accept();});
            };
            auto movePointer = [&](QPoint point) {
                QMouseEvent event(QEvent::MouseMove,point,window.mapToGlobal(point),Qt::NoButton,Qt::NoButton,Qt::NoModifier);
                QApplication::sendEvent(&window,&event);
            };
            movePointer(QPoint(500,775));
            interactDialog<QInputDialog>([&]{QTest::keyClick(&window,Qt::Key_X);},
                [](QInputDialog *dialog){dialog->setTextValue("seq(0, 1, 0.25)");dialog->accept();});
            QTest::qWait(100);
            movePointer(QPoint(252,763));
            interactDialog<QColorDialog>([&]{QTest::keyClick(&window,Qt::Key_Space);},
                [](QColorDialog *dialog){dialog->setCurrentColor(QColor(240,240,255));dialog->accept();});
            snapshot("colormap");
            drag(QPoint(252,763),QPoint(275,763));
            // Cancelling an environment dialog must leave the existing view installed.
            const QImage beforeCancel=window.grabFramebuffer();
            interactDialog<QInputDialog>([&]{drag(QPoint(50,57),QPoint(500,300));},
                [](QInputDialog *dialog){dialog->reject();});
            if(beforeCancel.size()!=window.grabFramebuffer().size()) throw std::runtime_error("Cancel changed layout size");
            float searchBounds[4];
            if(!Global::instance()->keyword_bounds("Florida",searchBounds) || searchBounds[2]<=0)
                throw std::runtime_error("State search failed");
            if(!Global::instance()->keyword_bounds("Graham, Kansas",searchBounds))
                throw std::runtime_error("County search failed");
            mode(74,"1 1");
            drag(QPoint(50,421),QPoint(500,400));
            for(int i=0;i<24;++i) QTest::keyClick(&window,Qt::Key_Right);
            snapshot("variation");
            mode(57,"1 2");
            drag(QPoint(50,421),QPoint(400,400));
            drag(QPoint(50,387),QPoint(950,400));
            for(int i=0;i<24;++i) QTest::keyClick(&window,Qt::Key_Right);
            snapshot("multi");
            window.resize(1000,700); QTest::qWait(100); snapshot("resized");
            window.showMinimized(); QTest::qWait(100); window.showNormal(); QTest::qWait(100);
            window.makeCurrent();
            const GLenum error=glGetError();
            if(error!=GL_NO_ERROR) throw std::runtime_error("OpenGL error after workflows: " + std::to_string(error));
            qInfo() << "Area/state check passed; screenshot" << output << "DPR" << window.devicePixelRatioF();
            holder.reset();
            GLContext replacement; replacement.resize(1000,700); replacement.show();
            QTest::qWait(200);
            if(!replacement.grabFramebuffer().save(output+"-recreated.png"))
                throw std::runtime_error("Context recreation failed");
            app.exit(0);
        } catch(const std::exception &e) { qCritical()<<e.what(); app.exit(1); }
    });
    QTimer::singleShot(60000,&app,[]{ qFatal("Native check timed out"); });
    return app.exec();
}
