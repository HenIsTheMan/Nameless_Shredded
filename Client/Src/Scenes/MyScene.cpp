#include "MyScene.h"

#include "Core.h"
#include "Geo/SpriteAni.h"
#include "Math/Pseudorand.h"
#include "Global/GlobalFuncs.h"

extern float angularFOV;
extern int winWidth;
extern int winHeight;
extern float lastX;
extern float lastY;
extern bool LMB;
extern bool RMB;

glm::vec3 Light::globalAmbient = glm::vec3(.2f);

MyScene::MyScene():
	meshes{
		new Mesh(Mesh::MeshType::Quad, GL_TRIANGLES, {
		}),
		new Mesh(Mesh::MeshType::Quad, GL_TRIANGLES, {
			{"Imgs/BoxAlbedo.png", Mesh::TexType::Diffuse, 0},
			{"Imgs/BoxSpec.png", Mesh::TexType::Spec, 0},
			{"Imgs/BoxEmission.png", Mesh::TexType::Emission, 0},
		}),
		new SpriteAni(18, 4),
	},
	forwardSP{"Shaders/Forward.vs", "Shaders/Forward.fs"},
	screenSP{"Shaders/Quad.vs", "Shaders/Screen.fs"},
	ptLights(),
	directionalLights(),
	spotlights(),
	cam(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f), 0.f, 150.f),
	view(glm::mat4(1.f)),
	projection(glm::mat4(1.f)),
	FPS(0.0f),
	elapsedTime(0.f),
	polyMode(0),
	modelStack(),
	gridCellWidth(80.0f),
	gridCellHeight(80.0f),
	gridLineThickness(10.0f),
	gridRows(5),
	gridCols(5),
	grid(Grid<float>(0.0f, 0.0f, 0.0f, 0, 0)),
	textRenderer(),
	mouseRow(0.0f),
	mouseCol(0.0f)
{
}

MyScene::~MyScene(){
	const size_t& pSize = ptLights.size();
	const size_t& dSize = directionalLights.size();
	const size_t& sSize = spotlights.size();
	for(size_t i = 0; i < pSize; ++i){
		if(ptLights[i]){
			delete ptLights[i];
			ptLights[i] = nullptr;
		}
	}
	for(size_t i = 0; i < dSize; ++i){
		if(directionalLights[i]){
			delete directionalLights[i];
			directionalLights[i] = nullptr;
		}
	}
	for(size_t i = 0; i < sSize; ++i){
		if(spotlights[i]){
			delete spotlights[i];
			spotlights[i] = nullptr;
		}
	}
	
	for(int i = 0; i < (int)MeshType::Amt; ++i){
		if(meshes[i]){
			delete meshes[i];
			meshes[i] = nullptr;
		}
	}
}

bool MyScene::Init(){
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);

	meshes[(int)MeshType::BG]->AddTexMap({"Imgs/BG.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(meshes[(int)MeshType::BG])->AddAni("BG", 0, 72);
	static_cast<SpriteAni*>(meshes[(int)MeshType::BG])->Play("BG", -1, 5.f);

	spotlights.emplace_back(CreateLight(LightType::Spot));

	return true;
}

void MyScene::Update(float dt){
	FPS = 1.0f / dt;
	elapsedTime += dt;
	if(winHeight){ //Avoid division by 0 when win is minimised
		cam.SetDefaultAspectRatio(float(winWidth) / float(winHeight));
		cam.ResetAspectRatio();
	}
	cam.Update(dt, GLFW_KEY_Q, GLFW_KEY_E, GLFW_KEY_A, GLFW_KEY_D, GLFW_KEY_W, GLFW_KEY_S);
	view = glm::mat4();
	projection = glm::ortho(0.0f, (float)winWidth, 0.0f, (float)winHeight);

	const glm::vec3& camWorldSpacePos = cam.GetPos();
	const glm::vec3& camFront = cam.CalcFront();

	spotlights[0]->ambient = glm::vec3(.05f);
	spotlights[0]->diffuse = glm::vec3(.8f);
	spotlights[0]->spec = glm::vec3(1.f);
	static_cast<Spotlight*>(spotlights[0])->pos = camWorldSpacePos;
	static_cast<Spotlight*>(spotlights[0])->dir = camFront;
	static_cast<Spotlight*>(spotlights[0])->cosInnerCutoff = cosf(glm::radians(12.5f));
	static_cast<Spotlight*>(spotlights[0])->cosOuterCutoff = cosf(glm::radians(17.5f));

	static_cast<SpriteAni*>(meshes[(int)MeshType::BG])->Update(dt);

	static float polyModeBT = 0.f;
	if(Key(VK_F2) && polyModeBT <= elapsedTime){
		polyMode += polyMode == GL_FILL ? -2 : 1;
		glPolygonMode(GL_FRONT_AND_BACK, polyMode);
		polyModeBT = elapsedTime + .5f;
	}

	static bool isKeyDown1 = false;
	static bool isKeyDown2 = false;
	static bool isKeyDown3 = false;
	static bool isKeyDown4 = false;
	static bool isKeyDown5 = false;
	static bool isKeyDown6 = false;
	static bool isKeyDown7 = false;
	static bool isKeyDown8 = false;
	static bool isKeyDown9 = false;
	static bool isKeyDown0 = false;
	if(!isKeyDown1 && Key('1')){
		++gridCellWidth;
		isKeyDown1 = true;
	} else if(isKeyDown1 && !Key('1')){
		isKeyDown1 = false;
	}
	if(!isKeyDown2 && Key('2')){
		--gridCellWidth;
		isKeyDown2 = true;
	} else if(isKeyDown2 && !Key('2')){
		isKeyDown2 = false;
	}
	if(!isKeyDown3 && Key('3')){
		++gridCellHeight;
		isKeyDown3 = true;
	} else if(isKeyDown3 && !Key('3')){
		isKeyDown3 = false;
	}
	if(!isKeyDown4 && Key('4')){
		--gridCellHeight;
		isKeyDown4 = true;
	} else if(isKeyDown4 && !Key('4')){
		isKeyDown4 = false;
	}
	if(!isKeyDown5 && Key('5')){
		++gridLineThickness;
		isKeyDown5 = true;
	} else if(isKeyDown5 && !Key('5')){
		isKeyDown5 = false;
	}
	if(!isKeyDown6 && Key('6')){
		--gridLineThickness;
		isKeyDown6 = true;
	} else if(isKeyDown6 && !Key('6')){
		isKeyDown6 = false;
	}
	if(!isKeyDown7 && Key('7')){
		++gridRows;
		isKeyDown7 = true;
	} else if(isKeyDown7 && !Key('7')){
		isKeyDown7 = false;
	}
	if(!isKeyDown8 && Key('8')){
		--gridRows;
		isKeyDown8 = true;
	} else if(isKeyDown8 && !Key('8')){
		isKeyDown8 = false;
	}
	if(!isKeyDown9 && Key('9')){
		++gridCols;
		isKeyDown9 = true;
	} else if(isKeyDown9 && !Key('9')){
		isKeyDown9 = false;
	}
	if(!isKeyDown0 && Key('0')){
		--gridCols;
		isKeyDown0 = true;
	} else if(isKeyDown0 && !Key('0')){
		isKeyDown0 = false;
	}

	grid.SetCellWidth(gridCellWidth);
	grid.SetCellHeight(gridCellHeight);
	grid.SetLineThickness(gridLineThickness);
	grid.SetRows(gridRows);
	grid.SetCols(gridCols);

	const float amtOfHorizLines = gridRows + 1.0f;
	const float amtOfVertLines = gridCols + 1.0f;
	const float gridWidth = gridCols * gridCellWidth + amtOfVertLines * gridLineThickness;
	const float gridHeight = gridRows * gridCellHeight + amtOfHorizLines * gridLineThickness;

	const float xOffset = ((float)winWidth - gridWidth) * 0.5f;
	const float yOffset = ((float)winHeight - gridHeight) * 0.5f;
	const float unitX = gridCellWidth + gridLineThickness;
	const float unitY = gridCellHeight + gridLineThickness;

	mouseRow = std::floor((winHeight - lastY - yOffset - gridLineThickness * 0.5f) / unitY);
	mouseCol = std::floor((lastX - xOffset - gridLineThickness * 0.5f) / unitX);

	if(lastX > xOffset + gridLineThickness * 0.5f && lastX < xOffset + gridWidth - gridLineThickness * 0.5f
		&& lastY > yOffset + gridLineThickness * 0.5f && lastY < yOffset + gridHeight - gridLineThickness * 0.5f){
		if(LMB){
			grid.SetData(EntityType::Block, (ptrdiff_t)mouseRow, (ptrdiff_t)mouseCol);
		} else if(RMB){
			grid.SetData(EntityType::Null, (ptrdiff_t)mouseRow, (ptrdiff_t)mouseCol);
		}
	}
}

void MyScene::ForwardRender(){
	forwardSP.Use();

	forwardSP.Set1f("shininess", 32.f); //More light scattering if lower
	forwardSP.Set3fv("globalAmbient", Light::globalAmbient);
	forwardSP.Set3fv("camWorldSpacePos", cam.GetPos());
	forwardSP.Set1i("pAmt", 0);
	forwardSP.Set1i("dAmt", 0);
	forwardSP.Set1i("sAmt", 0);

	forwardSP.SetMat4fv("PV", &(projection * view)[0][0]);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	forwardSP.Set1i("noNormals", 1);
	forwardSP.Set1i("useCustomColour", 1);

	const float amtOfHorizLines = gridRows + 1.0f;
	const float amtOfVertLines = gridCols + 1.0f;
	const float gridWidth = gridCols * gridCellWidth + amtOfVertLines * gridLineThickness;
	const float gridHeight = gridRows * gridCellHeight + amtOfHorizLines * gridLineThickness;

	RenderGrid(amtOfHorizLines, amtOfVertLines, gridWidth, gridHeight);
	RenderGridBG(gridWidth, gridHeight);
	RenderGridData(gridWidth, gridHeight);
	RenderTranslucentBlock(gridWidth, gridHeight);
	RenderBG();

	forwardSP.Set1i("useCustomColour", 0);
	forwardSP.Set1i("noNormals", 0);

	textRenderer.RenderText(forwardSP, {
		(str)"FPS: " + std::to_string(FPS).substr(0, std::to_string((int)FPS).length() + 3),
		(float)winWidth,
		0.0f,
		1.0f,
		30.0f,
		50.0f,
		glm::vec4(1.0f),
		//glm::vec4(0.5f, 0.3f, 0.7f, 1.0f),
		TextRenderer::TextAlignment::Right,
		0.7f,
	});

	glBlendFunc(GL_ONE, GL_ZERO);
}

void MyScene::DefaultRender(const uint& screenTexRefID){
	screenSP.Use();
	screenSP.UseTex("screenTexSampler", screenTexRefID);
	meshes[(int)MeshType::Quad]->SetModel(glm::scale(glm::mat4(), glm::vec3(2.0f, 2.0f, 1.0f)));
	meshes[(int)MeshType::Quad]->Render(screenSP, false);
	screenSP.ResetTexUnits();
}

void MyScene::RenderGrid(float amtOfHorizLines, float amtOfVertLines, float gridWidth, float gridHeight){
	const float xOffset = ((float)winWidth - gridWidth) * 0.5f + gridLineThickness * 0.5f;
	const float yOffset = ((float)winHeight - gridHeight) * 0.5f + gridLineThickness * 0.5f;

	for(int i = 0; i < amtOfVertLines; ++i){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(xOffset + (gridCellWidth + gridLineThickness) * (float)i, (float)winHeight * 0.5f, 0.0f)),
			modelStack.Scale(glm::vec3(gridLineThickness, gridHeight, 1.0f)),
		});
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(0.6f), 1.f));
			meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::Quad]->Render(forwardSP);
		modelStack.PopModel();
	}

	for(int i = 0; i < amtOfHorizLines; ++i){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3((float)winWidth * 0.5f, yOffset + (gridCellHeight + gridLineThickness) * (float)i, 0.0f)),
			modelStack.Scale(glm::vec3(gridWidth, gridLineThickness, 1.0f)),
		});
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(0.6f), 1.f));
			meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::Quad]->Render(forwardSP);
		modelStack.PopModel();
	}
}

void MyScene::RenderGridBG(float gridWidth, float gridHeight){
	modelStack.PushModel({
		modelStack.Translate(glm::vec3(winWidth * 0.5f, winHeight * 0.5f, 0.0f)),
		modelStack.Scale(glm::vec3(gridWidth, gridHeight, 1.0f)),
	});
		forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(0.0f), 1.f));
		meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
		meshes[(int)MeshType::Quad]->Render(forwardSP);
	modelStack.PopModel();
}

void MyScene::RenderGridData(float gridWidth, float gridHeight){
	const float xOffset = ((float)winWidth - gridWidth) * 0.5f + gridLineThickness + gridCellWidth * 0.5f;
	const float yOffset = ((float)winHeight - gridHeight) * 0.5f + gridLineThickness + gridCellHeight * 0.5f;

	const std::vector<std::vector<EntityType>>& gridData = grid.GetData();
	for(size_t i = (size_t)0; i < gridRows; ++i){
		for(size_t j = (size_t)0; j < gridCols; ++j){
			modelStack.PushModel({
				modelStack.Translate(glm::vec3(
					xOffset + (gridLineThickness + gridCellWidth) * (float)j,
					yOffset + (gridLineThickness + gridCellHeight) * (float)i,
					0.1f
				)),
				modelStack.Scale(glm::vec3(gridCellWidth, gridCellHeight, 1.0f)),
			});
				forwardSP.Set4fv("customColour", glm::vec4(1.0f));
				switch(gridData[i][j]){
					case EntityType::Block:
						meshes[(int)MeshType::QuadWithTex]->SetModel(modelStack.GetTopModel());
						meshes[(int)MeshType::QuadWithTex]->Render(forwardSP);
						break;
				}
			modelStack.PopModel();
		}
	}
}

void MyScene::RenderTranslucentBlock(float gridWidth, float gridHeight){
	const float xOffset = ((float)winWidth - gridWidth) * 0.5f;
	const float yOffset = ((float)winHeight - gridHeight) * 0.5f;

	if(lastX > xOffset + gridLineThickness * 0.5f && lastX < xOffset + gridWidth - gridLineThickness * 0.5f
		&& lastY > yOffset + gridLineThickness * 0.5f && lastY < yOffset + gridHeight - gridLineThickness * 0.5f){
		const float unitX = gridCellWidth + gridLineThickness;
		const float unitY = gridCellHeight + gridLineThickness;
		const float xTranslate = mouseCol * unitX
			+ xOffset
			+ gridCellWidth * 0.5f
			+ gridLineThickness;
		const float yTranslate = mouseRow * unitY
			+ yOffset
			+ gridCellHeight * 0.5f
			+ gridLineThickness;

		modelStack.PushModel({
			modelStack.Translate(glm::vec3(
				xTranslate,
				yTranslate,
				0.1f
			)),
			modelStack.Scale(glm::vec3(gridCellWidth, gridCellHeight, 1.0f)),
		});
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(1.0f), 0.2f));
			meshes[(int)MeshType::QuadWithTex]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::QuadWithTex]->Render(forwardSP);
		modelStack.PopModel();
	}
}

void MyScene::RenderBG(){
	modelStack.PushModel({
		modelStack.Translate(glm::vec3(winWidth * 0.5f, winHeight * 0.5f, 0.0f)),
		modelStack.Scale(glm::vec3(winWidth, winHeight, 1.0f)),
	});
		forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(1.f), 1.f));
		meshes[(int)MeshType::BG]->SetModel(modelStack.GetTopModel());
		meshes[(int)MeshType::BG]->Render(forwardSP);
	modelStack.PopModel();
}