#pragma once

#include "Geo/Mesh.h"

class TextRenderer final{
public:
	enum struct TextAlignment: int{
		Left,
		Center,
		Right,
		Amt
	};

	struct TextAttribs final{
		str text = str();
		float x = 0.0f;
		float y = 0.0f;
		float scaleFactor = 1.0f;
		glm::vec4 colour = glm::vec4(1.0f);
		uint texRefID = 0;
		TextAlignment alignment = TextAlignment::Left;
		float charSpacing = 0.0f;
	};

	inline TextRenderer();
	inline ~TextRenderer();

	inline void RenderText(ShaderProg& SP, const TextAttribs& attribs);
private:
	Mesh* mesh;
	glm::mat4 projection;
};

#include "TextRenderer.inl"