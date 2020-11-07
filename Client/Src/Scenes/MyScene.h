#pragma once

#include "Plat/Win/App/Scene/Cam/Cam.h"
#include "Geo/Mesh.h"
#include "Graphics/Light.h"
#include "Graphics/ModelStack.h"
#include "Graphics/ShaderProg.h"

#include "../DM2242/Grid.h"
#include "../DM2242/TextRenderer.h"

class MyScene final{
public:
	MyScene();
	~MyScene();
	void Init();
	void Update(float dt);
	void ForwardRender();
	void DefaultRender(const uint& screenTexRefID);
private:
	enum struct MeshType: int{
		Quad,
		QuadWithTex,
		DayBG,
		NightBG,
		Skele,
		Reptile,
		Boy,
		Orc,
		Amt
	};
	Mesh* meshes[(int)MeshType::Amt];

	ShaderProg forwardSP;
	ShaderProg screenSP;

	Cam cam;
	glm::mat4 view;
	glm::mat4 projection;

	float FPS;
	float elapsedTime;
	int polyMode;
	ModelStack modelStack;

	float gridCellWidth;
	float gridCellHeight;
	float gridLineThickness;
	int gridRows;
	int gridCols;
	Grid<float> grid;
	TextRenderer textRenderer;
	float mouseRow;
	float mouseCol;

	bool isDay;
	float dayNightBT;

	std::function<void(const str& aniName)> SkeleMove;
	std::function<void(const str& aniName)> SkeleThrust;
	std::function<void(const str& aniName)> ReptileMove;
	std::function<void(const str& aniName)> ReptileShoot;
	std::function<void(const str& aniName)> BoyMove;
	std::function<void(const str& aniName)> BoySlash;
	std::function<void(const str& aniName)> OrcMove;
	std::function<void(const str& aniName)> OrcSmack;

	void RenderGrid(float amtOfHorizLines, float amtOfVertLines, float gridWidth, float gridHeight);
	void RenderGridBG(float gridWidth, float gridHeight);
	void RenderGridData(float gridWidth, float gridHeight);
	void RenderTranslucentBlock(float gridWidth, float gridHeight);
	void RenderBG();
};