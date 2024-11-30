#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 instanceColor;
layout(location = 2) in vec2 texCoords;
out vec3 fragColor;
out vec2 fragTexCoords;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    fragColor = instanceColor;
    fragTexCoords = texCoords;
}
