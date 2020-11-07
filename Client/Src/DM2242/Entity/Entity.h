#pragma once

#include "EntityAttribs.hpp"

class Entity final{
public:
	Entity();
	Entity(const EntityAttribs& attribs);
	~Entity() = default;

	glm::vec3& RetrievePos();

	///Getter
	const glm::vec3& GetPos() const;
private:
	EntityAttribs im_Attribs;
};