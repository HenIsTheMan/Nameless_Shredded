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
	gridCellWidth(50.0f),
	gridCellHeight(50.0f),
	gridLineThickness(5.0f),
	gridRows(10),
	gridCols(10),
	grid(Grid<float>(0.0f, 0.0f, 0.0f, 0, 0)),
	meshes{
		new Mesh(Mesh::MeshType::Quad, GL_TRIANGLES, {
			{"Imgs/BoxAlbedo.png", Mesh::TexType::Diffuse, 0},
			{"Imgs/BoxSpec.png", Mesh::TexType::Spec, 0},
			{"Imgs/BoxEmission.png", Mesh::TexType::Emission, 0},
		}),
		new SpriteAni(4, 8),
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

	meshes[(int)MeshType::SpriteAni]->AddTexMap({"Imgs/Fire.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(meshes[(int)MeshType::SpriteAni])->AddAni("FireSpriteAni", 0, 32);
	static_cast<SpriteAni*>(meshes[(int)MeshType::SpriteAni])->Play("FireSpriteAni", -1, .5f);

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

	static_cast<SpriteAni*>(meshes[(int)MeshType::SpriteAni])->Update(dt);

	static float polyModeBT = 0.f;
	if(Key(GLFW_KEY_F2) && polyModeBT <= elapsedTime){
		polyMode += polyMode == GL_FILL ? -2 : 1;
		glPolygonMode(GL_FRONT_AND_BACK, polyMode);
		polyModeBT = elapsedTime + .5f;
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
			forwardSP.Set4fv("customColour", glm::vec4(1.0f));
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
			forwardSP.Set4fv("customColour", glm::vec4(1.0f));
			meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::Quad]->Render(forwardSP);
		modelStack.PopModel();
	}

	///Render translucent block
	const float factorX = gridCellWidth + gridLineThickness * 0.5f;
	const float offsetX = fmod(winWidth - gridLineThickness * 0.5f, factorX) * 0.5f;
	const float xTranslate = (float)int((lastX - offsetX) / factorX) * factorX + offsetX + gridCellWidth * 0.5f + gridLineThickness * 0.5f;

	const float factorY = gridCellHeight + gridLineThickness * 0.5f;
	const float offsetY = fmod(winHeight - gridLineThickness * 0.5f, factorY) * 0.5f;
	const float yTranslate = (float)int((winHeight - lastY - offsetY) / factorY) * factorY + offsetY + gridCellHeight * 0.5f + gridLineThickness * 0.5f;

	//extern bool LMB;
	//extern bool RMB;
	//if(LMB){
	//	printf((std::to_string(fmod((float)winWidth, gridCellWidth) * 0.5f) + '\n').c_str());
	//}

	//if(xTranslate >= xOffset && xTranslate <= xOffset + gridWidth && yTranslate >= yOffset && yTranslate <= yOffset + gridHeight){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(
				xTranslate,
				yTranslate,
				0.0f
			)),
			modelStack.Scale(glm::vec3(gridCellWidth * 0.5f, gridCellHeight * 0.5f, 1.0f)),
		});
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(1.0f), 0.2f));
			meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::Quad]->Render(forwardSP);
		modelStack.PopModel();
	//}

	forwardSP.Set1i("useCustomColour", 0);
	forwardSP.Set1i("noNormals", 0);

	glBlendFunc(GL_ONE, GL_ZERO);
}

void MyScene::DefaultRender(const uint& screenTexRefID){
	screenSP.Use();
	screenSP.UseTex("screenTexSampler", screenTexRefID);
	meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
	meshes[(int)MeshType::Quad]->Render(screenSP, false);
	screenSP.ResetTexUnits();
}