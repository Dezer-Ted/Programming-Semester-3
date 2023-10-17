//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"
#include <vector>
#include <algorithm>
#include <execution>

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
	//Initialize vectors for multithreading as an iterator
	m_ImageHorizontalIterator.resize(m_Width);
	m_ImageVerticalIterator.resize(m_Height);
	for (int index = 0; index < m_Width; ++index)
		m_ImageHorizontalIterator[index] = index;
	for (int index = 0; index < m_Height; ++index)
		m_ImageVerticalIterator[index] = index;
}
void Renderer::Render(Scene* pScene) const
{
	
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();
	Matrix cameraOnb{ camera.CalculateCameraToWorld() };
	float aspectRatio = static_cast<float>(m_Width) / m_Height;
	
	//Multithreaded to the max. Takes all the processing power it can.
	std::for_each(std::execution::par,m_ImageVerticalIterator.begin(), m_ImageVerticalIterator.end(),
		[this,camera,materials,lights,cameraOnb,aspectRatio,pScene](uint32_t py)
	{
			float cy{ (1.f - 2.f * (static_cast<float>(py) + 0.5f) / static_cast<float>(m_Height)) * camera.fovScale };

			std::for_each(std::execution::par, m_ImageHorizontalIterator.begin(), m_ImageHorizontalIterator.end(),
				[this, py,camera, materials, lights, cameraOnb, aspectRatio, pScene,cy](uint32_t px)
				{
					float cx{ (2.f * (static_cast<float>(px) + 0.5f) / static_cast<float>(m_Width) - 1.f) * aspectRatio * camera.fovScale };

					Vector3 rayDirection{ cx, cy,1.f };
					rayDirection = cameraOnb.TransformVector(rayDirection);
					rayDirection.Normalize();
					Ray hitRay{ camera.origin,rayDirection };
					Vector3 v{ hitRay.direction * -1.f};
					HitRecord closestHit{};

					//Get the object closest to the camera
					pScene->GetClosestHit(hitRay, closestHit);
					
					ColorRGB finalColor{};

					if (closestHit.didHit)
					{
						RenderLighting(lights, closestHit, camera.GetShadowState(), camera.GetLightingMode(), pScene, finalColor, materials, v);

					}
					//Update Color in Buffer
					finalColor.MaxToOne();

					m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
						static_cast<uint8_t>(finalColor.r * 255),
						static_cast<uint8_t>(finalColor.g * 255),
						static_cast<uint8_t>(finalColor.b * 255));
				});
	});

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void Renderer::RenderLighting( const std::vector<Light>& lights,HitRecord closestHit,bool ShadowsEnabled,Camera::LightingMode lightingMode,Scene* pScene , ColorRGB& finalColor,const std::vector<Material*>& materials, const Vector3& v ) const
{
	for (const Light& light : lights)
	{
		Vector3 lightDirection{ LightUtils::GetDirectionToLight(light, closestHit.origin) };
		const float lightDistance = lightDirection.Normalize();

		const Ray lightRay{
			closestHit.origin + closestHit.normal * 0.001f,
			lightDirection, 0.0001f, lightDistance
		};

		const float lambertCos = std::max(0.0f, closestHit.normal * lightRay.direction);

		if (!(pScene->DoesHit(lightRay) &&ShadowsEnabled))
		{

			Vector3 l{ (light.origin - closestHit.origin).Normalized() };

			switch (lightingMode)
			{
			case Camera::LightingMode::ObservedArea:
				finalColor += ColorRGB{ 1,1,1 } * lambertCos;
				break;
			case Camera::LightingMode::Radiance:
				finalColor += LightUtils::GetRadiance(light, closestHit.origin);
				break;
			case Camera::LightingMode::BRDF:
				finalColor += materials[closestHit.materialIndex]->Shade(closestHit, l, v);
				break;
			case Camera::LightingMode::Combined:
				finalColor += LightUtils::GetRadiance(light, closestHit.origin) * materials[closestHit.materialIndex]->Shade(closestHit, l, v) * lambertCos;
				break;
			}
		}
	}
}