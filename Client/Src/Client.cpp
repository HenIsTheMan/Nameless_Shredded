#include "Plat/Win/App/App.h"

#include "Scenes/YesScene.h"

extern bool endLoop;

void MainProcess(){
	Scene::SetInCtor(YesScene::InCtor);
	Scene::SetInDtor(YesScene::InDtor);
	Scene::SetInit(YesScene::Init);
	Scene::SetFixedUpdate(YesScene::FixedUpdate);
	Scene::SetUpdate(YesScene::Update);
	Scene::SetLateUpdate(YesScene::LateUpdate);
	Scene::SetPreRender(YesScene::PreRender);
	Scene::SetRender(YesScene::Render);
	Scene::SetPostRender(YesScene::PostRender);

	App* app = new App();
	app->Init();

	while(!endLoop){
		app->Update();
		app->Render();
	}

	delete app;
}

int main(const int&, const char* const* const&){
	if(!InitConsole()){
		return -1;
	}

	std::thread worker(&MainProcess);

	while(!endLoop){
		if(Key(VK_ESCAPE)){
			endLoop = true;
			break;
		}
	}

	worker.join();
}