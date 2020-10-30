#pragma once

#include "../../../../../Core.h"

class Cam final{
public:
	Cam();
	Cam(const glm::vec3& pos, const glm::vec3& target, const float& aspectRatio, const float& spd);

	glm::vec3 CalcFront(const bool& normalised = true) const;
	glm::vec3 CalcRight() const;
	glm::vec3 CalcUp() const;
	glm::mat4 LookAt() const;
	void Update(float dt, const int& up, const int& down, const int& left, const int& right, const int& front, const int& back);

	void Reset();
	void ResetAspectRatio();
	void ResetSpd();
	void ResetPos();
	void ResetTarget();

	///Getters
	const float& GetAspectRatio() const;
	const float& GetSpd() const;
	const glm::vec3& GetPos() const;
	const glm::vec3& GetTarget() const;
	const float& GetDefaultAspectRatio() const;
	const float& GetDefaultSpd() const;
	const glm::vec3& GetDefaultPos() const;
	const glm::vec3& GetDefaultTarget() const;

	///Setters
	void SetAspectRatio(const float& aspectRatio);
	void SetSpd(const float& spd);
	void SetPos(const glm::vec3 & pos);
	void SetTarget(const glm::vec3 & target);
	void SetDefaultAspectRatio(const float& defaultAspectRatio);
	void SetDefaultSpd(const float& defaultSpd);
	void SetDefaultPos(const glm::vec3 & defaultPos);
	void SetDefaultTarget(const glm::vec3 & defaultTarget);
private:
	float aspectRatio;
	float spd;
	glm::vec3 pos;
	glm::vec3 target;

	float defaultAspectRatio;
	float defaultSpd;
	glm::vec3 defaultPos;
	glm::vec3 defaultTarget;
};