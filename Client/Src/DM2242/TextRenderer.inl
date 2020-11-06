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
    //SP.Use();
    //SP.Set4fv("textColour", attribs.colour);
    //SP.SetMat4fv("projection", &projection[0][0]);

    //Centralise

	for(unsigned i = 0; i < attribs.text.length(); ++i){
        glm::mat4 transform = glm::mat4();
        transform = glm::translate(transform, glm::vec3((float)winWidth * 0.5f + attribs.charSpacing * i + attribs.x, (float)winHeight * 0.5f + attribs.y, 0.0f));
        transform = glm::scale(transform, glm::vec3(attribs.scaleFactor, attribs.scaleFactor, 1.0f));
		mesh->SetModel(transform);

		mesh->RenderText(SP, (unsigned)attribs.text[i] * 6, true);
	}
}