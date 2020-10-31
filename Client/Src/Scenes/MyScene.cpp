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
	lineWidth(5.5f),
	grid(),
	meshes{
		new Mesh(Mesh::MeshType::Line, GL_LINES, {
		}),
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

	glLineWidth(lineWidth);

	grid = Grid<float>((float)winHeight / 20.0f, (float)winHeight / 20.0f, 10, 10);

	for(int i = 0; i < 99; ++i){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(PseudorandMinMax(-2000.f, 2000.f), PseudorandMinMax(-2000.f, 2000.f), -5.f)),
			modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, -45.f)),
		});
			meshes[(int)MeshType::Quad]->AddModelMat(modelStack.GetTopModel());
		modelStack.PopModel();
	}

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
	static float distortionBT = 0.f;
	static float echoBT = 0.f;
	static float wavesReverbBT = 0.f;
	static float resetSoundFXBT = 0.f;

	if(Key(GLFW_KEY_F2) && polyModeBT <= elapsedTime){
		polyMode += polyMode == GL_FILL ? -2 : 1;
		glPolygonMode(GL_FRONT_AND_BACK, polyMode);
		polyModeBT = elapsedTime + .5f;
	}
}

void MyScene::ForwardRender(){
	forwardSP.Use();
	const int& pAmt = 0;
	const int& dAmt = 0;
	const int& sAmt = 0;

	forwardSP.Set1f("shininess", 32.f); //More light scattering if lower
	forwardSP.Set3fv("globalAmbient", Light::globalAmbient);
	forwardSP.Set3fv("camWorldSpacePos", cam.GetPos());
	forwardSP.Set1i("pAmt", pAmt);
	forwardSP.Set1i("dAmt", dAmt);
	forwardSP.Set1i("sAmt", sAmt);

	int i;
	for(i = 0; i < pAmt; ++i){
		const PtLight* const& ptLight = static_cast<PtLight*>(ptLights[i]);
		forwardSP.Set3fv(("ptLights[" + std::to_string(i) + "].ambient").c_str(), ptLight->ambient);
		forwardSP.Set3fv(("ptLights[" + std::to_string(i) + "].diffuse").c_str(), ptLight->diffuse);
		forwardSP.Set3fv(("ptLights[" + std::to_string(i) + "].spec").c_str(), ptLight->spec);
		forwardSP.Set3fv(("ptLights[" + std::to_string(i) + "].pos").c_str(), ptLight->pos);
		forwardSP.Set1f(("ptLights[" + std::to_string(i) + "].constant").c_str(), ptLight->constant);
		forwardSP.Set1f(("ptLights[" + std::to_string(i) + "].linear").c_str(), ptLight->linear);
		forwardSP.Set1f(("ptLights[" + std::to_string(i) + "].quadratic").c_str(), ptLight->quadratic);
	}
	for(i = 0; i < dAmt; ++i){
		const DirectionalLight* const& directionalLight = static_cast<DirectionalLight*>(directionalLights[i]);
		forwardSP.Set3fv(("directionalLights[" + std::to_string(i) + "].ambient").c_str(), directionalLight->ambient);
		forwardSP.Set3fv(("directionalLights[" + std::to_string(i) + "].diffuse").c_str(), directionalLight->diffuse);
		forwardSP.Set3fv(("directionalLights[" + std::to_string(i) + "].spec").c_str(), directionalLight->spec);
		forwardSP.Set3fv(("directionalLights[" + std::to_string(i) + "].dir").c_str(), directionalLight->dir);
	}
	for(i = 0; i < sAmt; ++i){
		const Spotlight* const& spotlight = static_cast<Spotlight*>(spotlights[i]);
		forwardSP.Set3fv(("spotlights[" + std::to_string(i) + "].ambient").c_str(), spotlight->ambient);
		forwardSP.Set3fv(("spotlights[" + std::to_string(i) + "].diffuse").c_str(), spotlight->diffuse);
		forwardSP.Set3fv(("spotlights[" + std::to_string(i) + "].spec").c_str(), spotlight->spec);
		forwardSP.Set3fv(("spotlights[" + std::to_string(i) + "].pos").c_str(), spotlight->pos);
		forwardSP.Set3fv(("spotlights[" + std::to_string(i) + "].dir").c_str(), spotlight->dir);
		forwardSP.Set1f(("spotlights[" + std::to_string(i) + "].cosInnerCutoff").c_str(), spotlight->cosInnerCutoff);
		forwardSP.Set1f(("spotlights[" + std::to_string(i) + "].cosOuterCutoff").c_str(), spotlight->cosOuterCutoff);
	}

	forwardSP.SetMat4fv("PV", &(projection * view)[0][0]);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	forwardSP.Set1i("noNormals", 1);
	forwardSP.Set1i("useCustomColour", 1);

	///Render grid
	const float gridCellWidth = grid.GetCellWidth();
	const float gridCellHeight = grid.GetCellHeight();
	const float gridWidth = (float)grid.GetCols() * gridCellWidth;
	const float gridHeight = (float)grid.GetRows() * gridCellHeight;

	const int amtOfHorizLines = grid.GetRows() + 1;
	const float yOffset = ((float)winHeight - gridHeight) * 0.5f;
	for(int i = 0; i < amtOfHorizLines; ++i){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3((float)winWidth / 2.0f, yOffset + gridCellHeight * (float)i, 0.0f)),
			modelStack.Scale(glm::vec3(gridWidth, 1.0f, 1.0f)),
		});
			forwardSP.Set4fv("customColour", glm::vec4(1.0f));
			meshes[(int)MeshType::Line]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::Line]->Render(forwardSP);
		modelStack.PopModel();
	}

	const int amtOfVertLines = grid.GetCols() + 1;
	const float xOffset = ((float)winWidth - gridWidth) * 0.5f;
	for(int i = 0; i < amtOfVertLines; ++i){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(xOffset + gridCellWidth * (float)i, (float)winHeight / 2.0f, 0.0f)),
			modelStack.Rotate(glm::vec4(0.0f, 0.0f, 1.0f, -90.0f)),
			modelStack.Scale(glm::vec3(gridHeight, 1.0f, 1.0f)),
		});
			forwardSP.Set4fv("customColour", glm::vec4(1.0f));
			meshes[(int)MeshType::Line]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::Line]->Render(forwardSP);
		modelStack.PopModel();
	}

	///Render translucent block
	const float xTranslate = (float)int((lastX - fmod((float)winWidth, gridCellWidth) * 0.5f) / gridCellWidth) * gridCellWidth
		+ gridCellWidth * 0.5f
		+ fmod((float)winWidth, gridCellWidth) * 0.5f;
	const float yTranslate = (float)int(((float)winHeight - lastY) / gridCellHeight) * gridCellHeight + gridCellHeight * 0.5f;

	if(xTranslate >= xOffset && xTranslate <= xOffset + gridWidth && yTranslate >= yOffset && yTranslate <= yOffset + gridHeight){
		modelStack.PushModel({
			modelStack.Translate(glm::vec3(
				xTranslate,
				yTranslate,
				0.0f
			)),
			modelStack.Scale(glm::vec3(gridCellHeight * 0.5f, gridCellHeight * 0.5f, 1.0f)),
		});
			forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(1.0f), 0.2f));
			meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::Quad]->Render(forwardSP);
		modelStack.PopModel();
	}

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