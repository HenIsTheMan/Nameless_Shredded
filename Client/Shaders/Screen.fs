#version 330 core
out vec4 fragColour;

in vec2 TexCoords;
uniform sampler2D screenTexSampler;

void main(){
	fragColour = vec4(texture(screenTexSampler, TexCoords).rgb, 1.f);
}