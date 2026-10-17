#pragma once
#include <GL/glew.h>

// Requires a current legacy OpenGL context. Preserves caller framebuffer and GL state.
double estimateAreaFraction(GLuint geometry, double x, double y, double w, double h);
