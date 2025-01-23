cmake_minimum_required(VERSION 3.10)

project(lava)

set(VULKAN_SDK_PATH "C:\\VulkanSDK\\1.3.296.0\\Include")
set(GLSLC "${VULKAN_SDK_PATH}/Bin/glslc.exe")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${VULKAN_SDK_PATH}/include)
link_directories(${VULKAN_SDK_PATH}/lib)

set(CC_FLAGS "-std=c++17 -Wall")
set(LD_FLAGS "-lglfw -lvulkan -Wl, -rpath, /usr/local/lib")

file (GLOB vertShaderSrc "shaders/*.vert")
file (GLOB fragShaderSrc "shaders/*.frag")

file (GLOB SOURCES "*.cpp")

foreach(shader ${vertShaderSrc} ${fragShaderSrc})
	get_filename_component(shaderName ${shader} NAME)
	set(outputFile "shaders/${shaderName}.spv")
	add_custom_command (
		OUTPUT ${outputFile}
		COMMAND ${GLSLC} ${shader} -o ${outputFile}
		DEPENDS ${shader} )
endforeach()

add_executable(${PROJECT_NAME} ${SOURCES} ${SPIRV_OUTPUTS})

target_link_libraries(${PROJECT_NAME} ${LD_FLAGS})

add_custom_target(
	test
	COMMAND ${PROJECT_NAME}
	DEPENDS ${PROJECT_NAME} )

add_custom_target(
	cleanShaders
	COMMAND ${CMAKE_COMMAND} -E remove ${SPIRV_OUTPUTS} )

add_custom_target(
	cleanAll
	COMMAND ${CMAKE_COMMAND} -R remove ${SPIRV_OUTPUTS} ${PROJECTNAME} )
