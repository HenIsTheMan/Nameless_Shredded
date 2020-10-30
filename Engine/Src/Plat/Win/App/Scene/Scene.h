#pragma once

class App;

class Scene final{ //Static class
	Scene() = delete;
	~Scene() = delete;
	Scene(const Scene&) = delete;
	Scene(Scene&&) noexcept = delete;
	Scene& operator=(const Scene&) = delete;
	Scene& operator=(Scene&&) noexcept = delete;

	friend App;
public:
	///Setters
	static void SetInCtor(void (*inCtor)());
	static void SetInDtor(void (*inDtor)());
	static void SetInit(void (*init)());
	static void SetFixedUpdate(void (*fixedUpdate)(float dt));
	static void SetUpdate(void (*update)(float dt));
	static void SetLateUpdate(void (*lateUpdate)(float dt));
	static void SetPreRender(void (*preRender)());
	static void SetRender(void (*render)());
	static void SetPostRender(void (*postRender)());
private:
	static void (*im_InCtor)();
	static void (*im_InDtor)();
	static void (*im_Init)();
	static void (*im_FixedUpdate)(float dt);
	static void (*im_Update)(float dt);
	static void (*im_LateUpdate)(float dt);
	static void (*im_PreRender)();
	static void (*im_Render)();
	static void (*im_PostRender)();

	static void InCtor();
	static void InDtor();
	static void Init();
	static void FixedUpdate(float dt);
	static void Update(float dt);
	static void LateUpdate(float dt);
	static void PreRender();
	static void Render();
	static void PostRender();
};