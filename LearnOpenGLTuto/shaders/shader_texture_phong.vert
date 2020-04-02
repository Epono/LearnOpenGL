#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec3 OurColor;
out vec2 TexCoord;
out vec3 FragNormal;
out vec3 FragPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   // So that FragPosition is in World Space
   FragPosition = vec3(model * vec4(aPos, 1.0));
   gl_Position = projection * view * vec4(FragPosition, 1.0);
   OurColor = aColor;
   TexCoord = aTexCoord;
   FragNormal = mat3(transpose(inverse(model))) * aNormal;
}