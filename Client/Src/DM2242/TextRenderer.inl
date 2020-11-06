extern int winWidth;
extern int winHeight;

TextRenderer::TextRenderer():
    mesh(new Mesh(Mesh::MeshType::Amt, GL_TRIANGLES, {
        {"Imgs/FiraMono.png", Mesh::TexType::Diffuse, 0},
    })),
    projection(glm::ortho(0.0f, (float)winWidth, 0.0f, (float)winHeight))
{
}

TextRenderer::~TextRenderer(){
    if(mesh){
        delete mesh;
        mesh = nullptr;
    }
}

void TextRenderer::RenderText(ShaderProg& SP, const TextAttribs& attribs){
    SP.Set1i("noNormals", 1);
    SP.Set1i("useCustomColour", 1);
    SP.Set4fv("customColour", attribs.colour);

    const size_t strLen = attribs.text.length();
	for(unsigned i = 0; i < strLen; ++i){
        glm::mat4 transform = glm::mat4();
        if(attribs.alignment == TextAlignment::Right){
            const float offset = (float)strLen - attribs.miniOffset;
            transform = glm::translate(transform, glm::vec3(attribs.x + attribs.charSpacing * (i - offset), attribs.y, attribs.z));
        } else if(attribs.alignment == TextAlignment::Center){
            const float offset = ((float)strLen - attribs.miniOffset) * 0.5f;
            transform = glm::translate(transform, glm::vec3(attribs.x + attribs.charSpacing * (i - offset), attribs.y, attribs.z));
        } else{
            transform = glm::translate(transform, glm::vec3(attribs.x + attribs.charSpacing * i, attribs.y, attribs.z));
        }
        transform = glm::scale(transform, glm::vec3(attribs.scaleFactor, attribs.scaleFactor, 1.0f));
        transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f)); //Offset due to size of each quad
		mesh->SetModel(transform);

		mesh->RenderText(SP, (unsigned)attribs.text[i] * 6, true);
	}
    SP.Set1i("useCustomColour", 0);
    SP.Set1i("noNormals", 0);
}