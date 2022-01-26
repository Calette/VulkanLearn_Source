cmake_minimum_required(VERSION 3.20)
# 工程名称
project(Engine)

# Property
set(CMAKE_CXX_STANDARD 17)

# 母目录
string(REGEX REPLACE "(.*)/(.*)" "\\1" PROJECT_INIT_PATH  ${PROJECT_SOURCE_DIR})
# Common
include_directories(${PROJECT_INIT_PATH}/Include)
include_directories(${PROJECT_SOURCE_DIR})

# Vulkan
find_package(Vulkan)
#message(STATUS "Found Vulkan in ${Vulkan_INCLUDE_DIRS}")
#message(STATUS "Found Vulkan libs in ${Vulkan_LIBRARIES}")
link_directories(${LINK_DIR})
include_directories(${Vulkan_INCLUDE_DIRS})

# GLFW
set(GLFW_LIBS glfw3)
set(LINK_DIR ${PROJECT_INIT_PATH}/lib/glfw-3.3.4.bin.WIN64/lib-vc2019)
link_directories(${LINK_DIR})

# define
add_definitions(-DB3_USE_CLEW)
add_definitions(-DSHADER_DIR=\"${PROJECT_INIT_PATH}/\")

macro(source_group_by_dir source_files)
    set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(sgbd_file ${${source_files}})
        string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
        string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
        string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
        string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
        if(sgbd_nogroup)
            set(sgbd_group_name "\\")
        endif(sgbd_nogroup)
        #message("The Group: " ${sgbd_group_name})
        source_group(${sgbd_group_name} FILES ${sgbd_file})
    endforeach(sgbd_file)
endmacro(source_group_by_dir)

# copy res
file(GLOB_RECURSE project_headers *.h *.hpp)
file(GLOB_RECURSE project_cpps *.cpp *.c)
set(all_files ${project_headers} ${project_cpps})
source_group_by_dir(all_files)

add_executable(${PROJECT_NAME} ${all_files})

target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${GLFW_LIBS})

#message(STATUS "!!!! ${CMAKE_BINARY_DIR}/third_party/glslang/SPIRV/Debug/SPIRVd.lib")
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/third_party/Debug/glslang-default-resource-limits.lib)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/third_party/glslang/SPIRV/Debug/SPIRVd.lib)

# 依赖
target_link_libraries(${PROJECT_NAME} PUBLIC
    glslang
	glslang-default-resource-limits
	SPIRV)

if(${NEED_LINK_ATOMIC})
    target_link_libraries(${PROJECT_NAME} PUBLIC atomic)
endif()