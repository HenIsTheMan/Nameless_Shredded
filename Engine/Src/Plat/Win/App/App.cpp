#include "App.h"

#include "../../../Core.h"

App::App():
	dt(0.0f),
	lastFrameTime(0.0f)
{
	Scene::InCtor();
}

App::~App(){
	Scene::InDtor();
}

void App::Init(){
	Scene::Init();
}

void App::Update(){
	float currFrameTime = (float)glfwGetTime();
	dt = currFrameTime - lastFrameTime;
	lastFrameTime = currFrameTime;

	//Scene::FixedUpdate(dt);
	Scene::Update(dt);
	Scene::LateUpdate(dt);
}

void App::Render(){
	Scene::PreRender();
	Scene::Render();
	Scene::PostRender();
}