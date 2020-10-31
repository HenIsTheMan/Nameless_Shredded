#pragma once

#include "Plat/Win/App/Scene/Cam/Cam.h"
#include "Geo/Mesh.h"
#include "Graphics/Light.h"
#include "Graphics/ShaderProg.h"

class MyScene final{
public:
	MyScene();
	~MyScene();
	bool Init();
	void Update(float dt);
	void ForwardRender();
	void DefaultRender(const uint& screenTexRefID);
private:
	Cam cam;

	enum struct MeshType{
		Quad,
		SpriteAni,
		Amt
	};
	Mesh* meshes[(int)MeshType::Amt];

	ShaderProg forwardSP;
	ShaderProg screenSP;

	std::vector<Light*> ptLights;
	std::vector<Light*> directionalLights;
	std::vector<Light*> spotlights;

	glm::mat4 view;
	glm::mat4 projection;
	//std::vector<Mesh::BatchRenderParams> params;

	float elapsedTime;
	int polyMode;
	mutable std::stack<glm::mat4> modelStack;
	glm::mat4 Translate(const glm::vec3& translate);
	glm::mat4 Rotate(const glm::vec4& rotate);
	glm::mat4 Scale(const glm::vec3& scale);
	glm::mat4 GetTopModel() const;
	void PushModel(const std::vector<glm::mat4>& vec) const;
	void PopModel() const;
};