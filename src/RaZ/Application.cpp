#include "RaZ/Application.hpp"

namespace Raz {

World& Application::addWorld(World&& world) {
  m_worlds.push_back(std::move(world));
  return m_worlds.back();
}

bool Application::run() {
  const auto currentTime = std::chrono::system_clock::now();
  m_deltaTime            = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - m_lastFrameTime).count();
  m_lastFrameTime        = currentTime;

  for (auto& world : m_worlds)
    world.update(m_deltaTime);

  return m_isRunning;
}

} // namespace Raz
