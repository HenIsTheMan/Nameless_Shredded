#pragma once

#include "EntityType.hpp"

struct EntityAttribs final{
	EntityType type = EntityType::Null;
};

class Entity final{
public:
	Entity();
	Entity(const EntityAttribs& attribs);
	~Entity() = default;
private:
	EntityAttribs attribs;
};