#ifndef RENDERER_H_
#define RENDERER_H_

#ifndef SWIG
#include "color.h"
#include "map.h"
#include "matrix.h"
#include "pixmap.h"
#include "skin.h"
#include "types.h"
#include "vector.h"
#endif

struct Font;
struct Shader;

/**
 * OpenGL fog description.
 */
struct GLFog
{
  GLFog()
    : mode(0)
  {
  }

  int   mode;           // GL_LINEAR or GL_EXP
  int   hint;           // GL_DONT_CARE
  int   startTime;
  int   finishTime;
  Color color;
  float near;
  float far;
  bool  useFarForClip;
  float density;        // [0, 1]
  bool  registered;
  bool  drawSky;
  bool  clearScreen;
};

/**
 * Rendering parameters, such as viewport and orientation.
 */
struct RefDef
{
  RefDef()
    : x(0),
      y(0),
      fovX(120),
      fovY(90),
      rdflags(0)
  {
    axis.identity();
  }

  /**
   * Set the viewport in window coordinates.
   *
   * \param x X value of left side of viewport.
   * \param y Y value of top side of viewport.
   * \param width Width of viewport.
   * \param height Height of viewport.
   */
  IMPORT void setViewport(int x, int y, int width, int height);

  /**
   * Set camera orientation.
   */
  IMPORT void setOrientation(float pitch, float yaw, float roll);

  enum Flag
  {
    NoWorldModel = 1,
    HyperSpace = 4,
    SkyBoxPortal = 8,
    UnderWater = 16,
    DrawingSky = 32,
    SnooperView = 64
  };

  int       x, y;
  int       width, height;
  float     fovX, fovY;
  Vector3   origin;
  Matrix3x3 axis; // Transformation matrix

  int       time;
  int       rdflags;

  uint8_t   areaMask[32];

  char      text[8][32];

  GLFog     glFog;
};

/**
 * Entity rendering parameters.
 */
struct RefEntity
{
  enum Type
  {
    Model,
    Poly,
    Sprite,
    Splash,
    Beam,
    RailCore,
    RailCoreTaper,
    RailRings,
    Lightning,
    PortalSurface
  };

  enum RenderFX
  {
    MinLight = 1,
    ThirdPerson = 2,
    FirstPerson = 4,
    DepthHack = 8,
    NoShadow = 64,
    LightingOrigin = 128,
    ShadowPlane = 256,
    WrapFrames = 512
  };

  Type type;
  int  renderfx;

  uint modelHandle;

  Vector3 lightningOrigin;
  float   shadowPlane;

  Matrix3x3 axis;
  Matrix3x3 torsoAxis;
  int       nonNormalizedAxes;
  Vector3   origin;
  int       frame;
  int       torsoFrame;

  Vector3   oldOrigin;
  int       oldFrame;
  int       oldTorsoFrame;
  float     backlerp;
  float     torsoBacklerp;

  int skinNum;
  int customSkin;
  int customShader;

  Color   color;
  Vector2 texCoord;
  float   time;

  float radius;
  float rotation;

  Vector3 fireRiseDir;

  int fadeStartTime;
  int fadeEndTime;

  float highlightIntensity;

  int reFlags;

  int entityNum;
};

/**
 * Namespace for rendering functions.
 *
 * \author Morten Hustveit
 */
struct Renderer
{
  /**
   * Initialize rendering library.
   */
  static IMPORT void initialize();

  /**
   * Clear color and depth buffer.
   *
   * \todo This functions does not make sense.
   */
  static IMPORT void clearScreen();

  /**
   * Update the screen with the current contents of the rendering buffer.
   */
  static IMPORT void updateScreen();

  /**
   * Set the entity color.
   */
  static IMPORT void setColor(const Color&);

  /**
   * Set the texture for the specified level.
   */
  static IMPORT void setTexture(uint texture, uint level = 0);

  enum Face
  {
    Face_Front,
    Face_Back,
    Face_Both,
    Face_None
  };

  /**
   * Specify which side of faces is to be culled.
   */
  static IMPORT void setCullFace(Face face);

  enum AlphaFunc
  {
    Alpha_GT0,
    Alpha_LT128,
    Alpha_GE128,
    Alpha_All
  };

  static IMPORT void setAlphaFunc(AlphaFunc func);

  enum Factor
  {
    Factor_Zero = 0,
    Factor_One = 1,
    Factor_DstColor = 0x0306,
    Factor_SrcColor = 0x0300,
    Factor_DstAlpha = 0x0304,
    Factor_SrcAlpha = 0x0302,
    Factor_OneMinusDstColor = 0x0307,
    Factor_OneMinusSrcColor = 0x0301,
    Factor_OneMinusDstAlpha = 0x0305,
    Factor_OneMinusSrcAlpha = 0x0303,
    Factor_SrcAlphaSaturate = 0x0308
  };

  static IMPORT void setBlendFunc(Factor source, Factor dest);

  static IMPORT void setDepthMask(bool mask);

  static IMPORT void setPolygonOffset(bool offset);

  enum EnvMode
  {
    EnvMode_Modulate,
    EnvMode_Decal,
    EnvMode_Blend,
    EnvMode_Replace
  };

  static IMPORT void setTexEnvMode(EnvMode mode);

  /**
   * Enter 2D rendering mode.
   *
   * This function maps space coordinates to window coordinates and disables
   * the depth buffer.  Useful for drawing graphical user interfaces.
   */
  static IMPORT void set2DMode();

  /**
   * Undo a call to set2DMode().
   */
  static IMPORT void set3DMode();

  // Synchronous rendering

  /**
   * Draw a 3D line.
   */
  static IMPORT void drawLine3D(const Vector3& start, const Vector3& end,
                                Shader*);

  /**
   * Draw a 2D rectangle with the specified texture coordinates.
   */
  static IMPORT void drawQuad2D(float x, float y, float width, float height,
                                float s1, float t1, float s2, float t2,
                                Shader*);

  /**
   * Draw a rotated 2D rectangle with the specified texture coordinates.
   */
  static IMPORT void drawQuad2D(float x, float y, float width, float height,
                                float s1, float t1, float s2, float t2,
                                Shader*, float angle);

  /**
   * Draw a pixmap.
   */
  static IMPORT void drawQuad2D(float x, float y, const Pixmap*);

  /**
   * Draw a 2D rectangle.
   */
  static void drawQuad2D(float x, float y, float width, float height,
                         Shader* shader)
  {
    drawQuad2D(x, y, width, height, 0, 0, 1, 1, shader);
  }

  /**
   * Draw a 2D rectangle.
   */
  static void drawQuad2D(Vector2 upperLeft, Vector2 lowerRight, Shader* shader)
  {
    drawQuad2D(upperLeft(0), upperLeft(1),
               lowerRight(0) - upperLeft(0), lowerRight(1) - upperLeft(1),
               0, 0, 1, 1, shader);
  }

  /**
   * Draw a pixmap.
   */
  static void drawQuad2D(Vector2 upperLeft, const Pixmap* pixmap)
  {
    drawQuad2D(upperLeft(0), upperLeft(1), pixmap);
  }

  /**
   * Draw a rotated 2D rectangle.
   */
  static void drawQuad2D(float x, float y, float width, float height,
                         Shader* shader, float angle)
  {
    drawQuad2D(x, y, width, height, 0, 0, 1, 1, shader, angle);
  }


  /**
   * Draw a 3D quadrilateral with the specified texture coordinates.
   */
  static IMPORT void drawQuad3D(const Vector3& v1, const Vector3& v2,
                                const Vector3& v3, const Vector3& v4,
                                const Vector2& t1, const Vector2& t2,
                                const Vector2& t3, const Vector2& t4,
                                Shader*);

  /**
   * Draw a triangle list using the current render state.
   */
  static IMPORT void drawTriangles(uint triangleCount, const uint* indexes);

  /**
   * Draw a triangle list using the specified shader.
   */
  static IMPORT void drawTriangles(uint triangleCount, const uint* indexes,
                                   Shader*);

  /**
   * Draw a triangle list using the specified skin and surface name.
   */
  static IMPORT void drawTriangles(uint triangleCount, const uint* indexes,
                                   Skin*, const char* surfaceName);

  // Matrix functions

  /**
   * Set the current projection matrix.
   */
  static IMPORT void setProjectionMatrix(const Matrix4x4&);

  /**
   * Set the current view matrix.
   */
  static IMPORT void setViewMatrix(const Matrix4x4&);

  /**
   * Get the current projection matrix.
   */
  static IMPORT Matrix4x4 projectionMatrix();

  /**
   * Get the current view matrix.
   */
  static IMPORT Matrix4x4 viewMatrix();

  // Font fuctions

  enum
  {
    Left = 0,
    Right = 1,
    Center = 2
  };

  /**
   * Draw a string using the specified font.
   *
   * \param x X coordinate of string.
   * \param y Y coordinate of string baseline.
   * \param font Font to use for drawing.
   * \param text Text to draw.
   * \param align Alignment of text relative to the X coordinate.
   */
  static IMPORT void put(float x, float y, const Font* font,
                         const char* text, int align = Left);

  // Vertex arrays

  static IMPORT void setVertexArray(const void* vertices, uint stride = 0);

  static IMPORT void setColorArray(const void* colors, uint stride = 0);

  static IMPORT void setTexCoordArray(uint index, const void* coords,
                                      uint stride = 0);

  static IMPORT void setNormalArray(const void* normals, uint stride = 0);

  enum Source
  {
    Source_Constant,
    Source_Array0,
    Source_Array1,
    Source_Array2,
    Source_Array3
  };

  static IMPORT void setColors(Source source, bool alpha = false);

  static IMPORT void setTexCoords(Source source, uint level = 0);

  static IMPORT void setNormals(Source source);

  // Buffered rendering

  /**
   * Adds a list of triangles to the render list.
   *
   * This function is for use with triangles that use simple texture+lightmap
   * pairs instead of shaders.
   *
   * \see flush()
   */
  static IMPORT void addTriangles(uint triangleCount, const uint* indexes,
                                  int texture, int lightmap = -1);

  /**
   * Adds a list of triangles to the render list.
   *
   * This function is for use with triangles that use shaders.
   *
   * \see flush()
   */
  static IMPORT void addTriangles(uint triangleCount, const uint* indexes,
                                  Shader*, int lightmap = -1,
                                  int sort = -(1 << 30));

  /**
   * Renders all primitives currently in the render list.
   */
  static IMPORT void flush();

  // Scene based rendering

  /**
   * Adds an entity to the start of the render list.
   */
  static IMPORT void prependRefEntity(const RefEntity&);

  /**
   * Adds an entity to the end of the render list.
   */
  static IMPORT void appendRefEntity(const RefEntity&);

  /**
   * Adds a light to the scene.
   */
  static IMPORT void addLight(const Vector3& origin, float intensity,
                              const Color&);

  /**
   * Adds a corona to the scene.
   */
  static IMPORT void addCorona(const Vector3& direction, const Color&);

  /**
   * Removes all objects from the current scene.
   */
  static IMPORT void clearScene();

  /**
   * Adds the current scene status to the scene stack.
   *
   * There is not limit to the stack size, so you may run out of memory if you
   * don't balance your push/pop calls.
   *
   * \see popScene()
   */
  static IMPORT void pushScene();

  /**
   * Replaces the current scene with the one last pushed to the scene stack.
   *
   * \see pushScene()
   */
  static IMPORT void popScene();

  /**
   * Renders the current scene.
   *
   * \param refdef Rendering parameters to use
   * \param worldModel Currently active world model, or NULL if there is no
   *                   such model.
   */
  static IMPORT void renderScene(const RefDef& refdef, Map* worldModel = 0);
};

#endif // !RENDERER_H_

// vim: ts=2 sw=2 et
