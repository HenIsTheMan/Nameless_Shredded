#include "Entity.h"

Entity::Entity():
	Entity(EntityAttribs())
{
}

Entity::Entity(const EntityAttribs& attribs):
	im_Attribs(attribs)
{
}

glm::vec3& Entity::RetrievePos(){
	return im_Attribs.im_Pos;
}

const glm::vec3& Entity::GetPos() const{
	return im_Attribs.im_Pos;
}