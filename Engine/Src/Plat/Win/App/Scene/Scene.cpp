#include "Scene.h"

void (*Scene::sm_InCtor)() = nullptr;
void (*Scene::sm_InDtor)() = nullptr;
void (*Scene::sm_Init)() = nullptr;
void (*Scene::sm_FixedUpdate)(float dt) = nullptr;
void (*Scene::sm_Update)(float dt) = nullptr;
void (*Scene::sm_LateUpdate)(float dt) = nullptr;
void (*Scene::sm_PreRender)() = nullptr;
void (*Scene::sm_Render)() = nullptr;
void (*Scene::sm_PostRender)() = nullptr;

void Scene::SetInCtor(void (*inCtor)()){
	sm_InCtor = inCtor;
}

void Scene::SetInDtor(void (*inDtor)()){
	sm_InDtor = inDtor;
}

void Scene::SetInit(void (*init)()){
	sm_Init = init;
}

void Scene::SetFixedUpdate(void (*fixedUpdate)(float dt)){
	sm_FixedUpdate = fixedUpdate;
}

void Scene::SetUpdate(void (*update)(float dt)){
	sm_Update = update;
}

void Scene::SetLateUpdate(void (*lateUpdate)(float dt)){
	sm_LateUpdate = lateUpdate;
}

void Scene::SetPreRender(void (*preRender)()){
	sm_PreRender = preRender;
}

void Scene::SetRender(void (*render)()){
	sm_Render = render;
}

void Scene::SetPostRender(void (*postRender)()){
	sm_PostRender = postRender;
}

void Scene::InCtor(){
	if(sm_InCtor){
		sm_InCtor();
	}
}

void Scene::InDtor(){
	if(sm_InDtor){
		sm_InDtor();
	}
}

void Scene::Init(){
	if(sm_Init){
		sm_Init();
	}
}

void Scene::FixedUpdate(float dt){
	if(sm_FixedUpdate){
		sm_FixedUpdate(dt);
	}
}

void Scene::Update(float dt){
	if(sm_Update){
		sm_Update(dt);
	}
}

void Scene::LateUpdate(float dt){
	if(sm_LateUpdate){
		sm_LateUpdate(dt);
	}
}

void Scene::PreRender(){
	if(sm_PreRender){
		sm_PreRender();
	}
}

void Scene::Render(){
	if(sm_Render){
		sm_Render();
	}
}

void Scene::PostRender(){
	if(sm_PostRender){
		sm_PostRender();
	}
}