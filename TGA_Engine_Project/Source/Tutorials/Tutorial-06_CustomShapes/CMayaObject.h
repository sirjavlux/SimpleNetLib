#pragma once
#include <tge/primitives/CustomShape.h>
class CMayaObject
{
public:
	CMayaObject();
	~CMayaObject();
	void Init(const char* aObjPath);
	void Render(float aDelta);
private:
	Tga::CustomShape2D* myShape;
	float myTotalTime = 0;
};

