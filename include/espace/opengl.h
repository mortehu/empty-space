#ifndef OPENGL_H_
#define OPENGL_H_

#include <set>

#include "string.h"
#include "types.h"

// Macros that cause problems for the massive enum below
#undef DOMAIN
#undef VERSION

/**
 * OpenGL configuration data.
 *
 * The members of this class are useful for choosing render paths.
 */
struct GLConfig
{
  enum DriverType
  {
    GLDRV_ICD,
    GLDRV_STANDALONE,
    GLDRV_VOODOO
  };

  enum HardwareType
  {
    GLHW_GENERIC,
    GLHW_3DFX_2D3D, // no secondary display
    GLHW_RIVA128,   // can't interpolate alpha
    GLHW_RAGEPRO,   // can't modulate alpha on alpha textures,
    GLHW_PERMEDIA2  // can't have src * dst
  };

  enum TextureCompression
  {
    TC_NONE,
    TC_S3TC,
    TC_EXT_COMP_S3TC
  };

  char  renderer[1024];
  char  vendor[1024];
  char  version[1024];
  char  extensionString[4096];

  uint  maxTextureSize;
  uint  maxActiveTextures;

  uint  colorBits;
  uint  depthBits;
  uint  stencilBits;

  DriverType  driverType;
  HardwareType hardwareType;

  int   gammaSupport;
  TextureCompression textureCompression;
  int   textureEnvAddSupport;
  int   anisotropicSupport;
  float maxAnisotropy;

  int   NVFogAvailable;
  int   NVFogMode;

  int   ATIMaxTruformTess;
  int   ATINormalMode;
  int   ATIPointMode;

  int   width;
  int   height;
  float aspect;

  int   refreshRate;

  int   fullscreen;
  int   stereo;
  int   smp;

  int   textureFilterAnisotropicSupport;

  std::set<String> extensions;
};

/**
 * Namespace for all OpenGL functions
 */
struct GL
{
  static IMPORT GLConfig config;

  static const char* lastError();

  typedef unsigned int	 GLenum;
  typedef unsigned char	 GLboolean;
  typedef unsigned int	 GLbitfield;
  typedef void		       GLvoid;
  typedef signed char	   GLbyte;
  typedef short		       GLshort;
  typedef int            GLint;
  typedef unsigned char  GLubyte;
  typedef unsigned short GLushort;
  typedef unsigned int   GLuint;
  typedef int            GLsizei;
  typedef float          GLfloat;
  typedef float          GLclampf;
  typedef double         GLdouble;
  typedef double         GLclampd;

#ifdef WIN32
#  define APIENTRY __stdcall
#else
#  define APIENTRY
#endif

  typedef void (APIENTRY *glClearIndex)(GLfloat c);
  typedef void (APIENTRY *glClearColor)(GLclampf red, GLclampf green,
                                        GLclampf blue, GLclampf alpha);
  typedef void (APIENTRY *glClear)(GLbitfield mask);
  typedef void (APIENTRY *glIndexMask)(GLuint mask);
  typedef void (APIENTRY *glColorMask)(GLboolean red, GLboolean green,
                                       GLboolean blue, GLboolean alpha);
  typedef void (APIENTRY *glAlphaFunc)(GLenum func, GLclampf ref);
  typedef void (APIENTRY *glBlendFunc)(GLenum sfactor, GLenum dfactor);
  typedef void (APIENTRY *glLogicOp)(GLenum opcode);
  typedef void (APIENTRY *glCullFace)(GLenum mode);
  typedef void (APIENTRY *glFrontFace)(GLenum mode);
  typedef void (APIENTRY *glPointSize)(GLfloat size);
  typedef void (APIENTRY *glLineWidth)(GLfloat width);
  typedef void (APIENTRY *glLineStipple)(GLint factor, GLushort pattern);
  typedef void (APIENTRY *glPolygonMode)(GLenum face, GLenum mode);
  typedef void (APIENTRY *glPolygonOffset)(GLfloat factor, GLfloat units);
  typedef void (APIENTRY *glPolygonStipple)(const GLubyte *mask);
  typedef void (APIENTRY *glGetPolygonStipple)(GLubyte *mask);
  typedef void (APIENTRY *glEdgeFlag)(GLboolean flag);
  typedef void (APIENTRY *glEdgeFlagv)(const GLboolean *flag);
  typedef void (APIENTRY *glScissor)(GLint x, GLint y, GLsizei width,
                                     GLsizei height);
  typedef void (APIENTRY *glClipPlane)(GLenum plane, const GLdouble *equation);
  typedef void (APIENTRY *glGetClipPlane)(GLenum plane, GLdouble *equation);
  typedef void (APIENTRY *glDrawBuffer)(GLenum mode);
  typedef void (APIENTRY *glReadBuffer)(GLenum mode);
  typedef void (APIENTRY *glEnable)(GLenum cap);
  typedef void (APIENTRY *glDisable)(GLenum cap);
  typedef GLboolean (APIENTRY *glIsEnabled)(GLenum cap);
  typedef void (APIENTRY *glEnableClientState)(GLenum cap);
  typedef void (APIENTRY *glDisableClientState)(GLenum cap);
  typedef void (APIENTRY *glGetBooleanv)(GLenum pname, GLboolean *params);
  typedef void (APIENTRY *glGetDoublev)(GLenum pname, GLdouble *params);
  typedef void (APIENTRY *glGetFloatv)(GLenum pname, GLfloat *params);
  typedef void (APIENTRY *glGetIntegerv)(GLenum pname, GLint *params);
  typedef void (APIENTRY *glPushAttrib)(GLbitfield mask);
  typedef void (APIENTRY *glPopAttrib)(void);
  typedef void (APIENTRY *glPushClientAttrib)(GLbitfield mask);
  typedef void (APIENTRY *glPopClientAttrib)(void);
  typedef GLint (APIENTRY *glRenderMode)(GLenum mode);
  typedef GLenum (APIENTRY *glGetError)(void);
  typedef const GLubyte* (*glGetString)(GLenum name);
  typedef void (APIENTRY *glFinish)(void);
  typedef void (APIENTRY *glFlush)(void);
  typedef void (APIENTRY *glHint)(GLenum target, GLenum mode);
  typedef void (APIENTRY *glClearDepth)(GLclampd depth);
  typedef void (APIENTRY *glDepthFunc)(GLenum func);
  typedef void (APIENTRY *glDepthMask)(GLboolean flag);
  typedef void (APIENTRY *glDepthRange)(GLclampd near_val, GLclampd far_val);
  typedef void (APIENTRY *glClearAccum)(GLfloat red, GLfloat green, GLfloat blue,
                               GLfloat alpha);
  typedef void (APIENTRY *glAccum)(GLenum op, GLfloat value);
  typedef void (APIENTRY *glMatrixMode)(GLenum mode);
  typedef void (APIENTRY *glOrtho)(GLdouble left, GLdouble right,
                                   GLdouble bottom, GLdouble top,
                                   GLdouble near_val, GLdouble far_val);
  typedef void (APIENTRY *glFrustum)(GLdouble left, GLdouble right,
                                     GLdouble bottom, GLdouble top,
                                     GLdouble near_val, GLdouble far_val);
  typedef void (APIENTRY *glViewport)(GLint x, GLint y, GLsizei width,
                                      GLsizei height);
  typedef void (APIENTRY *glPushMatrix)(void);
  typedef void (APIENTRY *glPopMatrix)(void);
  typedef void (APIENTRY *glLoadIdentity)(void);
  typedef void (APIENTRY *glLoadMatrixd)(const GLdouble *m);
  typedef void (APIENTRY *glLoadMatrixf)(const GLfloat *m);
  typedef void (APIENTRY *glMultMatrixd)(const GLdouble *m);
  typedef void (APIENTRY *glMultMatrixf)(const GLfloat *m);
  typedef void (APIENTRY *glRotated)(GLdouble angle, GLdouble x, GLdouble y,
                                     GLdouble z);
  typedef void (APIENTRY *glRotatef)(GLfloat angle, GLfloat x, GLfloat y,
                                     GLfloat z);
  typedef void (APIENTRY *glScaled)(GLdouble x, GLdouble y, GLdouble z);
  typedef void (APIENTRY *glScalef)(GLfloat x, GLfloat y, GLfloat z);
  typedef void (APIENTRY *glTranslated)(GLdouble x, GLdouble y, GLdouble z);
  typedef void (APIENTRY *glTranslatef)(GLfloat x, GLfloat y, GLfloat z);
  typedef GLboolean (APIENTRY *glIsList)(GLuint list);
  typedef void (APIENTRY *glDeleteLists)(GLuint list, GLsizei range);
  typedef GLuint (APIENTRY *glGenLists)(GLsizei range);
  typedef void (APIENTRY *glNewList)(GLuint list, GLenum mode);
  typedef void (APIENTRY *glEndList)(void);
  typedef void (APIENTRY *glCallList)(GLuint list);
  typedef void (APIENTRY *glCallLists)(GLsizei n, GLenum type,
                                       const GLvoid *lists);
  typedef void (APIENTRY *glListBase)(GLuint base);
  typedef void (APIENTRY *glBegin)(GLenum mode);
  typedef void (APIENTRY *glEnd)(void);
  typedef void (APIENTRY *glVertex2d)(GLdouble x, GLdouble y);
  typedef void (APIENTRY *glVertex2f)(GLfloat x, GLfloat y);
  typedef void (APIENTRY *glVertex2i)(GLint x, GLint y);
  typedef void (APIENTRY *glVertex2s)(GLshort x, GLshort y);
  typedef void (APIENTRY *glVertex3d)(GLdouble x, GLdouble y, GLdouble z);
  typedef void (APIENTRY *glVertex3f)(GLfloat x, GLfloat y, GLfloat z);
  typedef void (APIENTRY *glVertex3i)(GLint x, GLint y, GLint z);
  typedef void (APIENTRY *glVertex3s)(GLshort x, GLshort y, GLshort z);
  typedef void (APIENTRY *glVertex4d)(GLdouble x, GLdouble y, GLdouble z,
                                      GLdouble w);
  typedef void (APIENTRY *glVertex4f)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  typedef void (APIENTRY *glVertex4i)(GLint x, GLint y, GLint z, GLint w);
  typedef void (APIENTRY *glVertex4s)(GLshort x, GLshort y, GLshort z, GLshort w);
  typedef void (APIENTRY *glVertex2dv)(const GLdouble *v);
  typedef void (APIENTRY *glVertex2fv)(const GLfloat *v);
  typedef void (APIENTRY *glVertex2iv)(const GLint *v);
  typedef void (APIENTRY *glVertex2sv)(const GLshort *v);
  typedef void (APIENTRY *glVertex3dv)(const GLdouble *v);
  typedef void (APIENTRY *glVertex3fv)(const GLfloat *v);
  typedef void (APIENTRY *glVertex3iv)(const GLint *v);
  typedef void (APIENTRY *glVertex3sv)(const GLshort *v);
  typedef void (APIENTRY *glVertex4dv)(const GLdouble *v);
  typedef void (APIENTRY *glVertex4fv)(const GLfloat *v);
  typedef void (APIENTRY *glVertex4iv)(const GLint *v);
  typedef void (APIENTRY *glVertex4sv)(const GLshort *v);
  typedef void (APIENTRY *glNormal3b)(GLbyte nx, GLbyte ny, GLbyte nz);
  typedef void (APIENTRY *glNormal3d)(GLdouble nx, GLdouble ny, GLdouble nz);
  typedef void (APIENTRY *glNormal3f)(GLfloat nx, GLfloat ny, GLfloat nz);
  typedef void (APIENTRY *glNormal3i)(GLint nx, GLint ny, GLint nz);
  typedef void (APIENTRY *glNormal3s)(GLshort nx, GLshort ny, GLshort nz);
  typedef void (APIENTRY *glNormal3bv)(const GLbyte *v);
  typedef void (APIENTRY *glNormal3dv)(const GLdouble *v);
  typedef void (APIENTRY *glNormal3fv)(const GLfloat *v);
  typedef void (APIENTRY *glNormal3iv)(const GLint *v);
  typedef void (APIENTRY *glNormal3sv)(const GLshort *v);
  typedef void (APIENTRY *glIndexd)(GLdouble c);
  typedef void (APIENTRY *glIndexf)(GLfloat c);
  typedef void (APIENTRY *glIndexi)(GLint c);
  typedef void (APIENTRY *glIndexs)(GLshort c);
  typedef void (APIENTRY *glIndexub)(GLubyte c);
  typedef void (APIENTRY *glIndexdv)(const GLdouble *c);
  typedef void (APIENTRY *glIndexfv)(const GLfloat *c);
  typedef void (APIENTRY *glIndexiv)(const GLint *c);
  typedef void (APIENTRY *glIndexsv)(const GLshort *c);
  typedef void (APIENTRY *glIndexubv)(const GLubyte *c);
  typedef void (APIENTRY *glColor3b)(GLbyte red, GLbyte green, GLbyte blue);
  typedef void (APIENTRY *glColor3d)(GLdouble red, GLdouble green, GLdouble blue);
  typedef void (APIENTRY *glColor3f)(GLfloat red, GLfloat green, GLfloat blue);
  typedef void (APIENTRY *glColor3i)(GLint red, GLint green, GLint blue);
  typedef void (APIENTRY *glColor3s)(GLshort red, GLshort green, GLshort blue);
  typedef void (APIENTRY *glColor3ub)(GLubyte red, GLubyte green, GLubyte blue);
  typedef void (APIENTRY *glColor3ui)(GLuint red, GLuint green, GLuint blue);
  typedef void (APIENTRY *glColor3us)(GLushort red, GLushort green, GLushort blue);
  typedef void (APIENTRY *glColor4b)(GLbyte red, GLbyte green, GLbyte blue,
                            GLbyte alpha);
  typedef void (APIENTRY *glColor4d)(GLdouble red, GLdouble green, GLdouble blue,
                            GLdouble alpha);
  typedef void (APIENTRY *glColor4f)(GLfloat red, GLfloat green, GLfloat blue,
                            GLfloat alpha);
  typedef void (APIENTRY *glColor4i)(GLint red, GLint green, GLint blue, GLint alpha);
  typedef void (APIENTRY *glColor4s)(GLshort red, GLshort green, GLshort blue,
                            GLshort alpha);
  typedef void (APIENTRY *glColor4ub)(GLubyte red, GLubyte green, GLubyte blue,
                             GLubyte alpha);
  typedef void (APIENTRY *glColor4ui)(GLuint red, GLuint green, GLuint blue,
                             GLuint alpha);
  typedef void (APIENTRY *glColor4us)(GLushort red, GLushort green, GLushort blue,
                             GLushort alpha);
  typedef void (APIENTRY *glColor3bv)(const GLbyte *v);
  typedef void (APIENTRY *glColor3dv)(const GLdouble *v);
  typedef void (APIENTRY *glColor3fv)(const GLfloat *v);
  typedef void (APIENTRY *glColor3iv)(const GLint *v);
  typedef void (APIENTRY *glColor3sv)(const GLshort *v);
  typedef void (APIENTRY *glColor3ubv)(const GLubyte *v);
  typedef void (APIENTRY *glColor3uiv)(const GLuint *v);
  typedef void (APIENTRY *glColor3usv)(const GLushort *v);
  typedef void (APIENTRY *glColor4bv)(const GLbyte *v);
  typedef void (APIENTRY *glColor4dv)(const GLdouble *v);
  typedef void (APIENTRY *glColor4fv)(const GLfloat *v);
  typedef void (APIENTRY *glColor4iv)(const GLint *v);
  typedef void (APIENTRY *glColor4sv)(const GLshort *v);
  typedef void (APIENTRY *glColor4ubv)(const GLubyte *v);
  typedef void (APIENTRY *glColor4uiv)(const GLuint *v);
  typedef void (APIENTRY *glColor4usv)(const GLushort *v);
  typedef void (APIENTRY *glTexCoord1d)(GLdouble s);
  typedef void (APIENTRY *glTexCoord1f)(GLfloat s);
  typedef void (APIENTRY *glTexCoord1i)(GLint s);
  typedef void (APIENTRY *glTexCoord1s)(GLshort s);
  typedef void (APIENTRY *glTexCoord2d)(GLdouble s, GLdouble t);
  typedef void (APIENTRY *glTexCoord2f)(GLfloat s, GLfloat t);
  typedef void (APIENTRY *glTexCoord2i)(GLint s, GLint t);
  typedef void (APIENTRY *glTexCoord2s)(GLshort s, GLshort t);
  typedef void (APIENTRY *glTexCoord3d)(GLdouble s, GLdouble t, GLdouble r);
  typedef void (APIENTRY *glTexCoord3f)(GLfloat s, GLfloat t, GLfloat r);
  typedef void (APIENTRY *glTexCoord3i)(GLint s, GLint t, GLint r);
  typedef void (APIENTRY *glTexCoord3s)(GLshort s, GLshort t, GLshort r);
  typedef void (APIENTRY *glTexCoord4d)(GLdouble s, GLdouble t, GLdouble r, GLdouble q);
  typedef void (APIENTRY *glTexCoord4f)(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
  typedef void (APIENTRY *glTexCoord4i)(GLint s, GLint t, GLint r, GLint q);
  typedef void (APIENTRY *glTexCoord4s)(GLshort s, GLshort t, GLshort r, GLshort q);
  typedef void (APIENTRY *glTexCoord1dv)(const GLdouble *v);
  typedef void (APIENTRY *glTexCoord1fv)(const GLfloat *v);
  typedef void (APIENTRY *glTexCoord1iv)(const GLint *v);
  typedef void (APIENTRY *glTexCoord1sv)(const GLshort *v);
  typedef void (APIENTRY *glTexCoord2dv)(const GLdouble *v);
  typedef void (APIENTRY *glTexCoord2fv)(const GLfloat *v);
  typedef void (APIENTRY *glTexCoord2iv)(const GLint *v);
  typedef void (APIENTRY *glTexCoord2sv)(const GLshort *v);
  typedef void (APIENTRY *glTexCoord3dv)(const GLdouble *v);
  typedef void (APIENTRY *glTexCoord3fv)(const GLfloat *v);
  typedef void (APIENTRY *glTexCoord3iv)(const GLint *v);
  typedef void (APIENTRY *glTexCoord3sv)(const GLshort *v);
  typedef void (APIENTRY *glTexCoord4dv)(const GLdouble *v);
  typedef void (APIENTRY *glTexCoord4fv)(const GLfloat *v);
  typedef void (APIENTRY *glTexCoord4iv)(const GLint *v);
  typedef void (APIENTRY *glTexCoord4sv)(const GLshort *v);
  typedef void (APIENTRY *glRasterPos2d)(GLdouble x, GLdouble y);
  typedef void (APIENTRY *glRasterPos2f)(GLfloat x, GLfloat y);
  typedef void (APIENTRY *glRasterPos2i)(GLint x, GLint y);
  typedef void (APIENTRY *glRasterPos2s)(GLshort x, GLshort y);
  typedef void (APIENTRY *glRasterPos3d)(GLdouble x, GLdouble y, GLdouble z);
  typedef void (APIENTRY *glRasterPos3f)(GLfloat x, GLfloat y, GLfloat z);
  typedef void (APIENTRY *glRasterPos3i)(GLint x, GLint y, GLint z);
  typedef void (APIENTRY *glRasterPos3s)(GLshort x, GLshort y, GLshort z);
  typedef void (APIENTRY *glRasterPos4d)(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
  typedef void (APIENTRY *glRasterPos4f)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  typedef void (APIENTRY *glRasterPos4i)(GLint x, GLint y, GLint z, GLint w);
  typedef void (APIENTRY *glRasterPos4s)(GLshort x, GLshort y, GLshort z, GLshort w);
  typedef void (APIENTRY *glRasterPos2dv)(const GLdouble *v);
  typedef void (APIENTRY *glRasterPos2fv)(const GLfloat *v);
  typedef void (APIENTRY *glRasterPos2iv)(const GLint *v);
  typedef void (APIENTRY *glRasterPos2sv)(const GLshort *v);
  typedef void (APIENTRY *glRasterPos3dv)(const GLdouble *v);
  typedef void (APIENTRY *glRasterPos3fv)(const GLfloat *v);
  typedef void (APIENTRY *glRasterPos3iv)(const GLint *v);
  typedef void (APIENTRY *glRasterPos3sv)(const GLshort *v);
  typedef void (APIENTRY *glRasterPos4dv)(const GLdouble *v);
  typedef void (APIENTRY *glRasterPos4fv)(const GLfloat *v);
  typedef void (APIENTRY *glRasterPos4iv)(const GLint *v);
  typedef void (APIENTRY *glRasterPos4sv)(const GLshort *v);
  typedef void (APIENTRY *glRectd)(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
  typedef void (APIENTRY *glRectf)(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
  typedef void (APIENTRY *glRecti)(GLint x1, GLint y1, GLint x2, GLint y2);
  typedef void (APIENTRY *glRects)(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
  typedef void (APIENTRY *glRectdv)(const GLdouble *v1, const GLdouble *v2);
  typedef void (APIENTRY *glRectfv)(const GLfloat *v1, const GLfloat *v2);
  typedef void (APIENTRY *glRectiv)(const GLint *v1, const GLint *v2);
  typedef void (APIENTRY *glRectsv)(const GLshort *v1, const GLshort *v2);
  typedef void (APIENTRY *glVertexPointer)(GLint size, GLenum type, GLsizei stride,
                                  const GLvoid *ptr);
  typedef void (APIENTRY *glNormalPointer)(GLenum type, GLsizei stride,
                                  const GLvoid *ptr);
  typedef void (APIENTRY *glColorPointer)(GLint size, GLenum type, GLsizei stride,
                                 const GLvoid *ptr);
  typedef void (APIENTRY *glIndexPointer)(GLenum type, GLsizei stride,
                                 const GLvoid *ptr);
  typedef void (APIENTRY *glTexCoordPointer)(GLint size, GLenum type, GLsizei stride,
                                    const GLvoid *ptr);
  typedef void (APIENTRY *glEdgeFlagPointer)(GLsizei stride, const GLvoid *ptr);
  typedef void (APIENTRY *glGetPointerv)(GLenum pname, void **params);
  typedef void (APIENTRY *glArrayElement)(GLint i);
  typedef void (APIENTRY *glDrawArrays)(GLenum mode, GLint first, GLsizei count);
  typedef void (APIENTRY *glDrawElements)(GLenum mode, GLsizei count, GLenum type,
                                 const GLvoid *indices);
  typedef void (APIENTRY *glInterleavedArrays)(GLenum format, GLsizei stride,
                                      const GLvoid *pointer);
  typedef void (APIENTRY *glShadeModel)(GLenum mode);
  typedef void (APIENTRY *glLightf)(GLenum light, GLenum pname, GLfloat param);
  typedef void (APIENTRY *glLighti)(GLenum light, GLenum pname, GLint param);
  typedef void (APIENTRY *glLightfv)(GLenum light, GLenum pname, const GLfloat *params);
  typedef void (APIENTRY *glLightiv)(GLenum light, GLenum pname, const GLint *params);
  typedef void (APIENTRY *glGetLightfv)(GLenum light, GLenum pname, GLfloat *params);
  typedef void (APIENTRY *glGetLightiv)(GLenum light, GLenum pname, GLint *params);
  typedef void (APIENTRY *glLightModelf)(GLenum pname, GLfloat param);
  typedef void (APIENTRY *glLightModeli)(GLenum pname, GLint param);
  typedef void (APIENTRY *glLightModelfv)(GLenum pname, const GLfloat *params);
  typedef void (APIENTRY *glLightModeliv)(GLenum pname, const GLint *params);
  typedef void (APIENTRY *glMaterialf)(GLenum face, GLenum pname, GLfloat param);
  typedef void (APIENTRY *glMateriali)(GLenum face, GLenum pname, GLint param);
  typedef void (APIENTRY *glMaterialfv)(GLenum face, GLenum pname,
                               const GLfloat *params);
  typedef void (APIENTRY *glMaterialiv)(GLenum face, GLenum pname, const GLint *params);
  typedef void (APIENTRY *glGetMaterialfv)(GLenum face, GLenum pname, GLfloat *params);
  typedef void (APIENTRY *glGetMaterialiv)(GLenum face, GLenum pname, GLint *params);
  typedef void (APIENTRY *glColorMaterial)(GLenum face, GLenum mode);
  typedef void (APIENTRY *glPixelZoom)(GLfloat xfactor, GLfloat yfactor);
  typedef void (APIENTRY *glPixelStoref)(GLenum pname, GLfloat param);
  typedef void (APIENTRY *glPixelStorei)(GLenum pname, GLint param);
  typedef void (APIENTRY *glPixelTransferf)(GLenum pname, GLfloat param);
  typedef void (APIENTRY *glPixelTransferi)(GLenum pname, GLint param);
  typedef void (APIENTRY *glPixelMapfv)(GLenum map, GLint mapsize,
                               const GLfloat *values);
  typedef void (APIENTRY *glPixelMapuiv)(GLenum map, GLint mapsize,
                                const GLuint *values);
  typedef void (APIENTRY *glPixelMapusv)(GLenum map, GLint mapsize,
                                const GLushort *values);
  typedef void (APIENTRY *glGetPixelMapfv)(GLenum map, GLfloat *values);
  typedef void (APIENTRY *glGetPixelMapuiv)(GLenum map, GLuint *values);
  typedef void (APIENTRY *glGetPixelMapusv)(GLenum map, GLushort *values);
  typedef void (APIENTRY *glBitmap)(GLsizei width, GLsizei height, GLfloat xorig,
                           GLfloat yorig, GLfloat xmove, GLfloat ymove,
                           const GLubyte *bitmap);
  typedef void (APIENTRY *glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height,
                               GLenum format, GLenum type, GLvoid *pixels);
  typedef void (APIENTRY *glDrawPixels)(GLsizei width, GLsizei height, GLenum format,
                               GLenum type, const GLvoid *pixels);
  typedef void (APIENTRY *glCopyPixels)(GLint x, GLint y, GLsizei width, GLsizei height,
                               GLenum type);
  typedef void (APIENTRY *glStencilFunc)(GLenum func, GLint ref, GLuint mask);
  typedef void (APIENTRY *glStencilMask)(GLuint mask);
  typedef void (APIENTRY *glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
  typedef void (APIENTRY *glClearStencil)(GLint s);
  typedef void (APIENTRY *glTexGend)(GLenum coord, GLenum pname, GLdouble param);
  typedef void (APIENTRY *glTexGenf)(GLenum coord, GLenum pname, GLfloat param);
  typedef void (APIENTRY *glTexGeni)(GLenum coord, GLenum pname, GLint param);
  typedef void (APIENTRY *glTexGendv)(GLenum coord, GLenum pname,
                             const GLdouble *params);
  typedef void (APIENTRY *glTexGenfv)(GLenum coord, GLenum pname, const GLfloat *params);
  typedef void (APIENTRY *glTexGeniv)(GLenum coord, GLenum pname, const GLint *params);
  typedef void (APIENTRY *glGetTexGendv)(GLenum coord, GLenum pname, GLdouble *params);
  typedef void (APIENTRY *glGetTexGenfv)(GLenum coord, GLenum pname, GLfloat *params);
  typedef void (APIENTRY *glGetTexGeniv)(GLenum coord, GLenum pname, GLint *params);
  typedef void (APIENTRY *glTexEnvf)(GLenum target, GLenum pname, GLfloat param);
  typedef void (APIENTRY *glTexEnvi)(GLenum target, GLenum pname, GLint param);
  typedef void (APIENTRY *glTexEnvfv)(GLenum target, GLenum pname,
                             const GLfloat *params);
  typedef void (APIENTRY *glTexEnviv)(GLenum target, GLenum pname, const GLint *params);
  typedef void (APIENTRY *glGetTexEnvfv)(GLenum target, GLenum pname, GLfloat *params);
  typedef void (APIENTRY *glGetTexEnviv)(GLenum target, GLenum pname, GLint *params);
  typedef void (APIENTRY *glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
  typedef void (APIENTRY *glTexParameteri)(GLenum target, GLenum pname, GLint param);
  typedef void (APIENTRY *glTexParameterfv)(GLenum target, GLenum pname,
                                   const GLfloat *params);
  typedef void (APIENTRY *glTexParameteriv)(GLenum target, GLenum pname,
                                   const GLint *params);
  typedef void (APIENTRY *glGetTexParameterfv)(GLenum target, GLenum pname,
                                      GLfloat *params);
  typedef void (APIENTRY *glGetTexParameteriv)(GLenum target, GLenum pname,
                                      GLint *params);
  typedef void (APIENTRY *glGetTexLevelParameterfv)(GLenum target, GLint level,
                                           GLenum pname, GLfloat *params);
  typedef void (APIENTRY *glGetTexLevelParameteriv)(GLenum target, GLint level,
                                           GLenum pname, GLint *params);
  typedef void (APIENTRY *glTexImage1D)(GLenum target, GLint level, GLint internalFormat,
                               GLsizei width, GLint border, GLenum format,
                               GLenum type, const GLvoid *pixels);
  typedef void (APIENTRY *glTexImage2D)(GLenum target, GLint level, GLint internalFormat,
                               GLsizei width, GLsizei height, GLint border,
                               GLenum format, GLenum type,
                               const GLvoid *pixels);
  typedef void (APIENTRY *glGetTexImage)(GLenum target, GLint level, GLenum format,
                                GLenum type, GLvoid *pixels);
  typedef void (APIENTRY *glGenTextures)(GLsizei n, GLuint *textures);
  typedef void (APIENTRY *glDeleteTextures)(GLsizei n, const GLuint *textures);
  typedef void (APIENTRY *glBindTexture)(GLenum target, GLuint texture);
  typedef void (APIENTRY *glPrioritizeTextures)(GLsizei n, const GLuint *textures,
                                       const GLclampf *priorities);
  typedef GLboolean (APIENTRY *glAreTexturesResident)(GLsizei n, const GLuint *textures,
                                             GLboolean *residences);
  typedef GLboolean (APIENTRY *glIsTexture)(GLuint texture);
  typedef void (APIENTRY *glTexSubImage1D)(GLenum target, GLint level, GLint xoffset,
                                  GLsizei width, GLenum format, GLenum type,
                                  const GLvoid *pixels);
  typedef void (APIENTRY *glTexSubImage2D)(GLenum target, GLint level, GLint xoffset,
                                  GLint yoffset, GLsizei width, GLsizei height,
                                  GLenum format, GLenum type,
                                  const GLvoid *pixels);
  typedef void (APIENTRY *glCopyTexImage1D)(GLenum target, GLint level,
                                   GLenum internalformat, GLint x, GLint y,
                                   GLsizei width, GLint border);
  typedef void (APIENTRY *glCopyTexImage2D)(GLenum target, GLint level,
                                   GLenum internalformat, GLint x, GLint y,
                                   GLsizei width, GLsizei height, GLint border);
  typedef void (APIENTRY *glCopyTexSubImage1D)(GLenum target, GLint level, GLint xoffset,
                                      GLint x, GLint y, GLsizei width);
  typedef void (APIENTRY *glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset,
                                      GLint yoffset, GLint x, GLint y,
                                      GLsizei width, GLsizei height);
  typedef void (APIENTRY *glMap1d)(GLenum target, GLdouble u1, GLdouble u2, GLint stride,
                          GLint order, const GLdouble *points);
  typedef void (APIENTRY *glMap1f)(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
  typedef void (APIENTRY *glMap2d)(GLenum target, GLdouble u1, GLdouble u2,
                          GLint ustride, GLint uorder, GLdouble v1, GLdouble v2,
                          GLint vstride, GLint vorder, const GLdouble *points);
  typedef void (APIENTRY *glMap2f)(GLenum target, GLfloat u1, GLfloat u2, GLint ustride,
                          GLint uorder, GLfloat v1, GLfloat v2, GLint vstride,
                          GLint vorder, const GLfloat *points);
  typedef void (APIENTRY *glGetMapdv)(GLenum target, GLenum query, GLdouble *v);
  typedef void (APIENTRY *glGetMapfv)(GLenum target, GLenum query, GLfloat *v);
  typedef void (APIENTRY *glGetMapiv)(GLenum target, GLenum query, GLint *v);
  typedef void (APIENTRY *glEvalCoord1d)(GLdouble u);
  typedef void (APIENTRY *glEvalCoord1f)(GLfloat u);
  typedef void (APIENTRY *glEvalCoord1dv)(const GLdouble *u);
  typedef void (APIENTRY *glEvalCoord1fv)(const GLfloat *u);
  typedef void (APIENTRY *glEvalCoord2d)(GLdouble u, GLdouble v);
  typedef void (APIENTRY *glEvalCoord2f)(GLfloat u, GLfloat v);
  typedef void (APIENTRY *glEvalCoord2dv)(const GLdouble *u);
  typedef void (APIENTRY *glEvalCoord2fv)(const GLfloat *u);
  typedef void (APIENTRY *glMapGrid1d)(GLint un, GLdouble u1, GLdouble u2);
  typedef void (APIENTRY *glMapGrid1f)(GLint un, GLfloat u1, GLfloat u2);
  typedef void (APIENTRY *glMapGrid2d)(GLint un, GLdouble u1, GLdouble u2, GLint vn,
                              GLdouble v1, GLdouble v2);
  typedef void (APIENTRY *glMapGrid2f)(GLint un, GLfloat u1, GLfloat u2, GLint vn,
                              GLfloat v1, GLfloat v2);
  typedef void (APIENTRY *glEvalPoint1)(GLint i);
  typedef void (APIENTRY *glEvalPoint2)(GLint i, GLint j);
  typedef void (APIENTRY *glEvalMesh1)(GLenum mode, GLint i1, GLint i2);
  typedef void (APIENTRY *glEvalMesh2)(GLenum mode, GLint i1, GLint i2, GLint j1,
                              GLint j2);
  typedef void (APIENTRY *glFogf)(GLenum pname, GLfloat param);
  typedef void (APIENTRY *glFogi)(GLenum pname, GLint param);
  typedef void (APIENTRY *glFogfv)(GLenum pname, const GLfloat *params);
  typedef void (APIENTRY *glFogiv)(GLenum pname, const GLint *params);
  typedef void (APIENTRY *glFeedbackBuffer)(GLsizei size, GLenum type, GLfloat *buffer);
  typedef void (APIENTRY *glPassThrough)(GLfloat token);
  typedef void (APIENTRY *glSelectBuffer)(GLsizei size, GLuint *buffer);
  typedef void (APIENTRY *glInitNames)(void);
  typedef void (APIENTRY *glLoadName)(GLuint name);
  typedef void (APIENTRY *glPushName)(GLuint name);
  typedef void (APIENTRY *glPopName)(void);
  typedef void (APIENTRY *glDrawRangeElements)(GLenum mode, GLuint start, GLuint end,
                                      GLsizei count, GLenum type,
                                      const GLvoid *indices);
  typedef void (APIENTRY *glTexImage3D)(GLenum target, GLint level, GLint internalFormat,
                               GLsizei width, GLsizei height, GLsizei depth,
                               GLint border, GLenum format, GLenum type,
                               const GLvoid *pixels);
  typedef void (APIENTRY *glTexSubImage3D)(GLenum target, GLint level, GLint xoffset,
                                  GLint yoffset, GLint zoffset, GLsizei width,
                                  GLsizei height, GLsizei depth, GLenum format,
                                  GLenum type, const GLvoid *pixels);
  typedef void (APIENTRY *glCopyTexSubImage3D)(GLenum target, GLint level, GLint xoffset,
                                      GLint yoffset, GLint zoffset, GLint x,
                                      GLint y, GLsizei width, GLsizei height);
  typedef void (APIENTRY *glColorTable)(GLenum target, GLenum internalformat,
                               GLsizei width, GLenum format, GLenum type,
                               const GLvoid *table);
  typedef void (APIENTRY *glColorSubTable)(GLenum target, GLsizei start, GLsizei count,
                                  GLenum format, GLenum type,
                                 const GLvoid *data);
  typedef void (APIENTRY *glColorTableParameteriv)(GLenum target, GLenum pname,
                                          const GLint *params);
  typedef void (APIENTRY *glColorTableParameterfv)(GLenum target, GLenum pname,
                                          const GLfloat *params);
  typedef void (APIENTRY *glCopyColorSubTable)(GLenum target, GLsizei start, GLint x,
                                      GLint y, GLsizei width);
  typedef void (APIENTRY *glCopyColorTable)(GLenum target, GLenum internalformat,
                                   GLint x, GLint y, GLsizei width);
  typedef void (APIENTRY *glGetColorTable)(GLenum target, GLenum format, GLenum type,
                                  GLvoid *table);
  typedef void (APIENTRY *glGetColorTableParameterfv)(GLenum target, GLenum pname,
                                             GLfloat *params);
  typedef void (APIENTRY *glGetColorTableParameteriv)(GLenum target, GLenum pname,
                                             GLint *params);
  typedef void (APIENTRY *glBlendEquation)(GLenum mode);
  typedef void (APIENTRY *glBlendColor)(GLclampf red, GLclampf green, GLclampf blue,
                               GLclampf alpha);
  typedef void (APIENTRY *glHistogram)(GLenum target, GLsizei width,
                              GLenum internalformat, GLboolean sink);
  typedef void (APIENTRY *glResetHistogram)(GLenum target);
  typedef void (APIENTRY *glGetHistogram)(GLenum target, GLboolean reset, GLenum format,
                                 GLenum type, GLvoid *values);
  typedef void (APIENTRY *glGetHistogramParameterfv)(GLenum target, GLenum pname,
                                            GLfloat *params);
  typedef void (APIENTRY *glGetHistogramParameteriv)(GLenum target, GLenum pname,
                                            GLint *params);
  typedef void (APIENTRY *glMinmax)(GLenum target, GLenum internalformat,
                           GLboolean sink);
  typedef void (APIENTRY *glResetMinmax)(GLenum target);
  typedef void (APIENTRY *glGetMinmax)(GLenum target, GLboolean reset, GLenum format,
                              GLenum types, GLvoid *values);
  typedef void (APIENTRY *glGetMinmaxParameterfv)(GLenum target, GLenum pname,
                                         GLfloat *params);
  typedef void (APIENTRY *glGetMinmaxParameteriv)(GLenum target, GLenum pname,
                                         GLint *params);
  typedef void (APIENTRY *glConvolutionFilter1D)(GLenum target, GLenum internalformat,
                                        GLsizei width, GLenum format,
                                        GLenum type, const GLvoid *image);
  typedef void (APIENTRY *glConvolutionFilter2D)(GLenum target, GLenum internalformat,
                                        GLsizei width, GLsizei height,
                                        GLenum format, GLenum type,
                                        const GLvoid *image);
  typedef void (APIENTRY *glConvolutionParameterf)(GLenum target, GLenum pname,
                                          GLfloat params);
  typedef void (APIENTRY *glConvolutionParameterfv)(GLenum target, GLenum pname,
                                           const GLfloat *params);
  typedef void (APIENTRY *glConvolutionParameteri)(GLenum target, GLenum pname,
                                          GLint params);
  typedef void (APIENTRY *glConvolutionParameteriv)(GLenum target, GLenum pname,
                                           const GLint *params);
  typedef void (APIENTRY *glCopyConvolutionFilter1D)(GLenum target,
                                            GLenum internalformat,
                                            GLint x, GLint y, GLsizei width);
  typedef void (APIENTRY *glCopyConvolutionFilter2D)(GLenum target,
                                            GLenum internalformat, GLint x,
                                            GLint y, GLsizei width,
                                            GLsizei height);
  typedef void (APIENTRY *glGetConvolutionFilter)(GLenum target, GLenum format,
                                         GLenum type, GLvoid *image);
  typedef void (APIENTRY *glGetConvolutionParameterfv)(GLenum target, GLenum pname,
                                              GLfloat *params);
  typedef void (APIENTRY *glGetConvolutionParameteriv)(GLenum target, GLenum pname,
                                              GLint *params);
  typedef void (APIENTRY *glSeparableFilter2D)(GLenum target, GLenum internalformat,
                                      GLsizei width, GLsizei height,
                                      GLenum format, GLenum type,
                                      const GLvoid *row, const GLvoid *column);
  typedef void (APIENTRY *glGetSeparableFilter)(GLenum target, GLenum format,
                                       GLenum type, GLvoid *row, GLvoid *column,
                                       GLvoid *span);
  typedef void (APIENTRY *glActiveTextureARB)(GLenum texture);
  typedef void (APIENTRY *glClientActiveTextureARB)(GLenum texture);
  typedef void (APIENTRY *glMultiTexCoord1dARB)(GLenum target, GLdouble s);
  typedef void (APIENTRY *glMultiTexCoord1dvARB)(GLenum target, const GLdouble *v);
  typedef void (APIENTRY *glMultiTexCoord1fARB)(GLenum target, GLfloat s);
  typedef void (APIENTRY *glMultiTexCoord1fvARB)(GLenum target, const GLfloat *v);
  typedef void (APIENTRY *glMultiTexCoord1iARB)(GLenum target, GLint s);
  typedef void (APIENTRY *glMultiTexCoord1ivARB)(GLenum target, const GLint *v);
  typedef void (APIENTRY *glMultiTexCoord1sARB)(GLenum target, GLshort s);
  typedef void (APIENTRY *glMultiTexCoord1svARB)(GLenum target, const GLshort *v);
  typedef void (APIENTRY *glMultiTexCoord2dARB)(GLenum target, GLdouble s, GLdouble t);
  typedef void (APIENTRY *glMultiTexCoord2dvARB)(GLenum target, const GLdouble *v);
  typedef void (APIENTRY *glMultiTexCoord2fARB)(GLenum target, GLfloat s, GLfloat t);
  typedef void (APIENTRY *glMultiTexCoord2fvARB)(GLenum target, const GLfloat *v);
  typedef void (APIENTRY *glMultiTexCoord2iARB)(GLenum target, GLint s, GLint t);
  typedef void (APIENTRY *glMultiTexCoord2ivARB)(GLenum target, const GLint *v);
  typedef void (APIENTRY *glMultiTexCoord2sARB)(GLenum target, GLshort s, GLshort t);
  typedef void (APIENTRY *glMultiTexCoord2svARB)(GLenum target, const GLshort *v);
  typedef void (APIENTRY *glMultiTexCoord3dARB)(GLenum target, GLdouble s, GLdouble t,
                                       GLdouble r);
  typedef void (APIENTRY *glMultiTexCoord3dvARB)(GLenum target, const GLdouble *v);
  typedef void (APIENTRY *glMultiTexCoord3fARB)(GLenum target, GLfloat s, GLfloat t,
                                       GLfloat r);
  typedef void (APIENTRY *glMultiTexCoord3fvARB)(GLenum target, const GLfloat *v);
  typedef void (APIENTRY *glMultiTexCoord3iARB)(GLenum target, GLint s, GLint t,
                                       GLint r);
  typedef void (APIENTRY *glMultiTexCoord3ivARB)(GLenum target, const GLint *v);
  typedef void (APIENTRY *glMultiTexCoord3sARB)(GLenum target, GLshort s, GLshort t,
                                       GLshort r);
  typedef void (APIENTRY *glMultiTexCoord3svARB)(GLenum target, const GLshort *v);
  typedef void (APIENTRY *glMultiTexCoord4dARB)(GLenum target, GLdouble s, GLdouble t,
                                       GLdouble r, GLdouble q);
  typedef void (APIENTRY *glMultiTexCoord4dvARB)(GLenum target, const GLdouble *v);
  typedef void (APIENTRY *glMultiTexCoord4fARB)(GLenum target, GLfloat s, GLfloat t,
                                       GLfloat r, GLfloat q);
  typedef void (APIENTRY *glMultiTexCoord4fvARB)(GLenum target, const GLfloat *v);
  typedef void (APIENTRY *glMultiTexCoord4iARB)(GLenum target, GLint s, GLint t, GLint r,
                                       GLint q);
  typedef void (APIENTRY *glMultiTexCoord4ivARB)(GLenum target, const GLint *v);
  typedef void (APIENTRY *glMultiTexCoord4sARB)(GLenum target, GLshort s, GLshort t,
                                       GLshort r, GLshort q);
  typedef void (APIENTRY *glMultiTexCoord4svARB)(GLenum target, const GLshort *v);
  typedef void (APIENTRY *glBlendColorEXT)(GLclampf red, GLclampf green, GLclampf blue,
                                  GLclampf alpha);
  typedef void (APIENTRY *glPolygonOffsetEXT)(GLfloat factor, GLfloat bias);
  typedef void (APIENTRY *glTexImage3DEXT)(GLenum target, GLint level,
                                  GLenum internalFormat, GLsizei width,
                                  GLsizei height, GLsizei depth, GLint border,
                                  GLenum format, GLenum type,
                                  const GLvoid *pixels);
  typedef void (APIENTRY *glTexSubImage3DEXT)(GLenum target, GLint level, GLint xoffset,
                                     GLint yoffset, GLint zoffset,
                                     GLsizei width, GLsizei height,
                                     GLsizei depth, GLenum format, GLenum type,
                                     const GLvoid *pixels);
  typedef void (APIENTRY *glCopyTexSubImage3DEXT)(GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLint zoffset, GLint x, GLint y,
                                         GLsizei width, GLsizei height);
  typedef void (APIENTRY *glGenTexturesEXT)(GLsizei n, GLuint *textures);
  typedef void (APIENTRY *glDeleteTexturesEXT)(GLsizei n, const GLuint *textures);
  typedef void (APIENTRY *glBindTextureEXT)(GLenum target, GLuint texture);
  typedef void (APIENTRY *glPrioritizeTexturesEXT)(GLsizei n, const GLuint *textures,
                                          const GLclampf *priorities);
  typedef GLboolean (APIENTRY *glAreTexturesResidentEXT)(GLsizei n,
                                                const GLuint *textures,
                                                GLboolean *residences);
  typedef GLboolean (APIENTRY *glIsTextureEXT)(GLuint texture);
  typedef void (APIENTRY *glVertexPointerEXT)(GLint size, GLenum type, GLsizei stride,
                                     GLsizei count, const GLvoid *ptr);
  typedef void (APIENTRY *glNormalPointerEXT)(GLenum type, GLsizei stride, GLsizei count,
                                     const GLvoid *ptr);
  typedef void (APIENTRY *glColorPointerEXT)(GLint size, GLenum type, GLsizei stride,
                                    GLsizei count, const GLvoid *ptr);
  typedef void (APIENTRY *glIndexPointerEXT)(GLenum type, GLsizei stride, GLsizei count,
                                    const GLvoid *ptr);
  typedef void (APIENTRY *glTexCoordPointerEXT)(GLint size, GLenum type, GLsizei stride,
                                       GLsizei count, const GLvoid *ptr);
  typedef void (APIENTRY *glEdgeFlagPointerEXT)(GLsizei stride, GLsizei count,
                                       const GLboolean *ptr);
  typedef void (APIENTRY *glGetPointervEXT)(GLenum pname, void **params);
  typedef void (APIENTRY *glArrayElementEXT)(GLint i);
  typedef void (APIENTRY *glDrawArraysEXT)(GLenum mode, GLint first, GLsizei count);
  typedef void (APIENTRY *glBlendEquationEXT)(GLenum mode);
  typedef void (APIENTRY *glPointParameterfEXT)(GLenum pname, GLfloat param);
  typedef void (APIENTRY *glPointParameterfvEXT)(GLenum pname, const GLfloat *params);
  typedef void (APIENTRY *glPointParameterfSGIS)(GLenum pname, GLfloat param);
  typedef void (APIENTRY *glPointParameterfvSGIS)(GLenum pname, const GLfloat *params);
  typedef void (APIENTRY *glColorTableEXT)(GLenum target, GLenum internalformat,
                                  GLsizei width, GLenum format, GLenum type,
                                  const GLvoid *table);
  typedef void (APIENTRY *glColorSubTableEXT)(GLenum target, GLsizei start,
                                     GLsizei count, GLenum format, GLenum type,
                                     const GLvoid *data);
  typedef void (APIENTRY *glGetColorTableEXT)(GLenum target, GLenum format, GLenum type,
                                     GLvoid *table);
  typedef void (APIENTRY *glGetColorTableParameterfvEXT)(GLenum target, GLenum pname,
                                                GLfloat *params);
  typedef void (APIENTRY *glGetColorTableParameterivEXT)(GLenum target, GLenum pname,
                                                GLint *params);
  typedef void (APIENTRY *glLockArraysEXT)(GLint first, GLsizei count);
  typedef void (APIENTRY *glUnlockArraysEXT)(void);
  typedef void (APIENTRY *glWindowPos2iMESA)(GLint x, GLint y);
  typedef void (APIENTRY *glWindowPos2sMESA)(GLshort x, GLshort y);
  typedef void (APIENTRY *glWindowPos2fMESA)(GLfloat x, GLfloat y);
  typedef void (APIENTRY *glWindowPos2dMESA)(GLdouble x, GLdouble y);
  typedef void (APIENTRY *glWindowPos2ivMESA)(const GLint *p);
  typedef void (APIENTRY *glWindowPos2svMESA)(const GLshort *p);
  typedef void (APIENTRY *glWindowPos2fvMESA)(const GLfloat *p);
  typedef void (APIENTRY *glWindowPos2dvMESA)(const GLdouble *p);
  typedef void (APIENTRY *glWindowPos3iMESA)(GLint x, GLint y, GLint z);
  typedef void (APIENTRY *glWindowPos3sMESA)(GLshort x, GLshort y, GLshort z);
  typedef void (APIENTRY *glWindowPos3fMESA)(GLfloat x, GLfloat y, GLfloat z);
  typedef void (APIENTRY *glWindowPos3dMESA)(GLdouble x, GLdouble y, GLdouble z);
  typedef void (APIENTRY *glWindowPos3ivMESA)(const GLint *p);
  typedef void (APIENTRY *glWindowPos3svMESA)(const GLshort *p);
  typedef void (APIENTRY *glWindowPos3fvMESA)(const GLfloat *p);
  typedef void (APIENTRY *glWindowPos3dvMESA)(const GLdouble *p);
  typedef void (APIENTRY *glWindowPos4iMESA)(GLint x, GLint y, GLint z, GLint w);
  typedef void (APIENTRY *glWindowPos4sMESA)(GLshort x, GLshort y, GLshort z, GLshort w);
  typedef void (APIENTRY *glWindowPos4fMESA)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  typedef void (APIENTRY *glWindowPos4dMESA)(GLdouble x, GLdouble y, GLdouble z,
                                    GLdouble w);
  typedef void (APIENTRY *glWindowPos4ivMESA)(const GLint *p);
  typedef void (APIENTRY *glWindowPos4svMESA)(const GLshort *p);
  typedef void (APIENTRY *glWindowPos4fvMESA)(const GLfloat *p);
  typedef void (APIENTRY *glWindowPos4dvMESA)(const GLdouble *p);
  typedef void (APIENTRY *glResizeBuffersMESA)(void);
  typedef void (APIENTRY *glGenOcclusionQueriesNV)(GLsizei n, GLuint *ids);
  typedef void (APIENTRY *glDeleteOcclusionQueriesNV)(GLsizei n, const GLuint *ids);
  typedef void (APIENTRY *glBeginOcclusionQueryNV)(GLuint id);
  typedef void (APIENTRY *glEndOcclusionQueryNV)(void);
  typedef void (APIENTRY *glGetOcclusionQueryivNV)(GLuint id, GLenum pname, GLint *params);
  typedef void (APIENTRY *glGetOcclusionQueryuivNV)(GLuint id, GLenum pname, GLuint *params);

  static IMPORT glClearIndex                  clearIndex;
  static IMPORT glClearColor                  clearColor;
  static IMPORT glClear                       clear;
  static IMPORT glIndexMask                   indexMask;
  static IMPORT glColorMask                   colorMask;
  static IMPORT glAlphaFunc                   alphaFunc;
  static IMPORT glBlendFunc                   blendFunc;
  static IMPORT glLogicOp                     logicOp;
  static IMPORT glCullFace                    cullFace;
  static IMPORT glFrontFace                   frontFace;
  static IMPORT glPointSize                   pointSize;
  static IMPORT glLineWidth                   lineWidth;
  static IMPORT glLineStipple                 lineStipple;
  static IMPORT glPolygonMode                 polygonMode;
  static IMPORT glPolygonOffset               polygonOffset;
  static IMPORT glPolygonStipple              polygonStipple;
  static IMPORT glGetPolygonStipple           getPolygonStipple;
  static IMPORT glEdgeFlag                    edgeFlag;
  static IMPORT glEdgeFlagv                   edgeFlagv;
  static IMPORT glScissor                     scissor;
  static IMPORT glClipPlane                   clipPlane;
  static IMPORT glGetClipPlane                getClipPlane;
  static IMPORT glDrawBuffer                  drawBuffer;
  static IMPORT glReadBuffer                  readBuffer;
  static IMPORT glEnable                      enable;
  static IMPORT glDisable                     disable;
  static IMPORT glIsEnabled                   isEnabled;
  static IMPORT glEnableClientState           enableClientState;
  static IMPORT glDisableClientState          disableClientState;
  static IMPORT glGetBooleanv                 getBooleanv;
  static IMPORT glGetDoublev                  getDoublev;
  static IMPORT glGetFloatv                   getFloatv;
  static IMPORT glGetIntegerv                 getIntegerv;
  static IMPORT glPushAttrib                  pushAttrib;
  static IMPORT glPopAttrib                   popAttrib;
  static IMPORT glPushClientAttrib            pushClientAttrib;
  static IMPORT glPopClientAttrib             popClientAttrib;
  static IMPORT glRenderMode                  renderMode;
  static IMPORT glGetError                    getError;
  static IMPORT glGetString                   getString;
  static IMPORT glFinish                      finish;
  static IMPORT glFlush                       flush;
  static IMPORT glHint                        hint;
  static IMPORT glClearDepth                  clearDepth;
  static IMPORT glDepthFunc                   depthFunc;
  static IMPORT glDepthMask                   depthMask;
  static IMPORT glDepthRange                  depthRange;
  static IMPORT glClearAccum                  clearAccum;
  static IMPORT glAccum                       accum;
  static IMPORT glMatrixMode                  matrixMode;
  static IMPORT glOrtho                       ortho;
  static IMPORT glFrustum                     frustum;
  static IMPORT glViewport                    viewport;
  static IMPORT glPushMatrix                  pushMatrix;
  static IMPORT glPopMatrix                   popMatrix;
  static IMPORT glLoadIdentity                loadIdentity;
  static IMPORT glLoadMatrixd                 loadMatrixd;
  static IMPORT glLoadMatrixf                 loadMatrixf;
  static IMPORT glMultMatrixd                 multMatrixd;
  static IMPORT glMultMatrixf                 multMatrixf;
  static IMPORT glRotated                     rotated;
  static IMPORT glRotatef                     rotatef;
  static IMPORT glScaled                      scaled;
  static IMPORT glScalef                      scalef;
  static IMPORT glTranslated                  translated;
  static IMPORT glTranslatef                  translatef;
  static IMPORT glIsList                      isList;
  static IMPORT glDeleteLists                 deleteLists;
  static IMPORT glGenLists                    genLists;
  static IMPORT glNewList                     newList;
  static IMPORT glEndList                     endList;
  static IMPORT glCallList                    callList;
  static IMPORT glCallLists                   callLists;
  static IMPORT glListBase                    listBase;
  static IMPORT glBegin                       begin;
  static IMPORT glEnd                         end;
  static IMPORT glVertex2d                    vertex2d;
  static IMPORT glVertex2f                    vertex2f;
  static IMPORT glVertex2i                    vertex2i;
  static IMPORT glVertex2s                    vertex2s;
  static IMPORT glVertex3d                    vertex3d;
  static IMPORT glVertex3f                    vertex3f;
  static IMPORT glVertex3i                    vertex3i;
  static IMPORT glVertex3s                    vertex3s;
  static IMPORT glVertex4d                    vertex4d;
  static IMPORT glVertex4f                    vertex4f;
  static IMPORT glVertex4i                    vertex4i;
  static IMPORT glVertex4s                    vertex4s;
  static IMPORT glVertex2dv                   vertex2dv;
  static IMPORT glVertex2fv                   vertex2fv;
  static IMPORT glVertex2iv                   vertex2iv;
  static IMPORT glVertex2sv                   vertex2sv;
  static IMPORT glVertex3dv                   vertex3dv;
  static IMPORT glVertex3fv                   vertex3fv;
  static IMPORT glVertex3iv                   vertex3iv;
  static IMPORT glVertex3sv                   vertex3sv;
  static IMPORT glVertex4dv                   vertex4dv;
  static IMPORT glVertex4fv                   vertex4fv;
  static IMPORT glVertex4iv                   vertex4iv;
  static IMPORT glVertex4sv                   vertex4sv;
  static IMPORT glNormal3b                    normal3b;
  static IMPORT glNormal3d                    normal3d;
  static IMPORT glNormal3f                    normal3f;
  static IMPORT glNormal3i                    normal3i;
  static IMPORT glNormal3s                    normal3s;
  static IMPORT glNormal3bv                   normal3bv;
  static IMPORT glNormal3dv                   normal3dv;
  static IMPORT glNormal3fv                   normal3fv;
  static IMPORT glNormal3iv                   normal3iv;
  static IMPORT glNormal3sv                   normal3sv;
  static IMPORT glIndexd                      indexd;
  static IMPORT glIndexf                      indexf;
  static IMPORT glIndexi                      indexi;
  static IMPORT glIndexs                      indexs;
  static IMPORT glIndexub                     indexub;
  static IMPORT glIndexdv                     indexdv;
  static IMPORT glIndexfv                     indexfv;
  static IMPORT glIndexiv                     indexiv;
  static IMPORT glIndexsv                     indexsv;
  static IMPORT glIndexubv                    indexubv;
  static IMPORT glColor3b                     color3b;
  static IMPORT glColor3d                     color3d;
  static IMPORT glColor3f                     color3f;
  static IMPORT glColor3i                     color3i;
  static IMPORT glColor3s                     color3s;
  static IMPORT glColor3ub                    color3ub;
  static IMPORT glColor3ui                    color3ui;
  static IMPORT glColor3us                    color3us;
  static IMPORT glColor4b                     color4b;
  static IMPORT glColor4d                     color4d;
  static IMPORT glColor4f                     color4f;
  static IMPORT glColor4i                     color4i;
  static IMPORT glColor4s                     color4s;
  static IMPORT glColor4ub                    color4ub;
  static IMPORT glColor4ui                    color4ui;
  static IMPORT glColor4us                    color4us;
  static IMPORT glColor3bv                    color3bv;
  static IMPORT glColor3dv                    color3dv;
  static IMPORT glColor3fv                    color3fv;
  static IMPORT glColor3iv                    color3iv;
  static IMPORT glColor3sv                    color3sv;
  static IMPORT glColor3ubv                   color3ubv;
  static IMPORT glColor3uiv                   color3uiv;
  static IMPORT glColor3usv                   color3usv;
  static IMPORT glColor4bv                    color4bv;
  static IMPORT glColor4dv                    color4dv;
  static IMPORT glColor4fv                    color4fv;
  static IMPORT glColor4iv                    color4iv;
  static IMPORT glColor4sv                    color4sv;
  static IMPORT glColor4ubv                   color4ubv;
  static IMPORT glColor4uiv                   color4uiv;
  static IMPORT glColor4usv                   color4usv;
  static IMPORT glTexCoord1d                  texCoord1d;
  static IMPORT glTexCoord1f                  texCoord1f;
  static IMPORT glTexCoord1i                  texCoord1i;
  static IMPORT glTexCoord1s                  texCoord1s;
  static IMPORT glTexCoord2d                  texCoord2d;
  static IMPORT glTexCoord2f                  texCoord2f;
  static IMPORT glTexCoord2i                  texCoord2i;
  static IMPORT glTexCoord2s                  texCoord2s;
  static IMPORT glTexCoord3d                  texCoord3d;
  static IMPORT glTexCoord3f                  texCoord3f;
  static IMPORT glTexCoord3i                  texCoord3i;
  static IMPORT glTexCoord3s                  texCoord3s;
  static IMPORT glTexCoord4d                  texCoord4d;
  static IMPORT glTexCoord4f                  texCoord4f;
  static IMPORT glTexCoord4i                  texCoord4i;
  static IMPORT glTexCoord4s                  texCoord4s;
  static IMPORT glTexCoord1dv                 texCoord1dv;
  static IMPORT glTexCoord1fv                 texCoord1fv;
  static IMPORT glTexCoord1iv                 texCoord1iv;
  static IMPORT glTexCoord1sv                 texCoord1sv;
  static IMPORT glTexCoord2dv                 texCoord2dv;
  static IMPORT glTexCoord2fv                 texCoord2fv;
  static IMPORT glTexCoord2iv                 texCoord2iv;
  static IMPORT glTexCoord2sv                 texCoord2sv;
  static IMPORT glTexCoord3dv                 texCoord3dv;
  static IMPORT glTexCoord3fv                 texCoord3fv;
  static IMPORT glTexCoord3iv                 texCoord3iv;
  static IMPORT glTexCoord3sv                 texCoord3sv;
  static IMPORT glTexCoord4dv                 texCoord4dv;
  static IMPORT glTexCoord4fv                 texCoord4fv;
  static IMPORT glTexCoord4iv                 texCoord4iv;
  static IMPORT glTexCoord4sv                 texCoord4sv;
  static IMPORT glRasterPos2d                 rasterPos2d;
  static IMPORT glRasterPos2f                 rasterPos2f;
  static IMPORT glRasterPos2i                 rasterPos2i;
  static IMPORT glRasterPos2s                 rasterPos2s;
  static IMPORT glRasterPos3d                 rasterPos3d;
  static IMPORT glRasterPos3f                 rasterPos3f;
  static IMPORT glRasterPos3i                 rasterPos3i;
  static IMPORT glRasterPos3s                 rasterPos3s;
  static IMPORT glRasterPos4d                 rasterPos4d;
  static IMPORT glRasterPos4f                 rasterPos4f;
  static IMPORT glRasterPos4i                 rasterPos4i;
  static IMPORT glRasterPos4s                 rasterPos4s;
  static IMPORT glRasterPos2dv                rasterPos2dv;
  static IMPORT glRasterPos2fv                rasterPos2fv;
  static IMPORT glRasterPos2iv                rasterPos2iv;
  static IMPORT glRasterPos2sv                rasterPos2sv;
  static IMPORT glRasterPos3dv                rasterPos3dv;
  static IMPORT glRasterPos3fv                rasterPos3fv;
  static IMPORT glRasterPos3iv                rasterPos3iv;
  static IMPORT glRasterPos3sv                rasterPos3sv;
  static IMPORT glRasterPos4dv                rasterPos4dv;
  static IMPORT glRasterPos4fv                rasterPos4fv;
  static IMPORT glRasterPos4iv                rasterPos4iv;
  static IMPORT glRasterPos4sv                rasterPos4sv;
  static IMPORT glRectd                       rectd;
  static IMPORT glRectf                       rectf;
  static IMPORT glRecti                       recti;
  static IMPORT glRects                       rects;
  static IMPORT glRectdv                      rectdv;
  static IMPORT glRectfv                      rectfv;
  static IMPORT glRectiv                      rectiv;
  static IMPORT glRectsv                      rectsv;
  static IMPORT glVertexPointer               vertexPointer;
  static IMPORT glNormalPointer               normalPointer;
  static IMPORT glColorPointer                colorPointer;
  static IMPORT glIndexPointer                indexPointer;
  static IMPORT glTexCoordPointer             texCoordPointer;
  static IMPORT glEdgeFlagPointer             edgeFlagPointer;
  static IMPORT glGetPointerv                 getPointerv;
  static IMPORT glArrayElement                arrayElement;
  static IMPORT glDrawArrays                  drawArrays;
  static IMPORT glDrawElements                drawElements;
  static IMPORT glInterleavedArrays           interleavedArrays;
  static IMPORT glShadeModel                  shadeModel;
  static IMPORT glLightf                      lightf;
  static IMPORT glLighti                      lighti;
  static IMPORT glLightfv                     lightfv;
  static IMPORT glLightiv                     lightiv;
  static IMPORT glGetLightfv                  getLightfv;
  static IMPORT glGetLightiv                  getLightiv;
  static IMPORT glLightModelf                 lightModelf;
  static IMPORT glLightModeli                 lightModeli;
  static IMPORT glLightModelfv                lightModelfv;
  static IMPORT glLightModeliv                lightModeliv;
  static IMPORT glMaterialf                   materialf;
  static IMPORT glMateriali                   materiali;
  static IMPORT glMaterialfv                  materialfv;
  static IMPORT glMaterialiv                  materialiv;
  static IMPORT glGetMaterialfv               getMaterialfv;
  static IMPORT glGetMaterialiv               getMaterialiv;
  static IMPORT glColorMaterial               colorMaterial;
  static IMPORT glPixelZoom                   pixelZoom;
  static IMPORT glPixelStoref                 pixelStoref;
  static IMPORT glPixelStorei                 pixelStorei;
  static IMPORT glPixelTransferf              pixelTransferf;
  static IMPORT glPixelTransferi              pixelTransferi;
  static IMPORT glPixelMapfv                  pixelMapfv;
  static IMPORT glPixelMapuiv                 pixelMapuiv;
  static IMPORT glPixelMapusv                 pixelMapusv;
  static IMPORT glGetPixelMapfv               getPixelMapfv;
  static IMPORT glGetPixelMapuiv              getPixelMapuiv;
  static IMPORT glGetPixelMapusv              getPixelMapusv;
  static IMPORT glBitmap                      bitmap;
  static IMPORT glReadPixels                  readPixels;
  static IMPORT glDrawPixels                  drawPixels;
  static IMPORT glCopyPixels                  copyPixels;
  static IMPORT glStencilFunc                 stencilFunc;
  static IMPORT glStencilMask                 stencilMask;
  static IMPORT glStencilOp                   stencilOp;
  static IMPORT glClearStencil                clearStencil;
  static IMPORT glTexGend                     texGend;
  static IMPORT glTexGenf                     texGenf;
  static IMPORT glTexGeni                     texGeni;
  static IMPORT glTexGendv                    texGendv;
  static IMPORT glTexGenfv                    texGenfv;
  static IMPORT glTexGeniv                    texGeniv;
  static IMPORT glGetTexGendv                 getTexGendv;
  static IMPORT glGetTexGenfv                 getTexGenfv;
  static IMPORT glGetTexGeniv                 getTexGeniv;
  static IMPORT glTexEnvf                     texEnvf;
  static IMPORT glTexEnvi                     texEnvi;
  static IMPORT glTexEnvfv                    texEnvfv;
  static IMPORT glTexEnviv                    texEnviv;
  static IMPORT glGetTexEnvfv                 getTexEnvfv;
  static IMPORT glGetTexEnviv                 getTexEnviv;
  static IMPORT glTexParameterf               texParameterf;
  static IMPORT glTexParameteri               texParameteri;
  static IMPORT glTexParameterfv              texParameterfv;
  static IMPORT glTexParameteriv              texParameteriv;
  static IMPORT glGetTexParameterfv           getTexParameterfv;
  static IMPORT glGetTexParameteriv           getTexParameteriv;
  static IMPORT glGetTexLevelParameterfv      getTexLevelParameterfv;
  static IMPORT glGetTexLevelParameteriv      getTexLevelParameteriv;
  static IMPORT glTexImage1D                  texImage1D;
  static IMPORT glTexImage2D                  texImage2D;
  static IMPORT glGetTexImage                 getTexImage;
  static IMPORT glGenTextures                 genTextures;
  static IMPORT glDeleteTextures              deleteTextures;
  static IMPORT glBindTexture                 bindTexture;
  static IMPORT glPrioritizeTextures          prioritizeTextures;
  static IMPORT glAreTexturesResident         areTexturesResident;
  static IMPORT glIsTexture                   isTexture;
  static IMPORT glTexSubImage1D               texSubImage1D;
  static IMPORT glTexSubImage2D               texSubImage2D;
  static IMPORT glCopyTexImage1D              copyTexImage1D;
  static IMPORT glCopyTexImage2D              copyTexImage2D;
  static IMPORT glCopyTexSubImage1D           copyTexSubImage1D;
  static IMPORT glCopyTexSubImage2D           copyTexSubImage2D;
  static IMPORT glMap1d                       map1d;
  static IMPORT glMap1f                       map1f;
  static IMPORT glMap2d                       map2d;
  static IMPORT glMap2f                       map2f;
  static IMPORT glGetMapdv                    getMapdv;
  static IMPORT glGetMapfv                    getMapfv;
  static IMPORT glGetMapiv                    getMapiv;
  static IMPORT glEvalCoord1d                 evalCoord1d;
  static IMPORT glEvalCoord1f                 evalCoord1f;
  static IMPORT glEvalCoord1dv                evalCoord1dv;
  static IMPORT glEvalCoord1fv                evalCoord1fv;
  static IMPORT glEvalCoord2d                 evalCoord2d;
  static IMPORT glEvalCoord2f                 evalCoord2f;
  static IMPORT glEvalCoord2dv                evalCoord2dv;
  static IMPORT glEvalCoord2fv                evalCoord2fv;
  static IMPORT glMapGrid1d                   mapGrid1d;
  static IMPORT glMapGrid1f                   mapGrid1f;
  static IMPORT glMapGrid2d                   mapGrid2d;
  static IMPORT glMapGrid2f                   mapGrid2f;
  static IMPORT glEvalPoint1                  evalPoint1;
  static IMPORT glEvalPoint2                  evalPoint2;
  static IMPORT glEvalMesh1                   evalMesh1;
  static IMPORT glEvalMesh2                   evalMesh2;
  static IMPORT glFogf                        fogf;
  static IMPORT glFogi                        fogi;
  static IMPORT glFogfv                       fogfv;
  static IMPORT glFogiv                       fogiv;
  static IMPORT glFeedbackBuffer              feedbackBuffer;
  static IMPORT glPassThrough                 passThrough;
  static IMPORT glSelectBuffer                selectBuffer;
  static IMPORT glInitNames                   initNames;
  static IMPORT glLoadName                    loadName;
  static IMPORT glPushName                    pushName;
  static IMPORT glPopName                     popName;
  static IMPORT glDrawRangeElements           drawRangeElements;
  static IMPORT glTexImage3D                  texImage3D;
  static IMPORT glTexSubImage3D               texSubImage3D;
  static IMPORT glCopyTexSubImage3D           copyTexSubImage3D;
  static IMPORT glColorTable                  colorTable;
  static IMPORT glColorSubTable               colorSubTable;
  static IMPORT glColorTableParameteriv       colorTableParameteriv;
  static IMPORT glColorTableParameterfv       colorTableParameterfv;
  static IMPORT glCopyColorSubTable           copyColorSubTable;
  static IMPORT glCopyColorTable              copyColorTable;
  static IMPORT glGetColorTable               getColorTable;
  static IMPORT glGetColorTableParameterfv    getColorTableParameterfv;
  static IMPORT glGetColorTableParameteriv    getColorTableParameteriv;
  static IMPORT glBlendEquation               blendEquation;
  static IMPORT glBlendColor                  blendColor;
  static IMPORT glHistogram                   histogram;
  static IMPORT glResetHistogram              resetHistogram;
  static IMPORT glGetHistogram                getHistogram;
  static IMPORT glGetHistogramParameterfv     getHistogramParameterfv;
  static IMPORT glGetHistogramParameteriv     getHistogramParameteriv;
  static IMPORT glMinmax                      minmax;
  static IMPORT glResetMinmax                 resetMinmax;
  static IMPORT glGetMinmax                   getMinmax;
  static IMPORT glGetMinmaxParameterfv        getMinmaxParameterfv;
  static IMPORT glGetMinmaxParameteriv        getMinmaxParameteriv;
  static IMPORT glConvolutionFilter1D         convolutionFilter1D;
  static IMPORT glConvolutionFilter2D         convolutionFilter2D;
  static IMPORT glConvolutionParameterf       convolutionParameterf;
  static IMPORT glConvolutionParameterfv      convolutionParameterfv;
  static IMPORT glConvolutionParameteri       convolutionParameteri;
  static IMPORT glConvolutionParameteriv      convolutionParameteriv;
  static IMPORT glCopyConvolutionFilter1D     copyConvolutionFilter1D;
  static IMPORT glCopyConvolutionFilter2D     copyConvolutionFilter2D;
  static IMPORT glGetConvolutionFilter        getConvolutionFilter;
  static IMPORT glGetConvolutionParameterfv   getConvolutionParameterfv;
  static IMPORT glGetConvolutionParameteriv   getConvolutionParameteriv;
  static IMPORT glSeparableFilter2D           separableFilter2D;
  static IMPORT glGetSeparableFilter          getSeparableFilter;
  static IMPORT glActiveTextureARB            activeTextureARB;
  static IMPORT glClientActiveTextureARB      clientActiveTextureARB;
  static IMPORT glMultiTexCoord1dARB          multiTexCoord1dARB;
  static IMPORT glMultiTexCoord1dvARB         multiTexCoord1dvARB;
  static IMPORT glMultiTexCoord1fARB          multiTexCoord1fARB;
  static IMPORT glMultiTexCoord1fvARB         multiTexCoord1fvARB;
  static IMPORT glMultiTexCoord1iARB          multiTexCoord1iARB;
  static IMPORT glMultiTexCoord1ivARB         multiTexCoord1ivARB;
  static IMPORT glMultiTexCoord1sARB          multiTexCoord1sARB;
  static IMPORT glMultiTexCoord1svARB         multiTexCoord1svARB;
  static IMPORT glMultiTexCoord2dARB          multiTexCoord2dARB;
  static IMPORT glMultiTexCoord2dvARB         multiTexCoord2dvARB;
  static IMPORT glMultiTexCoord2fARB          multiTexCoord2fARB;
  static IMPORT glMultiTexCoord2fvARB         multiTexCoord2fvARB;
  static IMPORT glMultiTexCoord2iARB          multiTexCoord2iARB;
  static IMPORT glMultiTexCoord2ivARB         multiTexCoord2ivARB;
  static IMPORT glMultiTexCoord2sARB          multiTexCoord2sARB;
  static IMPORT glMultiTexCoord2svARB         multiTexCoord2svARB;
  static IMPORT glMultiTexCoord3dARB          multiTexCoord3dARB;
  static IMPORT glMultiTexCoord3dvARB         multiTexCoord3dvARB;
  static IMPORT glMultiTexCoord3fARB          multiTexCoord3fARB;
  static IMPORT glMultiTexCoord3fvARB         multiTexCoord3fvARB;
  static IMPORT glMultiTexCoord3iARB          multiTexCoord3iARB;
  static IMPORT glMultiTexCoord3ivARB         multiTexCoord3ivARB;
  static IMPORT glMultiTexCoord3sARB          multiTexCoord3sARB;
  static IMPORT glMultiTexCoord3svARB         multiTexCoord3svARB;
  static IMPORT glMultiTexCoord4dARB          multiTexCoord4dARB;
  static IMPORT glMultiTexCoord4dvARB         multiTexCoord4dvARB;
  static IMPORT glMultiTexCoord4fARB          multiTexCoord4fARB;
  static IMPORT glMultiTexCoord4fvARB         multiTexCoord4fvARB;
  static IMPORT glMultiTexCoord4iARB          multiTexCoord4iARB;
  static IMPORT glMultiTexCoord4ivARB         multiTexCoord4ivARB;
  static IMPORT glMultiTexCoord4sARB          multiTexCoord4sARB;
  static IMPORT glMultiTexCoord4svARB         multiTexCoord4svARB;
  static IMPORT glBlendColorEXT               blendColorEXT;
  static IMPORT glPolygonOffsetEXT            polygonOffsetEXT;
  static IMPORT glTexImage3DEXT               texImage3DEXT;
  static IMPORT glTexSubImage3DEXT            texSubImage3DEXT;
  static IMPORT glCopyTexSubImage3DEXT        copyTexSubImage3DEXT;
  static IMPORT glGenTexturesEXT              genTexturesEXT;
  static IMPORT glDeleteTexturesEXT           deleteTexturesEXT;
  static IMPORT glBindTextureEXT              bindTextureEXT;
  static IMPORT glPrioritizeTexturesEXT       prioritizeTexturesEXT;
  static IMPORT glAreTexturesResidentEXT      areTexturesResidentEXT;
  static IMPORT glIsTextureEXT                isTextureEXT;
  static IMPORT glVertexPointerEXT            vertexPointerEXT;
  static IMPORT glNormalPointerEXT            normalPointerEXT;
  static IMPORT glColorPointerEXT             colorPointerEXT;
  static IMPORT glIndexPointerEXT             indexPointerEXT;
  static IMPORT glTexCoordPointerEXT          texCoordPointerEXT;
  static IMPORT glEdgeFlagPointerEXT          edgeFlagPointerEXT;
  static IMPORT glGetPointervEXT              getPointervEXT;
  static IMPORT glArrayElementEXT             arrayElementEXT;
  static IMPORT glDrawArraysEXT               drawArraysEXT;
  static IMPORT glBlendEquationEXT            blendEquationEXT;
  static IMPORT glPointParameterfEXT          pointParameterfEXT;
  static IMPORT glPointParameterfvEXT         pointParameterfvEXT;
  static IMPORT glPointParameterfSGIS         pointParameterfSGIS;
  static IMPORT glPointParameterfvSGIS        pointParameterfvSGIS;
  static IMPORT glColorTableEXT               colorTableEXT;
  static IMPORT glColorSubTableEXT            colorSubTableEXT;
  static IMPORT glGetColorTableEXT            getColorTableEXT;
  static IMPORT glGetColorTableParameterfvEXT getColorTableParameterfvEXT;
  static IMPORT glGetColorTableParameterivEXT getColorTableParameterivEXT;
  static IMPORT glLockArraysEXT               lockArraysEXT;
  static IMPORT glUnlockArraysEXT             unlockArraysEXT;
  static IMPORT glWindowPos2iMESA             windowPos2iMESA;
  static IMPORT glWindowPos2sMESA             windowPos2sMESA;
  static IMPORT glWindowPos2fMESA             windowPos2fMESA;
  static IMPORT glWindowPos2dMESA             windowPos2dMESA;
  static IMPORT glWindowPos2ivMESA            windowPos2ivMESA;
  static IMPORT glWindowPos2svMESA            windowPos2svMESA;
  static IMPORT glWindowPos2fvMESA            windowPos2fvMESA;
  static IMPORT glWindowPos2dvMESA            windowPos2dvMESA;
  static IMPORT glWindowPos3iMESA             windowPos3iMESA;
  static IMPORT glWindowPos3sMESA             windowPos3sMESA;
  static IMPORT glWindowPos3fMESA             windowPos3fMESA;
  static IMPORT glWindowPos3dMESA             windowPos3dMESA;
  static IMPORT glWindowPos3ivMESA            windowPos3ivMESA;
  static IMPORT glWindowPos3svMESA            windowPos3svMESA;
  static IMPORT glWindowPos3fvMESA            windowPos3fvMESA;
  static IMPORT glWindowPos3dvMESA            windowPos3dvMESA;
  static IMPORT glWindowPos4iMESA             windowPos4iMESA;
  static IMPORT glWindowPos4sMESA             windowPos4sMESA;
  static IMPORT glWindowPos4fMESA             windowPos4fMESA;
  static IMPORT glWindowPos4dMESA             windowPos4dMESA;
  static IMPORT glWindowPos4ivMESA            windowPos4ivMESA;
  static IMPORT glWindowPos4svMESA            windowPos4svMESA;
  static IMPORT glWindowPos4fvMESA            windowPos4fvMESA;
  static IMPORT glWindowPos4dvMESA            windowPos4dvMESA;
  static IMPORT glResizeBuffersMESA           resizeBuffersMESA;
  static IMPORT glGenOcclusionQueriesNV       genOcclusionQueriesNV;
  static IMPORT glDeleteOcclusionQueriesNV    deleteOcclusionQueriesNV;
  static IMPORT glBeginOcclusionQueryNV       beginOcclusionQueryNV;
  static IMPORT glEndOcclusionQueryNV         endOcclusionQueryNV;
  static IMPORT glGetOcclusionQueryivNV       getOcclusionQueryivNV;
  static IMPORT glGetOcclusionQueryuivNV      getOcclusionQueryuivNV;

  enum
  {
    VERSION_1_1 = 1,
    VERSION_1_2 = 1,
    FALSE = 0x0,
    TRUE = 0x1,
    BYTE = 0x1400,
    UNSIGNED_BYTE = 0x1401,
    SHORT = 0x1402,
    UNSIGNED_SHORT = 0x1403,
    INT = 0x1404,
    UNSIGNED_INT = 0x1405,
    FLOAT = 0x1406,
    DOUBLE = 0x140A,
    // 2_BYTES = 0x1407,
    // 3_BYTES = 0x1408,
    // 4_BYTES = 0x1409,
    POINTS = 0x0000,
    LINES = 0x0001,
    LINE_LOOP = 0x0002,
    LINE_STRIP = 0x0003,
    TRIANGLES = 0x0004,
    TRIANGLE_STRIP = 0x0005,
    TRIANGLE_FAN = 0x0006,
    QUADS = 0x0007,
    QUAD_STRIP = 0x0008,
    POLYGON = 0x0009,
    VERTEX_ARRAY = 0x8074,
    NORMAL_ARRAY = 0x8075,
    COLOR_ARRAY = 0x8076,
    INDEX_ARRAY = 0x8077,
    TEXTURE_COORD_ARRAY = 0x8078,
    EDGE_FLAG_ARRAY = 0x8079,
    VERTEX_ARRAY_SIZE = 0x807A,
    VERTEX_ARRAY_TYPE = 0x807B,
    VERTEX_ARRAY_STRIDE = 0x807C,
    NORMAL_ARRAY_TYPE = 0x807E,
    NORMAL_ARRAY_STRIDE = 0x807F,
    COLOR_ARRAY_SIZE = 0x8081,
    COLOR_ARRAY_TYPE = 0x8082,
    COLOR_ARRAY_STRIDE = 0x8083,
    INDEX_ARRAY_TYPE = 0x8085,
    INDEX_ARRAY_STRIDE = 0x8086,
    TEXTURE_COORD_ARRAY_SIZE = 0x8088,
    TEXTURE_COORD_ARRAY_TYPE = 0x8089,
    TEXTURE_COORD_ARRAY_STRIDE = 0x808A,
    EDGE_FLAG_ARRAY_STRIDE = 0x808C,
    VERTEX_ARRAY_POINTER = 0x808E,
    NORMAL_ARRAY_POINTER = 0x808F,
    COLOR_ARRAY_POINTER = 0x8090,
    INDEX_ARRAY_POINTER = 0x8091,
    TEXTURE_COORD_ARRAY_POINTER = 0x8092,
    EDGE_FLAG_ARRAY_POINTER = 0x8093,
    V2F = 0x2A20,
    V3F = 0x2A21,
    C4UB_V2F = 0x2A22,
    C4UB_V3F = 0x2A23,
    C3F_V3F = 0x2A24,
    N3F_V3F = 0x2A25,
    C4F_N3F_V3F = 0x2A26,
    T2F_V3F = 0x2A27,
    T4F_V4F = 0x2A28,
    T2F_C4UB_V3F = 0x2A29,
    T2F_C3F_V3F = 0x2A2A,
    T2F_N3F_V3F = 0x2A2B,
    T2F_C4F_N3F_V3F = 0x2A2C,
    T4F_C4F_N3F_V4F = 0x2A2D,
    MATRIX_MODE = 0x0BA0,
    MODELVIEW = 0x1700,
    PROJECTION = 0x1701,
    TEXTURE = 0x1702,
    POINT_SMOOTH = 0x0B10,
    POINT_SIZE = 0x0B11,
    POINT_SIZE_GRANULARITY = 0x0B13,
    POINT_SIZE_RANGE = 0x0B12,
    LINE_SMOOTH = 0x0B20,
    LINE_STIPPLE = 0x0B24,
    LINE_STIPPLE_PATTERN = 0x0B25,
    LINE_STIPPLE_REPEAT = 0x0B26,
    LINE_WIDTH = 0x0B21,
    LINE_WIDTH_GRANULARITY = 0x0B23,
    LINE_WIDTH_RANGE = 0x0B22,
    POINT = 0x1B00,
    LINE = 0x1B01,
    FILL = 0x1B02,
    CW = 0x0900,
    CCW = 0x0901,
    FRONT = 0x0404,
    BACK = 0x0405,
    POLYGON_MODE = 0x0B40,
    POLYGON_SMOOTH = 0x0B41,
    POLYGON_STIPPLE = 0x0B42,
    EDGE_FLAG = 0x0B43,
    CULL_FACE = 0x0B44,
    CULL_FACE_MODE = 0x0B45,
    FRONT_FACE = 0x0B46,
    POLYGON_OFFSET_FACTOR = 0x8038,
    POLYGON_OFFSET_UNITS = 0x2A00,
    POLYGON_OFFSET_POINT = 0x2A01,
    POLYGON_OFFSET_LINE = 0x2A02,
    POLYGON_OFFSET_FILL = 0x8037,
    COMPILE = 0x1300,
    COMPILE_AND_EXECUTE = 0x1301,
    LIST_BASE = 0x0B32,
    LIST_INDEX = 0x0B33,
    LIST_MODE = 0x0B30,
    NEVER = 0x0200,
    LESS = 0x0201,
    EQUAL = 0x0202,
    LEQUAL = 0x0203,
    GREATER = 0x0204,
    NOTEQUAL = 0x0205,
    GEQUAL = 0x0206,
    ALWAYS = 0x0207,
    DEPTH_TEST = 0x0B71,
    DEPTH_BITS = 0x0D56,
    DEPTH_CLEAR_VALUE = 0x0B73,
    DEPTH_FUNC = 0x0B74,
    DEPTH_RANGE = 0x0B70,
    DEPTH_WRITEMASK = 0x0B72,
    DEPTH_COMPONENT = 0x1902,
    LIGHTING = 0x0B50,
    LIGHT0 = 0x4000,
    LIGHT1 = 0x4001,
    LIGHT2 = 0x4002,
    LIGHT3 = 0x4003,
    LIGHT4 = 0x4004,
    LIGHT5 = 0x4005,
    LIGHT6 = 0x4006,
    LIGHT7 = 0x4007,
    SPOT_EXPONENT = 0x1205,
    SPOT_CUTOFF = 0x1206,
    CONSTANT_ATTENUATION = 0x1207,
    LINEAR_ATTENUATION = 0x1208,
    QUADRATIC_ATTENUATION = 0x1209,
    AMBIENT = 0x1200,
    DIFFUSE = 0x1201,
    SPECULAR = 0x1202,
    SHININESS = 0x1601,
    EMISSION = 0x1600,
    POSITION = 0x1203,
    SPOT_DIRECTION = 0x1204,
    AMBIENT_AND_DIFFUSE = 0x1602,
    COLOR_INDEXES = 0x1603,
    LIGHT_MODEL_TWO_SIDE = 0x0B52,
    LIGHT_MODEL_LOCAL_VIEWER = 0x0B51,
    LIGHT_MODEL_AMBIENT = 0x0B53,
    FRONT_AND_BACK = 0x0408,
    SHADE_MODEL = 0x0B54,
    FLAT = 0x1D00,
    SMOOTH = 0x1D01,
    COLOR_MATERIAL = 0x0B57,
    COLOR_MATERIAL_FACE = 0x0B55,
    COLOR_MATERIAL_PARAMETER = 0x0B56,
    NORMALIZE = 0x0BA1,
    CLIP_PLANE0 = 0x3000,
    CLIP_PLANE1 = 0x3001,
    CLIP_PLANE2 = 0x3002,
    CLIP_PLANE3 = 0x3003,
    CLIP_PLANE4 = 0x3004,
    CLIP_PLANE5 = 0x3005,
    ACCUM_RED_BITS = 0x0D58,
    ACCUM_GREEN_BITS = 0x0D59,
    ACCUM_BLUE_BITS = 0x0D5A,
    ACCUM_ALPHA_BITS = 0x0D5B,
    ACCUM_CLEAR_VALUE = 0x0B80,
    ACCUM = 0x0100,
    ADD = 0x0104,
    LOAD = 0x0101,
    MULT = 0x0103,
    RETURN = 0x0102,
    ALPHA_TEST = 0x0BC0,
    ALPHA_TEST_REF = 0x0BC2,
    ALPHA_TEST_FUNC = 0x0BC1,
    BLEND = 0x0BE2,
    BLEND_SRC = 0x0BE1,
    BLEND_DST = 0x0BE0,
    ZERO = 0x0,
    ONE = 0x1,
    SRC_COLOR = 0x0300,
    ONE_MINUS_SRC_COLOR = 0x0301,
    DST_COLOR = 0x0306,
    ONE_MINUS_DST_COLOR = 0x0307,
    SRC_ALPHA = 0x0302,
    ONE_MINUS_SRC_ALPHA = 0x0303,
    DST_ALPHA = 0x0304,
    ONE_MINUS_DST_ALPHA = 0x0305,
    SRC_ALPHA_SATURATE = 0x0308,
    CONSTANT_COLOR = 0x8001,
    ONE_MINUS_CONSTANT_COLOR = 0x8002,
    CONSTANT_ALPHA = 0x8003,
    ONE_MINUS_CONSTANT_ALPHA = 0x8004,
    FEEDBACK = 0x1C01,
    RENDER = 0x1C00,
    SELECT = 0x1C02,
    // 2D = 0x0600,
    // 3D = 0x0601,
    // 3D_COLOR = 0x0602,
    // 3D_COLOR_TEXTURE = 0x0603,
    // 4D_COLOR_TEXTURE = 0x0604,
    POINT_TOKEN = 0x0701,
    LINE_TOKEN = 0x0702,
    LINE_RESET_TOKEN = 0x0707,
    POLYGON_TOKEN = 0x0703,
    BITMAP_TOKEN = 0x0704,
    DRAW_PIXEL_TOKEN = 0x0705,
    COPY_PIXEL_TOKEN = 0x0706,
    PASS_THROUGH_TOKEN = 0x0700,
    FEEDBACK_BUFFER_POINTER = 0x0DF0,
    FEEDBACK_BUFFER_SIZE = 0x0DF1,
    FEEDBACK_BUFFER_TYPE = 0x0DF2,
    SELECTION_BUFFER_POINTER = 0x0DF3,
    SELECTION_BUFFER_SIZE = 0x0DF4,
    FOG = 0x0B60,
    FOG_MODE = 0x0B65,
    FOG_DENSITY = 0x0B62,
    FOG_COLOR = 0x0B66,
    FOG_INDEX = 0x0B61,
    FOG_START = 0x0B63,
    FOG_END = 0x0B64,
    LINEAR = 0x2601,
    EXP = 0x0800,
    EXP2 = 0x0801,
    LOGIC_OP = 0x0BF1,
    INDEX_LOGIC_OP = 0x0BF1,
    COLOR_LOGIC_OP = 0x0BF2,
    LOGIC_OP_MODE = 0x0BF0,
    CLEAR = 0x1500,
    SET = 0x150F,
    COPY = 0x1503,
    COPY_INVERTED = 0x150C,
    NOOP = 0x1505,
    INVERT = 0x150A,
    AND = 0x1501,
    NAND = 0x150E,
    OR = 0x1507,
    NOR = 0x1508,
    XOR = 0x1506,
    EQUIV = 0x1509,
    AND_REVERSE = 0x1502,
    AND_INVERTED = 0x1504,
    OR_REVERSE = 0x150B,
    OR_INVERTED = 0x150D,
    STENCIL_TEST = 0x0B90,
    STENCIL_WRITEMASK = 0x0B98,
    STENCIL_BITS = 0x0D57,
    STENCIL_FUNC = 0x0B92,
    STENCIL_VALUE_MASK = 0x0B93,
    STENCIL_REF = 0x0B97,
    STENCIL_FAIL = 0x0B94,
    STENCIL_PASS_DEPTH_PASS = 0x0B96,
    STENCIL_PASS_DEPTH_FAIL = 0x0B95,
    STENCIL_CLEAR_VALUE = 0x0B91,
    STENCIL_INDEX = 0x1901,
    KEEP = 0x1E00,
    REPLACE = 0x1E01,
    INCR = 0x1E02,
    DECR = 0x1E03,
    NONE = 0x0,
    LEFT = 0x0406,
    RIGHT = 0x0407,
    FRONT_LEFT = 0x0400,
    FRONT_RIGHT = 0x0401,
    BACK_LEFT = 0x0402,
    BACK_RIGHT = 0x0403,
    AUX0 = 0x0409,
    AUX1 = 0x040A,
    AUX2 = 0x040B,
    AUX3 = 0x040C,
    COLOR_INDEX = 0x1900,
    RED = 0x1903,
    GREEN = 0x1904,
    BLUE = 0x1905,
    ALPHA = 0x1906,
    LUMINANCE = 0x1909,
    LUMINANCE_ALPHA = 0x190A,
    ALPHA_BITS = 0x0D55,
    RED_BITS = 0x0D52,
    GREEN_BITS = 0x0D53,
    BLUE_BITS = 0x0D54,
    INDEX_BITS = 0x0D51,
    SUBPIXEL_BITS = 0x0D50,
    AUX_BUFFERS = 0x0C00,
    READ_BUFFER = 0x0C02,
    DRAW_BUFFER = 0x0C01,
    DOUBLEBUFFER = 0x0C32,
    STEREO = 0x0C33,
    BITMAP = 0x1A00,
    COLOR = 0x1800,
    DEPTH = 0x1801,
    STENCIL = 0x1802,
    DITHER = 0x0BD0,
    RGB = 0x1907,
    RGBA = 0x1908,
    MAX_LIST_NESTING = 0x0B31,
    MAX_ATTRIB_STACK_DEPTH = 0x0D35,
    MAX_MODELVIEW_STACK_DEPTH = 0x0D36,
    MAX_NAME_STACK_DEPTH = 0x0D37,
    MAX_PROJECTION_STACK_DEPTH = 0x0D38,
    MAX_TEXTURE_STACK_DEPTH = 0x0D39,
    MAX_EVAL_ORDER = 0x0D30,
    MAX_LIGHTS = 0x0D31,
    MAX_CLIP_PLANES = 0x0D32,
    MAX_TEXTURE_SIZE = 0x0D33,
    MAX_PIXEL_MAP_TABLE = 0x0D34,
    MAX_VIEWPORT_DIMS = 0x0D3A,
    MAX_CLIENT_ATTRIB_STACK_DEPTH = 0x0D3B,
    ATTRIB_STACK_DEPTH = 0x0BB0,
    CLIENT_ATTRIB_STACK_DEPTH = 0x0BB1,
    COLOR_CLEAR_VALUE = 0x0C22,
    COLOR_WRITEMASK = 0x0C23,
    CURRENT_INDEX = 0x0B01,
    CURRENT_COLOR = 0x0B00,
    CURRENT_NORMAL = 0x0B02,
    CURRENT_RASTER_COLOR = 0x0B04,
    CURRENT_RASTER_DISTANCE = 0x0B09,
    CURRENT_RASTER_INDEX = 0x0B05,
    CURRENT_RASTER_POSITION = 0x0B07,
    CURRENT_RASTER_TEXTURE_COORDS = 0x0B06,
    CURRENT_RASTER_POSITION_VALID = 0x0B08,
    CURRENT_TEXTURE_COORDS = 0x0B03,
    INDEX_CLEAR_VALUE = 0x0C20,
    INDEX_MODE = 0x0C30,
    INDEX_WRITEMASK = 0x0C21,
    MODELVIEW_MATRIX = 0x0BA6,
    MODELVIEW_STACK_DEPTH = 0x0BA3,
    NAME_STACK_DEPTH = 0x0D70,
    PROJECTION_MATRIX = 0x0BA7,
    PROJECTION_STACK_DEPTH = 0x0BA4,
    RENDER_MODE = 0x0C40,
    RGBA_MODE = 0x0C31,
    TEXTURE_MATRIX = 0x0BA8,
    TEXTURE_STACK_DEPTH = 0x0BA5,
    VIEWPORT = 0x0BA2,
    AUTO_NORMAL = 0x0D80,
    MAP1_COLOR_4 = 0x0D90,
    MAP1_GRID_DOMAIN = 0x0DD0,
    MAP1_GRID_SEGMENTS = 0x0DD1,
    MAP1_INDEX = 0x0D91,
    MAP1_NORMAL = 0x0D92,
    MAP1_TEXTURE_COORD_1 = 0x0D93,
    MAP1_TEXTURE_COORD_2 = 0x0D94,
    MAP1_TEXTURE_COORD_3 = 0x0D95,
    MAP1_TEXTURE_COORD_4 = 0x0D96,
    MAP1_VERTEX_3 = 0x0D97,
    MAP1_VERTEX_4 = 0x0D98,
    MAP2_COLOR_4 = 0x0DB0,
    MAP2_GRID_DOMAIN = 0x0DD2,
    MAP2_GRID_SEGMENTS = 0x0DD3,
    MAP2_INDEX = 0x0DB1,
    MAP2_NORMAL = 0x0DB2,
    MAP2_TEXTURE_COORD_1 = 0x0DB3,
    MAP2_TEXTURE_COORD_2 = 0x0DB4,
    MAP2_TEXTURE_COORD_3 = 0x0DB5,
    MAP2_TEXTURE_COORD_4 = 0x0DB6,
    MAP2_VERTEX_3 = 0x0DB7,
    MAP2_VERTEX_4 = 0x0DB8,
    COEFF = 0x0A00,
    DOMAIN = 0x0A02,
    ORDER = 0x0A01,
    FOG_HINT = 0x0C54,
    LINE_SMOOTH_HINT = 0x0C52,
    PERSPECTIVE_CORRECTION_HINT = 0x0C50,
    POINT_SMOOTH_HINT = 0x0C51,
    POLYGON_SMOOTH_HINT = 0x0C53,
    DONT_CARE = 0x1100,
    FASTEST = 0x1101,
    NICEST = 0x1102,
    SCISSOR_TEST = 0x0C11,
    SCISSOR_BOX = 0x0C10,
    MAP_COLOR = 0x0D10,
    MAP_STENCIL = 0x0D11,
    INDEX_SHIFT = 0x0D12,
    INDEX_OFFSET = 0x0D13,
    RED_SCALE = 0x0D14,
    RED_BIAS = 0x0D15,
    GREEN_SCALE = 0x0D18,
    GREEN_BIAS = 0x0D19,
    BLUE_SCALE = 0x0D1A,
    BLUE_BIAS = 0x0D1B,
    ALPHA_SCALE = 0x0D1C,
    ALPHA_BIAS = 0x0D1D,
    DEPTH_SCALE = 0x0D1E,
    DEPTH_BIAS = 0x0D1F,
    PIXEL_MAP_S_TO_S_SIZE = 0x0CB1,
    PIXEL_MAP_I_TO_I_SIZE = 0x0CB0,
    PIXEL_MAP_I_TO_R_SIZE = 0x0CB2,
    PIXEL_MAP_I_TO_G_SIZE = 0x0CB3,
    PIXEL_MAP_I_TO_B_SIZE = 0x0CB4,
    PIXEL_MAP_I_TO_A_SIZE = 0x0CB5,
    PIXEL_MAP_R_TO_R_SIZE = 0x0CB6,
    PIXEL_MAP_G_TO_G_SIZE = 0x0CB7,
    PIXEL_MAP_B_TO_B_SIZE = 0x0CB8,
    PIXEL_MAP_A_TO_A_SIZE = 0x0CB9,
    PIXEL_MAP_S_TO_S = 0x0C71,
    PIXEL_MAP_I_TO_I = 0x0C70,
    PIXEL_MAP_I_TO_R = 0x0C72,
    PIXEL_MAP_I_TO_G = 0x0C73,
    PIXEL_MAP_I_TO_B = 0x0C74,
    PIXEL_MAP_I_TO_A = 0x0C75,
    PIXEL_MAP_R_TO_R = 0x0C76,
    PIXEL_MAP_G_TO_G = 0x0C77,
    PIXEL_MAP_B_TO_B = 0x0C78,
    PIXEL_MAP_A_TO_A = 0x0C79,
    PACK_ALIGNMENT = 0x0D05,
    PACK_LSB_FIRST = 0x0D01,
    PACK_ROW_LENGTH = 0x0D02,
    PACK_SKIP_PIXELS = 0x0D04,
    PACK_SKIP_ROWS = 0x0D03,
    PACK_SWAP_BYTES = 0x0D00,
    UNPACK_ALIGNMENT = 0x0CF5,
    UNPACK_LSB_FIRST = 0x0CF1,
    UNPACK_ROW_LENGTH = 0x0CF2,
    UNPACK_SKIP_PIXELS = 0x0CF4,
    UNPACK_SKIP_ROWS = 0x0CF3,
    UNPACK_SWAP_BYTES = 0x0CF0,
    ZOOM_X = 0x0D16,
    ZOOM_Y = 0x0D17,
    TEXTURE_ENV = 0x2300,
    TEXTURE_ENV_MODE = 0x2200,
    TEXTURE_1D = 0x0DE0,
    TEXTURE_2D = 0x0DE1,
    TEXTURE_WRAP_S = 0x2802,
    TEXTURE_WRAP_T = 0x2803,
    TEXTURE_MAG_FILTER = 0x2800,
    TEXTURE_MIN_FILTER = 0x2801,
    TEXTURE_ENV_COLOR = 0x2201,
    TEXTURE_GEN_S = 0x0C60,
    TEXTURE_GEN_T = 0x0C61,
    TEXTURE_GEN_MODE = 0x2500,
    TEXTURE_BORDER_COLOR = 0x1004,
    TEXTURE_WIDTH = 0x1000,
    TEXTURE_HEIGHT = 0x1001,
    TEXTURE_BORDER = 0x1005,
    TEXTURE_COMPONENTS = 0x1003,
    TEXTURE_RED_SIZE = 0x805C,
    TEXTURE_GREEN_SIZE = 0x805D,
    TEXTURE_BLUE_SIZE = 0x805E,
    TEXTURE_ALPHA_SIZE = 0x805F,
    TEXTURE_LUMINANCE_SIZE = 0x8060,
    TEXTURE_INTENSITY_SIZE = 0x8061,
    NEAREST_MIPMAP_NEAREST = 0x2700,
    NEAREST_MIPMAP_LINEAR = 0x2702,
    LINEAR_MIPMAP_NEAREST = 0x2701,
    LINEAR_MIPMAP_LINEAR = 0x2703,
    OBJECT_LINEAR = 0x2401,
    OBJECT_PLANE = 0x2501,
    EYE_LINEAR = 0x2400,
    EYE_PLANE = 0x2502,
    SPHERE_MAP = 0x2402,
    DECAL = 0x2101,
    MODULATE = 0x2100,
    NEAREST = 0x2600,
    REPEAT = 0x2901,
    CLAMP = 0x2900,
    S = 0x2000,
    T = 0x2001,
    R = 0x2002,
    Q = 0x2003,
    TEXTURE_GEN_R = 0x0C62,
    TEXTURE_GEN_Q = 0x0C63,
    PROXY_TEXTURE_1D = 0x8063,
    PROXY_TEXTURE_2D = 0x8064,
    TEXTURE_PRIORITY = 0x8066,
    TEXTURE_RESIDENT = 0x8067,
    TEXTURE_BINDING_1D = 0x8068,
    TEXTURE_BINDING_2D = 0x8069,
    TEXTURE_INTERNAL_FORMAT = 0x1003,
    PACK_SKIP_IMAGES = 0x806B,
    PACK_IMAGE_HEIGHT = 0x806C,
    UNPACK_SKIP_IMAGES = 0x806D,
    UNPACK_IMAGE_HEIGHT = 0x806E,
    TEXTURE_3D = 0x806F,
    PROXY_TEXTURE_3D = 0x8070,
    TEXTURE_DEPTH = 0x8071,
    TEXTURE_WRAP_R = 0x8072,
    MAX_3D_TEXTURE_SIZE = 0x8073,
    TEXTURE_BINDING_3D = 0x806A,
    ALPHA4 = 0x803B,
    ALPHA8 = 0x803C,
    ALPHA12 = 0x803D,
    ALPHA16 = 0x803E,
    LUMINANCE4 = 0x803F,
    LUMINANCE8 = 0x8040,
    LUMINANCE12 = 0x8041,
    LUMINANCE16 = 0x8042,
    LUMINANCE4_ALPHA4 = 0x8043,
    LUMINANCE6_ALPHA2 = 0x8044,
    LUMINANCE8_ALPHA8 = 0x8045,
    LUMINANCE12_ALPHA4 = 0x8046,
    LUMINANCE12_ALPHA12 = 0x8047,
    LUMINANCE16_ALPHA16 = 0x8048,
    INTENSITY = 0x8049,
    INTENSITY4 = 0x804A,
    INTENSITY8 = 0x804B,
    INTENSITY12 = 0x804C,
    INTENSITY16 = 0x804D,
    R3_G3_B2 = 0x2A10,
    RGB4 = 0x804F,
    RGB5 = 0x8050,
    RGB8 = 0x8051,
    RGB10 = 0x8052,
    RGB12 = 0x8053,
    RGB16 = 0x8054,
    RGBA2 = 0x8055,
    RGBA4 = 0x8056,
    RGB5_A1 = 0x8057,
    RGBA8 = 0x8058,
    RGB10_A2 = 0x8059,
    RGBA12 = 0x805A,
    RGBA16 = 0x805B,
    VENDOR = 0x1F00,
    RENDERER = 0x1F01,
    VERSION = 0x1F02,
    EXTENSIONS = 0x1F03,
    NO_ERROR = 0x0,
    INVALID_VALUE = 0x0501,
    INVALID_ENUM = 0x0500,
    INVALID_OPERATION = 0x0502,
    STACK_OVERFLOW = 0x0503,
    STACK_UNDERFLOW = 0x0504,
    OUT_OF_MEMORY = 0x0505,
    RESCALE_NORMAL = 0x803A,
    CLAMP_TO_EDGE = 0x812F,
    MAX_ELEMENTS_VERTICES = 0x80E8,
    MAX_ELEMENTS_INDICES = 0x80E9,
    BGR = 0x80E0,
    BGRA = 0x80E1,
    UNSIGNED_BYTE_3_3_2 = 0x8032,
    UNSIGNED_BYTE_2_3_3_REV = 0x8362,
    UNSIGNED_SHORT_5_6_5 = 0x8363,
    UNSIGNED_SHORT_5_6_5_REV = 0x8364,
    UNSIGNED_SHORT_4_4_4_4 = 0x8033,
    UNSIGNED_SHORT_4_4_4_4_REV = 0x8365,
    UNSIGNED_SHORT_5_5_5_1 = 0x8034,
    UNSIGNED_SHORT_1_5_5_5_REV = 0x8366,
    UNSIGNED_INT_8_8_8_8 = 0x8035,
    UNSIGNED_INT_8_8_8_8_REV = 0x8367,
    UNSIGNED_INT_10_10_10_2 = 0x8036,
    UNSIGNED_INT_2_10_10_10_REV = 0x8368,
    LIGHT_MODEL_COLOR_CONTROL = 0x81F8,
    SINGLE_COLOR = 0x81F9,
    SEPARATE_SPECULAR_COLOR = 0x81FA,
    TEXTURE_MIN_LOD = 0x813A,
    TEXTURE_MAX_LOD = 0x813B,
    TEXTURE_BASE_LEVEL = 0x813C,
    TEXTURE_MAX_LEVEL = 0x813D,
    SMOOTH_POINT_SIZE_RANGE = 0x0B12,
    SMOOTH_POINT_SIZE_GRANULARITY = 0x0B13,
    SMOOTH_LINE_WIDTH_RANGE = 0x0B22,
    SMOOTH_LINE_WIDTH_GRANULARITY = 0x0B23,
    ALIASED_POINT_SIZE_RANGE = 0x846D,
    ALIASED_LINE_WIDTH_RANGE = 0x846E,
    COLOR_TABLE = 0x80D0,
    POST_CONVOLUTION_COLOR_TABLE = 0x80D1,
    POST_COLOR_MATRIX_COLOR_TABLE = 0x80D2,
    PROXY_COLOR_TABLE = 0x80D3,
    PROXY_POST_CONVOLUTION_COLOR_TABLE = 0x80D4,
    PROXY_POST_COLOR_MATRIX_COLOR_TABLE = 0x80D5,
    COLOR_TABLE_SCALE = 0x80D6,
    COLOR_TABLE_BIAS = 0x80D7,
    COLOR_TABLE_FORMAT = 0x80D8,
    COLOR_TABLE_WIDTH = 0x80D9,
    COLOR_TABLE_RED_SIZE = 0x80DA,
    COLOR_TABLE_GREEN_SIZE = 0x80DB,
    COLOR_TABLE_BLUE_SIZE = 0x80DC,
    COLOR_TABLE_ALPHA_SIZE = 0x80DD,
    COLOR_TABLE_LUMINANCE_SIZE = 0x80DE,
    COLOR_TABLE_INTENSITY_SIZE = 0x80DF,
    CONVOLUTION_1D = 0x8010,
    CONVOLUTION_2D = 0x8011,
    SEPARABLE_2D = 0x8012,
    CONVOLUTION_BORDER_MODE = 0x8013,
    CONVOLUTION_FILTER_SCALE = 0x8014,
    CONVOLUTION_FILTER_BIAS = 0x8015,
    REDUCE = 0x8016,
    CONVOLUTION_FORMAT = 0x8017,
    CONVOLUTION_WIDTH = 0x8018,
    CONVOLUTION_HEIGHT = 0x8019,
    MAX_CONVOLUTION_WIDTH = 0x801A,
    MAX_CONVOLUTION_HEIGHT = 0x801B,
    POST_CONVOLUTION_RED_SCALE = 0x801C,
    POST_CONVOLUTION_GREEN_SCALE = 0x801D,
    POST_CONVOLUTION_BLUE_SCALE = 0x801E,
    POST_CONVOLUTION_ALPHA_SCALE = 0x801F,
    POST_CONVOLUTION_RED_BIAS = 0x8020,
    POST_CONVOLUTION_GREEN_BIAS = 0x8021,
    POST_CONVOLUTION_BLUE_BIAS = 0x8022,
    POST_CONVOLUTION_ALPHA_BIAS = 0x8023,
    CONSTANT_BORDER = 0x8151,
    REPLICATE_BORDER = 0x8153,
    CONVOLUTION_BORDER_COLOR = 0x8154,
    COLOR_MATRIX = 0x80B1,
    COLOR_MATRIX_STACK_DEPTH = 0x80B2,
    MAX_COLOR_MATRIX_STACK_DEPTH = 0x80B3,
    POST_COLOR_MATRIX_RED_SCALE = 0x80B4,
    POST_COLOR_MATRIX_GREEN_SCALE = 0x80B5,
    POST_COLOR_MATRIX_BLUE_SCALE = 0x80B6,
    POST_COLOR_MATRIX_ALPHA_SCALE = 0x80B7,
    POST_COLOR_MATRIX_RED_BIAS = 0x80B8,
    POST_COLOR_MATRIX_GREEN_BIAS = 0x80B9,
    POST_COLOR_MATRIX_BLUE_BIAS = 0x80BA,
    POST_COLOR_MATRIX_ALPHA_BIAS = 0x80BB,
    HISTOGRAM = 0x8024,
    PROXY_HISTOGRAM = 0x8025,
    HISTOGRAM_WIDTH = 0x8026,
    HISTOGRAM_FORMAT = 0x8027,
    HISTOGRAM_RED_SIZE = 0x8028,
    HISTOGRAM_GREEN_SIZE = 0x8029,
    HISTOGRAM_BLUE_SIZE = 0x802A,
    HISTOGRAM_ALPHA_SIZE = 0x802B,
    HISTOGRAM_LUMINANCE_SIZE = 0x802C,
    HISTOGRAM_SINK = 0x802D,
    MINMAX = 0x802E,
    MINMAX_FORMAT = 0x802F,
    MINMAX_SINK = 0x8030,
    TABLE_TOO_LARGE = 0x8031,
    BLEND_EQUATION = 0x8009,
    MIN = 0x8007,
    MAX = 0x8008,
    FUNC_ADD = 0x8006,
    FUNC_SUBTRACT = 0x800A,
    FUNC_REVERSE_SUBTRACT = 0x800B,
    BLEND_COLOR = 0x8005,
    CURRENT_BIT = 0x00000001,
    POINT_BIT = 0x00000002,
    LINE_BIT = 0x00000004,
    POLYGON_BIT = 0x00000008,
    POLYGON_STIPPLE_BIT = 0x00000010,
    PIXEL_MODE_BIT = 0x00000020,
    LIGHTING_BIT = 0x00000040,
    FOG_BIT = 0x00000080,
    DEPTH_BUFFER_BIT = 0x00000100,
    ACCUM_BUFFER_BIT = 0x00000200,
    STENCIL_BUFFER_BIT = 0x00000400,
    VIEWPORT_BIT = 0x00000800,
    TRANSFORM_BIT = 0x00001000,
    ENABLE_BIT = 0x00002000,
    COLOR_BUFFER_BIT = 0x00004000,
    HINT_BIT = 0x00008000,
    EVAL_BIT = 0x00010000,
    LIST_BIT = 0x00020000,
    TEXTURE_BIT = 0x00040000,
    SCISSOR_BIT = 0x00080000,
    ALL_ATTRIB_BITS = 0x000FFFFF,
    CLIENT_PIXEL_STORE_BIT = 0x00000001,
    CLIENT_VERTEX_ARRAY_BIT = 0x00000002,
    ALL_CLIENT_ATTRIB_BITS = 0xFFFFFFFF,
    ARB_MULTITEXTURE = 1,
    TEXTURE0_ARB = 0x84C0,
    TEXTURE1_ARB = 0x84C1,
    TEXTURE2_ARB = 0x84C2,
    TEXTURE3_ARB = 0x84C3,
    TEXTURE4_ARB = 0x84C4,
    TEXTURE5_ARB = 0x84C5,
    TEXTURE6_ARB = 0x84C6,
    TEXTURE7_ARB = 0x84C7,
    TEXTURE8_ARB = 0x84C8,
    TEXTURE9_ARB = 0x84C9,
    TEXTURE10_ARB = 0x84CA,
    TEXTURE11_ARB = 0x84CB,
    TEXTURE12_ARB = 0x84CC,
    TEXTURE13_ARB = 0x84CD,
    TEXTURE14_ARB = 0x84CE,
    TEXTURE15_ARB = 0x84CF,
    TEXTURE16_ARB = 0x84D0,
    TEXTURE17_ARB = 0x84D1,
    TEXTURE18_ARB = 0x84D2,
    TEXTURE19_ARB = 0x84D3,
    TEXTURE20_ARB = 0x84D4,
    TEXTURE21_ARB = 0x84D5,
    TEXTURE22_ARB = 0x84D6,
    TEXTURE23_ARB = 0x84D7,
    TEXTURE24_ARB = 0x84D8,
    TEXTURE25_ARB = 0x84D9,
    TEXTURE26_ARB = 0x84DA,
    TEXTURE27_ARB = 0x84DB,
    TEXTURE28_ARB = 0x84DC,
    TEXTURE29_ARB = 0x84DD,
    TEXTURE30_ARB = 0x84DE,
    TEXTURE31_ARB = 0x84DF,
    ACTIVE_TEXTURE_ARB = 0x84E0,
    CLIENT_ACTIVE_TEXTURE_ARB = 0x84E1,
    MAX_TEXTURE_UNITS_ARB = 0x84E2,
    EXT_ABGR = 1,
    ABGR_EXT = 0x8000,
    EXT_BLEND_COLOR = 1,
    CONSTANT_COLOR_EXT = 0x8001,
    ONE_MINUS_CONSTANT_COLOR_EXT = 0x8002,
    CONSTANT_ALPHA_EXT = 0x8003,
    ONE_MINUS_CONSTANT_ALPHA_EXT = 0x8004,
    BLEND_COLOR_EXT = 0x8005,
    EXT_POLYGON_OFFSET = 1,
    POLYGON_OFFSET_EXT = 0x8037,
    POLYGON_OFFSET_FACTOR_EXT = 0x8038,
    POLYGON_OFFSET_BIAS_EXT = 0x8039,
    EXT_TEXTURE3D = 1,
    PACK_SKIP_IMAGES_EXT = 0x806B,
    PACK_IMAGE_HEIGHT_EXT = 0x806C,
    UNPACK_SKIP_IMAGES_EXT = 0x806D,
    UNPACK_IMAGE_HEIGHT_EXT = 0x806E,
    TEXTURE_3D_EXT = 0x806F,
    PROXY_TEXTURE_3D_EXT = 0x8070,
    TEXTURE_DEPTH_EXT = 0x8071,
    TEXTURE_WRAP_R_EXT = 0x8072,
    MAX_3D_TEXTURE_SIZE_EXT = 0x8073,
    TEXTURE_3D_BINDING_EXT = 0x806A,
    EXT_TEXTURE_OBJECT = 1,
    TEXTURE_PRIORITY_EXT = 0x8066,
    TEXTURE_RESIDENT_EXT = 0x8067,
    TEXTURE_1D_BINDING_EXT = 0x8068,
    TEXTURE_2D_BINDING_EXT = 0x8069,
    EXT_RESCALE_NORMAL = 1,
    RESCALE_NORMAL_EXT = 0x803A,
    EXT_VERTEX_ARRAY = 1,
    VERTEX_ARRAY_EXT = 0x8074,
    NORMAL_ARRAY_EXT = 0x8075,
    COLOR_ARRAY_EXT = 0x8076,
    INDEX_ARRAY_EXT = 0x8077,
    TEXTURE_COORD_ARRAY_EXT = 0x8078,
    EDGE_FLAG_ARRAY_EXT = 0x8079,
    VERTEX_ARRAY_SIZE_EXT = 0x807A,
    VERTEX_ARRAY_TYPE_EXT = 0x807B,
    VERTEX_ARRAY_STRIDE_EXT = 0x807C,
    VERTEX_ARRAY_COUNT_EXT = 0x807D,
    NORMAL_ARRAY_TYPE_EXT = 0x807E,
    NORMAL_ARRAY_STRIDE_EXT = 0x807F,
    NORMAL_ARRAY_COUNT_EXT = 0x8080,
    COLOR_ARRAY_SIZE_EXT = 0x8081,
    COLOR_ARRAY_TYPE_EXT = 0x8082,
    COLOR_ARRAY_STRIDE_EXT = 0x8083,
    COLOR_ARRAY_COUNT_EXT = 0x8084,
    INDEX_ARRAY_TYPE_EXT = 0x8085,
    INDEX_ARRAY_STRIDE_EXT = 0x8086,
    INDEX_ARRAY_COUNT_EXT = 0x8087,
    TEXTURE_COORD_ARRAY_SIZE_EXT = 0x8088,
    TEXTURE_COORD_ARRAY_TYPE_EXT = 0x8089,
    TEXTURE_COORD_ARRAY_STRIDE_EXT = 0x808A,
    TEXTURE_COORD_ARRAY_COUNT_EXT = 0x808B,
    EDGE_FLAG_ARRAY_STRIDE_EXT = 0x808C,
    EDGE_FLAG_ARRAY_COUNT_EXT = 0x808D,
    VERTEX_ARRAY_POINTER_EXT = 0x808E,
    NORMAL_ARRAY_POINTER_EXT = 0x808F,
    COLOR_ARRAY_POINTER_EXT = 0x8090,
    INDEX_ARRAY_POINTER_EXT = 0x8091,
    TEXTURE_COORD_ARRAY_POINTER_EXT = 0x8092,
    EDGE_FLAG_ARRAY_POINTER_EXT = 0x8093,
    SGIS_TEXTURE_EDGE_CLAMP = 1,
    CLAMP_TO_EDGE_SGIS = 0x812F,
    EXT_BLEND_MINMAX = 1,
    FUNC_ADD_EXT = 0x8006,
    MIN_EXT = 0x8007,
    MAX_EXT = 0x8008,
    BLEND_EQUATION_EXT = 0x8009,
    EXT_BLEND_SUBTRACT = 1,
    FUNC_SUBTRACT_EXT = 0x800A,
    FUNC_REVERSE_SUBTRACT_EXT = 0x800B,
    EXT_BLEND_LOGIC_OP = 1,
    EXT_POINT_PARAMETERS = 1,
    POINT_SIZE_MIN_EXT = 0x8126,
    POINT_SIZE_MAX_EXT = 0x8127,
    POINT_FADE_THRESHOLD_SIZE_EXT = 0x8128,
    DISTANCE_ATTENUATION_EXT = 0x8129,
    EXT_PALETTED_TEXTURE = 1,
    TABLE_TOO_LARGE_EXT = 0x8031,
    COLOR_TABLE_FORMAT_EXT = 0x80D8,
    COLOR_TABLE_WIDTH_EXT = 0x80D9,
    COLOR_TABLE_RED_SIZE_EXT = 0x80DA,
    COLOR_TABLE_GREEN_SIZE_EXT = 0x80DB,
    COLOR_TABLE_BLUE_SIZE_EXT = 0x80DC,
    COLOR_TABLE_ALPHA_SIZE_EXT = 0x80DD,
    COLOR_TABLE_LUMINANCE_SIZE_EXT = 0x80DE,
    COLOR_TABLE_INTENSITY_SIZE_EXT = 0x80DF,
    TEXTURE_INDEX_SIZE_EXT = 0x80ED,
    COLOR_INDEX1_EXT = 0x80E2,
    COLOR_INDEX2_EXT = 0x80E3,
    COLOR_INDEX4_EXT = 0x80E4,
    COLOR_INDEX8_EXT = 0x80E5,
    COLOR_INDEX12_EXT = 0x80E6,
    COLOR_INDEX16_EXT = 0x80E7,
    EXT_CLIP_VOLUME_HINT = 1,
    CLIP_VOLUME_CLIPPING_HINT_EXT = 0x80F0,
    EXT_COMPILED_VERTEX_ARRAY = 1,
    ARRAY_ELEMENT_LOCK_FIRST_EXT = 0x81A8,
    ARRAY_ELEMENT_LOCK_COUNT_EXT = 0x81A9,
    HP_OCCLUSION_TEST = 1,
    OCCLUSION_TEST_HP = 0x8165,
    OCCLUSION_TEST_RESULT_HP = 0x8166,
    EXT_SHARED_TEXTURE_PALETTE = 1,
    SHARED_TEXTURE_PALETTE_EXT = 0x81FB,
    TEXTURE_RECTANGLE_NV = 0x84F5,
    TEXTURE_BINDING_RECTANGLE_NV = 0x84F6,
    PROXY_TEXTURE_RECTANGLE_NV = 0x84F7,
    MAX_RECTANGLE_TEXTURE_SIZE_NV = 0x84F8,
    EXT_STENCIL_WRAP = 1,
    INCR_WRAP_EXT = 0x8507,
    DECR_WRAP_EXT = 0x8508,
    NV_TEXGEN_REFLECTION = 1,
    NORMAL_MAP_NV = 0x8511,
    REFLECTION_MAP_NV = 0x8512,
    EXT_TEXTURE_ENV_ADD = 1,
    MESA_WINDOW_POS = 1,
    MESA_RESIZE_BUFFERS = 1,
    EXT_TEXTURE_ENV_DOT3 = 1,
    DOT3_RGB_EXT = 0x8740,
    DOT3_RGBA_EXT = 0x8741,
    COMBINE_EXT = 0x8570,
    COMBINE_RGB_EXT = 0x8571,
    COMBINE_ALPHA_EXT = 0x8572,
    SOURCE0_RGB_EXT = 0x8580,
    SOURCE1_RGB_EXT = 0x8581,
    SOURCE2_RGB_EXT = 0x8582,
    SOURCE0_ALPHA_EXT = 0x8588,
    SOURCE1_ALPHA_EXT = 0x8589,
    SOURCE2_ALPHA_EXT = 0x858A,
    OPERAND0_RGB_EXT = 0x8590,
    OPERAND1_RGB_EXT = 0x8591,
    OPERAND2_RGB_EXT = 0x8592,
    OPERAND0_ALPHA_EXT = 0x8598,
    OPERAND1_ALPHA_EXT = 0x8599,
    OPERAND2_ALPHA_EXT = 0x859A,
    RGB_SCALE_EXT = 0x8573,
    PIXEL_COUNTER_BITS_NV = 0x8864,
    CURRENT_OCCLUSION_QUERY_ID_NV = 0x8865,
    PIXEL_COUNT_NV = 0x8866,
    PIXEL_COUNT_AVAILABLE_NV = 0x8867
  };

protected:

  friend class Renderer;
  friend class System;

  static void* library;

  static void loadLibrary();

  static void initialize();
};

#endif // !OPENGL_H_

// vim: ts=2 sw=2 et
