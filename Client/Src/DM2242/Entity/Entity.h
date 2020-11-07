#pragma once

#include "EntityAttribs.hpp"

class Entity final{
public:
	Entity();
	Entity(const EntityAttribs& attribs);
	~Entity() = default;

	EntityType& RetrieveType();
	glm::vec3& RetrievePos();

	///Getters
	const EntityType& GetType() const;
	const glm::vec3& GetPos() const;
private:
	EntityAttribs im_Attribs;
};