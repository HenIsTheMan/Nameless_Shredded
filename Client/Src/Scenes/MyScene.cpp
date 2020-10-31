#include "MyScene.h"

#include "Core.h"
#include "Geo/SpriteAni.h"
#include "Math/Pseudorand.h"
#include "Global/GlobalFuncs.h"

extern float angularFOV;
extern int winWidth;
extern int winHeight;

glm::vec3 Light::globalAmbient = glm::vec3(.2f);

MyScene::MyScene():
	cam(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f), 0.f, 150.f),
	meshes{
		new Mesh(Mesh::MeshType::Quad, GL_TRIANGLES, {
			{"Imgs/BoxAlbedo.png", Mesh::TexType::Diffuse, 0},
			{"Imgs/BoxSpec.png", Mesh::TexType::Spec, 0},
			{"Imgs/BoxEmission.png", Mesh::TexType::Emission, 0},
		}),
		new Mesh(Mesh::MeshType::Cube, GL_TRIANGLES, {
			{"Imgs/BoxAlbedo.png", Mesh::TexType::Diffuse, 0},
		}),
		new Mesh(Mesh::MeshType::Sphere, GL_TRIANGLE_STRIP, {
			{"Imgs/Skydome.hdr", Mesh::TexType::Diffuse, 0},
		}),
		new Mesh(Mesh::MeshType::Cylinder, GL_TRIANGLE_STRIP, {
			{"Imgs/BoxAlbedo.png", Mesh::TexType::Diffuse, 0},
		}),
		new SpriteAni(4, 8),
	},
	blurSP{"Shaders/Quad.vs", "Shaders/Blur.fs"},
	forwardSP{"Shaders/Forward.vs", "Shaders/Forward.fs"},
	geoPassSP{"Shaders/GeoPass.vs", "Shaders/GeoPass.fs"},
	lightingPassSP{"Shaders/Quad.vs", "Shaders/LightingPass.fs"},
	normalsSP{"Shaders/Normals.vs", "Shaders/Normals.fs", "Shaders/Normals.gs"}, //??
	screenSP{"Shaders/Quad.vs", "Shaders/Screen.fs"},
	textSP{"Shaders/Text.vs", "Shaders/Text.fs"},
	ptLights({}),
	directionalLights({}),
	spotlights({}),
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

	for(int i = 0; i < 99; ++i){
		PushModel({
			Translate(glm::vec3(PseudorandMinMax(-2000.f, 2000.f), PseudorandMinMax(-2000.f, 2000.f), -5.f)),
			Rotate(glm::vec4(0.f, 1.f, 0.f, -45.f)),
		});
			meshes[(int)MeshType::Quad]->AddModelMat(GetTopModel());
		PopModel();
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
	view = cam.LookAt();
	projection = glm::perspective(glm::radians(angularFOV), cam.GetAspectRatio(), .1f, 9999.f);

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

	//for(int i = 0; i < 99999; ++i){
	//	PushModel({
	//		Translate(glm::vec3(PseudorandMinMax(-2000.f, 2000.f), PseudorandMinMax(-2000.f, 2000.f), -5.f)),
	//		Rotate(glm::vec4(0.f, 1.f, 0.f, -45.f)),
	//	});
	//		meshes[(int)MeshType::Quad]->SetModelMat(GetTopModel(), i);
	//	PopModel();
	//}
}

void MyScene::GeoRenderPass(){
	geoPassSP.Use();
	geoPassSP.SetMat4fv("PV", &(projection * glm::mat4(glm::mat3(view)))[0][0]);

	///Sky
	glDepthFunc(GL_LEQUAL); //Modify comparison operators used for depth test such that frags with depth <= 1.f are shown
	glCullFace(GL_FRONT);
	geoPassSP.Set1i("sky", 1);
	PushModel({
		Rotate(glm::vec4(0.f, 1.f, 0.f, glfwGetTime())),
	});
		meshes[(int)MeshType::Sphere]->SetModel(GetTopModel());
		meshes[(int)MeshType::Sphere]->Render(geoPassSP);
	PopModel();
	geoPassSP.Set1i("sky", 0);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);

	geoPassSP.SetMat4fv("PV", &(projection * view)[0][0]);

	///Shapes
	PushModel({
		Translate(glm::vec3(0.f, 100.f, 0.f)),
		Scale(glm::vec3(10.f)),
	});
		PushModel({
			Translate(glm::vec3(6.f, 0.f, 0.f)),
		});
			geoPassSP.Set1i("noNormals", 1);
			geoPassSP.Set1i("useCustomColour", 1);
			geoPassSP.Set4fv("customColour", glm::vec4(glm::vec3(5.f), 1.f));
			meshes[(int)MeshType::Quad]->SetModel(GetTopModel());
			meshes[(int)MeshType::Quad]->Render(geoPassSP);
			geoPassSP.Set1i("useCustomColour", 0);
			geoPassSP.Set1i("noNormals", 0);
			PushModel({
				Translate(glm::vec3(0.f, 0.f, 5.f)),
			});
				meshes[(int)MeshType::Sphere]->SetModel(GetTopModel());
				meshes[(int)MeshType::Sphere]->Render(geoPassSP);
			PopModel();
			PushModel({
				Translate(glm::vec3(0.f, 0.f, -5.f)),
			});
				meshes[(int)MeshType::Cylinder]->SetModel(GetTopModel());
				meshes[(int)MeshType::Cylinder]->Render(geoPassSP);
			PopModel();
		PopModel();
	PopModel();
}

void MyScene::LightingRenderPass(const uint& posTexRefID, const uint& coloursTexRefID, const uint& normalsTexRefID, const uint& specTexRefID, const uint& reflectionTexRefID){
	lightingPassSP.Use();
	const int& pAmt = (int)ptLights.size();
	const int& dAmt = (int)directionalLights.size();
	const int& sAmt = (int)spotlights.size();

	lightingPassSP.Set1f("shininess", 32.f); //More light scattering if lower
	lightingPassSP.Set3fv("globalAmbient", Light::globalAmbient);
	lightingPassSP.Set3fv("camWorldSpacePos", cam.GetPos());
	lightingPassSP.Set1i("pAmt", pAmt);
	lightingPassSP.Set1i("dAmt", dAmt);
	lightingPassSP.Set1i("sAmt", sAmt);
	lightingPassSP.UseTex("posTex", posTexRefID);
	lightingPassSP.UseTex("coloursTex", coloursTexRefID);
	lightingPassSP.UseTex("normalsTex", normalsTexRefID);
	lightingPassSP.UseTex("specTex", specTexRefID);
	lightingPassSP.UseTex("reflectionTex", reflectionTexRefID);

	int i;
	for(i = 0; i < pAmt; ++i){
		const PtLight* const& ptLight = static_cast<PtLight*>(ptLights[i]);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].ambient").c_str(), ptLight->ambient);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].diffuse").c_str(), ptLight->diffuse);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].spec").c_str(), ptLight->spec);
		lightingPassSP.Set3fv(("ptLights[" + std::to_string(i) + "].pos").c_str(), ptLight->pos);
		lightingPassSP.Set1f(("ptLights[" + std::to_string(i) + "].constant").c_str(), ptLight->constant);
		lightingPassSP.Set1f(("ptLights[" + std::to_string(i) + "].linear").c_str(), ptLight->linear);
		lightingPassSP.Set1f(("ptLights[" + std::to_string(i) + "].quadratic").c_str(), ptLight->quadratic);
	}
	for(i = 0; i < dAmt; ++i){
		const DirectionalLight* const& directionalLight = static_cast<DirectionalLight*>(directionalLights[i]);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].ambient").c_str(), directionalLight->ambient);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].diffuse").c_str(), directionalLight->diffuse);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].spec").c_str(), directionalLight->spec);
		lightingPassSP.Set3fv(("directionalLights[" + std::to_string(i) + "].dir").c_str(), directionalLight->dir);
	}
	for(i = 0; i < sAmt; ++i){
		const Spotlight* const& spotlight = static_cast<Spotlight*>(spotlights[i]);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].ambient").c_str(), spotlight->ambient);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].diffuse").c_str(), spotlight->diffuse);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].spec").c_str(), spotlight->spec);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].pos").c_str(), spotlight->pos);
		lightingPassSP.Set3fv(("spotlights[" + std::to_string(i) + "].dir").c_str(), spotlight->dir);
		lightingPassSP.Set1f(("spotlights[" + std::to_string(i) + "].cosInnerCutoff").c_str(), spotlight->cosInnerCutoff);
		lightingPassSP.Set1f(("spotlights[" + std::to_string(i) + "].cosOuterCutoff").c_str(), spotlight->cosOuterCutoff);
	}

	meshes[(int)MeshType::Quad]->SetModel(GetTopModel());
	meshes[(int)MeshType::Quad]->Render(lightingPassSP, false);
	lightingPassSP.ResetTexUnits();
}

void MyScene::BlurRender(const uint& brightTexRefID, const bool& horizontal){
	blurSP.Use();
	blurSP.Set1i("horizontal", horizontal);
	blurSP.UseTex("texSampler", brightTexRefID);
	meshes[(int)MeshType::Quad]->SetModel(GetTopModel());
	meshes[(int)MeshType::Quad]->Render(blurSP, false);
	blurSP.ResetTexUnits();
}

void MyScene::DefaultRender(const uint& screenTexRefID){
	screenSP.Use();
	screenSP.UseTex("screenTexSampler", screenTexRefID);
	meshes[(int)MeshType::Quad]->SetModel(GetTopModel());
	meshes[(int)MeshType::Quad]->Render(screenSP, false);
	screenSP.ResetTexUnits();
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

	///Shapes
	PushModel({
		Translate(glm::vec3(0.f, 100.f, 0.f)),
		Scale(glm::vec3(10.f)),
	});
		forwardSP.Set1i("useCustomColour", 1);
		forwardSP.Set4fv("customColour", glm::vec4(glm::rgbColor(glm::vec3(1.f, PseudorandMinMax(0.f, 255.f), 1.f)) * .5f, .5f));
		meshes[(int)MeshType::Cylinder]->SetModel(GetTopModel());
		meshes[(int)MeshType::Cylinder]->Render(forwardSP);
		forwardSP.Set1i("useCustomColour", 0);
		PushModel({
			Translate(glm::vec3(-3.f, 0.f, 0.f)),
		});
			forwardSP.Set1i("useCustomColour", 1);
			forwardSP.Set4fv("customColour", glm::vec4(glm::rgbColor(glm::vec3(1.f, 1.f, PseudorandMinMax(0.f, 255.f))) * 7.f, .3f));
			forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
			forwardSP.Set1i("customDiffuseTexIndex", -1);
			meshes[(int)MeshType::Sphere]->SetModel(GetTopModel());
			meshes[(int)MeshType::Sphere]->Render(forwardSP);
			forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
			forwardSP.Set1i("useCustomColour", 0);
		PopModel();
		PushModel({
			Translate(glm::vec3(3.f, 0.f, 0.f)),
		});
			forwardSP.Set1i("useCustomColour", 1);
			forwardSP.Set4fv("customColour", glm::vec4(glm::rgbColor(glm::vec3(PseudorandMinMax(0.f, 255.f), 1.f, 1.f)) * .5f, .7f));
			forwardSP.Set1i("useCustomDiffuseTexIndex", 1);
			forwardSP.Set1i("customDiffuseTexIndex", -1);
			meshes[(int)MeshType::Cube]->SetModel(GetTopModel());
			meshes[(int)MeshType::Cube]->Render(forwardSP);
			forwardSP.Set1i("useCustomDiffuseTexIndex", 0);
			forwardSP.Set1i("useCustomColour", 0);
		PopModel();
	PopModel();

	///SpriteAni
	PushModel({
		Translate(glm::vec3(0.f, 50.f, 0.f)),
		Scale(glm::vec3(20.f, 40.f, 20.f)),
	});
		forwardSP.Set1i("noNormals", 1);
		forwardSP.Set1i("useCustomColour", 1);
		forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(1.f), 1.f));
		meshes[(int)MeshType::SpriteAni]->SetModel(GetTopModel());
		meshes[(int)MeshType::SpriteAni]->Render(forwardSP);
		forwardSP.Set1i("useCustomColour", 0);
		forwardSP.Set1i("noNormals", 0);
	PopModel();

	glBlendFunc(GL_ONE, GL_ZERO);
}

glm::mat4 MyScene::Translate(const glm::vec3& translate){
	return glm::translate(glm::mat4(1.f), translate);
}

glm::mat4 MyScene::Rotate(const glm::vec4& rotate){
	return glm::rotate(glm::mat4(1.f), glm::radians(rotate.w), glm::vec3(rotate));
}

glm::mat4 MyScene::Scale(const glm::vec3& scale){
	return glm::scale(glm::mat4(1.f), scale);
}

glm::mat4 MyScene::GetTopModel() const{
	return modelStack.empty() ? glm::mat4(1.f) : modelStack.top();
}

void MyScene::PushModel(const std::vector<glm::mat4>& vec) const{
	modelStack.push(modelStack.empty() ? glm::mat4(1.f) : modelStack.top());
	const size_t& size = vec.size();
	for(size_t i = 0; i < size; ++i){
		modelStack.top() *= vec[i];
	}
}

void MyScene::PopModel() const{
	if(!modelStack.empty()){
		modelStack.pop();
	}
}