/***************************************************************************
                           opengl.cc  -  OpenGL function declarations
                               -------------------
      copyright            : (C) 2003 by Morten Hustveit
      email                : morten@debian.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iterator>

#include <espace/cvar.h>
#include <espace/opengl.h>
#include <espace/output.h>
#include <espace/stringlist.h>
#include <espace/system.h>

#ifdef WIN32
namespace
{
  typedef void* (APIENTRY *wglGetProcAddress)(const char*);

  wglGetProcAddress getProcAddress;
}
#endif

GLConfig GL::config;
void* GL::library;

const char* GL::lastError()
{
  if(!getError)
    return "OpenGL not initialized";

  GLenum error = getError();

  if(NO_ERROR != getError())
    return "Multiple error conditions";

  switch(error)
  {
  case NO_ERROR: return "No error";
  case INVALID_ENUM: return "Invalid enum";
  case INVALID_VALUE: return "Invalid value";
  case INVALID_OPERATION: return "Invalid operation";
  case STACK_OVERFLOW: return "Stack overflow";
  case STACK_UNDERFLOW: return "Stack underflow";
  case OUT_OF_MEMORY: return "Out of memory";
  case TABLE_TOO_LARGE: return "Table too large";
  default: return "Unknown error";
  }
}

void GL::loadLibrary()
{
  if(library)
    return;

  CVar glDriver = CVar::acquire("r_gldriver",
#ifdef WIN32
          "opengl32",
#else
          "libGL.so.1",
#endif
          CVar::Archive);


  library = System::dlopen(glDriver.string);

  if(!library)
  {
    esError << "Failed to load OpenGL library \"" << glDriver.string << "\": "
            << System::dlerror() << std::endl;

    exit(EXIT_FAILURE);
  }

  esInfo << "Loaded OpenGL library \"" << glDriver.string << "\"." << std::endl;
}

void GL::initialize()
{
  loadLibrary();

#define PROC(x) \
  (x) System::dlsym(library, #x)

#ifdef WIN32
  getProcAddress = PROC(wglGetProcAddress);
#define PROC_EXT(x) \
  (x) getProcAddress(#x)
#else // !WIN32

#define PROC_EXT(x) \
  (x) System::dlsym(library, #x)
#endif

  clearIndex = PROC(glClearIndex);
  clearColor = PROC(glClearColor);
  clear = PROC(glClear);
  indexMask = PROC(glIndexMask);
  colorMask = PROC(glColorMask);
  alphaFunc = PROC(glAlphaFunc);
  blendFunc = PROC(glBlendFunc);
  logicOp = PROC(glLogicOp);
  cullFace = PROC(glCullFace);
  frontFace = PROC(glFrontFace);
  pointSize = PROC(glPointSize);
  lineWidth = PROC(glLineWidth);
  lineStipple = PROC(glLineStipple);
  polygonMode = PROC(glPolygonMode);
  polygonOffset = PROC(glPolygonOffset);
  polygonStipple = PROC(glPolygonStipple);
  getPolygonStipple = PROC(glGetPolygonStipple);
  edgeFlag = PROC(glEdgeFlag);
  edgeFlagv = PROC(glEdgeFlagv);
  scissor = PROC(glScissor);
  clipPlane = PROC(glClipPlane);
  getClipPlane = PROC(glGetClipPlane);
  drawBuffer = PROC(glDrawBuffer);
  readBuffer = PROC(glReadBuffer);
  enable = PROC(glEnable);
  disable = PROC(glDisable);
  isEnabled = PROC(glIsEnabled);
  enableClientState = PROC(glEnableClientState);
  disableClientState = PROC(glDisableClientState);
  getBooleanv = PROC(glGetBooleanv);
  getDoublev = PROC(glGetDoublev);
  getFloatv = PROC(glGetFloatv);
  getIntegerv = PROC(glGetIntegerv);
  pushAttrib = PROC(glPushAttrib);
  popAttrib = PROC(glPopAttrib);
  pushClientAttrib = PROC(glPushClientAttrib);
  popClientAttrib = PROC(glPopClientAttrib);
  renderMode = PROC(glRenderMode);
  getError = PROC(glGetError);
  getString = PROC(glGetString);
  finish = PROC(glFinish);
  flush = PROC(glFlush);
  hint = PROC(glHint);
  clearDepth = PROC(glClearDepth);
  depthFunc = PROC(glDepthFunc);
  depthMask = PROC(glDepthMask);
  depthRange = PROC(glDepthRange);
  clearAccum = PROC(glClearAccum);
  accum = PROC(glAccum);
  matrixMode = PROC(glMatrixMode);
  ortho = PROC(glOrtho);
  frustum = PROC(glFrustum);
  viewport = PROC(glViewport);
  pushMatrix = PROC(glPushMatrix);
  popMatrix = PROC(glPopMatrix);
  loadIdentity = PROC(glLoadIdentity);
  loadMatrixd = PROC(glLoadMatrixd);
  loadMatrixf = PROC(glLoadMatrixf);
  multMatrixd = PROC(glMultMatrixd);
  multMatrixf = PROC(glMultMatrixf);
  rotated = PROC(glRotated);
  rotatef = PROC(glRotatef);
  scaled = PROC(glScaled);
  scalef = PROC(glScalef);
  translated = PROC(glTranslated);
  translatef = PROC(glTranslatef);
  isList = PROC(glIsList);
  deleteLists = PROC(glDeleteLists);
  genLists = PROC(glGenLists);
  newList = PROC(glNewList);
  endList = PROC(glEndList);
  callList = PROC(glCallList);
  callLists = PROC(glCallLists);
  listBase = PROC(glListBase);
  begin = PROC(glBegin);
  end = PROC(glEnd);
  vertex2d = PROC(glVertex2d);
  vertex2f = PROC(glVertex2f);
  vertex2i = PROC(glVertex2i);
  vertex2s = PROC(glVertex2s);
  vertex3d = PROC(glVertex3d);
  vertex3f = PROC(glVertex3f);
  vertex3i = PROC(glVertex3i);
  vertex3s = PROC(glVertex3s);
  vertex4d = PROC(glVertex4d);
  vertex4f = PROC(glVertex4f);
  vertex4i = PROC(glVertex4i);
  vertex4s = PROC(glVertex4s);
  vertex2dv = PROC(glVertex2dv);
  vertex2fv = PROC(glVertex2fv);
  vertex2iv = PROC(glVertex2iv);
  vertex2sv = PROC(glVertex2sv);
  vertex3dv = PROC(glVertex3dv);
  vertex3fv = PROC(glVertex3fv);
  vertex3iv = PROC(glVertex3iv);
  vertex3sv = PROC(glVertex3sv);
  vertex4dv = PROC(glVertex4dv);
  vertex4fv = PROC(glVertex4fv);
  vertex4iv = PROC(glVertex4iv);
  vertex4sv = PROC(glVertex4sv);
  normal3b = PROC(glNormal3b);
  normal3d = PROC(glNormal3d);
  normal3f = PROC(glNormal3f);
  normal3i = PROC(glNormal3i);
  normal3s = PROC(glNormal3s);
  normal3bv = PROC(glNormal3bv);
  normal3dv = PROC(glNormal3dv);
  normal3fv = PROC(glNormal3fv);
  normal3iv = PROC(glNormal3iv);
  normal3sv = PROC(glNormal3sv);
  indexd = PROC(glIndexd);
  indexf = PROC(glIndexf);
  indexi = PROC(glIndexi);
  indexs = PROC(glIndexs);
  indexub = PROC(glIndexub);
  indexdv = PROC(glIndexdv);
  indexfv = PROC(glIndexfv);
  indexiv = PROC(glIndexiv);
  indexsv = PROC(glIndexsv);
  indexubv = PROC(glIndexubv);
  color3b = PROC(glColor3b);
  color3d = PROC(glColor3d);
  color3f = PROC(glColor3f);
  color3i = PROC(glColor3i);
  color3s = PROC(glColor3s);
  color3ub = PROC(glColor3ub);
  color3ui = PROC(glColor3ui);
  color3us = PROC(glColor3us);
  color4b = PROC(glColor4b);
  color4d = PROC(glColor4d);
  color4f = PROC(glColor4f);
  color4i = PROC(glColor4i);
  color4s = PROC(glColor4s);
  color4ub = PROC(glColor4ub);
  color4ui = PROC(glColor4ui);
  color4us = PROC(glColor4us);
  color3bv = PROC(glColor3bv);
  color3dv = PROC(glColor3dv);
  color3fv = PROC(glColor3fv);
  color3iv = PROC(glColor3iv);
  color3sv = PROC(glColor3sv);
  color3ubv = PROC(glColor3ubv);
  color3uiv = PROC(glColor3uiv);
  color3usv = PROC(glColor3usv);
  color4bv = PROC(glColor4bv);
  color4dv = PROC(glColor4dv);
  color4fv = PROC(glColor4fv);
  color4iv = PROC(glColor4iv);
  color4sv = PROC(glColor4sv);
  color4ubv = PROC(glColor4ubv);
  color4uiv = PROC(glColor4uiv);
  color4usv = PROC(glColor4usv);
  texCoord1d = PROC(glTexCoord1d);
  texCoord1f = PROC(glTexCoord1f);
  texCoord1i = PROC(glTexCoord1i);
  texCoord1s = PROC(glTexCoord1s);
  texCoord2d = PROC(glTexCoord2d);
  texCoord2f = PROC(glTexCoord2f);
  texCoord2i = PROC(glTexCoord2i);
  texCoord2s = PROC(glTexCoord2s);
  texCoord3d = PROC(glTexCoord3d);
  texCoord3f = PROC(glTexCoord3f);
  texCoord3i = PROC(glTexCoord3i);
  texCoord3s = PROC(glTexCoord3s);
  texCoord4d = PROC(glTexCoord4d);
  texCoord4f = PROC(glTexCoord4f);
  texCoord4i = PROC(glTexCoord4i);
  texCoord4s = PROC(glTexCoord4s);
  texCoord1dv = PROC(glTexCoord1dv);
  texCoord1fv = PROC(glTexCoord1fv);
  texCoord1iv = PROC(glTexCoord1iv);
  texCoord1sv = PROC(glTexCoord1sv);
  texCoord2dv = PROC(glTexCoord2dv);
  texCoord2fv = PROC(glTexCoord2fv);
  texCoord2iv = PROC(glTexCoord2iv);
  texCoord2sv = PROC(glTexCoord2sv);
  texCoord3dv = PROC(glTexCoord3dv);
  texCoord3fv = PROC(glTexCoord3fv);
  texCoord3iv = PROC(glTexCoord3iv);
  texCoord3sv = PROC(glTexCoord3sv);
  texCoord4dv = PROC(glTexCoord4dv);
  texCoord4fv = PROC(glTexCoord4fv);
  texCoord4iv = PROC(glTexCoord4iv);
  texCoord4sv = PROC(glTexCoord4sv);
  rasterPos2d = PROC(glRasterPos2d);
  rasterPos2f = PROC(glRasterPos2f);
  rasterPos2i = PROC(glRasterPos2i);
  rasterPos2s = PROC(glRasterPos2s);
  rasterPos3d = PROC(glRasterPos3d);
  rasterPos3f = PROC(glRasterPos3f);
  rasterPos3i = PROC(glRasterPos3i);
  rasterPos3s = PROC(glRasterPos3s);
  rasterPos4d = PROC(glRasterPos4d);
  rasterPos4f = PROC(glRasterPos4f);
  rasterPos4i = PROC(glRasterPos4i);
  rasterPos4s = PROC(glRasterPos4s);
  rasterPos2dv = PROC(glRasterPos2dv);
  rasterPos2fv = PROC(glRasterPos2fv);
  rasterPos2iv = PROC(glRasterPos2iv);
  rasterPos2sv = PROC(glRasterPos2sv);
  rasterPos3dv = PROC(glRasterPos3dv);
  rasterPos3fv = PROC(glRasterPos3fv);
  rasterPos3iv = PROC(glRasterPos3iv);
  rasterPos3sv = PROC(glRasterPos3sv);
  rasterPos4dv = PROC(glRasterPos4dv);
  rasterPos4fv = PROC(glRasterPos4fv);
  rasterPos4iv = PROC(glRasterPos4iv);
  rasterPos4sv = PROC(glRasterPos4sv);
  rectd = PROC(glRectd);
  rectf = PROC(glRectf);
  recti = PROC(glRecti);
  rects = PROC(glRects);
  rectdv = PROC(glRectdv);
  rectfv = PROC(glRectfv);
  rectiv = PROC(glRectiv);
  rectsv = PROC(glRectsv);
  vertexPointer = PROC(glVertexPointer);
  normalPointer = PROC(glNormalPointer);
  colorPointer = PROC(glColorPointer);
  indexPointer = PROC(glIndexPointer);
  texCoordPointer = PROC(glTexCoordPointer);
  edgeFlagPointer = PROC(glEdgeFlagPointer);
  getPointerv = PROC(glGetPointerv);
  arrayElement = PROC(glArrayElement);
  drawArrays = PROC(glDrawArrays);
  drawElements = PROC(glDrawElements);
  interleavedArrays = PROC(glInterleavedArrays);
  shadeModel = PROC(glShadeModel);
  lightf = PROC(glLightf);
  lighti = PROC(glLighti);
  lightfv = PROC(glLightfv);
  lightiv = PROC(glLightiv);
  getLightfv = PROC(glGetLightfv);
  getLightiv = PROC(glGetLightiv);
  lightModelf = PROC(glLightModelf);
  lightModeli = PROC(glLightModeli);
  lightModelfv = PROC(glLightModelfv);
  lightModeliv = PROC(glLightModeliv);
  materialf = PROC(glMaterialf);
  materiali = PROC(glMateriali);
  materialfv = PROC(glMaterialfv);
  materialiv = PROC(glMaterialiv);
  getMaterialfv = PROC(glGetMaterialfv);
  getMaterialiv = PROC(glGetMaterialiv);
  colorMaterial = PROC(glColorMaterial);
  pixelZoom = PROC(glPixelZoom);
  pixelStoref = PROC(glPixelStoref);
  pixelStorei = PROC(glPixelStorei);
  pixelTransferf = PROC(glPixelTransferf);
  pixelTransferi = PROC(glPixelTransferi);
  pixelMapfv = PROC(glPixelMapfv);
  pixelMapuiv = PROC(glPixelMapuiv);
  pixelMapusv = PROC(glPixelMapusv);
  getPixelMapfv = PROC(glGetPixelMapfv);
  getPixelMapuiv = PROC(glGetPixelMapuiv);
  getPixelMapusv = PROC(glGetPixelMapusv);
  bitmap = PROC(glBitmap);
  readPixels = PROC(glReadPixels);
  drawPixels = PROC(glDrawPixels);
  copyPixels = PROC(glCopyPixels);
  stencilFunc = PROC(glStencilFunc);
  stencilMask = PROC(glStencilMask);
  stencilOp = PROC(glStencilOp);
  clearStencil = PROC(glClearStencil);
  texGend = PROC(glTexGend);
  texGenf = PROC(glTexGenf);
  texGeni = PROC(glTexGeni);
  texGendv = PROC(glTexGendv);
  texGenfv = PROC(glTexGenfv);
  texGeniv = PROC(glTexGeniv);
  getTexGendv = PROC(glGetTexGendv);
  getTexGenfv = PROC(glGetTexGenfv);
  getTexGeniv = PROC(glGetTexGeniv);
  texEnvf = PROC(glTexEnvf);
  texEnvi = PROC(glTexEnvi);
  texEnvfv = PROC(glTexEnvfv);
  texEnviv = PROC(glTexEnviv);
  getTexEnvfv = PROC(glGetTexEnvfv);
  getTexEnviv = PROC(glGetTexEnviv);
  texParameterf = PROC(glTexParameterf);
  texParameteri = PROC(glTexParameteri);
  texParameterfv = PROC(glTexParameterfv);
  texParameteriv = PROC(glTexParameteriv);
  getTexParameterfv = PROC(glGetTexParameterfv);
  getTexParameteriv = PROC(glGetTexParameteriv);
  getTexLevelParameterfv = PROC(glGetTexLevelParameterfv);
  getTexLevelParameteriv = PROC(glGetTexLevelParameteriv);
  texImage1D = PROC(glTexImage1D);
  texImage2D = PROC(glTexImage2D);
  getTexImage = PROC(glGetTexImage);
  genTextures = PROC(glGenTextures);
  deleteTextures = PROC(glDeleteTextures);
  bindTexture = PROC(glBindTexture);
  prioritizeTextures = PROC(glPrioritizeTextures);
  areTexturesResident = PROC(glAreTexturesResident);
  isTexture = PROC(glIsTexture);
  texSubImage1D = PROC(glTexSubImage1D);
  texSubImage2D = PROC(glTexSubImage2D);
  copyTexImage1D = PROC(glCopyTexImage1D);
  copyTexImage2D = PROC(glCopyTexImage2D);
  copyTexSubImage1D = PROC(glCopyTexSubImage1D);
  copyTexSubImage2D = PROC(glCopyTexSubImage2D);
  map1d = PROC(glMap1d);
  map1f = PROC(glMap1f);
  map2d = PROC(glMap2d);
  map2f = PROC(glMap2f);
  getMapdv = PROC(glGetMapdv);
  getMapfv = PROC(glGetMapfv);
  getMapiv = PROC(glGetMapiv);
  evalCoord1d = PROC(glEvalCoord1d);
  evalCoord1f = PROC(glEvalCoord1f);
  evalCoord1dv = PROC(glEvalCoord1dv);
  evalCoord1fv = PROC(glEvalCoord1fv);
  evalCoord2d = PROC(glEvalCoord2d);
  evalCoord2f = PROC(glEvalCoord2f);
  evalCoord2dv = PROC(glEvalCoord2dv);
  evalCoord2fv = PROC(glEvalCoord2fv);
  mapGrid1d = PROC(glMapGrid1d);
  mapGrid1f = PROC(glMapGrid1f);
  mapGrid2d = PROC(glMapGrid2d);
  mapGrid2f = PROC(glMapGrid2f);
  evalPoint1 = PROC(glEvalPoint1);
  evalPoint2 = PROC(glEvalPoint2);
  evalMesh1 = PROC(glEvalMesh1);
  evalMesh2 = PROC(glEvalMesh2);
  fogf = PROC(glFogf);
  fogi = PROC(glFogi);
  fogfv = PROC(glFogfv);
  fogiv = PROC(glFogiv);
  feedbackBuffer = PROC(glFeedbackBuffer);
  passThrough = PROC(glPassThrough);
  selectBuffer = PROC(glSelectBuffer);
  initNames = PROC(glInitNames);
  loadName = PROC(glLoadName);
  pushName = PROC(glPushName);
  popName = PROC(glPopName);
  drawRangeElements = PROC(glDrawRangeElements);
  texImage3D = PROC(glTexImage3D);
  texSubImage3D = PROC(glTexSubImage3D);
  copyTexSubImage3D = PROC(glCopyTexSubImage3D);
  colorTable = PROC(glColorTable);
  colorSubTable = PROC(glColorSubTable);
  colorTableParameteriv = PROC(glColorTableParameteriv);
  colorTableParameterfv = PROC(glColorTableParameterfv);
  copyColorSubTable = PROC(glCopyColorSubTable);
  copyColorTable = PROC(glCopyColorTable);
  getColorTable = PROC(glGetColorTable);
  getColorTableParameterfv = PROC(glGetColorTableParameterfv);
  getColorTableParameteriv = PROC(glGetColorTableParameteriv);
  blendEquation = PROC(glBlendEquation);
  blendColor = PROC(glBlendColor);
  histogram = PROC(glHistogram);
  resetHistogram = PROC(glResetHistogram);
  getHistogram = PROC(glGetHistogram);
  getHistogramParameterfv = PROC(glGetHistogramParameterfv);
  getHistogramParameteriv = PROC(glGetHistogramParameteriv);
  minmax = PROC(glMinmax);
  resetMinmax = PROC(glResetMinmax);
  getMinmax = PROC(glGetMinmax);
  getMinmaxParameterfv = PROC(glGetMinmaxParameterfv);
  getMinmaxParameteriv = PROC(glGetMinmaxParameteriv);
  convolutionFilter1D = PROC(glConvolutionFilter1D);
  convolutionFilter2D = PROC(glConvolutionFilter2D);
  convolutionParameterf = PROC(glConvolutionParameterf);
  convolutionParameterfv = PROC(glConvolutionParameterfv);
  convolutionParameteri = PROC(glConvolutionParameteri);
  convolutionParameteriv = PROC(glConvolutionParameteriv);
  copyConvolutionFilter1D = PROC(glCopyConvolutionFilter1D);
  copyConvolutionFilter2D = PROC(glCopyConvolutionFilter2D);
  getConvolutionFilter = PROC(glGetConvolutionFilter);
  getConvolutionParameterfv = PROC(glGetConvolutionParameterfv);
  getConvolutionParameteriv = PROC(glGetConvolutionParameteriv);
  separableFilter2D = PROC(glSeparableFilter2D);
  getSeparableFilter = PROC(glGetSeparableFilter);
  activeTextureARB = PROC_EXT(glActiveTextureARB);
  clientActiveTextureARB = PROC_EXT(glClientActiveTextureARB);
  multiTexCoord1dARB = PROC_EXT(glMultiTexCoord1dARB);
  multiTexCoord1dvARB = PROC_EXT(glMultiTexCoord1dvARB);
  multiTexCoord1fARB = PROC_EXT(glMultiTexCoord1fARB);
  multiTexCoord1fvARB = PROC_EXT(glMultiTexCoord1fvARB);
  multiTexCoord1iARB = PROC_EXT(glMultiTexCoord1iARB);
  multiTexCoord1ivARB = PROC_EXT(glMultiTexCoord1ivARB);
  multiTexCoord1sARB = PROC_EXT(glMultiTexCoord1sARB);
  multiTexCoord1svARB = PROC_EXT(glMultiTexCoord1svARB);
  multiTexCoord2dARB = PROC_EXT(glMultiTexCoord2dARB);
  multiTexCoord2dvARB = PROC_EXT(glMultiTexCoord2dvARB);
  multiTexCoord2fARB = PROC_EXT(glMultiTexCoord2fARB);
  multiTexCoord2fvARB = PROC_EXT(glMultiTexCoord2fvARB);
  multiTexCoord2iARB = PROC_EXT(glMultiTexCoord2iARB);
  multiTexCoord2ivARB = PROC_EXT(glMultiTexCoord2ivARB);
  multiTexCoord2sARB = PROC_EXT(glMultiTexCoord2sARB);
  multiTexCoord2svARB = PROC_EXT(glMultiTexCoord2svARB);
  multiTexCoord3dARB = PROC_EXT(glMultiTexCoord3dARB);
  multiTexCoord3dvARB = PROC_EXT(glMultiTexCoord3dvARB);
  multiTexCoord3fARB = PROC_EXT(glMultiTexCoord3fARB);
  multiTexCoord3fvARB = PROC_EXT(glMultiTexCoord3fvARB);
  multiTexCoord3iARB = PROC_EXT(glMultiTexCoord3iARB);
  multiTexCoord3ivARB = PROC_EXT(glMultiTexCoord3ivARB);
  multiTexCoord3sARB = PROC_EXT(glMultiTexCoord3sARB);
  multiTexCoord3svARB = PROC_EXT(glMultiTexCoord3svARB);
  multiTexCoord4dARB = PROC_EXT(glMultiTexCoord4dARB);
  multiTexCoord4dvARB = PROC_EXT(glMultiTexCoord4dvARB);
  multiTexCoord4fARB = PROC_EXT(glMultiTexCoord4fARB);
  multiTexCoord4fvARB = PROC_EXT(glMultiTexCoord4fvARB);
  multiTexCoord4iARB = PROC_EXT(glMultiTexCoord4iARB);
  multiTexCoord4ivARB = PROC_EXT(glMultiTexCoord4ivARB);
  multiTexCoord4sARB = PROC_EXT(glMultiTexCoord4sARB);
  multiTexCoord4svARB = PROC_EXT(glMultiTexCoord4svARB);
  blendColorEXT = PROC_EXT(glBlendColorEXT);
  polygonOffsetEXT = PROC_EXT(glPolygonOffsetEXT);
  texImage3DEXT = PROC_EXT(glTexImage3DEXT);
  texSubImage3DEXT = PROC_EXT(glTexSubImage3DEXT);
  copyTexSubImage3DEXT = PROC_EXT(glCopyTexSubImage3DEXT);
  genTexturesEXT = PROC_EXT(glGenTexturesEXT);
  deleteTexturesEXT = PROC_EXT(glDeleteTexturesEXT);
  bindTextureEXT = PROC_EXT(glBindTextureEXT);
  prioritizeTexturesEXT = PROC_EXT(glPrioritizeTexturesEXT);
  areTexturesResidentEXT = PROC_EXT(glAreTexturesResidentEXT);
  isTextureEXT = PROC_EXT(glIsTextureEXT);
  vertexPointerEXT = PROC_EXT(glVertexPointerEXT);
  normalPointerEXT = PROC_EXT(glNormalPointerEXT);
  colorPointerEXT = PROC_EXT(glColorPointerEXT);
  indexPointerEXT = PROC_EXT(glIndexPointerEXT);
  texCoordPointerEXT = PROC_EXT(glTexCoordPointerEXT);
  edgeFlagPointerEXT = PROC_EXT(glEdgeFlagPointerEXT);
  getPointervEXT = PROC_EXT(glGetPointervEXT);
  arrayElementEXT = PROC_EXT(glArrayElementEXT);
  drawArraysEXT = PROC_EXT(glDrawArraysEXT);
  blendEquationEXT = PROC_EXT(glBlendEquationEXT);
  pointParameterfEXT = PROC_EXT(glPointParameterfEXT);
  pointParameterfvEXT = PROC_EXT(glPointParameterfvEXT);
  pointParameterfSGIS = PROC_EXT(glPointParameterfSGIS);
  pointParameterfvSGIS = PROC_EXT(glPointParameterfvSGIS);
  colorTableEXT = PROC_EXT(glColorTableEXT);
  colorSubTableEXT = PROC_EXT(glColorSubTableEXT);
  getColorTableEXT = PROC_EXT(glGetColorTableEXT);
  getColorTableParameterfvEXT = PROC_EXT(glGetColorTableParameterfvEXT);
  getColorTableParameterivEXT = PROC_EXT(glGetColorTableParameterivEXT);
  lockArraysEXT = PROC_EXT(glLockArraysEXT);
  unlockArraysEXT = PROC_EXT(glUnlockArraysEXT);
  windowPos2iMESA = PROC_EXT(glWindowPos2iMESA);
  windowPos2sMESA = PROC_EXT(glWindowPos2sMESA);
  windowPos2fMESA = PROC_EXT(glWindowPos2fMESA);
  windowPos2dMESA = PROC_EXT(glWindowPos2dMESA);
  windowPos2ivMESA = PROC_EXT(glWindowPos2ivMESA);
  windowPos2svMESA = PROC_EXT(glWindowPos2svMESA);
  windowPos2fvMESA = PROC_EXT(glWindowPos2fvMESA);
  windowPos2dvMESA = PROC_EXT(glWindowPos2dvMESA);
  windowPos3iMESA = PROC_EXT(glWindowPos3iMESA);
  windowPos3sMESA = PROC_EXT(glWindowPos3sMESA);
  windowPos3fMESA = PROC_EXT(glWindowPos3fMESA);
  windowPos3dMESA = PROC_EXT(glWindowPos3dMESA);
  windowPos3ivMESA = PROC_EXT(glWindowPos3ivMESA);
  windowPos3svMESA = PROC_EXT(glWindowPos3svMESA);
  windowPos3fvMESA = PROC_EXT(glWindowPos3fvMESA);
  windowPos3dvMESA = PROC_EXT(glWindowPos3dvMESA);
  windowPos4iMESA = PROC_EXT(glWindowPos4iMESA);
  windowPos4sMESA = PROC_EXT(glWindowPos4sMESA);
  windowPos4fMESA = PROC_EXT(glWindowPos4fMESA);
  windowPos4dMESA = PROC_EXT(glWindowPos4dMESA);
  windowPos4ivMESA = PROC_EXT(glWindowPos4ivMESA);
  windowPos4svMESA = PROC_EXT(glWindowPos4svMESA);
  windowPos4fvMESA = PROC_EXT(glWindowPos4fvMESA);
  windowPos4dvMESA = PROC_EXT(glWindowPos4dvMESA);
  resizeBuffersMESA = PROC_EXT(glResizeBuffersMESA);
  genOcclusionQueriesNV = PROC_EXT(glGenOcclusionQueriesNV);
  deleteOcclusionQueriesNV = PROC_EXT(glDeleteOcclusionQueriesNV);
  beginOcclusionQueryNV = PROC_EXT(glBeginOcclusionQueryNV);
  endOcclusionQueryNV = PROC_EXT(glEndOcclusionQueryNV);
  getOcclusionQueryivNV = PROC_EXT(glGetOcclusionQueryivNV);
  getOcclusionQueryuivNV = PROC_EXT(glGetOcclusionQueryuivNV);

  if(!getString)
  {
    esError << "glGetString symbol missing." << std::endl;

    System::exit();
  }

  const char* renderer;
  const char* vendor;
  const char* version;
  const char* extensionString;

  if(!(renderer = reinterpret_cast<const char*>(getString(GL::RENDERER))))
  {
    esError << "glGetString(GL_RENDERER) returned 0: "
            << lastError() << std::endl;

    System::exit();
  }

  if(!(vendor = reinterpret_cast<const char*>(getString(GL::VENDOR))))
  {
    esError << "glGetString(GL_VENDOR) returned 0: "
            << lastError() << std::endl;

    System::exit();
  }

  if(!(version = reinterpret_cast<const char*>(getString(GL::VERSION))))
  {
    esError << "glGetString(GL_VERSION) returned 0: "
            << lastError() << std::endl;

    System::exit();
  }

  if(!(extensionString = reinterpret_cast<const char*>(getString(GL::EXTENSIONS))))
  {
    esError << "glGetString(GL_EXTENSIONS) returned 0: "
            << lastError() << std::endl;

    System::exit();
  }

  strncpy(config.renderer, renderer, sizeof(config.renderer));
  config.renderer[sizeof(config.renderer) - 1] = 0;

  strncpy(config.vendor, vendor, sizeof(config.vendor));
  config.vendor[sizeof(config.vendor) - 1] = 0;

  strncpy(config.version, version, sizeof(config.version));
  config.version[sizeof(config.version) - 1] = 0;

  strncpy(config.extensionString, extensionString, sizeof(config.extensionString));
  config.extensionString[sizeof(config.extensionString) - 1] = 0;

  StringList extensions = StringList::split(" ", config.extensionString);

  // Copy list of extensions into GL::config.extensions
  std::copy(extensions.begin(), extensions.end(),
            std::insert_iterator<std::set<String> >(config.extensions,
                                                    config.extensions.begin()));

  esInfo << "Renderer: \"" << config.renderer << "\"." << std::endl;

  getIntegerv(GL::MAX_TEXTURE_SIZE, reinterpret_cast<GLint*>(&config.maxTextureSize));

  esInfo << "Max texture size: " << config.maxTextureSize
         << "." << std::endl;

  if(config.extensions.count("GL_ARB_multitexture"))
  {
    getIntegerv(GL::MAX_TEXTURE_UNITS_ARB,
                reinterpret_cast<GLint*>(&config.maxActiveTextures));
  }
  else
  {
    config.maxActiveTextures = 1;
  }

  esInfo << "Max active textures: " << config.maxActiveTextures
         << "." << std::endl;

  {
    GLint tmp;

    getIntegerv(GL::RED_BITS, &tmp); config.colorBits = tmp;
    getIntegerv(GL::GREEN_BITS, &tmp); config.colorBits += tmp;
    getIntegerv(GL::BLUE_BITS, &tmp); config.colorBits += tmp;
  }

  esInfo << config.colorBits << " color bits per pixel." << std::endl;

  getIntegerv(GL::DEPTH_BITS, reinterpret_cast<GLint*>(&config.depthBits));

  esInfo << config.depthBits << " depth bits per pixel." << std::endl;

  getIntegerv(GL::STENCIL_BITS, reinterpret_cast<GLint*>(&config.stencilBits));

  esInfo << config.stencilBits << " stencil bits per pixel." << std::endl;

  config.driverType = GLConfig::GLDRV_ICD;
  config.hardwareType = GLConfig::GLHW_GENERIC;

  config.aspect = static_cast<float>(config.width / config.height);

  // XXX: Fill in all of the following
  config.gammaSupport = 0;
  config.textureCompression = GLConfig::TC_NONE;
  config.textureEnvAddSupport = 1;
  config.anisotropicSupport = 1;
  config.maxAnisotropy = 2;

  config.NVFogAvailable = 0;
  config.NVFogMode = 0;

  config.ATIMaxTruformTess = 0;
  config.ATINormalMode = 0;
  config.ATIPointMode = 0;
  config.refreshRate = 60;
  config.fullscreen = 1;

  {
    GLboolean tmp;

    getBooleanv(GL::STEREO, &tmp);

    config.stereo = tmp;
  }

  config.smp = 0; // XXX

  config.textureFilterAnisotropicSupport
    = config.extensions.count("GL_EXT_texture_filter_anisotropic");
}

GL::glClearIndex                  GL::clearIndex;
GL::glClearColor                  GL::clearColor;
GL::glClear                       GL::clear;
GL::glIndexMask                   GL::indexMask;
GL::glColorMask                   GL::colorMask;
GL::glAlphaFunc                   GL::alphaFunc;
GL::glBlendFunc                   GL::blendFunc;
GL::glLogicOp                     GL::logicOp;
GL::glCullFace                    GL::cullFace;
GL::glFrontFace                   GL::frontFace;
GL::glPointSize                   GL::pointSize;
GL::glLineWidth                   GL::lineWidth;
GL::glLineStipple                 GL::lineStipple;
GL::glPolygonMode                 GL::polygonMode;
GL::glPolygonOffset               GL::polygonOffset;
GL::glPolygonStipple              GL::polygonStipple;
GL::glGetPolygonStipple           GL::getPolygonStipple;
GL::glEdgeFlag                    GL::edgeFlag;
GL::glEdgeFlagv                   GL::edgeFlagv;
GL::glScissor                     GL::scissor;
GL::glClipPlane                   GL::clipPlane;
GL::glGetClipPlane                GL::getClipPlane;
GL::glDrawBuffer                  GL::drawBuffer;
GL::glReadBuffer                  GL::readBuffer;
GL::glEnable                      GL::enable;
GL::glDisable                     GL::disable;
GL::glIsEnabled                   GL::isEnabled;
GL::glEnableClientState           GL::enableClientState;
GL::glDisableClientState          GL::disableClientState;
GL::glGetBooleanv                 GL::getBooleanv;
GL::glGetDoublev                  GL::getDoublev;
GL::glGetFloatv                   GL::getFloatv;
GL::glGetIntegerv                 GL::getIntegerv;
GL::glPushAttrib                  GL::pushAttrib;
GL::glPopAttrib                   GL::popAttrib;
GL::glPushClientAttrib            GL::pushClientAttrib;
GL::glPopClientAttrib             GL::popClientAttrib;
GL::glRenderMode                  GL::renderMode;
GL::glGetError                    GL::getError;
GL::glGetString                   GL::getString;
GL::glFinish                      GL::finish;
GL::glFlush                       GL::flush;
GL::glHint                        GL::hint;
GL::glClearDepth                  GL::clearDepth;
GL::glDepthFunc                   GL::depthFunc;
GL::glDepthMask                   GL::depthMask;
GL::glDepthRange                  GL::depthRange;
GL::glClearAccum                  GL::clearAccum;
GL::glAccum                       GL::accum;
GL::glMatrixMode                  GL::matrixMode;
GL::glOrtho                       GL::ortho;
GL::glFrustum                     GL::frustum;
GL::glViewport                    GL::viewport;
GL::glPushMatrix                  GL::pushMatrix;
GL::glPopMatrix                   GL::popMatrix;
GL::glLoadIdentity                GL::loadIdentity;
GL::glLoadMatrixd                 GL::loadMatrixd;
GL::glLoadMatrixf                 GL::loadMatrixf;
GL::glMultMatrixd                 GL::multMatrixd;
GL::glMultMatrixf                 GL::multMatrixf;
GL::glRotated                     GL::rotated;
GL::glRotatef                     GL::rotatef;
GL::glScaled                      GL::scaled;
GL::glScalef                      GL::scalef;
GL::glTranslated                  GL::translated;
GL::glTranslatef                  GL::translatef;
GL::glIsList                      GL::isList;
GL::glDeleteLists                 GL::deleteLists;
GL::glGenLists                    GL::genLists;
GL::glNewList                     GL::newList;
GL::glEndList                     GL::endList;
GL::glCallList                    GL::callList;
GL::glCallLists                   GL::callLists;
GL::glListBase                    GL::listBase;
GL::glBegin                       GL::begin;
GL::glEnd                         GL::end;
GL::glVertex2d                    GL::vertex2d;
GL::glVertex2f                    GL::vertex2f;
GL::glVertex2i                    GL::vertex2i;
GL::glVertex2s                    GL::vertex2s;
GL::glVertex3d                    GL::vertex3d;
GL::glVertex3f                    GL::vertex3f;
GL::glVertex3i                    GL::vertex3i;
GL::glVertex3s                    GL::vertex3s;
GL::glVertex4d                    GL::vertex4d;
GL::glVertex4f                    GL::vertex4f;
GL::glVertex4i                    GL::vertex4i;
GL::glVertex4s                    GL::vertex4s;
GL::glVertex2dv                   GL::vertex2dv;
GL::glVertex2fv                   GL::vertex2fv;
GL::glVertex2iv                   GL::vertex2iv;
GL::glVertex2sv                   GL::vertex2sv;
GL::glVertex3dv                   GL::vertex3dv;
GL::glVertex3fv                   GL::vertex3fv;
GL::glVertex3iv                   GL::vertex3iv;
GL::glVertex3sv                   GL::vertex3sv;
GL::glVertex4dv                   GL::vertex4dv;
GL::glVertex4fv                   GL::vertex4fv;
GL::glVertex4iv                   GL::vertex4iv;
GL::glVertex4sv                   GL::vertex4sv;
GL::glNormal3b                    GL::normal3b;
GL::glNormal3d                    GL::normal3d;
GL::glNormal3f                    GL::normal3f;
GL::glNormal3i                    GL::normal3i;
GL::glNormal3s                    GL::normal3s;
GL::glNormal3bv                   GL::normal3bv;
GL::glNormal3dv                   GL::normal3dv;
GL::glNormal3fv                   GL::normal3fv;
GL::glNormal3iv                   GL::normal3iv;
GL::glNormal3sv                   GL::normal3sv;
GL::glIndexd                      GL::indexd;
GL::glIndexf                      GL::indexf;
GL::glIndexi                      GL::indexi;
GL::glIndexs                      GL::indexs;
GL::glIndexub                     GL::indexub;
GL::glIndexdv                     GL::indexdv;
GL::glIndexfv                     GL::indexfv;
GL::glIndexiv                     GL::indexiv;
GL::glIndexsv                     GL::indexsv;
GL::glIndexubv                    GL::indexubv;
GL::glColor3b                     GL::color3b;
GL::glColor3d                     GL::color3d;
GL::glColor3f                     GL::color3f;
GL::glColor3i                     GL::color3i;
GL::glColor3s                     GL::color3s;
GL::glColor3ub                    GL::color3ub;
GL::glColor3ui                    GL::color3ui;
GL::glColor3us                    GL::color3us;
GL::glColor4b                     GL::color4b;
GL::glColor4d                     GL::color4d;
GL::glColor4f                     GL::color4f;
GL::glColor4i                     GL::color4i;
GL::glColor4s                     GL::color4s;
GL::glColor4ub                    GL::color4ub;
GL::glColor4ui                    GL::color4ui;
GL::glColor4us                    GL::color4us;
GL::glColor3bv                    GL::color3bv;
GL::glColor3dv                    GL::color3dv;
GL::glColor3fv                    GL::color3fv;
GL::glColor3iv                    GL::color3iv;
GL::glColor3sv                    GL::color3sv;
GL::glColor3ubv                   GL::color3ubv;
GL::glColor3uiv                   GL::color3uiv;
GL::glColor3usv                   GL::color3usv;
GL::glColor4bv                    GL::color4bv;
GL::glColor4dv                    GL::color4dv;
GL::glColor4fv                    GL::color4fv;
GL::glColor4iv                    GL::color4iv;
GL::glColor4sv                    GL::color4sv;
GL::glColor4ubv                   GL::color4ubv;
GL::glColor4uiv                   GL::color4uiv;
GL::glColor4usv                   GL::color4usv;
GL::glTexCoord1d                  GL::texCoord1d;
GL::glTexCoord1f                  GL::texCoord1f;
GL::glTexCoord1i                  GL::texCoord1i;
GL::glTexCoord1s                  GL::texCoord1s;
GL::glTexCoord2d                  GL::texCoord2d;
GL::glTexCoord2f                  GL::texCoord2f;
GL::glTexCoord2i                  GL::texCoord2i;
GL::glTexCoord2s                  GL::texCoord2s;
GL::glTexCoord3d                  GL::texCoord3d;
GL::glTexCoord3f                  GL::texCoord3f;
GL::glTexCoord3i                  GL::texCoord3i;
GL::glTexCoord3s                  GL::texCoord3s;
GL::glTexCoord4d                  GL::texCoord4d;
GL::glTexCoord4f                  GL::texCoord4f;
GL::glTexCoord4i                  GL::texCoord4i;
GL::glTexCoord4s                  GL::texCoord4s;
GL::glTexCoord1dv                 GL::texCoord1dv;
GL::glTexCoord1fv                 GL::texCoord1fv;
GL::glTexCoord1iv                 GL::texCoord1iv;
GL::glTexCoord1sv                 GL::texCoord1sv;
GL::glTexCoord2dv                 GL::texCoord2dv;
GL::glTexCoord2fv                 GL::texCoord2fv;
GL::glTexCoord2iv                 GL::texCoord2iv;
GL::glTexCoord2sv                 GL::texCoord2sv;
GL::glTexCoord3dv                 GL::texCoord3dv;
GL::glTexCoord3fv                 GL::texCoord3fv;
GL::glTexCoord3iv                 GL::texCoord3iv;
GL::glTexCoord3sv                 GL::texCoord3sv;
GL::glTexCoord4dv                 GL::texCoord4dv;
GL::glTexCoord4fv                 GL::texCoord4fv;
GL::glTexCoord4iv                 GL::texCoord4iv;
GL::glTexCoord4sv                 GL::texCoord4sv;
GL::glRasterPos2d                 GL::rasterPos2d;
GL::glRasterPos2f                 GL::rasterPos2f;
GL::glRasterPos2i                 GL::rasterPos2i;
GL::glRasterPos2s                 GL::rasterPos2s;
GL::glRasterPos3d                 GL::rasterPos3d;
GL::glRasterPos3f                 GL::rasterPos3f;
GL::glRasterPos3i                 GL::rasterPos3i;
GL::glRasterPos3s                 GL::rasterPos3s;
GL::glRasterPos4d                 GL::rasterPos4d;
GL::glRasterPos4f                 GL::rasterPos4f;
GL::glRasterPos4i                 GL::rasterPos4i;
GL::glRasterPos4s                 GL::rasterPos4s;
GL::glRasterPos2dv                GL::rasterPos2dv;
GL::glRasterPos2fv                GL::rasterPos2fv;
GL::glRasterPos2iv                GL::rasterPos2iv;
GL::glRasterPos2sv                GL::rasterPos2sv;
GL::glRasterPos3dv                GL::rasterPos3dv;
GL::glRasterPos3fv                GL::rasterPos3fv;
GL::glRasterPos3iv                GL::rasterPos3iv;
GL::glRasterPos3sv                GL::rasterPos3sv;
GL::glRasterPos4dv                GL::rasterPos4dv;
GL::glRasterPos4fv                GL::rasterPos4fv;
GL::glRasterPos4iv                GL::rasterPos4iv;
GL::glRasterPos4sv                GL::rasterPos4sv;
GL::glRectd                       GL::rectd;
GL::glRectf                       GL::rectf;
GL::glRecti                       GL::recti;
GL::glRects                       GL::rects;
GL::glRectdv                      GL::rectdv;
GL::glRectfv                      GL::rectfv;
GL::glRectiv                      GL::rectiv;
GL::glRectsv                      GL::rectsv;
GL::glVertexPointer               GL::vertexPointer;
GL::glNormalPointer               GL::normalPointer;
GL::glColorPointer                GL::colorPointer;
GL::glIndexPointer                GL::indexPointer;
GL::glTexCoordPointer             GL::texCoordPointer;
GL::glEdgeFlagPointer             GL::edgeFlagPointer;
GL::glGetPointerv                 GL::getPointerv;
GL::glArrayElement                GL::arrayElement;
GL::glDrawArrays                  GL::drawArrays;
GL::glDrawElements                GL::drawElements;
GL::glInterleavedArrays           GL::interleavedArrays;
GL::glShadeModel                  GL::shadeModel;
GL::glLightf                      GL::lightf;
GL::glLighti                      GL::lighti;
GL::glLightfv                     GL::lightfv;
GL::glLightiv                     GL::lightiv;
GL::glGetLightfv                  GL::getLightfv;
GL::glGetLightiv                  GL::getLightiv;
GL::glLightModelf                 GL::lightModelf;
GL::glLightModeli                 GL::lightModeli;
GL::glLightModelfv                GL::lightModelfv;
GL::glLightModeliv                GL::lightModeliv;
GL::glMaterialf                   GL::materialf;
GL::glMateriali                   GL::materiali;
GL::glMaterialfv                  GL::materialfv;
GL::glMaterialiv                  GL::materialiv;
GL::glGetMaterialfv               GL::getMaterialfv;
GL::glGetMaterialiv               GL::getMaterialiv;
GL::glColorMaterial               GL::colorMaterial;
GL::glPixelZoom                   GL::pixelZoom;
GL::glPixelStoref                 GL::pixelStoref;
GL::glPixelStorei                 GL::pixelStorei;
GL::glPixelTransferf              GL::pixelTransferf;
GL::glPixelTransferi              GL::pixelTransferi;
GL::glPixelMapfv                  GL::pixelMapfv;
GL::glPixelMapuiv                 GL::pixelMapuiv;
GL::glPixelMapusv                 GL::pixelMapusv;
GL::glGetPixelMapfv               GL::getPixelMapfv;
GL::glGetPixelMapuiv              GL::getPixelMapuiv;
GL::glGetPixelMapusv              GL::getPixelMapusv;
GL::glBitmap                      GL::bitmap;
GL::glReadPixels                  GL::readPixels;
GL::glDrawPixels                  GL::drawPixels;
GL::glCopyPixels                  GL::copyPixels;
GL::glStencilFunc                 GL::stencilFunc;
GL::glStencilMask                 GL::stencilMask;
GL::glStencilOp                   GL::stencilOp;
GL::glClearStencil                GL::clearStencil;
GL::glTexGend                     GL::texGend;
GL::glTexGenf                     GL::texGenf;
GL::glTexGeni                     GL::texGeni;
GL::glTexGendv                    GL::texGendv;
GL::glTexGenfv                    GL::texGenfv;
GL::glTexGeniv                    GL::texGeniv;
GL::glGetTexGendv                 GL::getTexGendv;
GL::glGetTexGenfv                 GL::getTexGenfv;
GL::glGetTexGeniv                 GL::getTexGeniv;
GL::glTexEnvf                     GL::texEnvf;
GL::glTexEnvi                     GL::texEnvi;
GL::glTexEnvfv                    GL::texEnvfv;
GL::glTexEnviv                    GL::texEnviv;
GL::glGetTexEnvfv                 GL::getTexEnvfv;
GL::glGetTexEnviv                 GL::getTexEnviv;
GL::glTexParameterf               GL::texParameterf;
GL::glTexParameteri               GL::texParameteri;
GL::glTexParameterfv              GL::texParameterfv;
GL::glTexParameteriv              GL::texParameteriv;
GL::glGetTexParameterfv           GL::getTexParameterfv;
GL::glGetTexParameteriv           GL::getTexParameteriv;
GL::glGetTexLevelParameterfv      GL::getTexLevelParameterfv;
GL::glGetTexLevelParameteriv      GL::getTexLevelParameteriv;
GL::glTexImage1D                  GL::texImage1D;
GL::glTexImage2D                  GL::texImage2D;
GL::glGetTexImage                 GL::getTexImage;
GL::glGenTextures                 GL::genTextures;
GL::glDeleteTextures              GL::deleteTextures;
GL::glBindTexture                 GL::bindTexture;
GL::glPrioritizeTextures          GL::prioritizeTextures;
GL::glAreTexturesResident         GL::areTexturesResident;
GL::glIsTexture                   GL::isTexture;
GL::glTexSubImage1D               GL::texSubImage1D;
GL::glTexSubImage2D               GL::texSubImage2D;
GL::glCopyTexImage1D              GL::copyTexImage1D;
GL::glCopyTexImage2D              GL::copyTexImage2D;
GL::glCopyTexSubImage1D           GL::copyTexSubImage1D;
GL::glCopyTexSubImage2D           GL::copyTexSubImage2D;
GL::glMap1d                       GL::map1d;
GL::glMap1f                       GL::map1f;
GL::glMap2d                       GL::map2d;
GL::glMap2f                       GL::map2f;
GL::glGetMapdv                    GL::getMapdv;
GL::glGetMapfv                    GL::getMapfv;
GL::glGetMapiv                    GL::getMapiv;
GL::glEvalCoord1d                 GL::evalCoord1d;
GL::glEvalCoord1f                 GL::evalCoord1f;
GL::glEvalCoord1dv                GL::evalCoord1dv;
GL::glEvalCoord1fv                GL::evalCoord1fv;
GL::glEvalCoord2d                 GL::evalCoord2d;
GL::glEvalCoord2f                 GL::evalCoord2f;
GL::glEvalCoord2dv                GL::evalCoord2dv;
GL::glEvalCoord2fv                GL::evalCoord2fv;
GL::glMapGrid1d                   GL::mapGrid1d;
GL::glMapGrid1f                   GL::mapGrid1f;
GL::glMapGrid2d                   GL::mapGrid2d;
GL::glMapGrid2f                   GL::mapGrid2f;
GL::glEvalPoint1                  GL::evalPoint1;
GL::glEvalPoint2                  GL::evalPoint2;
GL::glEvalMesh1                   GL::evalMesh1;
GL::glEvalMesh2                   GL::evalMesh2;
GL::glFogf                        GL::fogf;
GL::glFogi                        GL::fogi;
GL::glFogfv                       GL::fogfv;
GL::glFogiv                       GL::fogiv;
GL::glFeedbackBuffer              GL::feedbackBuffer;
GL::glPassThrough                 GL::passThrough;
GL::glSelectBuffer                GL::selectBuffer;
GL::glInitNames                   GL::initNames;
GL::glLoadName                    GL::loadName;
GL::glPushName                    GL::pushName;
GL::glPopName                     GL::popName;
GL::glDrawRangeElements           GL::drawRangeElements;
GL::glTexImage3D                  GL::texImage3D;
GL::glTexSubImage3D               GL::texSubImage3D;
GL::glCopyTexSubImage3D           GL::copyTexSubImage3D;
GL::glColorTable                  GL::colorTable;
GL::glColorSubTable               GL::colorSubTable;
GL::glColorTableParameteriv       GL::colorTableParameteriv;
GL::glColorTableParameterfv       GL::colorTableParameterfv;
GL::glCopyColorSubTable           GL::copyColorSubTable;
GL::glCopyColorTable              GL::copyColorTable;
GL::glGetColorTable               GL::getColorTable;
GL::glGetColorTableParameterfv    GL::getColorTableParameterfv;
GL::glGetColorTableParameteriv    GL::getColorTableParameteriv;
GL::glBlendEquation               GL::blendEquation;
GL::glBlendColor                  GL::blendColor;
GL::glHistogram                   GL::histogram;
GL::glResetHistogram              GL::resetHistogram;
GL::glGetHistogram                GL::getHistogram;
GL::glGetHistogramParameterfv     GL::getHistogramParameterfv;
GL::glGetHistogramParameteriv     GL::getHistogramParameteriv;
GL::glMinmax                      GL::minmax;
GL::glResetMinmax                 GL::resetMinmax;
GL::glGetMinmax                   GL::getMinmax;
GL::glGetMinmaxParameterfv        GL::getMinmaxParameterfv;
GL::glGetMinmaxParameteriv        GL::getMinmaxParameteriv;
GL::glConvolutionFilter1D         GL::convolutionFilter1D;
GL::glConvolutionFilter2D         GL::convolutionFilter2D;
GL::glConvolutionParameterf       GL::convolutionParameterf;
GL::glConvolutionParameterfv      GL::convolutionParameterfv;
GL::glConvolutionParameteri       GL::convolutionParameteri;
GL::glConvolutionParameteriv      GL::convolutionParameteriv;
GL::glCopyConvolutionFilter1D     GL::copyConvolutionFilter1D;
GL::glCopyConvolutionFilter2D     GL::copyConvolutionFilter2D;
GL::glGetConvolutionFilter        GL::getConvolutionFilter;
GL::glGetConvolutionParameterfv   GL::getConvolutionParameterfv;
GL::glGetConvolutionParameteriv   GL::getConvolutionParameteriv;
GL::glSeparableFilter2D           GL::separableFilter2D;
GL::glGetSeparableFilter          GL::getSeparableFilter;
GL::glActiveTextureARB            GL::activeTextureARB;
GL::glClientActiveTextureARB      GL::clientActiveTextureARB;
GL::glMultiTexCoord1dARB          GL::multiTexCoord1dARB;
GL::glMultiTexCoord1dvARB         GL::multiTexCoord1dvARB;
GL::glMultiTexCoord1fARB          GL::multiTexCoord1fARB;
GL::glMultiTexCoord1fvARB         GL::multiTexCoord1fvARB;
GL::glMultiTexCoord1iARB          GL::multiTexCoord1iARB;
GL::glMultiTexCoord1ivARB         GL::multiTexCoord1ivARB;
GL::glMultiTexCoord1sARB          GL::multiTexCoord1sARB;
GL::glMultiTexCoord1svARB         GL::multiTexCoord1svARB;
GL::glMultiTexCoord2dARB          GL::multiTexCoord2dARB;
GL::glMultiTexCoord2dvARB         GL::multiTexCoord2dvARB;
GL::glMultiTexCoord2fARB          GL::multiTexCoord2fARB;
GL::glMultiTexCoord2fvARB         GL::multiTexCoord2fvARB;
GL::glMultiTexCoord2iARB          GL::multiTexCoord2iARB;
GL::glMultiTexCoord2ivARB         GL::multiTexCoord2ivARB;
GL::glMultiTexCoord2sARB          GL::multiTexCoord2sARB;
GL::glMultiTexCoord2svARB         GL::multiTexCoord2svARB;
GL::glMultiTexCoord3dARB          GL::multiTexCoord3dARB;
GL::glMultiTexCoord3dvARB         GL::multiTexCoord3dvARB;
GL::glMultiTexCoord3fARB          GL::multiTexCoord3fARB;
GL::glMultiTexCoord3fvARB         GL::multiTexCoord3fvARB;
GL::glMultiTexCoord3iARB          GL::multiTexCoord3iARB;
GL::glMultiTexCoord3ivARB         GL::multiTexCoord3ivARB;
GL::glMultiTexCoord3sARB          GL::multiTexCoord3sARB;
GL::glMultiTexCoord3svARB         GL::multiTexCoord3svARB;
GL::glMultiTexCoord4dARB          GL::multiTexCoord4dARB;
GL::glMultiTexCoord4dvARB         GL::multiTexCoord4dvARB;
GL::glMultiTexCoord4fARB          GL::multiTexCoord4fARB;
GL::glMultiTexCoord4fvARB         GL::multiTexCoord4fvARB;
GL::glMultiTexCoord4iARB          GL::multiTexCoord4iARB;
GL::glMultiTexCoord4ivARB         GL::multiTexCoord4ivARB;
GL::glMultiTexCoord4sARB          GL::multiTexCoord4sARB;
GL::glMultiTexCoord4svARB         GL::multiTexCoord4svARB;
GL::glBlendColorEXT               GL::blendColorEXT;
GL::glPolygonOffsetEXT            GL::polygonOffsetEXT;
GL::glTexImage3DEXT               GL::texImage3DEXT;
GL::glTexSubImage3DEXT            GL::texSubImage3DEXT;
GL::glCopyTexSubImage3DEXT        GL::copyTexSubImage3DEXT;
GL::glGenTexturesEXT              GL::genTexturesEXT;
GL::glDeleteTexturesEXT           GL::deleteTexturesEXT;
GL::glBindTextureEXT              GL::bindTextureEXT;
GL::glPrioritizeTexturesEXT       GL::prioritizeTexturesEXT;
GL::glAreTexturesResidentEXT      GL::areTexturesResidentEXT;
GL::glIsTextureEXT                GL::isTextureEXT;
GL::glVertexPointerEXT            GL::vertexPointerEXT;
GL::glNormalPointerEXT            GL::normalPointerEXT;
GL::glColorPointerEXT             GL::colorPointerEXT;
GL::glIndexPointerEXT             GL::indexPointerEXT;
GL::glTexCoordPointerEXT          GL::texCoordPointerEXT;
GL::glEdgeFlagPointerEXT          GL::edgeFlagPointerEXT;
GL::glGetPointervEXT              GL::getPointervEXT;
GL::glArrayElementEXT             GL::arrayElementEXT;
GL::glDrawArraysEXT               GL::drawArraysEXT;
GL::glBlendEquationEXT            GL::blendEquationEXT;
GL::glPointParameterfEXT          GL::pointParameterfEXT;
GL::glPointParameterfvEXT         GL::pointParameterfvEXT;
GL::glPointParameterfSGIS         GL::pointParameterfSGIS;
GL::glPointParameterfvSGIS        GL::pointParameterfvSGIS;
GL::glColorTableEXT               GL::colorTableEXT;
GL::glColorSubTableEXT            GL::colorSubTableEXT;
GL::glGetColorTableEXT            GL::getColorTableEXT;
GL::glGetColorTableParameterfvEXT GL::getColorTableParameterfvEXT;
GL::glGetColorTableParameterivEXT GL::getColorTableParameterivEXT;
GL::glLockArraysEXT               GL::lockArraysEXT;
GL::glUnlockArraysEXT             GL::unlockArraysEXT;
GL::glWindowPos2iMESA             GL::windowPos2iMESA;
GL::glWindowPos2sMESA             GL::windowPos2sMESA;
GL::glWindowPos2fMESA             GL::windowPos2fMESA;
GL::glWindowPos2dMESA             GL::windowPos2dMESA;
GL::glWindowPos2ivMESA            GL::windowPos2ivMESA;
GL::glWindowPos2svMESA            GL::windowPos2svMESA;
GL::glWindowPos2fvMESA            GL::windowPos2fvMESA;
GL::glWindowPos2dvMESA            GL::windowPos2dvMESA;
GL::glWindowPos3iMESA             GL::windowPos3iMESA;
GL::glWindowPos3sMESA             GL::windowPos3sMESA;
GL::glWindowPos3fMESA             GL::windowPos3fMESA;
GL::glWindowPos3dMESA             GL::windowPos3dMESA;
GL::glWindowPos3ivMESA            GL::windowPos3ivMESA;
GL::glWindowPos3svMESA            GL::windowPos3svMESA;
GL::glWindowPos3fvMESA            GL::windowPos3fvMESA;
GL::glWindowPos3dvMESA            GL::windowPos3dvMESA;
GL::glWindowPos4iMESA             GL::windowPos4iMESA;
GL::glWindowPos4sMESA             GL::windowPos4sMESA;
GL::glWindowPos4fMESA             GL::windowPos4fMESA;
GL::glWindowPos4dMESA             GL::windowPos4dMESA;
GL::glWindowPos4ivMESA            GL::windowPos4ivMESA;
GL::glWindowPos4svMESA            GL::windowPos4svMESA;
GL::glWindowPos4fvMESA            GL::windowPos4fvMESA;
GL::glWindowPos4dvMESA            GL::windowPos4dvMESA;
GL::glResizeBuffersMESA           GL::resizeBuffersMESA;
GL::glGenOcclusionQueriesNV       GL::genOcclusionQueriesNV;
GL::glDeleteOcclusionQueriesNV    GL::deleteOcclusionQueriesNV;
GL::glBeginOcclusionQueryNV       GL::beginOcclusionQueryNV;
GL::glEndOcclusionQueryNV         GL::endOcclusionQueryNV;
GL::glGetOcclusionQueryivNV       GL::getOcclusionQueryivNV;
GL::glGetOcclusionQueryuivNV      GL::getOcclusionQueryuivNV;

// vim: ts=2 sw=2 et
