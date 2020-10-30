#pragma once

#include <Windows.h>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/color_space.hpp>
#include <glm/gtx/quaternion.hpp>

#include <functional>
#include <fstream>
#include <map>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#define STR(text) #text

#pragma comment(lib, "glfw3.lib")

typedef const char* cstr;
typedef unsigned int uint;
typedef std::string str;