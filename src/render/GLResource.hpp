#pragma once
#include <GL/glew.h>
#include <QOpenGLContext>
#include <QPointer>
#include <functional>

// GPU objects belong to a context share group, not to an arbitrary current context.
class GLResource {
    QPointer<QOpenGLContext> owner;
    QMetaObject::Connection cleanup;
public:
    GLuint texture = 0, list = 0;
    std::function<void()> invalidated;
    GLResource() = default;
    GLResource(const GLResource&) = delete;
    GLResource &operator=(const GLResource&) = delete;
    ~GLResource() { invalidated = {}; reset(); }
    bool valid() const {
        auto *current=QOpenGLContext::currentContext();
        return owner && current && (owner==current || QOpenGLContext::areSharing(owner,current));
    }
    void acquire() {
        if (valid()) return;
        reset();
        owner=QOpenGLContext::currentContext();
        if (owner) cleanup=QObject::connect(owner,&QOpenGLContext::aboutToBeDestroyed,owner,[this]{ reset(); },Qt::DirectConnection);
    }
    void reset() {
        QObject::disconnect(cleanup);
        auto *previous=QOpenGLContext::currentContext();
        QSurface *surface=previous ? previous->surface() : nullptr;
        bool current=valid();
        if (!current && owner && owner->surface()) current=owner->makeCurrent(owner->surface());
        if (current) {
            if (texture) glDeleteTextures(1,&texture);
            if (list) glDeleteLists(list,1);
        }
        if (owner && previous!=QOpenGLContext::currentContext()) {
            owner->doneCurrent();
            if(previous && surface) previous->makeCurrent(surface);
        }
        texture=list=0;
        owner=nullptr;
        if(invalidated) invalidated();
    }
};
