#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aNormal;

out vec4 vertexColor;
out vec3 FragNormal;
out vec3 FragPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPosition = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPosition, 1.0);
    FragNormal = mat3(transpose(inverse(model))) * aNormal;
}