cmake_minimum_required(VERSION 3.8)
project(Assignment1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-DSTUDENT_SOLUTION)

aux_source_directory(src SOURCE)
add_executable(${PROJECT_NAME} main.cpp ${SOURCE})
target_include_directories(${PROJECT_NAME} PRIVATE src)

if(WIN32)
    find_package(OpenGL REQUIRED)
    find_package(glm CONFIG QUIET)

    target_include_directories(${PROJECT_NAME} PRIVATE
        C:/glfw-3.3.9.bin.WIN64/include/
        C:/glew-2.1.0/include/
        C:/glew-2.1.0/include/GL/
    )

    link_directories(
        C:/glew-2.1.0/lib/Release/x64/
        C:/glfw-3.3.9.bin.WIN64/lib-vc2019/
    )

    if(TARGET glm::glm)
        target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
    else()
        target_include_directories(${PROJECT_NAME} PRIVATE
            C:/GLM_0.9.9.8/g-truc-glm-bf71a83/
        )
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_gl_LIBRARY} glew32s glfw3)
else()
    set(CMAKE_BUILD_TYPE Debug)

    set(CMAKE_PREFIX_PATH
        /opt/homebrew  # Homebrew (Apple Silicon)
        /usr/local     # Homebrew (Intel)
        /encs          # ENCS lab computers
        /opt/local     # Macports
    )

    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    find_package(GLEW REQUIRED)
    find_package(glfw3 REQUIRED HINTS /encs/pkg/glfw-3.3.4/root)
    find_package(glm CONFIG QUIET)

    if(TARGET glm::glm)
        target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
    else()
        # ENCS glm installation fallback.
        target_include_directories(${PROJECT_NAME} PRIVATE /encs/pkg/glm-0.9.9.8/root/include)
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL GLEW::glew glfw)
endif()
