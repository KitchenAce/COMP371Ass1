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
            in >> modelview[c][r];
        }
    }
}

void A1solution::p_proj(is& in){
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            in >> projection[c][r];
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

void A1solution::dump_state(std::ostream& out) const {
    out << "modelview:\n";
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            out << modelview[c][r] << ((c == 3) ? '\n' : ' ');
        }
    }

    out << "projection:\n";
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            out << projection[c][r] << ((c == 3) ? '\n' : ' ');
        }
    }

    out << "display: " << width << " " << height << '\n';

    out << "vertices (" << vertices.size() << "):\n";
    for (size_t i = 0; i < vertices.size(); ++i) {
        out << i << ": "
            << vertices[i].x << " "
            << vertices[i].y << " "
            << vertices[i].z << '\n';
    }

    out << "triangles (" << triangles.size() << "):\n";
    for (size_t i = 0; i < triangles.size(); ++i) {
        out << i << ": "
            << triangles[i].x << " "
            << triangles[i].y << " "
            << triangles[i].z << '\n';
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

    const std::string phongVertexSource = loadTextFile("src/phongVertShad.glsl");
    const std::string phongFragmentSource = loadTextFile("src/phongFragShad.glsl");
    int vertexColorPhong =
        compileAndLinkShaders(phongVertexSource, phongFragmentSource);
   

    //debug_gl(0);

    GLuint VAO, VBO, EBO;
    createRenderingData(VAO,VBO,EBO);

    glViewport(0,0,width,height);
    bool wireframe = false;
    bool w_was_down = false;

    while(!glfwWindowShouldClose(window.get()))
    {
        const bool w_down = glfwGetKey(window.get(), GLFW_KEY_W) == GLFW_PRESS;
        if (w_down && !w_was_down) {
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        }
        w_was_down = w_down;

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(vertexColorPhong);

        glUniformMatrix4fv(glGetUniformLocation(vertexColorPhong, "modelview"),
            1, GL_FALSE, glm::value_ptr(modelview));
        glUniformMatrix4fv(glGetUniformLocation(vertexColorPhong, "projection"),
            1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(vertexColorPhong,"objcol"),0.5f,0.5f,0.5f);
        const glm::vec4 lightPosWorld(2.0f, 2.0f, -2.0f, 1.0f);
        const glm::vec3 lightPosView = glm::vec3(modelview * lightPosWorld);
        glUniform3fv(glGetUniformLocation(vertexColorPhong, "lPos"),
            1, glm::value_ptr(lightPosView));
        glUniform3f(glGetUniformLocation(vertexColorPhong, "lcol"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(vertexColorPhong, "ka"), 0.1f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(vertexColorPhong, "kd"), 1.0f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(vertexColorPhong, "ks"), 0.3f, 0.3f, 0.3f);
        glUniform1f(glGetUniformLocation(vertexColorPhong, "e"), 5.0f);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(triangles.size() * 3), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window.get());

        // Detect inputs
        glfwPollEvents();

        if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window.get(), true);
        }

        /*if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            shaderProgram = voronoiProgram;
        }*/
    }

}

std::string A1solution::loadTextFile(const std::string& path) const {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string((std::istreambuf_iterator<char>(in)),
        std::istreambuf_iterator<char>());
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

    void A1solution::createRenderingData(GLuint& VAO, GLuint& VBO,
        GLuint& EBO){
            glGenVertexArrays(1,&VAO);
            glBindVertexArray(VAO);

            glGenBuffers(1,&VBO);
            glBindBuffer(GL_ARRAY_BUFFER,VBO);
            const std::vector<GLfloat> interleaved = interleave();
            glBufferData(GL_ARRAY_BUFFER, interleaved.size()*sizeof(GLfloat),
                interleaved.data(),GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)0);
                glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
                glEnableVertexAttribArray(1);

            std::vector<GLuint> flat_indices;
            flat_indices.reserve(triangles.size() * 3);
            for (const auto& t : triangles) {
                flat_indices.push_back(t.x);
                flat_indices.push_back(t.y);
                flat_indices.push_back(t.z);
            }

            glGenBuffers(1,&EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, flat_indices.size() * sizeof(GLuint),
                flat_indices.data(),GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER,0);
            glBindVertexArray(0);
    }

    std::vector<GLfloat> A1solution::interleave() const{
        std::vector<GLfloat> interleaved;
        interleaved.reserve(vertices.size() * 6);

        for (size_t i = 0; i < vertices.size(); ++i) {
            interleaved.push_back(vertices[i].x);
            interleaved.push_back(vertices[i].y);
            interleaved.push_back(vertices[i].z);

            interleaved.push_back(normal_vertices[3*i + 0]);
            interleaved.push_back(normal_vertices[3*i + 1]);
            interleaved.push_back(normal_vertices[3*i + 2]);
        }
        return interleaved;
    }
