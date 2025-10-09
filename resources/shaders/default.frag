#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D aTexture;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = texture(aTexture, TexCoord);
}