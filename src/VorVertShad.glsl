#version 330 core

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 a;
layout (location = 3) in vec3 b;
layout (location = 4) in vec3 c;

uniform mat4 modelview;
uniform mat4 projection;

flat out vec3 mvA;
flat out vec3 mvB;
flat out vec3 mvC;
out vec3 posF;
out vec3 normalF;



void main(){
    gl_Position=projection*modelview*vec4(vert,1.0);
    posF = vec3(modelview*vec4(vert,1.0));
    mvA = vec3(modelview*vec4(a,1.0));
    mvB = vec3(modelview*vec4(b,1.0));
    mvC = vec3(modelview*vec4(c,1.0));


    //important so the normal is corrected with certain
    //transformations
    mat3 normalMatrix = transpose(inverse(mat3(modelview)));
    normalF = normalMatrix * normal;
}
