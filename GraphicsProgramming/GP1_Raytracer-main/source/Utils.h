#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest

		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{			
			const Vector3 l{ sphere.origin- ray.origin };
			//Calculate distance to a Point orthogonal to the center of the sphere and on the ray
			//by using the direction and the distance between the spheres origin and the rays
			const float tca{ Vector3::Dot(l , ray.direction) };
			/*if (tca < 0)
				return false;*/
			const Vector3 P{ l - tca * ray.direction };

			const float od{ P.Magnitude() };
			if (od >= sphere.radius)
				return false;

			//const float thc{ static_cast<float>(pow(sphere.radiusSquared,2)) - static_cast<float>(pow(od,2))};
			const float thc{ static_cast<float>(sqrt(sphere.radiusSquared * static_cast<float>(pow(od,2)))) };
			float t0{ tca - thc };
			if (t0 < ray.min)
			{
				t0 = tca + thc;
				if (t0 > ray.max || t0 < ray.min)
					return false;
			}
			hitRecord.didHit = true;
			hitRecord.t = t0;
			hitRecord.materialIndex = sphere.materialIndex;
			hitRecord.origin = ray.origin + ray.direction * t0;
			hitRecord.normal = (hitRecord.origin - sphere.origin) / sphere.radius;
			return true;
		}
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//check if ray hits
			const float denominator{ Vector3::Dot(ray.direction, plane.normal)};
			if (denominator > 0.0001f)
				return false;
			//calculate T
			const float result{Vector3::Dot((plane.origin - ray.origin), plane.normal) / denominator};
			//Check if T is in the range of the ray
			if(result < ray.min || result > ray.max)
				return false;

			//Assign Hitrecord
			hitRecord.didHit = true;
			hitRecord.materialIndex = plane.materialIndex;
			
			hitRecord.normal = plane.normal;
			hitRecord.origin = ray.origin + ray.direction * result;
			hitRecord.t = result;
			return true;
		
			

		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//todo W5
			assert(false && "No Implemented Yet!");
			return false;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			Vector3 direction{};
			if (light.type == LightType::Point)
			{
				direction =   light.origin- origin;
			}
			else
			{
				direction = light.direction * FLT_MAX;
			}
			return direction;
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			//todo W3
			assert(false && "No Implemented Yet!");
			return {};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}