include environment

MAIN_DIR = src
SRC_DIR = src/private
INC_DIR = src/public

CXX = g++
CC_FLAGS = -std=c++17 -I $(INC_DIR) -I ${USER_PATHS}/include -I $(VULKAN_SDK_PATH)/include -I ${USER_PATHS}/include/QtCore -I ${USER_PATHS}/include/qt
LD_FLAGS = -L ${USER_PATHS}/lib -L $(VULKAN_SDK_PATH)/lib -lglfw.3 -lvulkan
LD_FLAGS_EXT = -Wl, -rpath ${USER_PATHS}/lib

vertShaderSrc = $(shell find shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertShaderSrc))

fragShaderSrc = $(shell find shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragShaderSrc))

TARGET = a.out
$(TARGET): $(vertObjFiles) $(fragObjFiles)
$(TARGET): $(MAIN_DIR)/*.cpp $(SRC_DIR)/*.cpp $(INC_DIR)/*.hpp
	${CXX} $(CC_FLAGS) -o ${TARGET} $(MAIN_DIR)/*.cpp $(SRC_DIR)/*.cpp $(LD_FLAGS) $(LD_FLAGS_EXT)

%.spv: %
	${GLSLC} $< -o $@

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out
	rm -f shaders/*.spv