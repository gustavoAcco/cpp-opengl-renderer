#pragma once

#include <glad/gl.h>
#include <cstdio>
#include <cstdlib>

#ifndef NDEBUG
    #define GL_CHECK(call)                                                   \
        do {                                                                  \
            call;                                                             \
            GLenum _err = glGetError();                                       \
            if (_err != GL_NO_ERROR) {                                        \
                std::fprintf(stderr, "OpenGL error 0x%04x at %s:%d\n",       \
                             _err, __FILE__, __LINE__);                       \
                std::abort();                                                 \
            }                                                                 \
        } while (false)
#else
    #define GL_CHECK(call) call
#endif
