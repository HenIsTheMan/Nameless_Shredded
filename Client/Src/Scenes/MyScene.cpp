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

glm::vec3 Light::globalAmbient = glm::vec3(.2f);

MyScene::MyScene():
	gridCellWidth(45.0f),
	gridCellHeight(45.0f),
	gridLineThickness(2.0f),
	gridRows(20),
	gridCols(20),
	grid(Grid<float>(0.0f, 0.0f, 0.0f, 0, 0)),
	textRenderer(),
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
	elapsedTime(0.f),
	polyMode(0),
	modelStack()
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

	///Render grid
	const float amtOfHorizLines = gridRows + 1.0f;
	const float amtOfVertLines = gridCols + 1.0f;
	const float gridWidth = gridCols * gridCellWidth + amtOfVertLines * gridLineThickness * 0.5f;
	const float gridHeight = gridRows * gridCellHeight + amtOfHorizLines * gridLineThickness * 0.5f;

	const float yOffset = ((float)winHeight - gridHeight) * 0.5f + gridLineThickness * 0.5f * 0.5f;
	for(int i = 0; i < amtOfHorizLines; ++i){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3((float)winWidth * 0.5f, yOffset + gridCellHeight * (float)i + gridLineThickness * 0.5f * (float)i, 0.0f)),
			modelStack.Scale(glm::vec3(gridWidth * 0.5f, gridLineThickness * 0.5f, 1.0f)),
		});
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(0.6f), 1.f));
			meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::Quad]->Render(forwardSP);
		modelStack.PopModel();
	}

	const float xOffset = ((float)winWidth - gridWidth) * 0.5f + gridLineThickness * 0.5f * 0.5f;
	for(int i = 0; i < amtOfVertLines; ++i){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(xOffset + gridCellWidth * (float)i + gridLineThickness * 0.5f * (float)i, (float)winHeight * 0.5f, 0.0f)),
			modelStack.Scale(glm::vec3(gridLineThickness * 0.5f, gridHeight * 0.5f, 1.0f)),
		});
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(0.6f), 1.f));
			meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::Quad]->Render(forwardSP);
		modelStack.PopModel();
	}

	///Render translucent block
	int quotX; //Used for checking if len of x end regions combined is >= unitX
	const float unitX = gridCellWidth + gridLineThickness * 0.5f;
	const float resultX = remquof(winWidth - gridLineThickness * 0.5f, unitX, &quotX);
	const float offsetX = (gridCols & 1 ? (quotX & 1 ? resultX : resultX - unitX) : (quotX & 1 ? resultX - unitX : resultX)) * 0.5f;
	const float xTranslate = std::floor((lastX - offsetX) / unitX) * unitX
		+ offsetX
		+ gridCellWidth * 0.5f + gridLineThickness * 0.5f;

	int quotY; //Used for checking if len of y end regions combined is >= unitY
	const float unitY = gridCellHeight + gridLineThickness * 0.5f;
	const float resultY = remquof(winHeight - gridLineThickness * 0.5f, unitY, &quotY);
	const float offsetY = (gridRows & 1 ? (quotY & 1 ? resultY : resultY - unitY) : (quotY & 1 ? resultY - unitY : resultY)) * 0.5f;
	const float yTranslate = std::floor((winHeight - lastY - offsetY) / unitY) * unitY
		+ offsetY
		+ gridCellHeight * 0.5f + gridLineThickness * 0.5f;

	if(xTranslate >= xOffset && xTranslate <= xOffset + gridWidth && yTranslate >= yOffset && yTranslate <= yOffset + gridHeight){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(
				xTranslate,
				yTranslate,
				0.0f
			)),
			modelStack.Scale(glm::vec3(gridCellWidth * 0.5f, gridCellHeight * 0.5f, 1.0f)),
		});
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(1.0f), 0.2f));
			meshes[(int)MeshType::QuadWithTex]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::QuadWithTex]->Render(forwardSP);
		modelStack.PopModel();
	}

	///GridBG
	modelStack.PushModel({
		modelStack.Translate(glm::vec3(winWidth * 0.5f, winHeight * 0.5f, 0.0f)),
		modelStack.Scale(glm::vec3(gridWidth * 0.5f, gridHeight * 0.5f, 1.0f)),
	});
		forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(0.0f), 1.f));
		meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
		meshes[(int)MeshType::Quad]->Render(forwardSP);
	modelStack.PopModel();

	///BG
	modelStack.PushModel({
		modelStack.Translate(glm::vec3(winWidth * 0.5f, winHeight * 0.5f, 0.0f)),
		modelStack.Scale(glm::vec3(winWidth * 0.5f, winHeight * 0.5f, 1.0f)),
	});
		forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(1.f), 1.f));
		meshes[(int)MeshType::BG]->SetModel(modelStack.GetTopModel());
		meshes[(int)MeshType::BG]->Render(forwardSP);
	modelStack.PopModel();

	forwardSP.Set1i("useCustomColour", 0);
	forwardSP.Set1i("noNormals", 0);

	textRenderer.RenderText(forwardSP, {
		(str)"Hello World!",
		0.0f,
		0.0f,
		1.0f,
		30.0f,
		50.0f,
		TextRenderer::TextAlignment::Left,
		glm::vec4(1.0f),
		0,
	});

	glBlendFunc(GL_ONE, GL_ZERO);
}

void MyScene::DefaultRender(const uint& screenTexRefID){
	screenSP.Use();
	screenSP.UseTex("screenTexSampler", screenTexRefID);
	meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
	meshes[(int)MeshType::Quad]->Render(screenSP, false);
	screenSP.ResetTexUnits();
}