// Copyright Giorgio Gamba

#pragma region Includes

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "LavaDevice.hpp"

#pragma endregion

namespace lava {

class LavaDescriptorSetLayout {

public:
    
    class Builder {
    
    public:
        
        Builder(LavaDevice &Device) : Device{Device} {}
    
        Builder &addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
        
        std::unique_ptr<LavaDescriptorSetLayout> build() const;
    
    private:
    
        LavaDevice& Device;
        
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
    };

    LavaDescriptorSetLayout(LavaDevice& Device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~LavaDescriptorSetLayout();
    
    LavaDescriptorSetLayout(const LavaDescriptorSetLayout &) = delete;
    LavaDescriptorSetLayout &operator=(const LavaDescriptorSetLayout &) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

private:

    LavaDevice& Device;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    friend class LavaDescriptorWriter;
};

class LavaDescriptorPool {

public:

    class Builder {
    
    public:
        Builder(LavaDevice &InDevice) : Device{InDevice} {}

        Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder &setMaxSets(uint32_t count);
        
        std::unique_ptr<LavaDescriptorPool> build() const;

    private:
    
        LavaDevice& Device;
        
        std::vector<VkDescriptorPoolSize> poolSizes{};
        
        uint32_t maxSets = 1000;
        
        VkDescriptorPoolCreateFlags poolFlags = 0;
    };

    LavaDescriptorPool(LavaDevice& Device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize> &poolSizes);
    ~LavaDescriptorPool();
    
    LavaDescriptorPool(const LavaDescriptorPool &) = delete;
    LavaDescriptorPool &operator=(const LavaDescriptorPool &) = delete;

    bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

    void resetPool();

private:
    LavaDevice& Device;
    VkDescriptorPool descriptorPool;

    friend class LavaDescriptorWriter;
};

class LavaDescriptorWriter {

public:
    LavaDescriptorWriter(LavaDescriptorSetLayout &setLayout, LavaDescriptorPool &pool);

    LavaDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    LavaDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

    bool build(VkDescriptorSet &set);
    void overwrite(VkDescriptorSet &set);

private:

    LavaDescriptorSetLayout &setLayout;
    LavaDescriptorPool &pool;
    std::vector<VkWriteDescriptorSet> writes;
};

}