#pragma once

// abstract-trader https://github.com/ougi-washi/abstract-trader

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stddef.h>

typedef void (APIENTRY * PFNGLDELETEBUFFERS)(GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERS)(GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBINDBUFFER)(GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLBUFFERDATA)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRY * PFNGLUSEPROGRAM)(GLuint program);
typedef GLuint(APIENTRY * PFNGLCREATESHADER)(GLenum type);
typedef void (APIENTRY * PFNGLSHADERSOURCE)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
typedef void (APIENTRY * PFNGLCOMPILESHADER)(GLuint shader);
typedef GLuint(APIENTRY * PFNGLCREATEPROGRAM)(void);
typedef void (APIENTRY * PFNGLLINKPROGRAM)(GLuint program);
typedef void (APIENTRY * PFNGLATTACHSHADER)(GLuint program, GLuint shader);
typedef void (APIENTRY * PFNGLDELETEPROGRAM)(GLuint program);
typedef void (APIENTRY * PFNGLDELETESHADER)(GLuint shader);
typedef void (APIENTRY * PFNGLGENRENDERBUFFERS)(GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRY * PFNGLBINDFRAMEBUFFER)(GLenum target, GLuint framebuffer);
typedef void (APIENTRY * PFNGLFRAMEBUFFERRENDERBUFFER)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE)(GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (APIENTRY * PFNGLBINDTEXTURE)(GLenum target, GLuint texture);
typedef void (APIENTRY * PFNGLDELETETEXTURES)(GLsizei n, const GLuint *textures);
typedef void (APIENTRY * PFNGLGENTEXTURES)(GLsizei n, GLuint *textures);
typedef void (APIENTRY * PFNGLTEXIMAGE2D)(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (APIENTRY * PFNGLTEXPARAMETERI)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRY * PFNGLTEXPARAMETERF)(GLenum target, GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLACTIVETEXTURE)(GLenum texture);
typedef void (APIENTRY * PFNGLBINDVERTEXARRAY)(GLuint array);
typedef void (APIENTRY * PFNGLGENVERTEXARRAYS)(GLsizei n, GLuint *arrays);
typedef void (APIENTRY * PFNGLDELETEVERTEXARRAYS)(GLsizei n, const GLuint *arrays);
typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTER)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAY)(GLuint index);
typedef void (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAY)(GLuint index);
typedef void (APIENTRY * PFNGLVERTEXATTRIBDIVISOR)(GLuint index, GLuint divisor);
typedef void (APIENTRY * PFNGLDRAWARRAYSINSTANCED)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void (APIENTRY * PFNGLGENFRAMEBUFFERS)(GLsizei n, GLuint *framebuffers);
typedef void (APIENTRY * PFNGLFRAMEBUFFERTEXTURE2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRY * PFNGLGETSHADERIV)(GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETSHADERINFOLOG)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRY * PFNGLGETPROGRAMIV)(GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETPROGRAMINFOLOG)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRY * PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount);
typedef void* (APIENTRY * PFNGLMAPBUFFERPROC)(GLenum target, GLenum access);
typedef GLboolean (APIENTRY * PFNGLUNMAPBUFFERPROC)(GLenum target);

extern PFNGLDELETEBUFFERS glDeleteBuffers;
extern PFNGLGENBUFFERS glGenBuffers;
extern PFNGLBINDBUFFER glBindBuffer;
extern PFNGLBUFFERDATA glBufferData;
extern PFNGLUSEPROGRAM glUseProgram;
extern PFNGLCREATESHADER glCreateShader;
extern PFNGLSHADERSOURCE glShaderSource;
extern PFNGLCOMPILESHADER glCompileShader;
extern PFNGLCREATEPROGRAM glCreateProgram;
extern PFNGLLINKPROGRAM glLinkProgram;
extern PFNGLATTACHSHADER glAttachShader;
extern PFNGLDELETEPROGRAM glDeleteProgram;
extern PFNGLDELETESHADER glDeleteShader;
extern PFNGLGENRENDERBUFFERS glGenRenderbuffers;
extern PFNGLBINDFRAMEBUFFER glBindFramebuffer;
extern PFNGLFRAMEBUFFERRENDERBUFFER glFramebufferRenderbuffer;
extern PFNGLFRAMEBUFFERTEXTURE glFramebufferTexture;
extern PFNGLBINDVERTEXARRAY glBindVertexArray;
extern PFNGLGENVERTEXARRAYS glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYS glDeleteVertexArrays;
extern PFNGLVERTEXATTRIBPOINTER glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAY glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAY glDisableVertexAttribArray;
extern PFNGLVERTEXATTRIBDIVISOR glVertexAttribDivisor;
extern PFNGLDRAWARRAYSINSTANCED glDrawArraysInstanced;
extern PFNGLGENFRAMEBUFFERS glGenFramebuffers;
extern PFNGLFRAMEBUFFERTEXTURE2D glFramebufferTexture2D;
extern PFNGLGETSHADERIV glGetShaderiv;
extern PFNGLGETSHADERINFOLOG glGetShaderInfoLog; 
extern PFNGLGETPROGRAMIV glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOG glGetProgramInfoLog;
extern PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
extern PFNGLMAPBUFFERPROC glMapBuffer;
extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;

extern void at_init_opengl();