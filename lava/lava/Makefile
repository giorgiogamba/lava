include environment

CC_FLAGS = -std=c++17 -I ${USER_PATHS}/include -I $(VULKAN_SDK_PATH)/include
LD_FLAGS = -L ${USER_PATHS}/lib -L $(VULKAN_SDK_PATH)/lib -lglfw.3 -lvulkan
LD_FLAGS_EXT = -Wl, -rpath ${USER_PATHS}/lib

vertShaderSrc = $(shell find shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertShaderSrc))

fragShaderSrc = $(shell find shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragShaderSrc))

TARGET = a.out
$(TARGET): $(vertObjFiles) $(fragObjFiles)
$(TARGET): *.cpp *.hpp
	g++ $(CC_FLAGS) -o ${TARGET} *.cpp $(LD_FLAGS) $(LD_FLAGS_EXT)

%.spv: %
	${GLSLC} $< -o $@

.PHONY: test clean

test: a.out
	./a.out

clean:
	rm -f a.out
	rm -f *.spv