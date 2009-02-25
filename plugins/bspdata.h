#ifndef PLUGINS_BSPDATA_H_
#define PLUGINS_BSPDATA_H_ 1

#include <vector>
#include <stdint.h>
#include <stdio.h>

#include <espace/collision.h>
#include <espace/color.h>
#include <espace/map.h>
#include <espace/model.h>
#include <espace/types.h>
#include <espace/vector.h>

class Camera;
class Image;
class Shader;
class File;

class BSPData : public Map
{
public:

  BSPData();

  void render();

  uint    entityCount() const;
  Entity& entity(uint index);

  uint   modelCount() const;
  Model* model(uint index);

  void rayTrace(const Vector3& start, const Vector3& end,
                Trace& trace, int contentMask) const;
  void sphereTrace(const Vector3& start, const Vector3& end,
                   float radius, Trace& trace, int contentMask) const;
  void boxTrace(const Vector3& start, const Vector3& end,
                const Vector3& min, const Vector3& max,
                Trace& trace, int contentMask) const;
  void capsuleTrace(const Vector3& start, const Vector3& end,
                    const Vector3& min, const Vector3& max,
                    Trace& trace, int contentMask) const;
  void contents(const Vector3& min, const Vector3& max,
                Trace& trace, int contentMask) const;
  bool visible(const Vector3& from, const Vector3& to) const;

  enum Lump
  {
    Entities     = 0,
    Textures     = 1,
    Planes       = 2,
    Nodes        = 3,
    Leaves       = 4,
    LeafFaces    = 5,
    LeafBrushes  = 6,
    Models       = 7,
    Brushes      = 8,
    BrushSides   = 9,
    Vertices     = 10,
    MeshVertices = 11,
    Effects      = 12,
    Faces        = 13,
    Lightmaps    = 14,
    LightVolumes = 15,
    VisData      = 16,
    LumpCount    = 17
  };

  void readLump(File& file, uint offset, uint length, Lump lump);

  class Texture
  {
  public:

    char    name[64];
    int     flags;
    int     content;

    int     handle;
  };

  class Plane : public Vector3
  {
  public:

    float   distance;
  };

  class Node
  {
  public:

    int     plane;
    int     children[2];
    int     mins[3];
    int     maxs[3];
  };

  class Leaf
  {
  public:

    int  area;
    int  leafFace;
    int  leafBrush;
    uint leafBrushCount;
  };

  class RenderLeaf
  {
  public:

    int   cluster;
    uint  faceCount;
    int   mins[3];
    int   maxs[3];

    uint* faces;
  };

  class RenderNode
  {
  public:

    enum Flags
    {
      PlaneX = 1,
      PlaneY = 2,
      PlaneZ = 3,
      PlaneMX = 4,
      PlaneMY = 8,
      PlaneMZ = 12,
      ChildLeaf0 = 16,
      ChildLeaf1 = 32
    };

    Plane   plane;
    uint    flags;

    union
    {
      RenderNode* child0;
      RenderLeaf* childLeaf0;
    };

    union
    {
      RenderNode* child1;
      RenderLeaf* childLeaf1;
    };

    int mins[3];
    int maxs[3];
  };

  class InlineModel : public Model
  {
  public:

    void boundBox(Vector3& min, Vector3& max);
    void render(int frame, float backLerp, uint customShader, uint customSkin);

  protected:

    friend class BSPData;

    Vector3 min;
    Vector3 max;
    int     face;
    uint    faceCount;
    int     brush;
    uint    brushCount;

    BSPData* owner;
  };

  class Brush
  {
  public:

    int     brushSide;
    uint    brushSideCount;
    int     texture;
  };

  class BrushSide
  {
  public:

    int     plane;
    int     texture;
  };

  class Vertex : public Vector3
  {
  public:

    Vector2 textureCoord;
    Vector2 lightmapCoord;
    Vector3 normal;
    Color   color;

    Vertex() { }

    Vertex(float x, float y, float z) : Vector3(x, y, z) { }

    Vertex operator*(float f)
    {
      Vertex v((*this)(0) * f, (*this)(1) * f, (*this)(2) * f);

      v.textureCoord = textureCoord * f;
      v.lightmapCoord = lightmapCoord * f;
      v.normal = normal * f;
      v.color = color;

      return v;
    }

    Vertex& operator+=(const Vertex& v)
    {
      (*this)(0) += v(0);
      (*this)(1) += v(1);
      (*this)(2) += v(2);
      textureCoord += v.textureCoord;
      lightmapCoord += v.lightmapCoord;
      normal += v.normal;

      return *this;
    }
  };

  class Effect
  {
  public:

    char    name[64];
    int     brush;
    int     unknown;
  };

  class Face
  {
  public:


    enum Type
    {
      Polygon = 1,
      Patch = 2,
      Mesh = 3,
      Billboard = 4
    };

    int      effect;
    uint     type;
    int      vertex;
    uint     vertexCount;
    int      meshVertex;
    int      lightmapStart[2];
    int      lightmapSize[2];
    Vector3  origin;
    Vector3  lightmapUnits[2];
    uint     patchWidth;
    uint     patchHeight;
  };

  class RenderFace
  {
  public:

    enum Flags
    {
      PlaneX = 1,
      PlaneY = 2,
      PlaneZ = 3,
      PlaneMX = 4,
      PlaneMY = 8,
      PlaneMZ = 12,
      HasShader = 16
    };

    uint flags;

    union
    {
      int     texture;
      Shader* shader;
    };

    uint     meshVertexCount;
    int      lightmap;
    Vector3  normal;

    Vector3  center;
    uint*    meshVertices;
  };

  class LightVolume
  {
  public:

    uint8_t ambient[3];
    uint8_t directional[3];
    uint8_t direction[2];
  };

  class Visibility
  {
  public:

    uint     vectorCount;
    uint     vectorSize;
    uint8_t* data;
  };

  // *** Data loaded directly from file

  std::vector<Entity>      entities;
  std::vector<Texture>     textures;
  std::vector<Plane>       planes;
  std::vector<Node>        nodes;
  std::vector<Leaf>        leaves;
  std::vector<RenderLeaf>  rleaves;
  std::vector<uint>        leafFaces;
  std::vector<int>         leafBrushes;
  std::vector<InlineModel> models;
  std::vector<Brush>       brushes;
  std::vector<BrushSide>   brushSides;
  std::vector<Vertex>      vertices;
  std::vector<uint>        meshVertices;
  std::vector<Effect>      effects;
  std::vector<Face>        faces;
  std::vector<RenderFace>  rfaces;
  std::vector<Image*>      lightmaps;
  std::vector<uint>        lightmapHandles;
  std::vector<LightVolume> lightVolumes;
  Visibility               visibility;

  std::vector<RenderNode>  rnodes;
  // *** Data created after loading

public:

  void rayTrace(int nodeIndex, const Vector3& start, const Vector3& dir,
                Trace& trace) const;
  void sphereTrace(int nodeIndex, const Vector3& start, const Vector3& dir,
                   float radius, Trace& trace) const;
  void boxTrace(int nodeIndex, const Vector3& start, const Vector3& dir,
                const Vector3& min, const Vector3& max,
                Trace& trace, float t1 = 0, float t2 = 1) const;
  void capsuleTrace(int nodeIndex, const Vector3& start, const Vector3& dir,
                    const Vector3& min, const Vector3& max,
                    Trace& trace, float t1 = 0, float t2 = 1) const;
  void contents(int nodeIndex, const Vector3& min, const Vector3& max,
                Trace& trace) const;

  // *** Run time functions and data

  Vector3        cameraPosition;
  Vector3        cameraDirection;
  float          cameraDistance;
  Plane          frustum[4];
  float          backFace;
  int            currentCluster;
  bool*          faceMarks;
  bool*          brushMarks;
  void           addNode(const RenderNode&);
  void           addLeaf(const RenderLeaf&);
  uint           findLeaf(const Vector3& position) const;

  inline char testVisibility(uint from, uint to) const
  {
    return visibility.data[from * visibility.vectorSize + (to / 8)] & (1 << (to % 8));
  }

protected:

  ~BSPData();
};

#endif // !PLUGINS_BSPDATA_H_

// vim: ts=2 sw=2
