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


		Vector3 origin{};
		float fovAngle{90.f};
		float fovScale{ tan((TO_RADIANS * fovAngle) / 2) };
		const float movementSpeed{ 10.f };
		const float rotationSpeed{ 100.f };
		
		Vector3 forward{ Vector3::UnitZ};
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };
		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			//todo: W2
			Matrix onb{};
			
			right = Vector3::Cross(Vector3::UnitY,forward);
			Vector3 up{ Vector3::Cross(forward,right) };
			onb[0] = Vector4{ right.x, right.y,right.z,0 };
			onb[1] = Vector4{ up.x,up.y,up.z,0 };
			onb[2] = Vector4{ forward.x,forward.y,forward.z,0 };
			onb[3] = Vector4{ origin.x,origin.y,origin.z,1 };
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
		void CalculateFOVScale()
		{
			fovScale = tan((TO_RADIANS * fovAngle) / 2);
		}
		void SetFOVAngle(float angle)
		{
			fovAngle = angle;
			CalculateFOVScale();
		}
		
	};
}
