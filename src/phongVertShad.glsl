#version 330 core
layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 norm;

uniform mat4 modelview;
uniform mat4 projection;

out vec3 PosF;
out vec3 NormalF;

void main(){
    gl_Position=projection*modelview*vec4(vert,1.0);
    PosF= vec3(modelview*vec4(vert,1.0));
    mat3 normalMatrix = transpose(inverse(mat3(modelview)));
    NormalF = normalMatrix * norm;
}

