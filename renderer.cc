/***************************************************************************
                         renderer.cc  -  Rendering routines
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

#include <algorithm>
#include <list>

#include <math.h>

#include <espace/color.h>
#include <espace/cvar.h>
#include <espace/file.h>
#include <espace/font.h>
#include <espace/map.h>
#include <espace/model.h>
#include <espace/opengl.h>
#include <espace/output.h>
#include <espace/renderer.h>
#include <espace/shader.h>
#include <espace/skin.h>
#include <espace/system.h>
#include <espace/texture.h>

namespace
{
  struct Light
  {
    Light(const Vector3& origin, float intensity, const Color& color)
      : origin(origin),
        intensity(intensity),
        color(color)
    {
    }

    Vector3 origin;
    float   intensity;
    Color   color;
  };

  struct Corona
  {
    Corona(const Vector3& direction, const Color& color)
      : direction(direction),
        color(color)
    {
    }

    Vector3 direction;
    Color   color;
  };

  struct Scene
  {
    std::list<RefEntity> entities;
    std::list<Corona>    coronas;
    std::list<Light>     lights;
  };

  std::vector<Scene> scenes;

  Scene scene;

  uint lightmap;

  uint maxLights = 8; // XXX
  uint activeLights = 1;

  bool mode2D = false;

  // Render state

  uint activeTexture[16]; // Init in initialize()
  uint textureLevel = 0;
  uint clientTextureLevel = 0;

  Renderer::Face cullFace = Renderer::Face_None;
  Renderer::AlphaFunc alphaFunc = Renderer::Alpha_All;
  Renderer::Factor sourceBlend = Renderer::Factor_One;
  Renderer::Factor destBlend = Renderer::Factor_Zero;
  bool polygonOffset = false;
  Renderer::EnvMode texEnvMode = Renderer::EnvMode_Modulate;

  uint        vertexStride;
  const char* vertexPointer = 0;
  uint        colorStride;
  const char* colorPointer = 0;
  uint        texCoordStride[4];
  const char* texCoordPointer[4] = { 0 };
  uint        normalStride;
  const char* normalPointer = 0;

  Renderer::Source vertexSource = Renderer::Source_Constant;
  Renderer::Source colorSource = Renderer::Source_Constant;
  Renderer::Source texCoordSource[16]; // Init in initialize()
  Renderer::Source normalSource = Renderer::Source_Constant;
  bool colorAlpha = false;
}

struct PixmapPrivate
{
  uint* glHandles;
  std::vector<uint> widths;
  std::vector<uint> heights;
  bool nvRect;
};

void RefDef::setViewport(int x, int y, int width, int height)
{
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
}

void RefDef::setOrientation(float pitch, float yaw, float roll)
{
  axis.identity();
  axis.rotate(Vector3(0, 1, 0), -yaw);
  axis.rotate(Vector3(0, 0, 1), pitch);
  axis.rotate(Vector3(1, 0, 0), roll);
}

void Renderer::clearScreen()
{
  GL::clear(GL::COLOR_BUFFER_BIT | GL::DEPTH_BUFFER_BIT);
}

void Renderer::updateScreen()
{
  System::updateScreen();
}

void Renderer::setColor(const Color& color)
{
  Shader::st_entityColor = color;
}

void Renderer::setTexture(uint texture, uint level)
{
  if(texture == activeTexture[level])
    return;

  if(level != textureLevel)
  {
    GL::activeTextureARB(GL::TEXTURE0_ARB + level);

    textureLevel = level;
  }

  if(!texture)
  {
    if(level != clientTextureLevel)
    {
      GL::clientActiveTextureARB(GL::TEXTURE0_ARB + level);

      clientTextureLevel = level;
    }

    GL::disableClientState(GL::TEXTURE_COORD_ARRAY);
    GL::disable(GL::TEXTURE_2D);
  }
  else
  {
    if(!activeTexture[level])
    {
      if(level != clientTextureLevel)
      {
        GL::clientActiveTextureARB(GL::TEXTURE0_ARB + level);

        clientTextureLevel = level;
      }

      GL::enableClientState(GL::TEXTURE_COORD_ARRAY);
      GL::enable(GL::TEXTURE_2D);
    }

    GL::bindTexture(GL::TEXTURE_2D, texture);
  }

  activeTexture[level] = texture;
}

void Renderer::setCullFace(Renderer::Face face)
{
  if(face == cullFace)
    return;

  if(cullFace == Face_None)
    GL::enable(GL::CULL_FACE);

  switch(face)
  {
  case Face_Front: GL::cullFace(GL::FRONT); break;
  case Face_Back: GL::cullFace(GL::BACK); break;
  case Face_Both: GL::cullFace(GL::FRONT_AND_BACK); break;
  case Face_None: GL::disable(GL::CULL_FACE); break;

  default:

    esWarning << "Invalid cull face \"" << face << "\"." << std::endl;

    return;
  }

  cullFace = face;
}

void Renderer::setAlphaFunc(Renderer::AlphaFunc func)
{
  if(func == alphaFunc)
    return;

  if(alphaFunc == Alpha_All)
    GL::enable(GL::ALPHA_TEST);

  switch(func)
  {
  case Alpha_GT0: GL::alphaFunc(GL::GREATER, 0); break;
  case Alpha_LT128: GL::alphaFunc(GL::LESS, 128.0 / 255.0); break;
  case Alpha_GE128: GL::alphaFunc(GL::GEQUAL, 128.0 / 255.0); break;
  case Alpha_All: GL::disable(GL::ALPHA_TEST); break;

  default:

    esWarning << "Invalid alpha function \"" << func << "\"." << std::endl;

    return;
  }

  alphaFunc = func;
}

void Renderer::setBlendFunc(Renderer::Factor source, Renderer::Factor dest)
{
  if(source == sourceBlend && dest == destBlend)
    return;

  if(source == Factor_One && dest == Factor_Zero)
  {
    GL::disable(GL::BLEND);
  }
  else
  {
    if(sourceBlend == Factor_One && destBlend == Factor_Zero)
      GL::enable(GL::BLEND);

    GL::blendFunc(source, dest);
  }

  sourceBlend = source;
  destBlend = dest;
}

void Renderer::setDepthMask(bool mask)
{
  GL::depthMask(mask);
}

void Renderer::setPolygonOffset(bool offset)
{
  if(offset == polygonOffset)
    return;

  if(!offset)
  {
    GL::disable(GL::POLYGON_OFFSET_FILL);
  }
  else
  {
    GL::enable(GL::POLYGON_OFFSET_FILL);
    GL::polygonOffset(-2, -1);
  }

  polygonOffset = offset;
}

void Renderer::setTexEnvMode(EnvMode mode)
{
  if(mode == texEnvMode)
    return;

  switch(mode)
  {
  case EnvMode_Modulate:

    GL::texEnvi(GL::TEXTURE_ENV, GL::TEXTURE_ENV_MODE, GL::MODULATE);

    break;

  case EnvMode_Decal:

    GL::texEnvi(GL::TEXTURE_ENV, GL::TEXTURE_ENV_MODE, GL::DECAL);

    break;

  case EnvMode_Blend:

    GL::texEnvi(GL::TEXTURE_ENV, GL::TEXTURE_ENV_MODE, GL::BLEND);

    break;

  case EnvMode_Replace:

    GL::texEnvi(GL::TEXTURE_ENV, GL::TEXTURE_ENV_MODE, GL::REPLACE);

    break;

  default:

    esWarning << "Invalid texture environment mode \"" << mode
              << "\"." << std::endl;

    return;
  }

  texEnvMode = mode;
}

void Renderer::set2DMode()
{
  if(mode2D)
    return;

  GL::depthMask(GL::FALSE);
  GL::depthFunc(GL::ALWAYS);

  GL::viewport(0, 0, GL::config.width, GL::config.height);
  GL::scissor(0, 0, GL::config.width, GL::config.height);

  GL::matrixMode(GL::PROJECTION);
  GL::pushMatrix();
  GL::loadIdentity();
  GL::ortho(0, GL::config.width, GL::config.height, 0, 0, 1);

  GL::matrixMode(GL::MODELVIEW);
  GL::pushMatrix();
  GL::loadIdentity();

  mode2D = true;
}

void Renderer::set3DMode()
{
  if(!mode2D)
    return;

  GL::matrixMode(GL::MODELVIEW);
  GL::popMatrix();
  GL::matrixMode(GL::PROJECTION);
  GL::popMatrix();

  GL::depthMask(GL::TRUE);
  GL::depthFunc(GL::LEQUAL);

  mode2D = false;
}

void Renderer::drawLine3D(const Vector3& start, const Vector3& end,
                          Shader* shader)
{
  for(uint pass = 0; pass < shader->passCount(); ++pass)
  {
    shader->pushState(pass);

    GL::begin(GL::LINES);
    GL::vertex3fv(start.data());
    GL::vertex3fv(end.data());
    GL::end();

    shader->popState();
  }
}

void Renderer::drawQuad2D(float __x, float _y, const Pixmap* pixmap)
{
  setCullFace(Face_Back);
  setTexEnvMode(EnvMode_Replace);

  if(pixmap->d->nvRect)
  {
    // XXX: Respect and update texture state

    GL::enable(GL::TEXTURE_RECTANGLE_NV);
    GL::bindTexture(GL::TEXTURE_RECTANGLE_NV, pixmap->d->glHandles[0]);

    GL::begin(GL::QUADS);
    GL::texCoord2f(0, 0);
    GL::vertex2f(__x, _y);
    GL::texCoord2f(0, pixmap->d->heights[0]);
    GL::vertex2f(__x, _y + pixmap->d->heights[0]);
    GL::texCoord2f(pixmap->d->widths[0], pixmap->d->heights[0]);
    GL::vertex2f(__x + pixmap->d->widths[0], _y + pixmap->d->heights[0]);
    GL::texCoord2f(pixmap->d->widths[0], 0);
    GL::vertex2f(__x + pixmap->d->widths[0], _y);
    GL::end();

    GL::disable(GL::TEXTURE_RECTANGLE_NV);
  }
  else
  {
    for(uint y = 0; y < pixmap->d->heights.size(); ++y)
    {
      float _x = __x;

      for(uint x = 0; x < pixmap->d->widths.size(); ++x)
      {
        uint glHandle = pixmap->d->glHandles[y * pixmap->d->widths.size() + x];

        setTexture(glHandle);

        GL::begin(GL::QUADS);
        GL::texCoord2f(0, 0);
        GL::vertex2f(_x, _y);
        GL::texCoord2f(0, 1);
        GL::vertex2f(_x, _y + pixmap->d->heights[y]);
        GL::texCoord2f(1, 1);
        GL::vertex2f(_x + pixmap->d->widths[x], _y + pixmap->d->heights[y]);
        GL::texCoord2f(1, 0);
        GL::vertex2f(_x + pixmap->d->widths[x], _y);
        GL::end();

        _x += pixmap->d->widths[x];
      }

      _y += pixmap->d->heights[y];
    }
  }
}

void Renderer::drawQuad2D(float x, float y, float width, float height,
                          float s1, float t1, float s2, float t2,
                          Shader* shader)
{
  for(uint pass = 0; pass < shader->passCount(); ++pass)
  {
    shader->pushState(pass);

    GL::begin(GL::QUADS);

    if(cullFace == Face_Back)
    {
      GL::texCoord2f(s1, t1); GL::vertex2f(x, y);
      GL::texCoord2f(s1, t2); GL::vertex2f(x, y + height);
      GL::texCoord2f(s2, t2); GL::vertex2f(x + width, y + height);
      GL::texCoord2f(s2, t1); GL::vertex2f(x + width, y);
    }
    else
    {
      GL::texCoord2f(s1, t1); GL::vertex2f(x, y);
      GL::texCoord2f(s2, t1); GL::vertex2f(x + width, y);
      GL::texCoord2f(s2, t2); GL::vertex2f(x + width, y + height);
      GL::texCoord2f(s1, t2); GL::vertex2f(x, y + height);
    }

    GL::end();

    shader->popState();
  }
}

void Renderer::drawQuad2D(float x, float y, float width, float height,
                          float s1, float t1, float s2, float t2,
                          Shader* shader, float angle)
{
  angle = angle / 180.0 * M_PI;

  width /= 2;
  height /= 2;

  Vector3 center(x + width, y + height, 0);
  Matrix3x3 matrix;
  matrix.identity();
  matrix.rotate(Vector3(0, 0, 1), angle);

  Vector3 upperLeft =  (Vector3(-width, -height, 0) * matrix) + center;
  Vector3 upperRight = (Vector3( width, -height, 0) * matrix) + center;
  Vector3 lowerLeft =  (Vector3(-width,  height, 0) * matrix) + center;
  Vector3 lowerRight = (Vector3( width,  height, 0) * matrix) + center;

  for(uint pass = 0; pass < shader->passCount(); ++pass)
  {
    shader->pushState(pass);

    GL::begin(GL::QUADS);
    GL::texCoord2f(s1, t1); GL::vertex2fv(upperLeft.data());
    GL::texCoord2f(s1, t2); GL::vertex2fv(lowerLeft.data());
    GL::texCoord2f(s2, t2); GL::vertex2fv(lowerRight.data());
    GL::texCoord2f(s2, t1); GL::vertex2fv(upperRight.data());
    GL::end();

    shader->popState();
  }
}

void Renderer::drawQuad3D(const Vector3& v1, const Vector3& v2,
                          const Vector3& v3, const Vector3& v4,
                          const Vector2& t1, const Vector2& t2,
                          const Vector2& t3, const Vector2& t4,
                          Shader* shader)
{
  for(uint pass = 0; pass < shader->passCount(); ++pass)
  {
    shader->pushState(pass);

    GL::begin(GL::QUADS);
    GL::texCoord2fv(t1.data()); GL::vertex3fv(v1.data());
    GL::texCoord2fv(t2.data()); GL::vertex3fv(v2.data());
    GL::texCoord2fv(t3.data()); GL::vertex3fv(v3.data());
    GL::texCoord2fv(t4.data()); GL::vertex3fv(v4.data());
    GL::end();

    shader->popState();
  }
}

void Renderer::drawTriangles(uint triangleCount, const uint* indexes)
{
  GL::drawElements(GL::TRIANGLES, triangleCount * 3, GL::UNSIGNED_INT,
                   indexes);
}

void Renderer::drawTriangles(uint triangleCount, const uint* indexes,
                             Shader* shader)
{
  for(uint pass = 0; pass < shader->passCount(); ++pass)
  {
    shader->pushState(pass);

    GL::drawElements(GL::TRIANGLES, triangleCount * 3, GL::UNSIGNED_INT,
                     indexes);

    shader->popState();
  }
}

void Renderer::drawTriangles(uint triangleCount, const uint* indexes,
                             Skin* skin, const char* surfaceName)
{
  skin->pushState(surfaceName);

  GL::drawElements(GL::TRIANGLES, triangleCount * 3, GL::UNSIGNED_INT, indexes);
}

// Matrix functions

void Renderer::setProjectionMatrix(const Matrix4x4& matrix)
{
  GL::matrixMode(GL::PROJECTION);

  GL::loadMatrixf(matrix.data());
}

void Renderer::setViewMatrix(const Matrix4x4& matrix)
{
  GL::matrixMode(GL::MODELVIEW);

  GL::loadMatrixf(matrix.data());
}

Matrix4x4 Renderer::projectionMatrix()
{
  float temp[16];

  GL::getFloatv(GL::PROJECTION_MATRIX, temp);

  return Matrix4x4(temp);
}

Matrix4x4 Renderer::viewMatrix()
{
  float temp[16];

  GL::getFloatv(GL::MODELVIEW_MATRIX, temp);

  return Matrix4x4(temp);
}

// Font functions

void Renderer::put(float x, float y, const Font* font, const char* text,
                   int align)
{
  if(!font)
    return;

  int width = 0;

  if(align != Left)
  {
    for(const char* ch = text; *ch; ++ch)
      width += font->glyphs[static_cast<int>(*ch)].xSkip;

    if(align == Center)
      x -= width / 2;
    else // align == RightToLeft
      x -= width;
  }

  // Used for caching the results from Shader::shaderForHandle()
  int lastHandle = 0;
  Shader* shader = 0;

  while(*text)
  {
    const Font::Glyph& glyph = font->glyphs[static_cast<int>(*text)];

    if(glyph.shaderHandle != lastHandle)
    {
      shader = Shader::shaderForHandle(glyph.shaderHandle);

      lastHandle = glyph.shaderHandle;
    }

    if(shader)
    {
      drawQuad2D(x, y - glyph.top, glyph.imageWidth, glyph.imageHeight,
                 glyph.s1, glyph.t1, glyph.s2, glyph.t2, shader);
    }

    x += glyph.xSkip;
    ++text;
  }
}

// Buffered rendering

namespace
{
  class Primitive
  {
  public:

    enum Flags
    {
      PF_Texture = 1,
      PF_Lightmap = 2,
      PF_Shader = 4
    };

    uint        type;
    uint        flags;
    const uint* indexes;
    uint        indexCount;
    union
    {
      Shader*   shader;
      int       texture;
    };
    uint        lightmap;
    int         sort0;
    int         sort1;

    bool operator<(const Primitive& p) const
    {
      if(sort0 != p.sort0)
        return sort0 < p.sort0;

      return sort1 < p.sort1;
    }
  };

  const int maxPrimitives = 32768;

  Primitive simplePrimitives[maxPrimitives];
  uint      simplePrimitiveCount = 0;
  Primitive shaderPrimitives[maxPrimitives];
  uint      shaderPrimitiveCount = 0;
}

void Renderer::setVertexArray(const void* vertices, uint stride)
{
  vertexStride = stride;
  vertexPointer = reinterpret_cast<const char*>(vertices);

  GL::vertexPointer(3, GL::FLOAT, stride, vertices);
}

void Renderer::setColorArray(const void* colors, uint stride)
{
  colorStride = stride;
  colorPointer = reinterpret_cast<const char*>(colors);

  if(colorSource == Source_Array0)
    GL::colorPointer(colorAlpha ? 4 : 3, GL::UNSIGNED_BYTE, stride, colors);
}

void Renderer::setTexCoordArray(uint index, const void* coords, uint stride)
{
  texCoordStride[index] = stride;
  texCoordPointer[index] = reinterpret_cast<const char*>(coords);

  // XXX: Iterate to max used texture level instead of 16.
  for(uint level = 0; level < 16; ++level)
  {
    if(texCoordSource[level] == Source_Array0 + index)
    {
      GL::clientActiveTextureARB(GL::TEXTURE0_ARB + level);

      clientTextureLevel = level;

      GL::texCoordPointer(2, GL::FLOAT, stride, coords);
    }
  }
}

void Renderer::setNormalArray(const void* normals, uint stride)
{
  normalStride = stride;
  normalPointer = reinterpret_cast<const char*>(normals);

  if(normalSource == Source_Array0)
    GL::normalPointer(GL::FLOAT, stride, normals);
}

void Renderer::setColors(Source source, bool alpha)
{
  if(source == colorSource && alpha == colorAlpha)
    return;

  switch(source)
  {
  case Source_Constant:

    GL::disableClientState(GL::COLOR_ARRAY);

    break;

  case Source_Array0:

    GL::enableClientState(GL::COLOR_ARRAY);

    GL::colorPointer(alpha ? 4 : 3, GL::UNSIGNED_BYTE, colorStride,
                     colorPointer);

    break;

  default:

    esWarning << "Unsupported color source \"" << source
              << "\"." << std::endl;

    return;
  }

  colorSource = source;
  colorAlpha = alpha;
}

void Renderer::setTexCoords(Source source, uint level)
{
  if(source == texCoordSource[level])
    return;

  if(level != clientTextureLevel)
  {
    GL::clientActiveTextureARB(GL::TEXTURE0_ARB + level);

    clientTextureLevel = level;
  }

  if(texCoordSource[level] == Source_Constant)
    GL::enableClientState(GL::TEXTURE_COORD_ARRAY);

  switch(source)
  {
  case Source_Constant:

    GL::disableClientState(GL::TEXTURE_COORD_ARRAY);

    break;

  case Source_Array0:

    GL::texCoordPointer(2, GL::FLOAT, texCoordStride[0], texCoordPointer[0]);

    break;

  case Source_Array1:

    GL::texCoordPointer(2, GL::FLOAT, texCoordStride[1], texCoordPointer[1]);

    break;

  case Source_Array2:

    GL::texCoordPointer(2, GL::FLOAT, texCoordStride[2], texCoordPointer[2]);

    break;

  case Source_Array3:

    GL::texCoordPointer(2, GL::FLOAT, texCoordStride[3], texCoordPointer[3]);

    break;

  default:

    esWarning << "Unsupported texture coordinate source \"" << source
              << "\"." << std::endl;

    return;
  }

  texCoordSource[level] = source;
}

void Renderer::setNormals(Source source)
{
  if(source == normalSource)
    return;

  switch(source)
  {
  case Source_Constant:

    GL::disableClientState(GL::NORMAL_ARRAY);

    break;

  case Source_Array0:

    GL::enableClientState(GL::NORMAL_ARRAY);

    GL::normalPointer(GL::FLOAT, normalStride, normalPointer);

    break;

  default:

    esWarning << "Unsupported normal source \"" << source
              << "\"." << std::endl;

    return;
  }

  normalSource = source;
}

void Renderer::addTriangles(uint triangleCount, const uint* indexes,
                            int texture, int lightmap)
{
  Primitive& primitive = simplePrimitives[simplePrimitiveCount++];

  primitive.type = GL::TRIANGLES;
  primitive.flags = Primitive::PF_Texture;

  if(lightmap >= 0)
  {
    primitive.flags |= Primitive::PF_Lightmap;
    primitive.lightmap = lightmap;
  }

  primitive.indexes = indexes;
  primitive.indexCount = triangleCount * 3;
  primitive.texture = texture;
}

void Renderer::addTriangles(uint triangleCount, const uint* indexes,
                            Shader* shader, int lightmap, int sort)
{
  Primitive& primitive = shaderPrimitives[shaderPrimitiveCount++];

  primitive.type = GL::TRIANGLES;
  primitive.flags = Primitive::PF_Shader;

  if(lightmap >= 0)
  {
    primitive.flags |= Primitive::PF_Lightmap;
    primitive.lightmap = lightmap;
  }

  primitive.indexes = indexes;
  primitive.indexCount = triangleCount * 3;
  primitive.shader = shader;
  primitive.sort0 = sort;
  primitive.sort1 = shader->sort();
}

void Renderer::flush()
{
  std::stable_sort(simplePrimitives,
                   simplePrimitives + simplePrimitiveCount);
  std::stable_sort(shaderPrimitives,
                   shaderPrimitives + shaderPrimitiveCount);

  setCullFace(Face_None);
  setAlphaFunc(Alpha_All);
  setPolygonOffset(false);
  setBlendFunc(Factor_One, Factor_Zero);

  const Primitive* end;

  if(simplePrimitiveCount)
  {
    setTexCoords(Source_Array0, 0);
    setTexCoords(Source_Array1, 1);

    end = &simplePrimitives[simplePrimitiveCount];

    for(const Primitive* primitive = &simplePrimitives[0];
        primitive != end; ++primitive)
    {
      if(primitive->flags & Primitive::PF_Lightmap)
        setTexture(primitive->lightmap, 1);
      else
        setTexture(0, 1);

      setTexture(primitive->texture);

      for(;;)
      {
        GL::drawElements(primitive->type, primitive->indexCount,
                         GL::UNSIGNED_INT, primitive->indexes);

        if((primitive + 1)->texture != primitive->texture
        || (primitive + 1)->lightmap != primitive->lightmap
        || (primitive + 1) == end)
          break;

        ++primitive;
      }
    }

    setTexCoords(Source_Constant, 1);
    setTexture(0, 1);
  }

  end = &shaderPrimitives[shaderPrimitiveCount];

  for(const Primitive* primitive = &shaderPrimitives[0]; primitive != end;)
  {
    uint count = 1;

    while((&primitive[count] != end)
       && (primitive[count].shader == primitive->shader))
    {
      count++;
    }

    for(uint pass = 0; pass < primitive->shader->passCount(); ++pass)
    {
      primitive->shader->pushState(pass);

      for(uint i = 0; i < count; ++i)
      {
        if(texCoordSource[0] == Source_Array1)
          setTexture(primitive[i].lightmap);

        GL::drawElements(primitive[i].type, primitive[i].indexCount,
                         GL::UNSIGNED_INT, primitive[i].indexes);
      }

      primitive->shader->popState();
    }

    primitive += count;
  }

  CVar dynamicLights = CVar::acquire("r_dynamiclight", "1", CVar::Archive);

  if(!dynamicLights.integer)
    scene.lights.clear();

  if(!scene.lights.empty())
  {
    setDepthMask(false);
    setPolygonOffset(false);
    setBlendFunc(Factor_DstColor, Factor_One);
    setTexture(lightmap);
    setTexEnvMode(EnvMode_Modulate);

    setTexCoords(Source_Constant);
    setColors(Source_Constant);

    for(std::list<Light>::iterator light = scene.lights.begin();
        light != scene.lights.end(); ++light)
    {
      Vector3 origin = light->origin;

      GL::disable(GL::CULL_FACE);

      for(const Primitive* primitive = &shaderPrimitives[0];
          primitive != &simplePrimitives[simplePrimitiveCount]; ++primitive)
      {
        if(primitive == &shaderPrimitives[shaderPrimitiveCount])
        {
          if(simplePrimitiveCount == 0)
            break;

          primitive = &simplePrimitives[0];
        }

        Vector3 min = *reinterpret_cast<const Vector3*>
            (vertexPointer + primitive->indexes[0] * vertexStride);
        Vector3 max = min;

        uint i;

        for(i = 1; i < primitive->indexCount; ++i)
        {
          const Vector3& vertex = *reinterpret_cast<const Vector3*>
            (vertexPointer + primitive->indexes[i] * vertexStride);

               if(vertex(0) < min(0)) min(0) = vertex(0);
          else if(vertex(0) > max(0)) max(0) = vertex(0);
               if(vertex(1) < min(1)) min(1) = vertex(1);
          else if(vertex(1) > max(1)) max(1) = vertex(1);
               if(vertex(2) < min(2)) min(2) = vertex(2);
          else if(vertex(2) > max(2)) max(2) = vertex(2);
        }

        float dist = light->intensity * 2;

        if(origin(0) < min(0) - dist
        || origin(0) > max(0) + dist
        || origin(1) < min(1) - dist
        || origin(1) > max(1) + dist
        || origin(2) < min(2) - dist
        || origin(2) > max(2) + dist)
          continue;

        GL::begin(primitive->type);

        for(uint i = 0; i < primitive->indexCount; ++i)
        {
          Vector3 normal = *reinterpret_cast<const Vector3*>
            (normalPointer + primitive->indexes[i] * normalStride);

          const Vector3& vertex = *reinterpret_cast<const Vector3*>
            (vertexPointer + primitive->indexes[i] * vertexStride);

          float distance = normal * origin;
          float distance2 = normal * vertex;

          if(distance < distance2)
            continue;

          float intensity = 1.0 / ((distance - distance2) * (distance - distance2) / (light->intensity * light->intensity));

          if(intensity > 1)
            intensity = 1;

          GL::color3ub(static_cast<unsigned char>(light->color(0) * intensity),
                       static_cast<unsigned char>(light->color(1) * intensity),
                       static_cast<unsigned char>(light->color(2) * intensity));

          Vector3 nearest = origin - distance * normal;

          Vector3 U;

          if(fabs(normal(0)) >= fabs(normal(1)))
          {
            U = normal.cross(Vector3(0, 1, 0));
          }
          else
          {
            U = normal.cross(Vector3(1, 0, 0));
          }

          Vector3 V = normal.cross(U);

          float u = (vertex - nearest) * U;
          float v = (vertex - nearest) * V;

          u *= intensity;
          v *= intensity;

          u /= (light->intensity * 2);
          v /= (light->intensity * 2);

          u += 0.5;
          v += 0.5;

          GL::texCoord2f(u, v);

          GL::arrayElement(primitive->indexes[i]);
        }

        GL::end();
      }
    }
  }

  simplePrimitiveCount = 0;
  shaderPrimitiveCount = 0;
}

void Renderer::clearScene()
{
  scene.entities.clear();
  scene.lights.clear();
  scene.coronas.clear();

  // XXX: clear polys
}

void Renderer::addLight(const Vector3& origin, float intensity, const Color& color)
{
  scene.lights.push_back(Light(origin, intensity, color));
}

void Renderer::addCorona(const Vector3& direction, const Color& color)
{
  scene.coronas.push_back(Corona(direction, color));
}

void Renderer::prependRefEntity(const RefEntity& refEntity)
{
  scene.entities.push_front(refEntity);
}

void Renderer::appendRefEntity(const RefEntity& refEntity)
{
  scene.entities.push_back(refEntity);
}

void Renderer::renderScene(const RefDef& refDef, Map* map)
{

  if(refDef.fovX == 0 || refDef.fovY == 0)
    return;

  if(refDef.glFog.mode)
  {
    GL::fogi(GL::FOG_MODE, refDef.glFog.mode);

    float color[4] =
    {
      refDef.glFog.color(0) / 255.0,
      refDef.glFog.color(1) / 255.0,
      refDef.glFog.color(2) / 255.0,
      refDef.glFog.color(3) / 255.0,
    };

    GL::fogfv(GL::FOG_COLOR, color);
    GL::fogf(GL::FOG_START, refDef.glFog.near);
    GL::fogf(GL::FOG_END, refDef.glFog.far);
    GL::fogf(GL::FOG_DENSITY, refDef.glFog.density);
  }

  set3DMode();

  GL::clear(GL::DEPTH_BUFFER_BIT);

  int y = GL::config.height - (refDef.y + refDef.height);

  GL::viewport(refDef.x, y, refDef.width, refDef.height);
  GL::scissor(refDef.x, y, refDef.width, refDef.height);

  GL::matrixMode(GL::PROJECTION);
  GL::pushMatrix();

  // Create projection matrix with far plane at infinity

  const float zNear = 1;

  Vector3 min, max;

  max(0) = zNear * tan(refDef.fovX * M_PI / 360.0);
  max(1) = zNear * tan(refDef.fovY * M_PI / 360.0);

  min = -max;

  Matrix4x4 projMatrix;

  projMatrix(0, 0) = 2 * zNear / (max(0) - min(0));
  projMatrix(0, 1) = 0;
  projMatrix(0, 2) = (max(0) + min(0)) / (max(0) - min(0));
  projMatrix(0, 3) = 0;
  projMatrix(1, 0) = 0;
  projMatrix(1, 1) = 2 * zNear / (max(1) - min(1));
  projMatrix(1, 2) = (max(1) + min(1)) / (max(1) - min(1));
  projMatrix(1, 3) = 0;
  projMatrix(2, 0) = 0;
  projMatrix(2, 1) = 0;
  projMatrix(2, 2) = -1;
  projMatrix(2, 3) = -2 * zNear;
  projMatrix(3, 0) = 0;
  projMatrix(3, 1) = 0;
  projMatrix(3, 2) = -1;
  projMatrix(3, 3) = 0;

  GL::loadMatrixf(projMatrix.data());

  GL::matrixMode(GL::MODELVIEW);
  GL::pushMatrix();

  Matrix4x4 viewMatrix;

  viewMatrix.identity();
  viewMatrix.rotate(Vector3(1, 0, 0), -M_PI / 2);
  viewMatrix.rotate(Vector3(0, 0, 1), M_PI / 2);
  viewMatrix *= refDef.axis;
  viewMatrix.translate(-refDef.origin(0),
                       -refDef.origin(1),
                       -refDef.origin(2));

  GL::loadMatrixf(viewMatrix.data());

  Matrix3x3 orientation = refDef.axis;

  uint light = 0;

  float specular[] =
  {
    1.0, 1.0, 1.0, 1.0
  };
  float shininess = 100.0;

  GL::materialfv(GL::FRONT_AND_BACK, GL::AMBIENT, specular);
  GL::materialfv(GL::FRONT_AND_BACK, GL::DIFFUSE, specular);
  GL::materialfv(GL::FRONT_AND_BACK, GL::SPECULAR, specular);
  GL::materialfv(GL::FRONT_AND_BACK, GL::SHININESS, &shininess);

  /*
  // Set the GL_AMBIENT_AND_DIFFUSE color state variable to be the
  // one referred to by all following calls to glColor
  GL::colorMaterial(GL::FRONT_AND_BACK, GL::AMBIENT_AND_DIFFUSE);
  GL::enable(GL::COLOR_MATERIAL);
*/
  for(std::list<Corona>::iterator i = scene.coronas.begin();
      i != scene.coronas.end() && light < maxLights; ++i)
  {
    GL::enable(GL::LIGHT0 + light);

    float position[4] =
    {
      i->direction(0),
      i->direction(1),
      i->direction(2),
      0
    };

    GL::lightfv(GL::LIGHT0 + light, GL::POSITION, position);

    float color[4] =
    {
      i->color(0) / 255.0,
      i->color(1) / 255.0,
      i->color(2) / 255.0,
      1.0
    };

    GL::lightfv(GL::LIGHT0 + light, GL::DIFFUSE, color);

    color[0] = 0;
    color[1] = 0;
    color[2] = 0;

    GL::lightfv(GL::LIGHT0 + light, GL::AMBIENT, color);

    ++light;
  }

  for(uint i = light; i < activeLights; ++i)
    GL::disable(GL::LIGHT0 + i);

  activeLights = light;

  if(map && !(refDef.rdflags & RefDef::NoWorldModel))
    map->render();

  // map->render may change matrix mode
  GL::matrixMode(GL::MODELVIEW);

  for(std::list<RefEntity>::iterator i = scene.entities.begin();
      i != scene.entities.end(); ++i)
  {
    Shader::st_entityColor = i->color;

    switch(i->type)
    {
    case RefEntity::Model:

      if(i->modelHandle)
      {
        if(i->renderfx & RefEntity::ThirdPerson)
          break;

        GL::pushMatrix();

        Matrix4x4 modelMatrix;

        modelMatrix.identity();
        modelMatrix.translate(i->origin(0), i->origin(1), i->origin(2));
        modelMatrix *= i->axis;

        GL::multMatrixf(modelMatrix.data());

        Model* model = Model::modelForHandle(i->modelHandle);

        int skin = i->customSkin ? i->customSkin : i->skinNum;

        model->render(i->frame, i->backlerp, i->customShader, skin);

        GL::matrixMode(GL::MODELVIEW);
        GL::popMatrix();
      }

      break;

    case RefEntity::Poly:

      break;

    case RefEntity::Sprite:

      {
        Shader* shader = Shader::shaderForHandle(i->customShader);

        if(!shader)
          break;

        // XXX: handle i->rotation
        Matrix3x3 rotation;

        rotation.identity();
        rotation.rotate(orientation.direction(), i->rotation);

        Vector3 right = orientation.right() * rotation * i->radius;
        Vector3 up = orientation.up() * rotation * i->radius;

        float oldShaderTime = Shader::time;

        Shader::time = i->time / 100.0;

        Renderer::drawQuad3D(i->origin + up - right,
                             i->origin - up - right,
                             i->origin - up + right,
                             i->origin + up + right,
                             Vector2(0, 0),
                             Vector2(0, 1),
                             Vector2(1, 1),
                             Vector2(1, 0),
                             shader);

        Shader::time = oldShaderTime;
      }

      break;

    case RefEntity::Splash:

      break;

    case RefEntity::Beam:

      {
        Shader* shader = Shader::shaderForHandle(i->customShader);

        if(!shader)
          break;

        Vector3 direction = i->origin - i->oldOrigin;

        Vector3 side = direction.cross(i->origin - refDef.origin);

        side.normalize();
        side *= i->frame;

        Renderer::drawQuad3D(i->origin + side,
                             i->origin - side,
                             i->oldOrigin - side,
                             i->oldOrigin + side,
                             Vector2(0, 0),
                             Vector2(0, 1),
                             Vector2(1, 1),
                             Vector2(1, 0),
                             shader);
      }

      break;

    case RefEntity::RailCore:

      {
        Shader* shader = Shader::shaderForHandle(i->customShader);

        if(!shader)
          break;

        Vector3 direction = i->oldOrigin - i->origin;

        float length = direction.magnitude();

        direction /= length;

        Vector3 side0, side1;

        if(fabs(direction(0)) > 0.5)
        {
          side0 = direction.cross(Vector3(0, 1, 0));
        }
        else
        {
          side0 = direction.cross(Vector3(1, 0, 0));
        }

        side1 = direction.cross(side0);

        side0 *= 5;
        side1 *= 5;

        Vector3 v = i->origin;

        shader->pushState(0);

        GL::pointSize(3);
        GL::begin(GL::POINTS);

        for(float t = 0; t < length; t += 15)
        {
          GL::vertex3fv((v + sin(t * t + i->time) * side0
                           + cos(t * t + i->time) * side1).data());

          v += direction * 15;
        }

        GL::end();

        shader->popState();
      }

      break;

    case RefEntity::RailCoreTaper:

      break;

    case RefEntity::RailRings:

      {
        Shader* shader = Shader::shaderForHandle(i->customShader);

        if(!shader)
          break;

        Vector3 direction = i->oldOrigin - i->origin;

        float length = direction.magnitude();

        direction /= length;

        Vector3 side0, side1;

        if(fabs(direction(0)) > 0.5)
        {
          side0 = direction.cross(Vector3(0, 1, 0));
        }
        else
        {
          side0 = direction.cross(Vector3(1, 0, 0));
        }

        side1 = direction.cross(side0);

        side0 *= 10;
        side1 *= 10;

        Vector3 v = i->origin;

        shader->pushState(0);

        GL::lineWidth(2);
        GL::begin(GL::LINE_STRIP);

        for(float t = 0; t < length; t += 10)
        {
          GL::vertex3fv((v + sin(t / 80.0 * M_PI + i->time) * side0
                           + cos(t / 80.0 * M_PI + i->time) * side1).data());

          v += direction * 10;
        }

        GL::end();

        shader->popState();
      }


      break;

    case RefEntity::Lightning:

      break;

    case RefEntity::PortalSurface:

      break;
    }
  }

  // Matrix mode is GL::MODELVIEW
  GL::popMatrix();
  GL::matrixMode(GL::PROJECTION);
  GL::popMatrix();
}

void Renderer::pushScene()
{
  scenes.push_back(scene);
}

void Renderer::popScene()
{
  scene = scenes.back();

  if(scenes.empty())
  {
    esWarning << "Attempt to pop from an empty scene stack." << std::endl;

    return;
  }

  scenes.pop_back();
}

void Renderer::initialize()
{
  static bool initialized = false;

  if(initialized)
    return;

  System::initialize();

  GL::initialize();

  GL::genTextures(1, &lightmap);

  setTexture(lightmap);

  GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MAG_FILTER,
                    GL::LINEAR);
  GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_MIN_FILTER,
                    GL::LINEAR);

  GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_S, GL::CLAMP);
  GL::texParameteri(GL::TEXTURE_2D, GL::TEXTURE_WRAP_T, GL::CLAMP);

  unsigned char data[256][256];

  for(uint x = 0; x < 256; ++x)
  {
    for(uint y = 0; y < 256; ++y)
    {
      float dist = 13.0f / (sqrtf(pow(x / 255.0 - 0.5, 2)
                         + pow(y / 255.0 - 0.5, 2)) * 100);

      int intensity = static_cast<int>(dist * 255 - 66);

      data[y][x] = (intensity >= 255) ? 255
                 : (intensity <= 0)   ? 0
                                      : intensity;
    }
  }

  GL::texImage2D(GL::TEXTURE_2D, 0, 1, 256, 256, 0, GL::LUMINANCE,
                 GL::UNSIGNED_BYTE, data);

  for(int i = 0; i < 16; ++i)
  {
    activeTexture[i] = 0;
    texCoordSource[i] = Source_Constant;
  }

  // From Shader::setDefaults()

  GL::enable(GL::DEPTH_TEST);
  GL::depthFunc(GL::LEQUAL);
  GL::enableClientState(GL::VERTEX_ARRAY);
  GL::disableClientState(GL::COLOR_ARRAY);
  GL::enableClientState(GL::TEXTURE_COORD_ARRAY);

  initialized = true;
}

Pixmap* Pixmap::acquire(const char* fileName)
{
  Image* image = Image::acquire(fileName);

  if(!image)
    return 0;

  Pixmap* pixmap = new Pixmap(image);

  Image::unacquire(image);

  return pixmap;
}

Pixmap::Pixmap(Image* image)
  : d(new PixmapPrivate)
{
  d->nvRect = GL::config.extensions.count("GL_NV_texture_rectangle")
              && CVar::getInt("r_recttextures");

  if(d->nvRect)
  {
    d->widths.push_back(image->width());
    d->heights.push_back(image->height());

    d->glHandles = new uint[1];

    d->glHandles[0] = Texture::acquire(image,   Texture::NoLinear
                                              | Texture::NoRepeat
                                              | Texture::NoMipMaps
                                              | Texture::NVRect);
  }
  else
  {
    uint w = image->width();
    uint h = image->height();

    while(w)
    {
      if(w >= GL::config.maxTextureSize)
      {
        d->widths.push_back(GL::config.maxTextureSize);

        w -= GL::config.maxTextureSize;
      }
      else
      {
        uint i = 1;

        while(i * 2 <= w)
          i *= 2;

        d->widths.push_back(i);

        w -= i;
      }
    }

    while(h)
    {
      if(h >= GL::config.maxTextureSize)
      {
        d->heights.push_back(GL::config.maxTextureSize);

        h -= GL::config.maxTextureSize;
      }
      else
      {
        uint i = 1;

        while(i * 2 <= h)
          i *= 2;

        d->heights.push_back(i);

        h -= i;
      }
    }

    d->glHandles = new uint[d->widths.size() * d->heights.size()];

    uint8_t* sourceData = image->data();
    uint     bpp = image->bytesPerPixel();
    uint     sourceWidth = image->width();

    for(uint y = 0; y < d->heights.size(); ++y)
    {
      w = 0;

      for(uint x = 0; x < d->widths.size(); ++x)
      {
        Image* subImage = new Image(d->widths[x], d->heights[y],
                                    image->pixelFormat());

        uint8_t* data = subImage->data();

        for(uint i = 0; i < d->heights[y]; ++i)
        {
          memcpy(data + i * d->widths[x] * bpp,
                 sourceData + ((i + h) * sourceWidth + w) * bpp,
                 d->widths[x] * bpp);
        }

        d->glHandles[y * d->widths.size() + x]
          = Texture::acquire(subImage,   Texture::NoLinear
                                       | Texture::NoRepeat
                                       | Texture::NoMipMaps);

        delete subImage;

        w += d->widths[x];
      }

      h += d->heights[y];
    }
  }
}

Pixmap::~Pixmap()
{
  for(uint i = 0; i < d->widths.size() * d->heights.size(); ++i)
    Texture::unacquire(d->glHandles[i]);

  delete [] d->glHandles;

  delete d;
}

// vim: ts=2 sw=2 et
