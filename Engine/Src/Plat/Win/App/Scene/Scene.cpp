#include "Scene.h"

void (*Scene::im_InCtor)() = nullptr;
void (*Scene::im_InDtor)() = nullptr;
void (*Scene::im_Init)() = nullptr;
void (*Scene::im_FixedUpdate)(float dt) = nullptr;
void (*Scene::im_Update)(float dt) = nullptr;
void (*Scene::im_LateUpdate)(float dt) = nullptr;
void (*Scene::im_PreRender)() = nullptr;
void (*Scene::im_Render)() = nullptr;
void (*Scene::im_PostRender)() = nullptr;

void Scene::SetInCtor(void (*inCtor)()){
	im_InCtor = inCtor;
}

void Scene::SetInDtor(void (*inDtor)()){
	im_InDtor = inDtor;
}

void Scene::SetInit(void (*init)()){
	im_Init = init;
}

void Scene::SetFixedUpdate(void (*fixedUpdate)(float dt)){
	im_FixedUpdate = fixedUpdate;
}

void Scene::SetUpdate(void (*update)(float dt)){
	im_Update = update;
}

void Scene::SetLateUpdate(void (*lateUpdate)(float dt)){
	im_LateUpdate = lateUpdate;
}

void Scene::SetPreRender(void (*preRender)()){
	im_PreRender = preRender;
}

void Scene::SetRender(void (*render)()){
	im_Render = render;
}

void Scene::SetPostRender(void (*postRender)()){
	im_PostRender = postRender;
}

void Scene::InCtor(){
	if(im_InCtor){
		im_InCtor();
	}
}

void Scene::InDtor(){
	if(im_InDtor){
		im_InDtor();
	}
}

void Scene::Init(){
	if(im_Init){
		im_Init();
	}
}

void Scene::FixedUpdate(float dt){
	if(im_FixedUpdate){
		im_FixedUpdate(dt);
	}
}

void Scene::Update(float dt){
	if(im_Update){
		im_Update(dt);
	}
}

void Scene::LateUpdate(float dt){
	if(im_LateUpdate){
		im_LateUpdate(dt);
	}
}

void Scene::PreRender(){
	if(im_PreRender){
		im_PreRender();
	}
}

void Scene::Render(){
	if(im_Render){
		im_Render();
	}
}

void Scene::PostRender(){
	if(im_PostRender){
		im_PostRender();
	}
}