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

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();
	float aspectRatio = static_cast<float>(m_Width) / m_Height;
	Matrix cameraOnb{ camera.CalculateCameraToWorld() };


	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			Vector3 rayDirection{
				(2.f * (static_cast<float>(px) + 0.5f) / static_cast<float>(m_Width) - 1.f) * aspectRatio * camera.fovScale,
				(1.f-2.f* static_cast<float>(py)/ static_cast<float>(m_Height))*camera.fovScale,
				1.f
			};
			rayDirection = cameraOnb.TransformVector(rayDirection);
			Ray hitRay{ camera.origin,rayDirection };
			//ColorRGB finalColor{ rayDirection.x, rayDirection.y, rayDirection.z };
			ColorRGB finalColor{};
			HitRecord closestHit{};
			/*Sphere testsphere{ {0.f,0.f,100.f},50.f,0 };
			GeometryUtils::HitTest_Sphere(testsphere, hitRay,closestHit);*/
			pScene->GetClosestHit(hitRay, closestHit);
			/*Plane testPlane{ {0.f,-50,0.f},{0.f,1.f,0.f},0 };
			GeometryUtils::HitTest_Plane(testPlane, hitRay, closestHit);*/
			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();
				//const float scaled_t = (closestHit.t - 50.f) / 40.f;
				
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
