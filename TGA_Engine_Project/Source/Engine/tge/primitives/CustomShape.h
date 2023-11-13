#pragma once
#include <tge/math/Transform.h>
#include <tge/render/RenderObject.h>
#include <tge/EngineDefines.h>
namespace Tga
{
	struct SCustomPoint
	{
		SCustomPoint()
		{
			myPosition = { 0.f, 0.f, 0.f };
			myColor.Set(1, 1, 1, 1);
			myUV.Set(0.5f, 0.5f);
		}
		Vector3f myPosition;
		Color myColor;
		Vector2f myUV;
		int myIndex;
	};

	class Texture;
	class CustomShape : public RenderObjectSharedData
	{
	protected:
		CustomShape();
		~CustomShape();
	public:
		void Reset();
		void RemovePoint(int aIndex);

		void BuildShape();
		void SetTexture(const wchar_t* aPath);

		const Texture* GetTexture() const { return myTexture; }

		std::vector<SCustomPoint> myPoints;
		BlendState myBlendState;
	protected:
		Texture* myTexture;
		bool myIsDirty;
		int myIndex;
		bool myHasTexture;
	};

	class CustomShape3D : public CustomShape
	{
	public:
		int AddPoint(Vector3f aPoint, Color aColor, Vector2f aUV = Vector2f(0, 0));

		Transform& GetTransform() { return myTransform; }
		const Transform& GetTransform() const { return myTransform; }
		void SetTransform(const Transform& someTransform) { myTransform = someTransform; }
	private:
		Transform myTransform;
	};

	class CustomShape2D : public CustomShape
	{
	public:
		int AddPoint(Vector2f aPoint, Color aColor, Vector2f aUV = Vector2f(0, 0));

		void SetSize(const Vector2f& aSize)
		{
			mySize = aSize;
		}
		const Vector2f& GetSize()
		{
			return mySize;
		}

		void SetPosition(const Vector2f& aPosition)
		{
			myPosition = aPosition;
		}
		const Vector2f& GetPosition()
		{
			return myPosition;
		}

		void SetRotation(const float aRotation)
		{
			myRotation = aRotation;
		}
		const float GetRotation()
		{
			return myRotation;
		}

	private:
		Vector2f mySize = { 1.f, 1.f };
		Vector2f myPosition = {0.f, 0.f};
		float myRotation = 0;
	};
}