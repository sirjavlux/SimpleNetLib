#pragma once
#include <vector>
#include <tge/graphics/AmbientLight.h>
#include <tge/graphics/DirectionalLight.h>
#include <tge/graphics/PointLight.h>

namespace Tga
{
	class LightManager
	{
	public:
		LightManager();
		~LightManager();

		size_t GetPointLightCount() const { return myPointLights.size(); }

		void ClearPointLights() { myPointLights.clear(); }
		void AddPointLight(const PointLight& aPointLight);
		const PointLight* GetPointLights() const { return myPointLights.data(); };

		void SetDirectionalLight(DirectionalLight light) { myDirectionalLight = light; }
		const DirectionalLight& GetDirectionalLight() const { return myDirectionalLight; }

		void SetAmbientLight(AmbientLight light) { myAmbientLight = light; }
		const AmbientLight& GetAmbientLight() const { return myAmbientLight; }

	private:
		std::vector<PointLight> myPointLights;
		DirectionalLight myDirectionalLight;
		AmbientLight myAmbientLight;
	};
}

