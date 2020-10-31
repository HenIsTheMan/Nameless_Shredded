#include "YesScene.h"

bool YesScene::fullscreen = false;

float YesScene::elapsedTime = 0.0f;

const GLFWvidmode* YesScene::mode = nullptr;
GLFWwindow* YesScene::win = nullptr;

uint YesScene::FBORefIDs[(int)FBO::Amt]{};
uint YesScene::texRefIDs[(int)Tex::Amt]{};
uint YesScene::RBORefIDs[1]{};

MyScene* YesScene::scene = new MyScene();

extern bool endLoop;
extern int optimalWinXPos;
extern int optimalWinYPos;
extern int optimalWinWidth;
extern int optimalWinHeight;
extern int winWidth;
extern int winHeight;

void YesScene::InCtor(){
	if(!InitAPI(win)){
		(void)puts("Failed to init API\n");
		endLoop = true;
	}
}

void YesScene::InDtor(){
	delete scene;

	glDeleteTextures(sizeof(texRefIDs) / sizeof(texRefIDs[0]), texRefIDs);
	glDeleteRenderbuffers(sizeof(RBORefIDs) / sizeof(RBORefIDs[0]), RBORefIDs);
	glDeleteFramebuffers(sizeof(FBORefIDs) / sizeof(FBORefIDs[0]), FBORefIDs);

	glfwTerminate(); //Clean/Del all GLFW's resources that were allocated
}

void YesScene::Init(){
	//Stencil buffer usually contains 8 bits per stencil value that amts to 256 diff stencil values per pixel
	//Use stencil buffer operations to write to the stencil buffer when rendering frags (read stencil values in the same or following frame(s) to pass or discard frags based on their stencil value)
	glEnable(GL_STENCIL_TEST); //Discard frags based on frags of other drawn objs in the scene

	glEnable(GL_DEPTH_TEST); //Done in screen space after the frag shader has run and after the stencil test //(pass ? frag is rendered and depth buffer is updated with new depth value : frag is discarded)

	glEnable(GL_BLEND); //Colour resulting from blend eqn replaces prev colour stored in the colour buffer

	glEnable(GL_CULL_FACE); //Specify vertices in CCW winding order so front faces are rendered in CW order while... //Actual winding order is calculated at the rasterization stage after the vertex shader has run //Vertices are then seen from the viewer's POV

	glEnable(GL_PROGRAM_POINT_SIZE);
	//glEnable(GL_MULTISAMPLE); //Enabled by default on most OpenGL drivers //Multisample buffer (stores a given amt of sample pts per pixel) for MSAA
	//Multisampling algs are implemented in the rasterizer (combination of all algs and processes that transform vertices of primitives into frags, frags are bound by screen resolution unlike vertices so there is almost nvr a 1-on-1 mapping between... and it must decide at what screen coords will each frag of each interpolated vertex end up at) of the OpenGL drivers
	//Poor screen resolution leads to aliasing as the limited amt of screen pixels causes some pixels to not be rendered along an edge of a fig //Colour output is stored directly in the framebuffer if pixel is fully covered and blending is disabled

	//glEnable(GL_FRAMEBUFFER_SRGB); //Colours from sRGB colour space are gamma corrected after each frag shader run before they are stored in colour buffers of all framebuffers
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glPointSize(10.f);
	glLineWidth(20.f);

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	glGenFramebuffers(sizeof(FBORefIDs) / sizeof(FBORefIDs[0]), FBORefIDs);
	glGenTextures(sizeof(texRefIDs) / sizeof(texRefIDs[0]), texRefIDs);
	glGenRenderbuffers(sizeof(RBORefIDs) / sizeof(RBORefIDs[0]), RBORefIDs);

	glBindFramebuffer(GL_FRAMEBUFFER, FBORefIDs[(int)Tex::Normal]);
	int currTexRefID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &currTexRefID);
	glBindTexture(GL_TEXTURE_2D, texRefIDs[(int)Tex::Normal]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2048, 2048, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (int)Tex::Normal, GL_TEXTURE_2D, texRefIDs[(int)Tex::Normal], 0);
	glBindTexture(GL_TEXTURE_2D, currTexRefID);

	glBindRenderbuffer(GL_RENDERBUFFER, RBORefIDs[0]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 2048, 2048);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBORefIDs[0]);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		(void)printf(STR(FBO::Normal));
		return (void)puts(" is incomplete!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	scene->Init();
}

void YesScene::FixedUpdate(float dt){
}

void YesScene::Update(float dt){
	if(glfwWindowShouldClose(win)){
		endLoop = true;
		return;
	}

	elapsedTime += dt;
	static float toggleFullscreenBT = 0.f;
	if(Key(VK_F1) && toggleFullscreenBT <= elapsedTime){
		if(fullscreen){
			glfwSetWindowMonitor(win, 0, optimalWinXPos, optimalWinYPos, optimalWinWidth, optimalWinHeight, GLFW_DONT_CARE);
			fullscreen = false;
		} else{
			glfwSetWindowMonitor(win, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
			fullscreen = true;
		}
		toggleFullscreenBT = elapsedTime + .5f;
	}

	scene->Update(dt);
}

void YesScene::LateUpdate(float dt){
}

void YesScene::PreRender(){
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //Frags update stencil buffer with their ref value when... //++params and options??
	//glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_DST_ALPHA, GL_DST_ALPHA);
	//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_SUBTRACT);
}

void YesScene::Render(){
	glViewport(0, 0, 2048, 2048);
	glBindFramebuffer(GL_FRAMEBUFFER, FBORefIDs[(int)FBO::Normal]);
	glClearColor(1.0f, 0.82f, 0.86f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->ForwardRender();

	glViewport(0, 0, winWidth, winHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 0.52f, 0.23f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->DefaultRender(texRefIDs[(int)Tex::Normal]);
}

void YesScene::PostRender(){
	glfwSwapBuffers(win); //Swap the large 2D colour buffer containing colour values for each pixel in GLFW's window
	glfwPollEvents(); //Check for triggered events and call corresponding functions registered via callback methods
}