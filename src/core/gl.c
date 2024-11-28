#include "gl.h"
#include <assert.h>

PFNGLDELETEBUFFERS glDeleteBuffers = NULL;
PFNGLGENBUFFERS glGenBuffers = NULL;
PFNGLBINDBUFFER glBindBuffer = NULL;
PFNGLBUFFERDATA glBufferData = NULL;
PFNGLUSEPROGRAM glUseProgram = NULL;
PFNGLCREATESHADER glCreateShader = NULL;
PFNGLSHADERSOURCE glShaderSource = NULL;
PFNGLCOMPILESHADER glCompileShader = NULL;
PFNGLCREATEPROGRAM glCreateProgram = NULL;
PFNGLLINKPROGRAM glLinkProgram = NULL;
PFNGLATTACHSHADER glAttachShader = NULL;
PFNGLDELETEPROGRAM glDeleteProgram = NULL;
PFNGLDELETESHADER glDeleteShader = NULL;
PFNGLGENRENDERBUFFERS glGenRenderbuffers = NULL;
PFNGLBINDFRAMEBUFFER glBindFramebuffer = NULL;
PFNGLFRAMEBUFFERRENDERBUFFER glFramebufferRenderbuffer = NULL;
PFNGLFRAMEBUFFERTEXTURE glFramebufferTexture = NULL;
PFNGLBINDVERTEXARRAY glBindVertexArray = NULL;
PFNGLGENVERTEXARRAYS glGenVertexArrays = NULL;
PFNGLDELETEVERTEXARRAYS glDeleteVertexArrays = NULL;
PFNGLVERTEXATTRIBPOINTER glVertexAttribPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAY glEnableVertexAttribArray = NULL;
PFNGLDISABLEVERTEXATTRIBARRAY glDisableVertexAttribArray = NULL;
PFNGLVERTEXATTRIBDIVISOR glVertexAttribDivisor = NULL;
PFNGLDRAWARRAYSINSTANCED glDrawArraysInstanced = NULL;
PFNGLGENFRAMEBUFFERS glGenFramebuffers = NULL;
PFNGLFRAMEBUFFERTEXTURE2D glFramebufferTexture2D = NULL;
PFNGLGETSHADERIV glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOG glGetShaderInfoLog = NULL;
PFNGLGETPROGRAMIV glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOG glGetProgramInfoLog = NULL;

#define INIT_OPENGL_FUNCTION(func, func_type) \
    func = (func_type)glfwGetProcAddress(#func); \
    assert(func);

void at_init_opengl() {
    INIT_OPENGL_FUNCTION(glDeleteBuffers, PFNGLDELETEBUFFERS);
    INIT_OPENGL_FUNCTION(glGenBuffers, PFNGLGENBUFFERS);
    INIT_OPENGL_FUNCTION(glBindBuffer, PFNGLBINDBUFFER);
    INIT_OPENGL_FUNCTION(glBufferData, PFNGLBUFFERDATA);
    INIT_OPENGL_FUNCTION(glUseProgram, PFNGLUSEPROGRAM);
    INIT_OPENGL_FUNCTION(glCreateShader, PFNGLCREATESHADER);
    INIT_OPENGL_FUNCTION(glShaderSource, PFNGLSHADERSOURCE);
    INIT_OPENGL_FUNCTION(glCompileShader, PFNGLCOMPILESHADER);
    INIT_OPENGL_FUNCTION(glCreateProgram, PFNGLCREATEPROGRAM);
    INIT_OPENGL_FUNCTION(glLinkProgram, PFNGLLINKPROGRAM);
    INIT_OPENGL_FUNCTION(glAttachShader, PFNGLATTACHSHADER);
    INIT_OPENGL_FUNCTION(glDeleteProgram, PFNGLDELETEPROGRAM);
    INIT_OPENGL_FUNCTION(glDeleteShader, PFNGLDELETESHADER);
    INIT_OPENGL_FUNCTION(glGenRenderbuffers, PFNGLGENRENDERBUFFERS);
    INIT_OPENGL_FUNCTION(glBindFramebuffer, PFNGLBINDFRAMEBUFFER);
    INIT_OPENGL_FUNCTION(glFramebufferRenderbuffer, PFNGLFRAMEBUFFERRENDERBUFFER);
    INIT_OPENGL_FUNCTION(glFramebufferTexture, PFNGLFRAMEBUFFERTEXTURE);
    INIT_OPENGL_FUNCTION(glBindVertexArray, PFNGLBINDVERTEXARRAY);
    INIT_OPENGL_FUNCTION(glGenVertexArrays, PFNGLGENVERTEXARRAYS);
    INIT_OPENGL_FUNCTION(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYS);
    INIT_OPENGL_FUNCTION(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTER);
    INIT_OPENGL_FUNCTION(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAY);
    INIT_OPENGL_FUNCTION(glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAY);
    INIT_OPENGL_FUNCTION(glVertexAttribDivisor, PFNGLVERTEXATTRIBDIVISOR);
    INIT_OPENGL_FUNCTION(glDrawArraysInstanced, PFNGLDRAWARRAYSINSTANCED);
    INIT_OPENGL_FUNCTION(glGenFramebuffers, PFNGLGENFRAMEBUFFERS);
    INIT_OPENGL_FUNCTION(glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2D);
    INIT_OPENGL_FUNCTION(glGetShaderiv, PFNGLGETSHADERIV);
    INIT_OPENGL_FUNCTION(glGetShaderInfoLog, PFNGLGETSHADERINFOLOG);
    INIT_OPENGL_FUNCTION(glGetProgramiv, PFNGLGETPROGRAMIV);
    INIT_OPENGL_FUNCTION(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOG);
}
