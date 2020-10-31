#pragma once

#include "Global/GlobalFuncs.h"
#include "MyScene.h"

class YesScene final{ //Static class
	YesScene() = delete;
	~YesScene() = delete;
	YesScene(const YesScene&) = delete;
	YesScene(YesScene&&) noexcept = delete;
	YesScene& operator=(const YesScene&) = delete;
	YesScene& operator=(YesScene&&) noexcept = delete;
public:
	static void InCtor();
	static void InDtor();
	static void Init();
	static void FixedUpdate(float dt);
	static void Update(float dt);
	static void LateUpdate(float dt);
	static void PreRender();
	static void Render();
	static void PostRender();
private:
	enum struct FBO{
		Normal,
		Amt
	};
	enum struct Tex{
		Normal,
		Amt
	};

	static bool fullscreen;

	static float elapsedTime;

	static const GLFWvidmode* mode;
	static GLFWwindow* win;

	static uint FBORefIDs[(int)FBO::Amt];
	static uint texRefIDs[(int)Tex::Amt];
	static uint RBORefIDs[1];

	static MyScene* scene;
};