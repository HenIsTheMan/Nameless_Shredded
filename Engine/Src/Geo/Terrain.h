#pragma once
#include "Mesh.h"

class Terrain final: public Mesh{
public:
	Terrain();
	Terrain(cstr const& fPath, const float& hTile, const float& vTile);
	~Terrain() = default;

	float GetHeightAtPt(const float& x, const float& z) const;
	void Render(ShaderProg& SP, const bool& autoConfig = true) override;
private:
	cstr terrainPath;
	float hTile;
	float vTile;
	std::vector<unsigned char> data;

	bool Load();
	void Create();
};