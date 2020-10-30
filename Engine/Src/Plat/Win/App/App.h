#pragma once

#include "Scene/Scene.h"

class App final{
public:
	App();
	~App();

	void Init();
	void Update();
	void Render();
private:
	float dt;
	float lastFrameTime;
};