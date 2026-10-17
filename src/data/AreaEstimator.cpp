#include "AreaEstimator.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace {
struct SavedState {
    GLint draw = 0, read = 0, matrix = 0, packBuffer = 0, unpackBuffer = 0;
    const bool separate = GLEW_EXT_framebuffer_blit;
    SavedState() {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &draw);
        if (separate) glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING_EXT, &read);
        glGetIntegerv(GL_MATRIX_MODE, &matrix);
        glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &packBuffer);
        glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &unpackBuffer);
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
        glMatrixMode(GL_PROJECTION); glPushMatrix();
        glMatrixMode(GL_MODELVIEW); glPushMatrix();
    }
    ~SavedState() {
        if (separate) {
            glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, draw);
            glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, read);
        } else glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, draw);
        glMatrixMode(GL_MODELVIEW); glPopMatrix();
        glMatrixMode(GL_PROJECTION); glPopMatrix();
        glMatrixMode(matrix);
        glPopClientAttrib();
        glPopAttrib();
        glBindBuffer(GL_PIXEL_PACK_BUFFER, packBuffer);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, unpackBuffer);
    }
};
struct Target {
    GLuint framebuffer = 0, texture = 0;
    ~Target() {
        glDeleteFramebuffersEXT(1, &framebuffer);
        glDeleteTextures(1, &texture);
    }
};
}

double estimateAreaFraction(GLuint geometry, double x, double y, double w, double h) {
    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(w) || !std::isfinite(h)
        || w < 0 || h < 0 || !std::isfinite(x+w) || !std::isfinite(y+h))
        throw std::invalid_argument("Area estimate requires a finite positive rectangle");
    if (w == 0 || h == 0) return 0;
    if (!glGetString(GL_VERSION) || !GLEW_EXT_framebuffer_object)
        throw std::runtime_error("Area estimate requires a current framebuffer-capable OpenGL context");
    GLint maximum = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maximum);
    maximum = std::min(maximum, 4096);
    // Bound each dimension before conversion, including extremely thin selections.
    const double aspect = w / h;
    const int height = int(std::clamp(std::sqrt(1e6 / aspect), 1.0, double(maximum)));
    const int width = int(std::clamp(height * aspect, 1.0, double(maximum)));
    std::vector<unsigned char> pixels(size_t(width) * height * 4);
    SavedState saved;
    Target target;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glGenTextures(1, &target.texture);
    glBindTexture(GL_TEXTURE_2D, target.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenFramebuffersEXT(1, &target.framebuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, target.framebuffer);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, target.texture, 0);
    if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
        throw std::runtime_error("Cannot allocate area-estimation framebuffer");
    glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
    glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
    for (GLenum cap : {GL_BLEND, GL_DEPTH_TEST, GL_STENCIL_TEST, GL_SCISSOR_TEST,
                      GL_ALPHA_TEST, GL_LIGHTING, GL_CULL_FACE, GL_TEXTURE_2D, GL_COLOR_LOGIC_OP,
                      GL_POLYGON_SMOOTH, GL_DITHER, GL_FOG, GL_POLYGON_STIPPLE, GL_TEXTURE_1D, GL_TEXTURE_3D}) glDisable(cap);
    for(int i=0;i<6;++i) glDisable(GL_CLIP_PLANE0+i);
    glPixelTransferi(GL_MAP_COLOR,GL_FALSE);
    for(GLenum scale : {GL_RED_SCALE,GL_GREEN_SCALE,GL_BLUE_SCALE,GL_ALPHA_SCALE}) glPixelTransferf(scale,1);
    for(GLenum bias : {GL_RED_BIAS,GL_GREEN_BIAS,GL_BLUE_BIAS,GL_ALPHA_BIAS}) glPixelTransferf(bias,0);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); glOrtho(x, x+w, y, y+h, -10, 10);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glClearColor(0, 0, 0, 1); glClear(GL_COLOR_BUFFER_BIT);
    glColor4f(1, 1, 1, 1);
    glCallList(geometry);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    size_t filled = 0;
    for (size_t i = 0; i < pixels.size(); i += 4) filled += pixels[i] != 0;
    return double(filled) / (size_t(width) * height);
}
