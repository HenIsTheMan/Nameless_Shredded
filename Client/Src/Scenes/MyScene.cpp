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
		new SpriteAni(3, 5),
		new SpriteAni(1, 5),
		new SpriteAni(21, 13),
		new SpriteAni(21, 13),
		new SpriteAni(21, 13),
		new SpriteAni(21, 13),
	},
	forwardSP{"Shaders/Forward.vs", "Shaders/Forward.fs"},
	screenSP{"Shaders/Quad.vs", "Shaders/Screen.fs"},
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
	isDay(false),
	dayNightBT(0.0f),
	objPool(new ObjPool<Entity>()),
	SkeleMove(nullptr),
	SkeleThrust(nullptr),
	ReptileMove(nullptr),
	ReptileShoot(nullptr),
	BoyMove(nullptr),
	BoySlash(nullptr),
	OrcMove(nullptr),
	OrcSmack(nullptr)
{
}

MyScene::~MyScene(){
	for(int i = 0; i < (int)MeshType::Amt; ++i){
		if(meshes[i]){
			delete meshes[i];
			meshes[i] = nullptr;
		}
	}

	if(objPool){
		delete objPool;
		objPool = nullptr;
	}
}

void MyScene::Init(){
	glGetIntegerv(GL_POLYGON_MODE, &polyMode);

	objPool->CreateObjs(10000);

	meshes[(int)MeshType::DayBG]->AddTexMap({"Imgs/DayBG.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(meshes[(int)MeshType::DayBG])->AddAni("DayBG", 0, 12);
	static_cast<SpriteAni*>(meshes[(int)MeshType::DayBG])->Play("DayBG", -1, 1.0f);

	meshes[(int)MeshType::NightBG]->AddTexMap({"Imgs/NightBG.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(meshes[(int)MeshType::NightBG])->AddAni("NightBG", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::NightBG])->Play("NightBG", -1, 0.33f);

	meshes[(int)MeshType::Skele]->AddTexMap({"Imgs/Skele.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleStaticUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleStaticDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleStaticLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleStaticRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleMoveUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleMoveDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleMoveLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleMoveRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleThrustUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleThrustDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleThrustLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleThrustRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleHibernate", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->AddAni("SkeleFacePlant", 0, 4);
	SkeleMove = [this](const str& aniName)->void{
		static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->Play(aniName, -1, 2.0f);
	};
	SkeleThrust = [this](const str& aniName)->void{
		static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->Play(aniName, -1, 1.0f);
	};

	meshes[(int)MeshType::Reptile]->AddTexMap({"Imgs/Reptile.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileStaticUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileStaticDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileStaticLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileStaticRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileMoveUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileMoveDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileMoveLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileMoveRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileShootUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileShootDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileShootLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileShootRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileProcreate", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->AddAni("ReptileFacePlant", 0, 4);
	ReptileMove = [this](const str& aniName)->void{
		static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->Play(aniName, -1, 2.0f);
	};
	ReptileShoot = [this](const str& aniName)->void{
		static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->Play(aniName, -1, 1.0f);
	};

	meshes[(int)MeshType::Boy]->AddTexMap({"Imgs/Boy.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoyStaticUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoyStaticDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoyStaticLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoyStaticRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoyMoveUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoyMoveDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoyMoveLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoyMoveRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoySlashUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoySlashDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoySlashLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoySlashRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoyHeal", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->AddAni("BoyFacePlant", 0, 4);
	BoyMove = [this](const str& aniName)->void{
		static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->Play(aniName, -1, 2.0f);
	};
	BoySlash = [this](const str& aniName)->void{
		static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->Play(aniName, -1, 1.0f);
	};

	meshes[(int)MeshType::Orc]->AddTexMap({"Imgs/Orc.png", Mesh::TexType::Diffuse, 0});
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcStaticUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcStaticDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcStaticLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcStaticRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcMoveUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcMoveDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcMoveLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcMoveRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcSmackUp", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcSmackDown", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcSmackLeft", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcSmackRight", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcShield", 0, 4);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->AddAni("OrcFacePlant", 0, 4);
	OrcMove = [this](const str& aniName)->void{
		static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->Play(aniName, -1, 2.0f);
	};
	OrcSmack = [this](const str& aniName)->void{
		static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->Play(aniName, -1, 1.0f);
	};
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

	static_cast<SpriteAni*>(meshes[(int)MeshType::DayBG])->Update(dt);
	static_cast<SpriteAni*>(meshes[(int)MeshType::NightBG])->Update(dt);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Skele])->Update(dt);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Reptile])->Update(dt);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Boy])->Update(dt);
	static_cast<SpriteAni*>(meshes[(int)MeshType::Orc])->Update(dt);

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

	if(dayNightBT <= elapsedTime){
		isDay = !isDay;
		dayNightBT = elapsedTime + 7.0f;
	}

	const float gridWidth = grid.CalcWidth();
	const float gridHeight = grid.CalcHeight();

	const float xOffset = ((float)winWidth - gridWidth) * 0.5f;
	const float yOffset = ((float)winHeight - gridHeight) * 0.5f;
	const float unitX = gridCellWidth + gridLineThickness;
	const float unitY = gridCellHeight + gridLineThickness;

	const float mouseRow = std::floor((winHeight - lastY - yOffset - gridLineThickness * 0.5f) / unitY);
	const float mouseCol = std::floor((lastX - xOffset - gridLineThickness * 0.5f) / unitX);
	const float xTranslate = mouseCol * unitX
		+ xOffset
		+ gridCellWidth * 0.5f
		+ gridLineThickness;
	const float yTranslate = mouseRow * unitY
		+ yOffset
		+ gridCellHeight * 0.5f
		+ gridLineThickness;

	if(lastX > xOffset + gridLineThickness * 0.5f && lastX < xOffset + gridWidth - gridLineThickness * 0.5f
		&& lastY > yOffset + gridLineThickness * 0.5f && lastY < yOffset + gridHeight - gridLineThickness * 0.5f){
		if(LMB){
			Entity* const& entity = objPool->RetrieveInactiveObj();
			entity->RetrievePos() = glm::vec3(xTranslate, yTranslate, 0.0f);
		} else if(RMB){
			//grid.SetData(EntityType::Null, (ptrdiff_t)mouseRow, (ptrdiff_t)mouseCol);
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

	RenderGrid();
	RenderGridBG();
	RenderTranslucentBlock();
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

void MyScene::RenderGrid(){
	const float gridWidth = grid.CalcWidth();
	const float gridHeight = grid.CalcHeight();

	const float xOffset = ((float)winWidth - gridWidth) * 0.5f + gridLineThickness * 0.5f;
	const float yOffset = ((float)winHeight - gridHeight) * 0.5f + gridLineThickness * 0.5f;

	const int amtOfVertLines = grid.CalcAmtOfVertLines();
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

	const int amtOfHorizLines = grid.CalcAmtOfHorizLines();
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

void MyScene::RenderGridBG(){
	modelStack.PushModel({
		modelStack.Translate(glm::vec3(winWidth * 0.5f, winHeight * 0.5f, 0.0f)),
		modelStack.Scale(glm::vec3(grid.CalcWidth(), grid.CalcHeight(), 1.0f)),
	});
		forwardSP.Set4fv("customColour", glm::vec4(glm::vec3(0.2f), 1.f));
		meshes[(int)MeshType::Quad]->SetModel(modelStack.GetTopModel());
		meshes[(int)MeshType::Quad]->Render(forwardSP);
	modelStack.PopModel();
}

void MyScene::RenderEntities(){
	const std::vector<std::pair<bool, Entity*>>& entityPool = objPool->GetObjPool();
	const size_t entityPoolSize = entityPool.size();
	
	for(size_t i = 0; i < entityPoolSize; ++i){
		if(entityPool[i].first){
			const Entity* const& entity = entityPool[i].second;
			const glm::vec3& entityPos = entity->GetPos();
			modelStack.PushModel({
				modelStack.Translate(glm::vec3(
					entityPos.x,
					entityPos.y,
					0.1f
				)),
				modelStack.Scale(glm::vec3(gridCellWidth, gridCellHeight, 1.0f)),
			});
				forwardSP.Set4fv("customColour", glm::vec4(1.0f));
				meshes[(int)MeshType::QuadWithTex]->SetModel(modelStack.GetTopModel());
				meshes[(int)MeshType::QuadWithTex]->Render(forwardSP);
			modelStack.PopModel();
		}
	}
}

void MyScene::RenderTranslucentBlock(){
	const float gridWidth = grid.CalcWidth();
	const float gridHeight = grid.CalcHeight();

	const float xOffset = ((float)winWidth - gridWidth) * 0.5f;
	const float yOffset = ((float)winHeight - gridHeight) * 0.5f;

	if(lastX > xOffset + gridLineThickness * 0.5f && lastX < xOffset + gridWidth - gridLineThickness * 0.5f
		&& lastY > yOffset + gridLineThickness * 0.5f && lastY < yOffset + gridHeight - gridLineThickness * 0.5f){
		const float unitX = gridCellWidth + gridLineThickness;
		const float unitY = gridCellHeight + gridLineThickness;

		const float mouseRow = std::floor((winHeight - lastY - yOffset - gridLineThickness * 0.5f) / unitY);
		const float mouseCol = std::floor((lastX - xOffset - gridLineThickness * 0.5f) / unitX);
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
		forwardSP.Set4fv("customColour", glm::vec4(1.0f));
		if(isDay){
			meshes[(int)MeshType::DayBG]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::DayBG]->Render(forwardSP);
		} else{
			meshes[(int)MeshType::NightBG]->SetModel(modelStack.GetTopModel());
			meshes[(int)MeshType::NightBG]->Render(forwardSP);
		}
	modelStack.PopModel();
}