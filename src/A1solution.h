#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <iostream>
#include <istream>
#include <string>
#include <vector>

using vec3 = glm::vec3;
using vec3i = glm::uvec3;
using mat4 = glm::mat4;
using is = std::istream;

class A1solution {
public:
    A1solution() = default;
    void run(const std::string& fname);
    void dump_state(std::ostream& out = std::cout) const;

private:
    void p_mv(is& in);
    void p_proj(is& in);
    void p_dis(is& in);
    void p_vert(is& in);
    void p_tri(is& in);
    void c_nv();
    void s_rend();
    int compileAndLinkShaders(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
    std::string loadTextFile(const std::string& path) const;
    void createRenderingData(
        unsigned int& VAO,
        unsigned int& VBO,
        unsigned int& EBO);
    std::vector<GLfloat> interleave() const;

    mat4 modelview{};
    mat4 projection{};
    int width = 0;
    int height = 0;
    std::vector<vec3> vertices;
    std::vector<vec3i> triangles;
    std::vector<GLfloat> normal_vertices;
};
