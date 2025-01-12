// Swap chain
// The swao chain is the action of swapping following buffers in order to show a video on screen
// Tipically we use 2 frame buffers at least, swapped every time the screen refreshes itself
// v-sync: moment when the screen starts drawing the next image (back buffer). Strictly tied to the screen's refresh rate
// It is necessary to sync the sync with the screen because otherwise we get "tearing", which is the moment when on
// screen you can see different frames at the same time
// Generally eac frame buffer contains unique attachments, like the color buffer and the depth buffer which
// are used to compose the image
// Since the GPU frame computation is faster than the screen window show of an image, the GPU will need
// to wait for the image to be show before starting working on the next back buffer (because it cannot
// overwrite the memory). We can solve it with the triple buffer, which permits the GPU to always have
// an image to work on. That's how the swap chain is created
// There are also other types of sync that change the screen refresh rate to adapt to GPU

// Command Buffer
// Necessary because in Vulkan w cannot execute commands directly with function calls
// bu they need to be stored inside buffers
// What we can do is to create the buffer with a list of commands and reuse them in multiple frames.
// We create a command buffer for each frame of the swap chain. This way we can draw separately on each frame
// with a different set of buffers
// NOte that this is a very important different with OpenGL, which required to write commands at every frame

#pragma once

#include "LveDevice.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>

namespace Lve {

class LveSwapChain {
    
public:
    
      static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

      LveSwapChain(LveDevice &deviceRef, VkExtent2D windowExtent);
      LveSwapChain(LveDevice &deviceRef, VkExtent2D windowExtent, std::shared_ptr<LveSwapChain> PreviousSwapChain);
      ~LveSwapChain();

      LveSwapChain(const LveSwapChain &) = delete;
      LveSwapChain& operator=(const LveSwapChain &) = delete;

      VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
      VkRenderPass getRenderPass() { return renderPass; }
      VkImageView getImageView(int index) { return swapChainImageViews[index]; }
      size_t imageCount() { return swapChainImages.size(); }
      VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
      VkExtent2D getSwapChainExtent() { return swapChainExtent; }
      uint32_t width() { return swapChainExtent.width; }
      uint32_t height() { return swapChainExtent.height; }

      float extentAspectRatio() {return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);}
      VkFormat findDepthFormat();

      VkResult acquireNextImage(uint32_t *imageIndex);
      VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);
    
    bool CompareSwapFormats(const LveSwapChain& Other) const
    {
        return Other.swapChainDepthFormat == swapChainDepthFormat && Other.swapChainImageFormat == swapChainImageFormat;
    }
    
private:
      void Init();
      void createSwapChain();
      void createImageViews();
      void createDepthResources();
      void createRenderPass();
      void createFramebuffers();
      void createSyncObjects();

      // Helper functions
      VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
      VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
      VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

      VkFormat swapChainImageFormat;
      VkFormat swapChainDepthFormat;
      VkExtent2D swapChainExtent;

      std::vector<VkFramebuffer> swapChainFramebuffers;
      VkRenderPass renderPass;

      std::vector<VkImage> depthImages;
      std::vector<VkDeviceMemory> depthImageMemorys;
      std::vector<VkImageView> depthImageViews;
      std::vector<VkImage> swapChainImages;
      std::vector<VkImageView> swapChainImageViews;

      LveDevice &device;
      VkExtent2D windowExtent;

      VkSwapchainKHR swapChain;
      std::shared_ptr<LveSwapChain> OldSwapChain;

      std::vector<VkSemaphore> imageAvailableSemaphores;
      std::vector<VkSemaphore> renderFinishedSemaphores;
      std::vector<VkFence> inFlightFences;
      std::vector<VkFence> imagesInFlight;
      size_t currentFrame = 0;
};

}  // namespace lve
