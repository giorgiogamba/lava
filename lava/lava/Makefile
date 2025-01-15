VULKAN_SDK_PATH = /Users/giorgiogamba/VulkanSDK/1.3.296.0/macOS
GLSLC = /Users/giorgiogamba/Documents/Projects/lava/lava/lava/compile.sh

CC_FLAGS = -std=c++17 -I /usr/local/include/ -I $(VULKAN_SDK_PATH)/include
LD_FLAGS = -L /usr/local/lib/ -L $(VULKAN_SDK_PATH)/lib -lglfw.3 -lvulkan
LD_FLAGS_EXT = -Wl, -rpath /usr/local/lib

vertShaderSrc = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertShaderSrc))

fragShaderSrc = $(shell find ./shaders -type f -name "*.frag")
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