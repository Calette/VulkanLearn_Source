cmake_minimum_required(VERSION 3.20)
# 工程名称
project(VulkanLearn)

#Message(STATUS "Processing Project ${PROJNAME}:")

# Property
set(CMAKE_CXX_STANDARD 17)
#set(PWD "${CMAKE_CURRENT_LIST_DIR}")
#set_property(TARGET MyTarget PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
#set_target_properties(${PROJECT_BIN} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")

# Configure Setting
set(VULKANSDK_PATH ${PROJECT_SOURCE_DIR} CACHE PATH "Directory where outputs will be stored")

# Common
include_directories(${PROJECT_SOURCE_DIR}/Include)
include_directories(${PROJECT_SOURCE_DIR}/Source)

# Vulkan
set(LIBS ${LIBS} vulkan-1)
set(LINK_DIR ${VULKANSDK_PATH}/Lib)
link_directories(${LINK_DIR})
include_directories(${VULKANSDK_PATH}/Include)

# GLFW
set(LIBS ${LIBS} glfw3)
set(LINK_DIR ${PROJECT_SOURCE_DIR}/lib/glfw-3.3.4.bin.WIN64/lib-vc2019)
link_directories(${LINK_DIR})

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
# define
add_definitions(-DB3_USE_CLEW)

file(GLOB_RECURSE project_headers *.h *.hpp)
file(GLOB_RECURSE project_cpps *.cpp *.c)
set(all_files ${project_headers} ${project_cpps})
source_group_by_dir(all_files)
set(TARGET ${CMAKE_PROJECT_NAME})
add_executable(${TARGET} ${all_files})
target_link_libraries(${TARGET} ${LIBS})