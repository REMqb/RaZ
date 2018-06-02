#pragma once

#ifndef RAZ_CAMERA_HPP
#define RAZ_CAMERA_HPP

#include <memory>

#include "RaZ/Math/Constants.hpp"
#include "RaZ/Math/Matrix.hpp"
#include "RaZ/Math/Transform.hpp"
#include "RaZ/Math/Vector.hpp"

namespace Raz {

class Camera : public Transform {
public:
  Camera(unsigned int frameWidth, unsigned int frameHeight,
         float fieldOfViewDegrees,
         float nearPlane, float farPlane,
         const Vec3f& position = Vec3f(0.f));

  const Mat4f& getViewMatrix() const { return m_viewMat; }
  const Mat4f& getInverseViewMatrix() const { return m_invViewMat; }
  const Mat4f& getProjectionMatrix() const { return m_projMat; }
  const Mat4f& getInverseProjectionMatrix() const { return m_invProjMat; }

  void move(float x, float y, float z) override { move(Vec3f({ x, y, z })); }
  void move(const Vec3f& displacement) override;
  void translate(float x, float y, float z) override;
  void translate(const Vec3f& values) override { translate(values[0], values[1], values[2]); }
  void rotate(float angle, float x, float y, float z) override;
  const Mat4f& computeViewMatrix();
  const Mat4f& computeLookAt(const Vec3f& target = Vec3f(0.f), const Vec3f& orientation = Vec3f({ 0.f, 1.f, 0.f }));
  const Mat4f& computePerspectiveMatrix();

private:
  float m_frameRatio;
  float m_fieldOfView;
  float m_nearPlane;
  float m_farPlane;

  Mat4f m_viewMat;
  Mat4f m_invViewMat;
  Mat4f m_projMat;
  Mat4f m_invProjMat;
};

using CameraPtr = std::unique_ptr<Camera>;

} // namespace Raz

#endif // RAZ_CAMERA_HPP
