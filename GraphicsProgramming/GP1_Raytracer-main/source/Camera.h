#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <iostream>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		const float rotationSpeed{ 100.f };
		const float movementSpeed{ 10.f };
		const float maxCooldown{ .2f };

		Vector3 origin{};
		float fovAngle{90.f};
		float fovScale{ tan((TO_RADIANS * fovAngle) / 2) };
		Vector3 forward{ Vector3::UnitZ};
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };
		float totalPitch{0.f};
		float totalYaw{0.f};
		float currentCooldown{ maxCooldown };
		bool shadowToggle{ true };
		bool justClicked{ false };

		enum class LightingMode
		{
			ObservedArea,
			Radiance,
			BRDF,
			Combined
		};
		LightingMode lightingMode{LightingMode::Combined};
		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			//todo: W2
			Matrix onb{};
			
			right = Vector3::Cross(Vector3::UnitY,forward);
			Vector3 up{ Vector3::Cross(forward,right) };
			onb[0] = Vector4{ right,0 };
			onb[1] = Vector4{ up,0 };
			onb[2] = Vector4{ forward,0 };
			onb[3] = Vector4{ origin,1 };
			return onb;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			HandleKeyboardMovement(pTimer,pKeyboardState);
			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
			ChangeMode(pKeyboardState,pTimer);
			HandleMouseMovement(mouseState, mouseY, pTimer);

			HandleMouseRotation(mouseState, mouseY, pTimer, mouseX);

			Matrix finalRotation{ Matrix::CreateRotationX(totalPitch*TO_RADIANS)* Matrix::CreateRotationY(totalYaw * TO_RADIANS) };
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();
			//todo: W2
		}
		void HandleMouseRotation(const uint32_t& mouseState, int mouseY, dae::Timer* pTimer, int mouseX)
		{
			if (mouseState == 4)
			{
				if (mouseY > 0)
				{
					totalPitch += rotationSpeed * pTimer->GetElapsed();
				}
				else if (mouseY < 0)
				{
					totalPitch -= rotationSpeed * pTimer->GetElapsed();
				}
				if (mouseX > 0)
				{
					totalYaw -= rotationSpeed * pTimer->GetElapsed();
				}
				else if (mouseX < 0)
				{
					totalYaw += rotationSpeed * pTimer->GetElapsed();
				}
			}
		}
		void HandleMouseMovement(const uint32_t& mouseState, int mouseY, dae::Timer* pTimer)
		{
			if (mouseState == 5)
			{
				if (mouseY > 0)
				{
					Vector3 up{ Vector3::Cross(forward,right) };
					origin += ((up / up.Magnitude()) * movementSpeed) * pTimer->GetElapsed();
				}
				else if (mouseY < 0)
				{
					Vector3 up{ Vector3::Cross(forward,right) };
					origin -= ((up / up.Magnitude()) * movementSpeed) * pTimer->GetElapsed();
				}
			}
			if (mouseState == 1)
			{
				if (mouseY < 0)
				{
					origin -= ((forward / forward.Magnitude()) * movementSpeed) * pTimer->GetElapsed();
				}
				else if (mouseY>0)
				{
					origin += ((forward / forward.Magnitude()) * movementSpeed) * pTimer->GetElapsed();
				}
				
			}
		}
		void HandleKeyboardMovement(dae::Timer* pTimer, const uint8_t* pKeyboardState)
		{

			if (pKeyboardState[SDL_SCANCODE_W] == true)
			{

				origin += ((forward / forward.Magnitude()) * movementSpeed) * pTimer->GetElapsed();
			}
			if (pKeyboardState[SDL_SCANCODE_S] == true)
			{

				origin -= ((forward / forward.Magnitude()) * movementSpeed) * pTimer->GetElapsed();
			}
			if (pKeyboardState[SDL_SCANCODE_A] == true)
			{

				origin -= ((right / right.Magnitude()) * movementSpeed) * pTimer->GetElapsed();
			}
			if (pKeyboardState[SDL_SCANCODE_D] == true)
			{

				origin += ((right / right.Magnitude()) * movementSpeed) * pTimer->GetElapsed();
			}
		}
		void ChangeMode(const uint8_t* pKeyboardState, dae::Timer* pTimer)
		{
			if (justClicked)
			{
				currentCooldown -= pTimer->GetElapsed();
				if (currentCooldown <= 0)
				{
					currentCooldown = maxCooldown;
					justClicked = false;
				}
			}
			else if (pKeyboardState[SDL_SCANCODE_F2] == true)
			{
				if (shadowToggle)
				{
					shadowToggle = false;
					justClicked = true;
				}
				else
				{
					shadowToggle = true;
					justClicked = true;
				}
			}
			else if (pKeyboardState[SDL_SCANCODE_F3] == true)
			{
				int lightmodeInt = static_cast<int>(lightingMode);
				lightmodeInt++;
				if (lightmodeInt >= 4)
					lightmodeInt = 0;
				lightingMode = static_cast<LightingMode>(lightmodeInt);

				justClicked = true;
			}
			else if (pKeyboardState[SDL_SCANCODE_F6] == true)
			{
				if (pTimer->IsRunning())
					pTimer->StartBenchmark();
				else
					pTimer->Stop();
			}
		}
		void CalculateFOVScale()
		{
			fovScale = tan((TO_RADIANS * fovAngle) / 2);
		}
		void SetFOVAngle(float angle)
		{
			fovAngle = angle;
			CalculateFOVScale();
		}
		bool GetShadowState() const
		{
			return shadowToggle;
		}
		LightingMode GetLightingMode() const
		{
			return lightingMode;
		}
	};
}
