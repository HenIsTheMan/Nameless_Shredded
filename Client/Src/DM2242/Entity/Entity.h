#pragma once

#include "EntityAttribs.hpp"

class Entity final{
public:
	Entity();
	Entity(const EntityAttribs& attribs);
	~Entity() = default;
private:
	EntityAttribs attribs;
};