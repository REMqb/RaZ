#include "RaZ/Math/Constants.hpp"
#include "RaZ/Render/Mesh.hpp"

#include <unordered_map>

namespace Raz {

Mesh::Mesh(const Plane& plane, float width, float depth, RenderMode renderMode) : Mesh() {
  const float height = (plane.getNormal() * plane.getDistance())[1];

  const Vec3f firstPos(-width, height, depth);
  const Vec3f secondPos(width, height, depth);
  const Vec3f thirdPos(width, height, -depth);
  const Vec3f fourthPos(-width, height, -depth);

  Vertex firstCorner {};
  firstCorner.position  = firstPos;
  firstCorner.normal    = plane.getNormal();
  firstCorner.texcoords = Vec2f(0.f, 0.f);

  Vertex secondCorner {};
  secondCorner.position  = secondPos;
  secondCorner.normal    = plane.getNormal();
  secondCorner.texcoords = Vec2f(1.f, 0.f);

  Vertex thirdCorner {};
  thirdCorner.position  = thirdPos;
  thirdCorner.normal    = plane.getNormal();
  thirdCorner.texcoords = Vec2f(1.f, 1.f);

  Vertex fourthCorner {};
  fourthCorner.position  = fourthPos;
  fourthCorner.normal    = plane.getNormal();
  fourthCorner.texcoords = Vec2f(0.f, 1.f);

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.resize(4);

  vertices[0] = firstCorner;
  vertices[1] = secondCorner;
  vertices[2] = thirdCorner;
  vertices[3] = fourthCorner;

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.resize(6);

  indices[0] = 1;
  indices[1] = 0;
  indices[2] = 2;

  indices[3] = 2;
  indices[4] = 0;
  indices[5] = 3;

  setRenderMode(renderMode);
  load();
}

Mesh::Mesh(const Sphere& sphere, uint32_t subdivCount, SphereMeshType type, RenderMode renderMode) : Mesh() {
  switch (type) {
    case SphereMeshType::UV:
      createUvSphere(sphere, subdivCount, subdivCount);
      break;

    case SphereMeshType::ICO:
      createIcosphere(sphere, subdivCount);
      break;
  }

  setRenderMode(renderMode);
  load();
}

Mesh::Mesh(const Triangle& triangle, RenderMode renderMode) : Mesh() {
  const Vec3f& firstPos  = triangle.getFirstPos();
  const Vec3f& secondPos = triangle.getSecondPos();
  const Vec3f& thirdPos  = triangle.getThirdPos();
  const Vec3f normal     = triangle.computeNormal();

  Vertex firstVert {};
  firstVert.position  = firstPos;
  firstVert.texcoords = Vec2f(0.f, 0.f);
  firstVert.normal    = normal;

  Vertex secondVert {};
  secondVert.position  = secondPos;
  secondVert.texcoords = Vec2f(0.5f, 1.f);
  secondVert.normal   = normal;

  Vertex thirdVert {};
  thirdVert.position  = thirdPos;
  thirdVert.texcoords = Vec2f(1.f, 0.f);
  thirdVert.normal    = normal;

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.resize(3);

  vertices[0] = firstVert;
  vertices[1] = secondVert;
  vertices[2] = thirdVert;

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.resize(3);

  indices[0] = 1;
  indices[1] = 0;
  indices[2] = 2;

  setRenderMode(renderMode);
  load();
}

Mesh::Mesh(const Quad& quad, RenderMode renderMode) : Mesh() {
  const Vec3f& leftTopPos     = quad.getLeftTopPos();
  const Vec3f& rightTopPos    = quad.getRightTopPos();
  const Vec3f& rightBottomPos = quad.getRightBottomPos();
  const Vec3f& leftBottomPos  = quad.getLeftBottomPos();

  Vertex leftTop {};
  leftTop.position  = leftTopPos;
  leftTop.texcoords = Vec2f(0.f, 1.f);

  Vertex rightTop {};
  rightTop.position  = rightTopPos;
  rightTop.texcoords = Vec2f(1.f, 1.f);

  Vertex rightBottom {};
  rightBottom.position  = rightBottomPos;
  rightBottom.texcoords = Vec2f(1.f, 0.f);

  Vertex leftBottom {};
  leftBottom.position  = leftBottomPos;
  leftBottom.texcoords = Vec2f(0.f, 0.f);

  // Computing normals
  // TODO: normals should not be computed (or even exist) for simple display quads like a framebuffer
  leftTop.normal     = (leftTopPos - rightTopPos).cross(leftTopPos - leftBottomPos).normalize();
  rightTop.normal    = (rightTopPos - rightBottomPos).cross(rightTopPos - leftTopPos).normalize();
  rightBottom.normal = (rightBottomPos - leftBottomPos).cross(rightBottomPos - rightTopPos).normalize();
  leftBottom.normal  = (leftBottomPos - leftTopPos).cross(leftBottomPos - rightBottomPos).normalize();

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.resize(4);

  vertices[0] = leftTop;
  vertices[1] = leftBottom;
  vertices[2] = rightBottom;
  vertices[3] = rightTop;

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.resize(6);

  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;

  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  setRenderMode(renderMode);
  load();
}

Mesh::Mesh(const AABB& box, RenderMode renderMode) : Mesh() {
  const Vec3f& rightTopFrontPos  = box.getRightTopFrontPos();
  const Vec3f& leftBottomBackPos = box.getLeftBottomBackPos();

  const float rightPos  = rightTopFrontPos[0];
  const float leftPos   = leftBottomBackPos[0];
  const float topPos    = rightTopFrontPos[1];
  const float bottomPos = leftBottomBackPos[1];
  const float frontPos  = rightTopFrontPos[2];
  const float backPos   = leftBottomBackPos[2];

  // TODO: texcoords should not exist for simple display cubes like a skybox

  Vertex rightTopBack {};
  rightTopBack.position  = Vec3f(rightPos, topPos, backPos);
  rightTopBack.texcoords = Vec2f(0.f, 1.f);

  Vertex rightTopFront {};
  rightTopFront.position  = rightTopFrontPos;
  rightTopFront.texcoords = Vec2f(1.f, 1.f);

  Vertex rightBottomBack {};
  rightBottomBack.position  = Vec3f(rightPos, bottomPos, backPos);
  rightBottomBack.texcoords = Vec2f(0.f, 0.f);

  Vertex rightBottomFront {};
  rightBottomFront.position  = Vec3f(rightPos, bottomPos, frontPos);
  rightBottomFront.texcoords = Vec2f(1.f, 0.f);

  Vertex leftTopBack {};
  leftTopBack.position  = Vec3f(leftPos, topPos, backPos);
  leftTopBack.texcoords = Vec2f(1.f, 1.f);

  Vertex leftTopFront {};
  leftTopFront.position  = Vec3f(leftPos, topPos, frontPos);
  leftTopFront.texcoords = Vec2f(0.f, 1.f);

  Vertex leftBottomBack {};
  leftBottomBack.position  = leftBottomBackPos;
  leftBottomBack.texcoords = Vec2f(1.f, 0.f);

  Vertex leftBottomFront {};
  leftBottomFront.position  = Vec3f(leftPos, bottomPos, frontPos);
  leftBottomFront.texcoords = Vec2f(0.f, 0.f);

  // Computing normals
  // TODO: normals should not be computed (or even exist) for simple display cubes like a skybox
  // TODO: compute normals

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.resize(8);

  vertices[0] = rightTopBack;
  vertices[1] = rightTopFront;

  vertices[2] = rightBottomBack;
  vertices[3] = rightBottomFront;

  vertices[4] = leftTopBack;
  vertices[5] = leftTopFront;

  vertices[6] = leftBottomBack;
  vertices[7] = leftBottomFront;

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.resize(36);

  // Right face
  indices[0] = 1;
  indices[1] = 0;
  indices[2] = 2;

  indices[3] = 1;
  indices[4] = 2;
  indices[5] = 3;

  // Left face
  indices[6] = 4;
  indices[7] = 5;
  indices[8] = 7;

  indices[9]  = 4;
  indices[10] = 7;
  indices[11] = 6;

  // Top face
  indices[12] = 4;
  indices[13] = 0;
  indices[14] = 1;

  indices[15] = 4;
  indices[16] = 1;
  indices[17] = 5;

  // Bottom face
  indices[18] = 7;
  indices[19] = 3;
  indices[20] = 2;

  indices[21] = 7;
  indices[22] = 2;
  indices[23] = 6;

  // Front face
  indices[24] = 5;
  indices[25] = 1;
  indices[26] = 3;

  indices[27] = 5;
  indices[28] = 3;
  indices[29] = 7;

  // Back face
  indices[30] = 0;
  indices[31] = 4;
  indices[32] = 6;

  indices[33] = 0;
  indices[34] = 6;
  indices[35] = 2;

  setRenderMode(renderMode);
  load();
}

const AABB& Mesh::computeBoundingBox() {
  Vec3f maxPos(std::numeric_limits<float>::lowest());
  Vec3f minPos(std::numeric_limits<float>::max());

  for (Submesh& submesh : m_submeshes) {
    const AABB& boundingBox = submesh.computeBoundingBox();

    maxPos[0] = std::max(maxPos[0], boundingBox.getRightTopFrontPos()[0]);
    maxPos[1] = std::max(maxPos[1], boundingBox.getRightTopFrontPos()[1]);
    maxPos[2] = std::max(maxPos[2], boundingBox.getRightTopFrontPos()[2]);

    minPos[0] = std::min(minPos[0], boundingBox.getLeftBottomBackPos()[0]);
    minPos[1] = std::min(minPos[1], boundingBox.getLeftBottomBackPos()[1]);
    minPos[2] = std::min(minPos[2], boundingBox.getLeftBottomBackPos()[2]);
  }

  m_boundingBox = AABB(minPos, maxPos);
  return m_boundingBox;
}

void Mesh::createUvSphere(const Sphere& sphere, uint32_t widthCount, uint32_t heightCount) {
  // Algorithm based on the standard/UV sphere presented here: http://www.songho.ca/opengl/gl_sphere.html#sphere

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.reserve((heightCount + 1) * (widthCount + 1));

  const float widthStep  = 2 * Pi<float> / static_cast<float>(widthCount);
  const float heightStep = Pi<float> / static_cast<float>(heightCount);
  const float invLength  = 1.f / sphere.getRadius();
  const Vec3f center     = sphere.getCenter();

  for (unsigned int heightIndex = 0; heightIndex <= heightCount; ++heightIndex) {
    const float heightAngle = Pi<float> / 2 - static_cast<float>(heightIndex) * heightStep;

    const float xz = sphere.getRadius() * std::cos(heightAngle);
    const float y  = sphere.getRadius() * std::sin(heightAngle);

    for (unsigned int widthIndex = 0; widthIndex <= widthCount; ++widthIndex) {
      const float widthAngle = static_cast<float>(widthIndex) * widthStep;

      const float x = xz * std::cos(widthAngle);
      const float z = xz * std::sin(widthAngle);

      Vertex vert;
      vert.position  = Vec3f(x + center[0], y + center[1], z + center[2]);
      vert.texcoords = Vec2f(static_cast<float>(widthIndex) / static_cast<float>(widthCount),
                             static_cast<float>(heightIndex) / static_cast<float>(heightCount));
      vert.normal    = Vec3f(x * invLength, y * invLength, z * invLength);
      vert.tangent   = Vec3f(vert.normal[1], vert.normal[0], vert.normal[2]); // TODO: how does this give seemingly accurate results in basic cases?

      vertices.emplace_back(vert);
    }
  }

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.reserve(widthCount * 6 + (heightCount - 2) * widthCount * 6);

  // Upper circle
  for (unsigned int widthIndex = 0; widthIndex < widthCount; ++widthIndex) {
    const unsigned int widthStride = widthCount + widthIndex;

    indices.push_back(widthIndex + 1);
    indices.push_back(widthStride + 1);
    indices.push_back(widthStride + 2);
  }

  for (unsigned int heightIndex = 1; heightIndex < heightCount - 1; ++heightIndex) {
    unsigned int curHeightStride  = heightIndex * (widthCount + 1);
    unsigned int nextHeightStride = curHeightStride + widthCount + 1;

    for (unsigned int widthIndex = 0; widthIndex < widthCount; ++widthIndex, ++curHeightStride, ++nextHeightStride) {
      indices.push_back(curHeightStride);
      indices.push_back(nextHeightStride);
      indices.push_back(curHeightStride + 1);

      indices.push_back(curHeightStride + 1);
      indices.push_back(nextHeightStride);
      indices.push_back(nextHeightStride + 1);
    }
  }

  // Lower circle
  {
    unsigned int curHeightStride  = (heightCount - 1) * (widthCount + 1);
    unsigned int nextHeightStride = curHeightStride + widthCount + 1;

    for (unsigned int widthIndex = 0; widthIndex < widthCount; ++widthIndex, ++curHeightStride, ++nextHeightStride) {
      indices.push_back(curHeightStride);
      indices.push_back(nextHeightStride);
      indices.push_back(curHeightStride + 1);
    }
  }
}

void Mesh::createIcosphere(const Sphere& sphere, uint32_t /* subdivCount */) {
  // Algorithm based on the icosphere presented here:
  // - http://www.songho.ca/opengl/gl_sphere.html#icosphere
  // - https://gist.github.com/warmwaffles/402b9c04318d6ee6dfa4

  const float radius       = sphere.getRadius();
  const float goldenRadius = radius * GoldenRatio<float>;

  std::vector<Vertex>& vertices = m_submeshes.front().getVertices();
  vertices.resize(12);

  // TODO: how does this tangent calculation give seemingly accurate results in basic cases?

  constexpr float invFactor = 1.f / (Pi<float> * 2);

  vertices[0].normal    = Vec3f(-radius, goldenRadius, 0.f).normalize();
  vertices[0].position  = vertices[0].normal * radius;
  vertices[0].tangent   = Vec3f(vertices[0].normal[1], vertices[0].normal[0], vertices[0].normal[2]);
  vertices[0].texcoords = Vec2f(std::atan2(vertices[0].normal[0], vertices[0].normal[2]) * invFactor + 0.5f, vertices[0].normal[1] * 0.5f + 0.5f);

  vertices[1].normal    = Vec3f(radius, goldenRadius, 0.f).normalize();
  vertices[1].position  = vertices[1].normal * radius;
  vertices[1].tangent   = Vec3f(vertices[1].normal[1], vertices[1].normal[0], vertices[1].normal[2]);
  vertices[1].texcoords = Vec2f(std::atan2(vertices[1].normal[0], vertices[1].normal[2]) * invFactor + 0.5f, vertices[1].normal[1] * 0.5f + 0.5f);

  vertices[2].normal    = Vec3f(-radius, -goldenRadius, 0.f).normalize();
  vertices[2].position  = vertices[2].normal * radius;
  vertices[2].tangent   = Vec3f(vertices[2].normal[1], vertices[2].normal[0], vertices[2].normal[2]);
  vertices[2].texcoords = Vec2f(std::atan2(vertices[2].normal[0], vertices[2].normal[2]) * invFactor + 0.5f, vertices[2].normal[1] * 0.5f + 0.5f);

  vertices[3].normal    = Vec3f(radius, -goldenRadius, 0.f).normalize();
  vertices[3].position  = vertices[3].normal * radius;
  vertices[3].tangent   = Vec3f(vertices[3].normal[1], vertices[3].normal[0], vertices[3].normal[2]);
  vertices[3].texcoords = Vec2f(std::atan2(vertices[3].normal[0], vertices[3].normal[2]) * invFactor + 0.5f, vertices[3].normal[1] * 0.5f + 0.5f);

  vertices[4].normal    = Vec3f(0.f, -radius, goldenRadius).normalize();
  vertices[4].position  = vertices[4].normal * radius;
  vertices[4].tangent   = Vec3f(vertices[4].normal[1], vertices[4].normal[0], vertices[4].normal[2]);
  vertices[4].texcoords = Vec2f(std::atan2(vertices[4].normal[0], vertices[4].normal[2]) * invFactor + 0.5f, vertices[4].normal[1] * 0.5f + 0.5f);

  vertices[5].normal    = Vec3f(0.f, radius, goldenRadius).normalize();
  vertices[5].position  = vertices[5].normal * radius;
  vertices[5].tangent   = Vec3f(vertices[5].normal[1], vertices[5].normal[0], vertices[5].normal[2]);
  vertices[5].texcoords = Vec2f(std::atan2(vertices[5].normal[0], vertices[5].normal[2]) * invFactor + 0.5f, vertices[5].normal[1] * 0.5f + 0.5f);

  vertices[6].normal    = Vec3f(0.f, -radius, -goldenRadius).normalize();
  vertices[6].position  = vertices[6].normal * radius;
  vertices[6].tangent   = Vec3f(vertices[6].normal[1], vertices[6].normal[0], vertices[6].normal[2]);
  vertices[6].texcoords = Vec2f(std::atan2(vertices[6].normal[0], vertices[6].normal[2]) * invFactor + 0.5f, vertices[6].normal[1] * 0.5f + 0.5f);

  vertices[7].normal    = Vec3f(0.f, radius, -goldenRadius).normalize();
  vertices[7].position  = vertices[7].normal * radius;
  vertices[7].tangent   = Vec3f(vertices[7].normal[1], vertices[7].normal[0], vertices[7].normal[2]);
  vertices[7].texcoords = Vec2f(std::atan2(vertices[7].normal[0], vertices[7].normal[2]) * invFactor + 0.5f, vertices[7].normal[1] * 0.5f + 0.5f);

  vertices[8].normal    = Vec3f(goldenRadius, 0.f, -radius).normalize();
  vertices[8].position  = vertices[8].normal * radius;
  vertices[8].tangent   = Vec3f(vertices[8].normal[1], vertices[8].normal[0], vertices[8].normal[2]);
  vertices[8].texcoords = Vec2f(std::atan2(vertices[8].normal[0], vertices[8].normal[2]) * invFactor + 0.5f, vertices[8].normal[1] * 0.5f + 0.5f);

  vertices[9].normal    = Vec3f(goldenRadius, 0.f, radius).normalize();
  vertices[9].position  = vertices[9].normal * radius;
  vertices[9].tangent   = Vec3f(vertices[9].normal[1], vertices[9].normal[0], vertices[9].normal[2]);
  vertices[9].texcoords = Vec2f(std::atan2(vertices[9].normal[0], vertices[9].normal[2]) * invFactor + 0.5f, vertices[9].normal[1] * 0.5f + 0.5f);

  vertices[10].normal    = Vec3f(-goldenRadius, 0.f, -radius).normalize();
  vertices[10].position  = vertices[10].normal * radius;
  vertices[10].tangent   = Vec3f(vertices[10].normal[1], vertices[10].normal[0], vertices[10].normal[2]);
  vertices[10].texcoords = Vec2f(std::atan2(vertices[10].normal[0], vertices[10].normal[2]) * invFactor + 0.5f, vertices[10].normal[1] * 0.5f + 0.5f);

  vertices[11].normal    = Vec3f(-goldenRadius, 0.f, radius).normalize();
  vertices[11].position  = vertices[11].normal * radius;
  vertices[11].tangent   = Vec3f(vertices[11].normal[1], vertices[11].normal[0], vertices[11].normal[2]);
  vertices[11].texcoords = Vec2f(std::atan2(vertices[11].normal[0], vertices[11].normal[2]) * invFactor + 0.5f, vertices[11].normal[1] * 0.5f + 0.5f);

  // Applying a translation on each vertex by the sphere's center
  for (Vertex& vertex : vertices)
    vertex.position += sphere.getCenter();

  std::vector<unsigned int>& indices = m_submeshes.front().getTriangleIndices();
  indices.resize(60);

  indices[0] = 0;
  indices[1] = 5;
  indices[2] = 11;

  indices[3] = 0;
  indices[4] = 1;
  indices[5] = 5;

  indices[6] = 0;
  indices[7] = 7;
  indices[8] = 1;

  indices[9]  = 0;
  indices[10] = 10;
  indices[11] = 7;

  indices[12] = 0;
  indices[13] = 11;
  indices[14] = 10;

  indices[15] = 1;
  indices[16] = 9;
  indices[17] = 5;

  indices[18] = 5;
  indices[19] = 4;
  indices[20] = 11;

  indices[21] = 11;
  indices[22] = 2;
  indices[23] = 10;

  indices[24] = 10;
  indices[25] = 6;
  indices[26] = 7;

  indices[27] = 7;
  indices[28] = 8;
  indices[29] = 1;

  indices[30] = 3;
  indices[31] = 4;
  indices[32] = 9;

  indices[33] = 3;
  indices[34] = 2;
  indices[35] = 4;

  indices[36] = 3;
  indices[37] = 6;
  indices[38] = 2;

  indices[39] = 3;
  indices[40] = 8;
  indices[41] = 6;

  indices[42] = 3;
  indices[43] = 9;
  indices[44] = 8;

  indices[45] = 4;
  indices[46] = 5;
  indices[47] = 9;

  indices[48] = 2;
  indices[49] = 11;
  indices[50] = 4;

  indices[51] = 6;
  indices[52] = 10;
  indices[53] = 2;

  indices[54] = 8;
  indices[55] = 7;
  indices[56] = 6;

  indices[57] = 9;
  indices[58] = 1;
  indices[59] = 8;

  // Applying subdivisions

  // TODO: subdivisions are clearly broken for the moment, must be investigated

//  std::unordered_map<Vertex, std::size_t> verticesIndices;
//  verticesIndices.emplace(vertices[0], 0);
//  verticesIndices.emplace(vertices[1], 1);
//  verticesIndices.emplace(vertices[2], 2);
//  verticesIndices.emplace(vertices[3], 3);
//  verticesIndices.emplace(vertices[4], 4);
//  verticesIndices.emplace(vertices[5], 5);
//  verticesIndices.emplace(vertices[6], 6);
//  verticesIndices.emplace(vertices[7], 7);
//  verticesIndices.emplace(vertices[8], 8);
//  verticesIndices.emplace(vertices[9], 9);
//  verticesIndices.emplace(vertices[10], 10);
//  verticesIndices.emplace(vertices[11], 11);
//
//  for (uint32_t subdivIndex = 0; subdivIndex < subdivCount; ++subdivIndex) {
//    std::vector<Vertex> newVertices;
//    newVertices.reserve(vertices.size() * 2);
//
//    std::vector<unsigned int> newIndices;
//    newIndices.reserve(indices.size() * 9);
//
//    const std::size_t indexCount = indices.size();
//
//    for (std::size_t triangleIndex = 0; triangleIndex < indexCount; triangleIndex += 3) {
//      // Recovering the original vertices
//      //       3
//      //      / \
//      //     /   \
//      //    /     \
//      //   /       \
//      //  /         \
//      // 1-----------2
//
//      const Vertex& firstVert  = vertices[indices[triangleIndex]];
//      const Vertex& secondVert = vertices[indices[triangleIndex + 1]];
//      const Vertex& thirdVert  = vertices[indices[triangleIndex + 2]];
//
//      newVertices.emplace_back(firstVert);
//      newVertices.emplace_back(secondVert);
//      newVertices.emplace_back(thirdVert);
//
//      // Computing the edge vertices to form another triangle
//      //       x
//      //      / \
//      //     /   \
//      //    3-----2
//      //   / \   / \
//      //  /   \ /   \
//      // x-----1-----x
//
//      Vertex& firstEdgeVert   = newVertices.emplace_back();
//      firstEdgeVert.position  = (firstVert.position + secondVert.position) * 0.5f;
//      firstEdgeVert.normal    = (firstVert.normal + secondVert.normal).normalize();
//      firstEdgeVert.tangent   = Vec3f(firstEdgeVert.normal[1], firstEdgeVert.normal[0], firstEdgeVert.normal[2]);
//      firstEdgeVert.texcoords = (firstVert.texcoords + secondVert.texcoords) * 0.5f;
//
//      Vertex& secondEdgeVert   = newVertices.emplace_back();
//      secondEdgeVert.position  = (secondVert.position + thirdVert.position) * 0.5f;
//      secondEdgeVert.normal    = (secondVert.normal + thirdVert.normal).normalize();
//      secondEdgeVert.tangent   = Vec3f(secondEdgeVert.normal[1], secondEdgeVert.normal[0], secondEdgeVert.normal[2]);
//      secondEdgeVert.texcoords = (secondVert.texcoords + thirdVert.texcoords) * 0.5f;
//
//      Vertex& thirdEdgeVert   = newVertices.emplace_back();
//      thirdEdgeVert.position  = (thirdVert.position + firstVert.position) * 0.5f;
//      thirdEdgeVert.normal    = (thirdVert.normal + firstVert.normal).normalize();
//      thirdEdgeVert.tangent   = Vec3f(thirdEdgeVert.normal[1], thirdEdgeVert.normal[0], thirdEdgeVert.normal[2]);
//      thirdEdgeVert.texcoords = (thirdVert.texcoords + firstVert.texcoords) * 0.5f;
//
//      // Adding indices to create the 4 resulting triangles
//      //       x
//      //      / \
//      //     / 4 \
//      //    x-----x
//      //   / \ 1 / \
//      //  / 2 \ / 3 \
//      // x-----x-----x
//
//      const std::size_t firstEdgeVertIndex = newVertices.size() - 3;
//      const std::size_t secondEdgeVertIndex = newVertices.size() - 2;
//      const std::size_t thirdEdgeVertIndex = newVertices.size() - 1;
//
//      verticesIndices.emplace(firstEdgeVert, firstEdgeVertIndex);
//      verticesIndices.emplace(secondEdgeVert, secondEdgeVertIndex);
//      verticesIndices.emplace(thirdEdgeVert, thirdEdgeVertIndex);
//
//      newIndices.emplace_back(firstEdgeVertIndex);
//      newIndices.emplace_back(secondEdgeVertIndex);
//      newIndices.emplace_back(thirdEdgeVertIndex);
//
//      newIndices.emplace_back(verticesIndices.find(firstVert)->second);
//      newIndices.emplace_back(firstEdgeVertIndex);
//      newIndices.emplace_back(thirdEdgeVertIndex);
//
//      newIndices.emplace_back(firstEdgeVertIndex);
//      newIndices.emplace_back(verticesIndices.find(secondVert)->second);
//      newIndices.emplace_back(secondEdgeVertIndex);
//
//      newIndices.emplace_back(thirdEdgeVertIndex);
//      newIndices.emplace_back(secondEdgeVertIndex);
//      newIndices.emplace_back(verticesIndices.find(thirdVert)->second);
//    }
//
//    vertices = std::move(newVertices);
//    indices  = std::move(newIndices);
//  }
}

} // namespace Raz
