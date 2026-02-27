#include "A1solution.h"

//STL
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <memory>
#include <array>

//OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
std::string getPhongVertShad() {
    return R"(#version 330 core
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
)";
}

std::string getPhongFragShad() {
    return R"(#version 330 core

in vec3 PosF;
in vec3 NormalF;

uniform vec3 objcol;
uniform vec3 lPos;
uniform vec3 lcol;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float e;

out vec4 col;

void main(){
    vec3 uNorm= normalize(NormalF);
    vec3 ldir=normalize(lPos-PosF);
    float costh=dot(uNorm,ldir);
    float E=max(0.0,costh);
    vec3 Lrd=kd*lcol*E;
    vec3 v=normalize(-PosF);
    vec3 h=normalize(ldir+v);
    float res=pow(max(0.00,dot(uNorm,h)),e);
    vec3 Lrs=(ks*res)*lcol*E;
    vec3 Lra=ka*lcol;
    vec3 Lr=(Lrs+Lrd+Lra)*objcol;
    col=vec4(Lr,1.0);
}
)";
}

std::string getFlatVertShad() {
    return R"(#version 330 core
layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 norm;

uniform mat4 modelview;
uniform mat4 projection;

out vec3 PosF;
flat out vec3 NormalF;

void main(){
    gl_Position=projection*modelview*vec4(vert,1.0);
    PosF= vec3(modelview*vec4(vert,1.0));
}
)";
}

std::string getFlatFragShad() {
    return R"(#version 330 core

in vec3 PosF;

uniform vec3 objcol;
uniform vec3 lPos;
uniform vec3 lcol;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float e;

out vec4 col;

void main(){
    vec3 xDir= dFdx(PosF);
    vec3 yDir=dFdy(PosF);
    vec3 fNorm=normalize(cross(xDir,yDir));
    vec3 uNorm= normalize(fNorm);
    vec3 ldir=normalize(lPos-PosF);
    float costh=dot(uNorm,ldir);
    float E=max(0.0,costh);
    vec3 Lrd=kd*lcol*E;
    vec3 v=normalize(-PosF);
    vec3 h=normalize(ldir+v);
    float res=pow(max(0.00,dot(uNorm,h)),e);
    vec3 Lrs=(ks*res)*lcol*E;
    vec3 Lra=ka*lcol;
    vec3 Lr=(Lrs+Lrd+Lra)*objcol;
    col=vec4(Lr,1.0);
}
)";
}

std::string getCircleVertShad() {
    return R"(#version 330 core

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 a;
layout (location = 3) in vec3 b;
layout (location = 4) in vec3 c;

uniform mat4 modelview;
uniform mat4 projection;

out vec3 mvA;
out vec3 mvB;
out vec3 mvC;
out vec3 posF;
out vec3 normalF;

void main(){
    gl_Position=projection*modelview*vec4(vert,1.0);
    posF = vec3(modelview*vec4(vert,1.0));
    mvA = vec3(modelview*vec4(a,1.0));
    mvB = vec3(modelview*vec4(b,1.0));
    mvC = vec3(modelview*vec4(c,1.0));
    mat3 normalMatrix = transpose(inverse(mat3(modelview)));
    normalF = normalMatrix * normal;
}
)";
}

std::string getCircleFragShad() {
    return R"(#version 330 core

vec3 kdo=vec3(1.0,0.5,0.5);
vec3 kao=vec3(0.1,0.05,0.05);
vec3 kso=vec3(0.3,0.3,0.3);

vec3 kdi=vec3(0.5,0.5,1.0);
vec3 kai=vec3(0.05,0.05,0.1);
vec3 ksi=vec3(0.0,0.0,0.0);
float e=5.0;

uniform vec3 lPos;
uniform vec3 lcol;
uniform vec3 objcol;

in vec3 mvA;
in vec3 mvB;
in vec3 mvC;
in vec3 posF;
in vec3 normalF;

out vec4 col;

void main(){
    float a = length(mvB-mvC);
    float b = length(mvC-mvA);
    float c = length(mvA-mvB);
    vec3 ic = (a*mvA+b*mvB+c*mvC)/(a+b+c);
    float area = 0.5*length(cross(mvB-mvA,mvC-mvA));
    float s = (a+b+c)*0.5;
    float r = area/s;
    float l = length(posF-ic);
    vec3 kd=kdi;
    vec3 ka=kai;
    vec3 ks=ksi;

    if(l<=r){
        kd=kdo;
        ka=kao;
        ks=kso;
    }

    vec3 uNorm= normalize(normalF);
    vec3 ldir=normalize(lPos-posF);
    float costh=dot(uNorm,ldir);
    float E=max(0.0,costh);
    vec3 Lrd=kd*lcol*E;
    vec3 v=normalize(-posF);
    vec3 h=normalize(ldir+v);
    float res=pow(max(0.00,dot(uNorm,h)),e);
    vec3 Lrs=(ks*res)*lcol*E;
    vec3 Lra=ka*lcol;
    vec3 Lr=(Lrs+Lrd+Lra)*objcol;
    col=vec4(Lr,1.0);
}
)";
}

std::string getVorVertShad() {
    return R"(#version 330 core

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
    mat3 normalMatrix = transpose(inverse(mat3(modelview)));
    normalF = normalMatrix * normal;
}
)";
}

std::string getVorFragShad() {
    return R"(#version 330 core

vec3 kda=vec3(1.0,0.5,0.5);
vec3 kdb=vec3(0.5,1.0,0.5);
vec3 kdc=vec3(0.5,0.5,1.0);

float e=5.0;

uniform vec3 lPos;
uniform vec3 lcol;
uniform vec3 objcol;

flat in vec3 mvA;
flat in vec3 mvB;
flat in vec3 mvC;
in vec3 posF;
in vec3 normalF;

out vec4 col;

void main(){
    float la=distance(mvA,posF);
    float lb=distance(mvB,posF);
    float lc=distance(mvC,posF);
    vec3 kd;
    vec3 ka=vec3(0.1,0.05,0.05);
    vec3 ks=vec3(0.3,0.3,0.3);

    if(la<=lb){
        if(la<=lc){
            kd=kda;
        }else{
            kd=kdc;
        }
    }else{
        if(lc<=lb){
            kd=kdc;
        }else{
            kd=kdb;
        }
    }

    vec3 uNorm= normalize(normalF);
    vec3 ldir=normalize(lPos-posF);
    float costh=dot(uNorm,ldir);
    float E=max(0.0,costh);
    vec3 Lrd=kd*lcol*E;
    vec3 v=normalize(-posF);
    vec3 h=normalize(ldir+v);
    float res=pow(max(0.00,dot(uNorm,h)),e);
    vec3 Lrs=(ks*res)*lcol*E;
    vec3 Lra=ka*lcol;
    vec3 Lr=(Lrs+Lrd+Lra)*objcol;
    col=vec4(Lr,1.0);
}
)";
}

std::string getBlackVertShad() {
    return R"(#version 330 core

layout (location = 0) in vec3 vert;

uniform mat4 modelview;
uniform mat4 projection;

void main() {
    gl_Position = projection * modelview * vec4(vert, 1.0);
}
)";
}

std::string getBlackFragShad() {
    return R"(#version 330 core

out vec4 col;

void main() {
    col = vec4(0.0, 0.0, 0.0, 1.0);
}
)";
}
} // namespace

void A1solution::run(const std::string& fname){
std::ifstream ifs(fname);
if(!ifs.is_open()){
    std::cerr<<"Couldn't open "<<fname<<std::endl;
    exit(EXIT_FAILURE);
}

p_mv(ifs);
p_proj(ifs);
p_dis(ifs);
p_vert(ifs);
p_tri(ifs);
c_nv();
s_rend();

//dump_state();



}

void A1solution::p_mv(is& in){
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            in >> modelview[r][c];
        }
    }
}

void A1solution::p_proj(is& in){
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            in >> projection[r][c];
        }
    }
}

void A1solution::p_dis(is& in){
    in>>width;
    in>>height;
}

void A1solution::p_vert(is& in){
    size_t n;
    in>>n;
    vertices.resize(n);
    for(size_t i=0;i<n;++i){
        in >> vertices[i].x >> vertices[i].y >> vertices[i].z;
    }
}

void A1solution::p_tri(is& in){
    size_t n;
        in>>n;
    triangles.resize(n);
    for(size_t i=0;i<n;++i){
       in >> triangles[i].x >> triangles[i].y >> triangles[i].z;
    }
}

void A1solution::c_nv(){
    std::vector<vec3> accum_normals(vertices.size(), vec3(0.0f));

    for (const auto& tri : triangles) {
        const size_t i0 = tri.x;
        const size_t i1 = tri.y;
        const size_t i2 = tri.z;

        if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size()) {
            continue;
        }

        const vec3& v0 = vertices[i0];
        const vec3& v1 = vertices[i1];
        const vec3& v2 = vertices[i2];

        const vec3 tri_normal = glm::cross(v1 - v0, v2 - v0);
        if (glm::length(tri_normal) > 0.0f) {
            accum_normals[i0] += tri_normal;
            accum_normals[i1] += tri_normal;
            accum_normals[i2] += tri_normal;
        }
    }

    normal_vertices.assign(vertices.size() * 3, 0.0f);
    for (size_t i = 0; i < accum_normals.size(); ++i) {
        vec3 n = accum_normals[i];
        const float len = glm::length(n);
        if (len > 0.0f) {
            n /= len;
        } else {
            n = vec3(0.0f, 0.0f, 1.0f);
        }

        normal_vertices[3 * i + 0] = n.x;
        normal_vertices[3 * i + 1] = n.y;
        normal_vertices[3 * i + 2] = n.z;
    }
}


void A1solution::s_rend(){
    // auto debug_gl = [](uint32_t place) {};
    
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    //Creating Window
    /*I've been looking for an opportunity to learn how to use smart pointers.
    I'm not even pretending that this is optimal*/
    auto window = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>
        (glfwCreateWindow(width,height,"COMP371-Assignment1",NULL,NULL),glfwDestroyWindow);

    if(window==NULL){
        std::cerr<<"Failed to create GLFW window"<<std::endl;
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window.get());

    glewExperimental=true;

    if(glewInit()!=GLEW_OK){
        std::cerr<<"Failed to create GLEW"<<std::endl;
        exit(EXIT_FAILURE);
    }

    glClearColor(0.5f,0.5f,0.5f,1.0f);

    const std::string phongVertexSource = getPhongVertShad();
    const std::string phongFragmentSource = getPhongFragShad();
    int vertexColorPhong =
        compileAndLinkShaders(phongVertexSource, phongFragmentSource);

    const std::string flatVertexSource = getFlatVertShad();
    const std::string flatFragmentSource = getFlatFragShad();
    int flatProgram =
        compileAndLinkShaders(flatVertexSource, flatFragmentSource);

    const std::string circleVertexSource = getCircleVertShad();
    const std::string circleFragmentSource = getCircleFragShad();
    int circleProgram =
        compileAndLinkShaders(circleVertexSource, circleFragmentSource);

    const std::string vorVertexSource = getVorVertShad();
    const std::string vorFragmentSource = getVorFragShad();
    int vorProgram =
        compileAndLinkShaders(vorVertexSource, vorFragmentSource);
   

    //debug_gl(0);

    GLuint VAO, VBO, EBO, PBO[3];
    createRenderingData(VAO,VBO,PBO,EBO);

    glViewport(0,0,width,height);
    bool wireframe = false;
    bool w_was_down = false;
    bool s_was_down = false;
    int shaderMode = 0; // 0=phong, 1=flat, 2=circle, 3=voronoi

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearDepth(1.0);

    while(!glfwWindowShouldClose(window.get()))
    {
        const bool w_down = glfwGetKey(window.get(), GLFW_KEY_W) == GLFW_PRESS;
        if (w_down && !w_was_down) {
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        }
        w_was_down = w_down;

        const bool s_down = glfwGetKey(window.get(), GLFW_KEY_S) == GLFW_PRESS;
        if (s_down && !s_was_down) {
            shaderMode = (shaderMode + 1) % 4;
        }
        s_was_down = s_down;

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int activeProgram = vertexColorPhong;
        if (shaderMode == 1) {
            activeProgram = flatProgram;
        } else if (shaderMode == 2) {
            activeProgram = circleProgram;
        } else if (shaderMode == 3) {
            activeProgram = vorProgram;
        }
        glUseProgram(activeProgram);

        glUniformMatrix4fv(glGetUniformLocation(activeProgram, "modelview"),
            1, GL_FALSE, glm::value_ptr(modelview));
        glUniformMatrix4fv(glGetUniformLocation(activeProgram, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(activeProgram, "objcol"), 1.0f, 1.0f, 1.0f);
        // alternative function to get lightpos into the correct position
        // const glm::vec4 lightPosWorld(0.0f, 0.0f, 0.0f, 1.0f);
        // const glm::vec3 lightPosView = glm::vec3(modelview * lightPosWorld);
        // glUniform3fv(glGetUniformLocation(activeProgram, "lPos"),
        //     1, glm::value_ptr(lightPosView));
        glUniform3f(glGetUniformLocation(activeProgram, "lPos"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(activeProgram, "lcol"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(activeProgram, "ka"), 0.1f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(activeProgram, "kd"), 1.0f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(activeProgram, "ks"), 0.3f, 0.3f, 0.3f);
        glUniform1f(glGetUniformLocation(activeProgram, "e"), 5.0f);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(triangles.size() * 3), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window.get());

        // Detect inputs
        glfwPollEvents();

        if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window.get(), true);
        }
    }

}

int A1solution::compileAndLinkShaders(const std::string& vertexShaderSource,
     const std::string& fragmentShaderSource){
        
        int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char* vertexSource = vertexShaderSource.c_str();
        glShaderSource(vertexShader,1,&vertexSource,NULL);
        glCompileShader(vertexShader);

        int success;
        GLchar infoLog[512];

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
            std::cerr<<"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"<<
                infoLog<<std::endl;
        }

        int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentSource = fragmentShaderSource.c_str();
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }


        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return shaderProgram;
     }

    void A1solution::createRenderingData(GLuint& VAO, GLuint& VBO, GLuint PBO[3],
        GLuint& EBO){
            glGenVertexArrays(1,&VAO);
            glBindVertexArray(VAO);

            std::vector<GLfloat> expanded_vertices;
            std::vector<GLfloat> pbo_data[3];
            std::vector<GLuint> flat_indices;

            expanded_vertices.reserve(triangles.size() * 3 * 6);
            for (int i = 0; i < 3; ++i) {
                pbo_data[i].reserve(triangles.size() * 3 * 3);
            }
            flat_indices.reserve(triangles.size() * 3);

            GLuint running_index = 0;
            for (const auto& tri : triangles) {
                const GLuint ids[3] = {
                    tri.x,
                    tri.y,
                    tri.z
                };

                const vec3 tri_pts[3] = {
                    vertices[ids[0]],
                    vertices[ids[1]],
                    vertices[ids[2]]
                };

                for (int corner = 0; corner < 3; ++corner) {
                    const GLuint vid = ids[corner];
                    const vec3& pos = vertices[vid];

                    expanded_vertices.push_back(pos.x);
                    expanded_vertices.push_back(pos.y);
                    expanded_vertices.push_back(pos.z);
                    expanded_vertices.push_back(normal_vertices[3 * vid + 0]);
                    expanded_vertices.push_back(normal_vertices[3 * vid + 1]);
                    expanded_vertices.push_back(normal_vertices[3 * vid + 2]);

                    for (int p = 0; p < 3; ++p) {
                        pbo_data[p].push_back(tri_pts[p].x);
                        pbo_data[p].push_back(tri_pts[p].y);
                        pbo_data[p].push_back(tri_pts[p].z);
                    }

                    flat_indices.push_back(running_index++);
                }
            }

            glGenBuffers(1,&VBO);
            glBindBuffer(GL_ARRAY_BUFFER,VBO);
            glBufferData(GL_ARRAY_BUFFER, expanded_vertices.size()*sizeof(GLfloat),
                expanded_vertices.data(),GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)0);
                glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
                glEnableVertexAttribArray(1);

            glGenBuffers(3, PBO);
            for (int p = 0; p < 3; ++p) {
                glBindBuffer(GL_ARRAY_BUFFER, PBO[p]);
                glBufferData(GL_ARRAY_BUFFER, pbo_data[p].size() * sizeof(GLfloat),
                    pbo_data[p].data(), GL_STATIC_DRAW);
                glVertexAttribPointer(2 + p, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
                glEnableVertexAttribArray(2 + p);
            }

            glGenBuffers(1,&EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, flat_indices.size() * sizeof(GLuint),
                flat_indices.data(),GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER,0);
            glBindVertexArray(0);
    }
