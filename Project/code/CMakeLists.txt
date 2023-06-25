CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(PT CXX)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -fopenmp")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -fopenmp")
SET(CMAKE_BUILD_TYPE Release)

ADD_SUBDIRECTORY(deps/vecmath)

SET(PROJECT_SOURCES
        src/image.cpp
        src/main.cpp
        src/mesh.cpp
        src/scene_parser.cpp
        src/texture.cpp
        )

SET(PROJECT_INCLUDES
        include/camera.hpp
        include/group.hpp
        include/hit.hpp
        include/image.hpp
        include/light.hpp
        include/material.hpp
        include/mesh.hpp
        include/object3d.hpp
        include/plane.hpp
        include/ray.hpp
        include/scene_parser.hpp
        include/sphere.hpp
        include/transform.hpp
        include/triangle.hpp

        include/renderer.hpp
        # include/sppm/sppm.hpp
        # include/sppm/kdtree.hpp
        # include/sppm/hitpoint.hpp
        include/pt.hpp
        # include/pt_thread.hpp
        include/moving_sphere.hpp
        include/bounding.hpp
        include/bvh.hpp
        include/texture.hpp
        include/stb_image.h
        include/box.hpp
        include/rand.hpp
        )

SET(CMAKE_CXX_STANDARD 11)

ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_INCLUDES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
