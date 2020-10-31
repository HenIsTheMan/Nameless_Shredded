#pragma once

#include "Plat/Win/App/Scene/Cam/Cam.h"
#include "Geo/Mesh.h"
#include "Graphics/Light.h"
#include "Graphics/ModelStack.h"
#include "Graphics/ShaderProg.h"

#include "../DM2242/Grid.h"

class MyScene final{
public:
	MyScene();
	~MyScene();
	bool Init();
	void Update(float dt);
	void ForwardRender();
	void DefaultRender(const uint& screenTexRefID);
private:
	float lineWidth;
	Grid<float> grid;

	enum struct MeshType{
		Line,
		Quad,
		SpriteAni,
		Amt
	};
	Mesh* meshes[(int)MeshType::Amt];

	ShaderProg forwardSP;
	ShaderProg screenSP;

	std::vector<Light*> ptLights;
	std::vector<Light*> directionalLights;
	std::vector<Light*> spotlights;

	Cam cam;
	glm::mat4 view;
	glm::mat4 projection;

	float elapsedTime;
	int polyMode;
	ModelStack modelStack;
};