#include "CMayaObject.h"
#include <tge/drawers/CustomShapeDrawer.h>
#include <tge/engine.h>
#include <tge/graphics/GraphicsEngine.h>
#include <fstream>
#include <sstream>
#include <string>
#include <tge/settings/settings.h>

CMayaObject::CMayaObject()
{
	myShape = nullptr;
}


CMayaObject::~CMayaObject()
{
	if (myShape)
	{
		delete myShape;
	}
}

void CMayaObject::Init(const char* aObjPath)
{
	std::ifstream infile(Tga::Settings::ResolveAssetPath(aObjPath));
	if (!infile.is_open())
	{
		return;
	}

	std::vector<Tga::Vector2f> positions;
	std::vector<int> indexes;
	
	std::string line;
	while (std::getline(infile, line))
	{
		if (line[0] == 'v' && line[1] == ' ')
		{
			line.erase(line.begin());
			std::istringstream iss(line);
			float X, Y, Z;
			iss >> X >> Y >> Z;

			Y -= 3.0f;
			positions.push_back({ X, Y });
		}
		else if (line[0] == 'f')
		{
			line.erase(line.begin());
			line.erase(line.begin());



			std::istringstream iss(line);

			std::string vertexOne;
			std::string vertexTwo;
			std::string vertexThree;
			std::string trash;
			std::getline(iss, vertexOne, '/');
			std::getline(iss, trash, ' ');
			std::getline(iss, vertexTwo, '/');
			std::getline(iss, trash, ' ');
			std::getline(iss, vertexThree, '/');

			int theIndexOne = 0;
			int theIndexTwo = 0;
			int theIndexThree = 0;
			iss >> theIndexOne >> theIndexTwo >> theIndexThree;


			indexes.push_back(stoi(vertexOne));
			indexes.push_back(stoi(vertexTwo));
			indexes.push_back(stoi(vertexThree));
		}
	}
	infile.close();

	myShape = new Tga::CustomShape2D();

	std::vector<Tga::Vector2f> positionsTriangulated;
	for (int i=0; i< indexes.size(); i++)
	{
		float randomR = ((float)(rand() % 1000)) / 1000.0f;
		float randomG = ((float)(rand() % 1000)) / 1000.0f;
		float randomB = ((float)(rand() % 1000)) / 1000.0f;
		myShape->AddPoint(positions[indexes[i] - 1], Tga::Color(randomR, randomG, randomB, 1));
	}
	myShape->SetSize({ 0.2f, 0.2f });
	
	
	myShape->BuildShape();

}

void CMayaObject::Render(float aDelta)
{
	if (!myShape)
	{
		return;
	}
	myTotalTime += aDelta;

	myShape->SetPosition({ -1.0f + cos(myTotalTime * 0.25f) * 4 , myShape->GetPosition().y });
	
	Tga::Engine::GetInstance()->GetGraphicsEngine().GetCustomShapeDrawer().Draw(*myShape);
}
