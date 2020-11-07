#pragma once

enum struct EntityType: int{
	Null,
	Block,
	Skele,
	Reptile,
	Boy,
	Orc,
	Amt
};

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