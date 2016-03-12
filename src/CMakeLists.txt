cmake_minimum_required(VERSION 2.8)

project(spheres_shader)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLM REQUIRED)
include_directories( ${PROJECT_SOURCE_DIR} ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} )


set(SOURCES tools.cpp shader.cpp camera.cpp)
file(COPY ${PROJECT_SOURCE_DIR}/shader DESTINATION ${PROJECT_SOURCE_DIR}/../build/)

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES})

set(PrBdVbo ${PROJECT_NAME}_billboard_vbo)
add_executable(${PrBdVbo} main.cpp ${SOURCES})
set_target_properties(${PrBdVbo} PROPERTIES COMPILE_FLAGS "-DSPHERES=SpheresBillboardVBO")
target_link_libraries(${PrBdVbo} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES})

set(PrBdTbo ${PROJECT_NAME}_billboard_tbo)
add_executable(${PrBdTbo} main.cpp ${SOURCES})
set_target_properties(${PrBdTbo} PROPERTIES COMPILE_FLAGS "-DSPHERES=SpheresBillboardTBO")
target_link_libraries(${PrBdTbo} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES})

set(PrPS ${PROJECT_NAME}_point_sprite)
add_executable(${PrPS} main.cpp ${SOURCES})
set_target_properties(${PrPS} PROPERTIES COMPILE_FLAGS "-DSPHERES=SpheresPointSprite")
target_link_libraries(${PrPS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES})

set(PrBdGs ${PROJECT_NAME}_billboard_geometry_shader)
add_executable(${PrBdGs} main.cpp ${SOURCES})
set_target_properties(${PrBdGs} PROPERTIES COMPILE_FLAGS "-DSPHERES=SpheresBillboardGeometryShader")
target_link_libraries(${PrBdGs} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES})