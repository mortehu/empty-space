/***************************************************************************
                         q3shader.cc  -  Quake 3 shader handler
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

#include <map>
#include <vector>

#include <espace/color.h>
#include <espace/file.h>
#include <espace/texture.h>
#include <espace/opengl.h>
#include <espace/output.h>
#include <espace/predicates.h>
#include <espace/preprocessor.h>
#include <espace/renderer.h>
#include <espace/shader.h>

#include "q3shader.h"

class Q3ShaderData : public Shader
{
public:

  Q3ShaderData();

  void acquire();
  void unacquire();

  uint passCount() const;
  void pushState(uint pass);
  void popState();

  bool isSky() const;
  uint sort() const;
  Renderer::Face cullFace() const;

  class Map
  {
  public:

    Map();

    enum ColorGen
    {
      None,
      Identity,
      IdentityLighting,
      Entity,
      OneMinusEntity,
      Vertex,
      ExactVertex,
      LightingDiffuse,
      Wave,
      Constant
    };

    enum TCGen
    {
      Base,
      Lightmap,
      Environment
    };

    std::vector<String> textureNames;
    uint*  textureHandles;
    uint   frameCount;
    float  frequency;
    bool   clamp;
    Renderer::Factor sourceBlend;
    Renderer::Factor destBlend;

    ColorGen rgbGen;
    WaveForm rgbGenWaveForm;
    float    rgbGenWaveBase;
    float    rgbGenWaveAmp;
    float    rgbGenWavePhase; // in [0..1] maps to [0..2*pi]
    float    rgbGenWaveFreq;  // in Hz
    Color    rgbGenColor;
    ColorGen alphaGen;
    WaveForm alphaGenWaveForm;
    float    alphaGenWaveBase;
    float    alphaGenWaveAmp;
    float    alphaGenWavePhase;
    float    alphaGenWaveFreq;
    TCGen    tcGen;

    class TCMod
    {
    public:

      enum Type
      {
        Rotate,
        Scale,
        Scroll,
        Stretch,
        Transform,
        Turbulence
      };

      Type  type;
      float s, t;
      float base;
      float amplitude;
      float phase;
      float freq;
      float matrix[2][3];
    };

    TCMod     tcMods[10];
    uint      tcModCount;
    bool      depthWrite;
    bool      detail;
    Renderer::AlphaFunc alphaFunc;
  };

  std::vector<Map> maps;

  Renderer::Face _cullFace;
  WaveForm deformVertsWave;
  float    deformVertsBase;
  float    deformVertsAmp;
  float    deformVertsPhase;
  float    deformVertsFreq;
  float    deformVertsBulgeWidth;
  float    deformVertsBulgeHeight;
  float    deformVertsBulgeSpeed;
  Vector3  deformVertsDir;
  bool     fog;
  Color    fogColor;
  float    fogDistance; // distance until fogs becomes totally opaque
  bool     nopicmip;
  bool     nomipmaps;
  bool     polygonOffset;
  bool     portal;
  uint     _sort;
  String   farbox;
  uint     farboxHandles[6];
  uint     cloudHeight;
  String   nearbox;
  uint     nearboxHandles[6];
  bool     sky;
  bool     flesh;
  bool     lava;
  bool     metalsteps;
  bool     nodamage;
  bool     nodlight;
  bool     nodraw; // XXX: Is this needed?
  bool     noimpact;
  bool     nomarks;
  bool     nolightmap;
  bool     nosteps;
  bool     origin; // XXX: is this needed?
  bool     playerclip; // XXX: is this needed?
  bool     slick;
  bool     slime;
  bool     water;

  static bool tcGen;
  static bool lighting;
};

namespace
{
  std::map<String, Q3ShaderData*> shaders;

  void fileHook(const char* name);
}

bool Q3ShaderData::tcGen = false;
bool Q3ShaderData::lighting = false;

Q3Shader::Q3Shader()
{
  File::addFileHook(fileHook);
}

namespace
{
  void fileHook(const char* name)
  {
    if(!EndsWith(".shader")(name))
      return;

    esDebug(3) << "Found shader file \"" << name << "\"." << std::endl;

    Preprocessor input(name);

    String token;
    String shaderName;
    Q3ShaderData* shader = 0;

    while(!(token = input.nextToken()).isNull())
    {
      bool hasSort = false;

      if(*token == '\n')
        continue;

      if(*token == '{')
      {
        while('}' != *(token = input.nextToken()))
        {
          if(*token == '\n')
            continue;

          if(token.beginsWith("q3map_"))
          {
            while('\n' != *(input.nextToken()));

            continue;
          }

          if(token == "cull")
          {
            token = input.nextToken().toLower();

            shader->_cullFace = (token == "back")  ? Renderer::Face_Back
                              : (token == "front") ? Renderer::Face_Front
                                                   : Renderer::Face_None;
          }
          else if(token == "sort")
          {
            token = input.nextToken().toLower();

            bool ok = true;

            shader->_sort = (token == "portal")     ? Shader::Portal
                          : (token == "sky")        ? Shader::Sky
                          : (token == "opaque")     ? Shader::Opaque
                          : (token == "banner")     ? Shader::Banner
                          : (token == "underwater") ? Shader::Underwater
                          : (token == "additive")   ? Shader::Additive
                          : (token == "nearest")    ? Shader::Nearest
                          : (token.toInt(&ok) << 24);

            hasSort = true;

            if(!ok)
            {
              esWarning << "Shader: Invalid sort value: " << token << std::endl;

              shader->_sort = Shader::Opaque;
            }
          }
          else if(token == "nomipmaps" || token == "nomipmap")
          {
            shader->nomipmaps = true;
          }
          else if(token == "skyparms")
          {
            shader->farbox = input.nextToken().toLower();
            shader->cloudHeight = input.nextToken().toUInt();
            shader->nearbox = input.nextToken().toLower();
          }
          else if(token == "surfaceparm")
          {
            token = input.nextToken().toLower();

            if(token == "sky") shader->sky = true;
            else if(token == "flesh") shader->flesh = true;
            else if(token == "lava") shader->lava = true;
            else if(token == "metalsteps") shader->metalsteps = true;
            else if(token == "nodamage") shader->nodamage = true;
            else if(token == "nodlight") shader->nodlight = true;
            else if(token == "nodraw") shader->nodraw = true;
            else if(token == "noimpact") shader->noimpact = true;
            else if(token == "nomarks") shader->nomarks = true;
            else if(token == "nolightmap") shader->nolightmap = true;
            else if(token == "nosteps") shader->nosteps = true;
            else if(token == "origin") shader->origin = true;
            else if(token == "playerclip") shader->playerclip = true;
            else if(token == "slick") shader->slick = true;
            else if(token == "slime") shader->slime = true;
            else if(token == "water") shader->water = true;
          }
          else if(token == "polygonOffset")
          {
            shader->polygonOffset = true;
          }
          else if(*token == '{')
          {
            Q3ShaderData::Map& map = *shader->maps.insert(shader->maps.end(),
                                                          Q3ShaderData::Map());

            bool hasDepthWrite = false;

            while('}' != *(token = input.nextToken().toLower()))
            {
              if(*token == '\n')
                continue;

              if((1 /* XXX: Compression? */ && token == "mapcomp")
              || (0 /* XXX: Compression? */ && token == "mapnocomp")
              || (token == "map"))
              {
                map.textureNames.clear();
                map.textureNames.push_back(input.nextToken().replace('\\', '/'));
              }
              else if(token == "clampmap")
              {
                // XXX: clamp this!
                map.textureNames.clear();
                map.textureNames.push_back(input.nextToken().replace('\\', '/'));
              }
              else if((1 /* XXX: Compression? */ && token == "animmapcomp")
                   || (0 /* XXX: Compression? */ && token == "animmapnocomp")
                   || (token == "animmap"))
              {
                map.frequency = input.nextToken().toUInt();

                while("\n" != (token = input.nextToken()))
                  map.textureNames.insert(map.textureNames.begin(), token.replace('\\', '/'));
              }
              else if(token == "rgbgen")
              {
                token = input.nextToken().toLower();

                map.rgbGen
                  = (token == "wave")             ? Q3ShaderData::Map::Wave
                  : (token == "identitylighting") ? Q3ShaderData::Map::IdentityLighting
                  : (token == "entity")           ? Q3ShaderData::Map::Entity
                  : (token == "oneminusentity")   ? Q3ShaderData::Map::OneMinusEntity
                  : (token == "vertex")           ? Q3ShaderData::Map::Vertex
                  : (token == "exactvertex")      ? Q3ShaderData::Map::ExactVertex
                  : (token == "lightingdiffuse")  ? Q3ShaderData::Map::LightingDiffuse
                  : (token == "const")            ? Q3ShaderData::Map::Constant
                  : /* token == "identity" */       Q3ShaderData::Map::Identity;

                if(map.rgbGen == Q3ShaderData::Map::Wave)
                {
                  token = input.nextToken().toLower();

                  map.rgbGenWaveForm
                    = (token == "inversesawtooth") ? Shader::InverseSawtooth
                    : (token == "triangle") ?        Shader::Triangle
                    : (token == "square") ?          Shader::Square
                    : (token == "sawtooth") ?        Shader::Sawtooth
                    : /* token == "sin" */           Shader::Sin;

                  map.rgbGenWaveBase = input.nextToken().toFloat();
                  map.rgbGenWaveAmp = input.nextToken().toFloat();
                  map.rgbGenWavePhase = input.nextToken().toFloat();
                  map.rgbGenWaveFreq = input.nextToken().toFloat();
                }
                else if(map.rgbGen == Q3ShaderData::Map::Constant)
                {
                  token = input.nextToken(); // Skip '('

                  map.rgbGenColor.setRed(input.nextToken().toFloat());
                  map.rgbGenColor.setGreen(input.nextToken().toFloat());
                  map.rgbGenColor.setBlue(input.nextToken().toFloat());

                  token = input.nextToken(); // Skip ')'
                }
              }
              else if(token == "alphagen")
              {
                token = input.nextToken().toLower();

                map.alphaGen
                  = (token == "wave")             ? Q3ShaderData::Map::Wave
                  : (token == "identitylighting") ? Q3ShaderData::Map::IdentityLighting
                  : (token == "entity")           ? Q3ShaderData::Map::Entity
                  : (token == "oneminusentity")   ? Q3ShaderData::Map::OneMinusEntity
                  : (token == "vertex")           ? Q3ShaderData::Map::Vertex
                  : (token == "exactvertex")      ? Q3ShaderData::Map::ExactVertex
                  : (token == "lightingdiffuse")  ? Q3ShaderData::Map::LightingDiffuse
                  : /* token == "identity" */       Q3ShaderData::Map::Identity;

                if(map.alphaGen == Q3ShaderData::Map::Wave)
                {
                  token = input.nextToken().toLower();

                  map.alphaGenWaveForm
                    = (token == "inversesawtooth") ? Shader::InverseSawtooth
                    : (token == "triangle") ?        Shader::Triangle
                    : (token == "square") ?          Shader::Square
                    : (token == "sawtooth") ?        Shader::Sawtooth
                    : /* token == "sin" */           Shader::Sin;

                  map.alphaGenWaveBase = input.nextToken().toFloat();
                  map.alphaGenWaveAmp = input.nextToken().toFloat();
                  map.alphaGenWavePhase = input.nextToken().toFloat();
                  map.alphaGenWaveFreq = input.nextToken().toFloat();
                }
              }
              else if(token == "tcgen")
              {
                token = input.nextToken().toLower();

                map.tcGen
                  = (token == "environment") ? Q3ShaderData::Map::Environment
                  : (token == "lightmap")    ? Q3ShaderData::Map::Lightmap
                  : /* token == "base" */      Q3ShaderData::Map::Base;
              }
              else if(token == "tcmod")
              {
                token = input.nextToken().toLower();

                Q3ShaderData::Map::TCMod& tcMod = map.tcMods[map.tcModCount++];

                tcMod.type
                  = (token == "rotate") ?    Q3ShaderData::Map::TCMod::Rotate
                  : (token == "scale") ?     Q3ShaderData::Map::TCMod::Scale
                  : (token == "scroll") ?    Q3ShaderData::Map::TCMod::Scroll
                  : (token == "stretch") ?   Q3ShaderData::Map::TCMod::Stretch
                  : (token == "transform") ? Q3ShaderData::Map::TCMod::Transform
                  : /* token == "turb" */    Q3ShaderData::Map::TCMod::Turbulence;

                switch(tcMod.type)
                {
                case Q3ShaderData::Map::TCMod::Rotate:

                  tcMod.freq = input.nextToken().toFloat();

                  break;

                case Q3ShaderData::Map::TCMod::Scale:
                case Q3ShaderData::Map::TCMod::Scroll:

                  tcMod.s = input.nextToken().toFloat();
                  tcMod.t = input.nextToken().toFloat();

                  break;

                case Q3ShaderData::Map::TCMod::Stretch:

                  // XXX

                  break;

                case Q3ShaderData::Map::TCMod::Transform:

                  // XXX

                  break;

                case Q3ShaderData::Map::TCMod::Turbulence:

                  // XXX

                  break;
                }
              }
              else if(token == "depthwrite")
              {
                map.depthWrite = true;
                hasDepthWrite = true;
              }
              else if(token == "blendfunc")
              {
                String sourceBlend = input.nextToken().toUpper();
                String destBlend = input.nextToken().toUpper();

                if(sourceBlend == "ADD" || sourceBlend == "GL_ADD")
                {
                  map.sourceBlend = Renderer::Factor_One;
                  map.destBlend = Renderer::Factor_One;
                }
                else if(sourceBlend == "FILTER")
                {
                  map.sourceBlend = Renderer::Factor_DstColor;
                  map.destBlend = Renderer::Factor_Zero;
                }
                else if(sourceBlend == "BLEND")
                {
                  map.sourceBlend = Renderer::Factor_SrcAlpha;
                  map.destBlend = Renderer::Factor_OneMinusSrcAlpha;
                }
                else
                {
                  if(sourceBlend == "GL_ZERO")
                    map.sourceBlend = Renderer::Factor_Zero;
                  else if(sourceBlend == "GL_ONE")
                    map.sourceBlend = Renderer::Factor_One;
                  else if(sourceBlend == "GL_DST_COLOR")
                    map.sourceBlend = Renderer::Factor_DstColor;
                  else if(sourceBlend == "GL_ONE_MINUS_DST_COLOR")
                    map.sourceBlend = Renderer::Factor_OneMinusDstColor;
                  else if(sourceBlend == "GL_SRC_ALPHA")
                    map.sourceBlend = Renderer::Factor_SrcAlpha;
                  else if(sourceBlend == "GL_ONE_MINUS_SRC_ALPHA")
                    map.sourceBlend = Renderer::Factor_OneMinusSrcAlpha;
                  else if(sourceBlend == "GL_DST_ALPHA")
                    map.sourceBlend = Renderer::Factor_DstAlpha;
                  else if(sourceBlend == "GL_ONE_MINUS_DST_ALPHA")
                    map.sourceBlend = Renderer::Factor_OneMinusDstAlpha;
                  else if(sourceBlend == "GL_SRC_ALPHA_SATURATE")
                    map.sourceBlend = Renderer::Factor_SrcAlphaSaturate;
                  else
                  {
                    esWarning << "Invalid source blend value \"" << sourceBlend
                              << "\"." << std::endl;
                  }

                  if(destBlend == "GL_ZERO")
                    map.destBlend = Renderer::Factor_Zero;
                  else if(destBlend == "GL_ONE")
                    map.destBlend = Renderer::Factor_One;
                  else if(destBlend == "GL_SRC_COLOR")
                    map.destBlend = Renderer::Factor_SrcColor;
                  else if(destBlend == "GL_ONE_MINUS_SRC_COLOR")
                    map.destBlend = Renderer::Factor_OneMinusSrcColor;
                  else if(destBlend == "GL_SRC_ALPHA")
                    map.destBlend = Renderer::Factor_SrcAlpha;
                  else if(destBlend == "GL_ONE_MINUS_SRC_ALPHA")
                    map.destBlend = Renderer::Factor_OneMinusSrcAlpha;
                  else if(destBlend == "GL_DST_ALPHA")
                    map.destBlend = Renderer::Factor_DstAlpha;
                  else if(destBlend == "GL_ONE_MINUS_DST_ALPHA")
                    map.destBlend = Renderer::Factor_OneMinusDstAlpha;
                  else
                  {
                    esWarning << "Invalid destination blend value \""
                              << destBlend << "\"." << std::endl;
                  }
                }

                if(shader->maps.size() == 1) // This is the first map
                {
                  if(map.destBlend != Renderer::Factor_Zero
                  || map.sourceBlend == Renderer::Factor_DstColor
                  || map.sourceBlend == Renderer::Factor_OneMinusDstColor
                  || map.sourceBlend == Renderer::Factor_OneMinusDstAlpha)
                  {
                    if(!hasSort)
                      shader->_sort = Shader::Additive;

                    if(!hasDepthWrite)
                      map.depthWrite = false;
                  }
                }
              }
              else if(token == "alphafunc")
              {
                token = input.nextToken().toUpper();

                if(token == "GT0")
                  map.alphaFunc = Renderer::Alpha_GT0;
                else if(token == "LT128")
                  map.alphaFunc = Renderer::Alpha_LT128;
                else if(token == "GE128")
                  map.alphaFunc = Renderer::Alpha_GE128;
                else
                {
                  esWarning << "Invalid alpha function \"" << token
                            << "\"." << std::endl;
                }
              }
            }
          }
        }
      }
      else
      {
        shaderName = token.replace('\\', '/').toLower();
        shader = new Q3ShaderData;
        shaders[shaderName] = shader;
      }
    }
  }
}

uint32_t Q3Shader::id()
{
  return 0x51334153; // 'Q3AS'
}

bool Q3Shader::canHandle(File&)
{
  return false;
}

Shader* Q3Shader::acquire(const char* name, bool mipmaps)
{
  std::map<String, Q3ShaderData*>::iterator shader
    = shaders.find(String(name).toLower());

  if(shader == shaders.end())
    return 0;

  if(!mipmaps)
    shader->second->nomipmaps = true;

  shader->second->acquire();

  ++shader->second->refCount;

  return shader->second;
}

void Q3ShaderData::acquire()
{
  std::vector<Map>::iterator map;

  for(map = maps.begin(); map != maps.end(); ++map)
  {
    if(map->textureNames.size() && !map->textureHandles)
    {
      map->textureHandles = new uint[map->textureNames.size()];
      map->frameCount = map->textureNames.size();

      bool set = true; // XXX: Set to false if texture memory is low

      for(uint i = 0; i < map->textureNames.size(); ++i)
      {
        if(map->textureNames[i] == "*white"
        || map->textureNames[i] == "$whiteimage")
        {
          map->textureHandles[i] = 0;
        }
        else
        {
          map->textureHandles[i] = Texture::acquire(map->textureNames[i],
                                                    nomipmaps ? Texture::NoMipMaps : 0);
        }

        if(map->textureHandles[i] >= 0x1000000 || !map->textureHandles[i])
          continue;

        if(!set)
        {
          _sort = (_sort & 0xFF000000) | map->textureHandles[i];

          set = true;
        }
      }
    }
  }
}

void Q3ShaderData::unacquire()
{
  std::vector<Q3ShaderData::Map>::iterator map;

  for(map = maps.begin(); map != maps.end(); ++map)
  {
    if(map->textureHandles)
    {
      for(uint i = 0; i < map->textureNames.size(); ++i)
        if(map->textureHandles[i])
          Texture::unacquire(map->textureHandles[i]);

      delete [] map->textureHandles;

      map->textureHandles = 0;
    }
  }
}

Q3ShaderData::Q3ShaderData()
  : _cullFace(Renderer::Face_Front),
    nopicmip(false),
    nomipmaps(false),
    polygonOffset(false),
    _sort(Opaque),
    sky(false),
    flesh(false),
    lava(false),
    metalsteps(false),
    nodamage(false),
    nodlight(false),
    nodraw(false),
    noimpact(false),
    nomarks(false),
    nolightmap(false),
    nosteps(false),
    origin(false),
    playerclip(false),
    slick(false),
    slime(false),
    water(false)
{
  refCount = 0;
}

Q3ShaderData::Map::Map()
  : textureHandles(0),
    frameCount(0),
    sourceBlend(Renderer::Factor_One),
    destBlend(Renderer::Factor_Zero),
    rgbGen(Identity),
    alphaGen(Identity),
    tcGen(Base),
    tcModCount(0),
    depthWrite(true),
    alphaFunc(Renderer::Alpha_All)
{
}

uint Q3ShaderData::passCount() const
{
  return maps.size();
}

void Q3ShaderData::pushState(uint pass)
{
  Renderer::setCullFace(_cullFace);
  Renderer::setPolygonOffset(polygonOffset);

  if(maps.empty())
  {
    Renderer::setTexture(0);
  }
  else // maps.size() >= 1
  {
    const Map& map = maps[pass];

    if(map.rgbGen == Map::Identity
    || map.rgbGen == Map::IdentityLighting)
    {
      Renderer::setTexEnvMode(Renderer::EnvMode_Replace);
    }
    else // map.rgbGen != Map::Identity
    {
      Renderer::setTexEnvMode(Renderer::EnvMode_Modulate);
    }

    if(map.rgbGen == Map::Vertex)
    {
      Renderer::setColors(Renderer::Source_Array0,
                          map.alphaGen == Map::Vertex);
    }
    else // map.rgbGen != Map::Vertex
    {
      Renderer::setColors(Renderer::Source_Constant);

      if(map.rgbGen == Map::Constant)
      {
        GL::color3ubv(map.rgbGenColor.data());
      }
      else if(map.rgbGen == Map::Entity)
      {
        if(map.alphaGen == Map::Entity)
        {
          GL::color4ubv(Shader::st_entityColor.data());
        }
        else // map.alphaGen != Map::Entity
        {
          GL::color3ubv(Shader::st_entityColor.data());
        }
      }
      else if(map.rgbGen == Map::Wave)
      {
        float value = wave(map.rgbGenWaveForm, map.rgbGenWaveBase,
                           map.rgbGenWaveAmp, map.rgbGenWavePhase,
                           map.rgbGenWaveFreq, time);

        GL::color3f(value, value, value);
      }
      else if(map.rgbGen == Map::LightingDiffuse)
      {
        GL::enableClientState(GL::NORMAL_ARRAY);
        GL::enable(GL::LIGHTING);
        GL::enable(GL::COLOR_MATERIAL);
        GL::enable(GL::NORMALIZE);

        lighting = true;

        GL::color3f(1, 1, 1);
      }
    }

    Renderer::setAlphaFunc(map.alphaFunc);
    Renderer::setBlendFunc(map.sourceBlend, map.destBlend);

    if(map.textureHandles[0] == Texture::lightmap)
    {
      Renderer::setTexCoords(Renderer::Source_Array1);
    }
    else // map.textureHandles[0] != Texture::lightmap
    {
      Renderer::setTexCoords(Renderer::Source_Array0);

      uint frame = (map.frameCount > 1)
                 ? (static_cast<uint>(time * map.frequency) % map.frameCount)
                 : 0;

      Renderer::setTexture(map.textureHandles[frame]);
    }

    if(map.tcModCount)
    {
      st_tcMod = true;

      GL::matrixMode(GL::TEXTURE);
      GL::pushMatrix();

      for(uint i = 0; i < map.tcModCount; ++i)
      {
        switch(map.tcMods[i].type)
        {
        case Map::TCMod::Rotate:

          GL::translatef(0.5, 0.5, 0);
          GL::rotatef(map.tcMods[i].freq / 180.0 * M_PI * time, 0, 0, 1);
          GL::translatef(-0.5, -0.5, 0);

          break;

        case Map::TCMod::Scale:

          GL::scalef(map.tcMods[i].s, map.tcMods[i].t, 1.0);

          break;

        case Map::TCMod::Scroll:

          GL::translatef(map.tcMods[i].s * time, map.tcMods[i].t * time, 0);

          break;

        case Map::TCMod::Stretch:

          break;

        case Map::TCMod::Transform:

          break;

        case Map::TCMod::Turbulence:

          break;
        }
      }
    }

    Renderer::setDepthMask(map.depthWrite);

    if(map.tcGen == Map::Environment)
    {
      GL::enable(GL::TEXTURE_GEN_S);
      GL::enable(GL::TEXTURE_GEN_T);
      GL::texGeni(GL::S, GL::TEXTURE_GEN_MODE, GL::SPHERE_MAP);
      GL::texGeni(GL::T, GL::TEXTURE_GEN_MODE, GL::SPHERE_MAP);
      Renderer::setNormals(Renderer::Source_Array0);
      GL::enableClientState(GL::NORMAL_ARRAY);

      tcGen = true;
    }
  }
}

void Q3ShaderData::popState()
{
  if(lighting)
  {
    Renderer::setNormals(Renderer::Source_Constant);
    GL::disableClientState(GL::NORMAL_ARRAY);
    GL::disable(GL::LIGHTING);
    GL::disable(GL::NORMALIZE);

    lighting = false;
  }

  if(tcGen)
  {
    GL::disableClientState(GL::NORMAL_ARRAY);
    GL::disable(GL::TEXTURE_GEN_S);
    GL::disable(GL::TEXTURE_GEN_T);

    tcGen = false;
  }

  if(st_tcMod)
  {
    GL::popMatrix();

    st_tcMod = false;
  }
}

bool Q3ShaderData::isSky() const
{
  return sky;
}

uint Q3ShaderData::sort() const
{
  return _sort;
}

Renderer::Face Q3ShaderData::cullFace() const
{
  return _cullFace;
}

// vim: ts=2 sw=2 et
