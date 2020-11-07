#pragma once

#include "Core.h"

#include "EntityType.hpp"

struct EntityAttribs final{
	EntityType im_Type = EntityType::Null;
	glm::vec3 im_Pos = glm::vec3();
};