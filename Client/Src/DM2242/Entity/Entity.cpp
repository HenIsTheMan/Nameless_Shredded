#include "Entity.h"

Entity::Entity():
	Entity(EntityAttribs())
{
}

Entity::Entity(const EntityAttribs& attribs):
	attribs(attribs)
{
}