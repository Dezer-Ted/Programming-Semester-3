#pragma once

#include <cstdint>
#include <vector>
#include "Camera.h"
struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Scene;
	class Light;
	class HitRecord;
	class Material;
	struct Vector3;
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		bool SaveBufferToImage() const;
		void RenderLighting(const std::vector<Light>& lights, HitRecord closestHit, bool ShadowsEnabled, dae::Camera::LightingMode lightingMode, Scene* pScene, ColorRGB& finalColor, const std::vector<Material*>& materials, const Vector3& v) const;
		
	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};
		std::vector<uint32_t> m_ImageHorizontalIterator, m_ImageVerticalIterator;
		int m_Width{};
		int m_Height{};
	};
}
