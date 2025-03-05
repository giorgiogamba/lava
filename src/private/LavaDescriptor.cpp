// Copyright Giorgio Gamba

#pragma region Includes

#include "LavaDescriptor.hpp"

#include <cassert>
#include <stdexcept>

#pragma endregion

namespace lava {

LavaDescriptorSetLayout::Builder &LavaDescriptorSetLayout::Builder::addBinding(
    uint32_t binding,
    VkDescriptorType descriptorType,
    VkShaderStageFlags stageFlags,
    uint32_t count)
{
    assert(bindings.count(binding) == 0 && "Binding already in use");
    
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = binding;
    layoutBinding.descriptorType = descriptorType;
    layoutBinding.descriptorCount = count;
    layoutBinding.stageFlags = stageFlags;
    bindings[binding] = layoutBinding;
    
    return *this;
}

std::unique_ptr<LavaDescriptorSetLayout> LavaDescriptorSetLayout::Builder::build() const
{
  return std::make_unique<LavaDescriptorSetLayout>(Device, bindings);
}

LavaDescriptorSetLayout::LavaDescriptorSetLayout(LavaDevice &Device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
    : Device{Device}
    , bindings{bindings}
{
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto kv : bindings) {
        setLayoutBindings.push_back(kv.second);
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(
        Device.device(),
        &descriptorSetLayoutInfo,
        nullptr,
        &descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

LavaDescriptorSetLayout::~LavaDescriptorSetLayout()
{
    vkDestroyDescriptorSetLayout(Device.device(), descriptorSetLayout, nullptr);
}

LavaDescriptorPool::Builder &LavaDescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count)
{
    poolSizes.push_back({descriptorType, count});
    return *this;
}

LavaDescriptorPool::Builder &LavaDescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags)
{
    poolFlags = flags;
    return *this;
}

LavaDescriptorPool::Builder &LavaDescriptorPool::Builder::setMaxSets(uint32_t count)
{
    maxSets = count;
    return *this;
}

std::unique_ptr<LavaDescriptorPool> LavaDescriptorPool::Builder::build() const
{
    return std::make_unique<LavaDescriptorPool>(Device, maxSets, poolFlags, poolSizes);
}

LavaDescriptorPool::LavaDescriptorPool(LavaDevice &Device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags,const std::vector<VkDescriptorPoolSize> &poolSizes)
    : Device{Device}
{
    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolInfo.pPoolSizes = poolSizes.data();
    descriptorPoolInfo.maxSets = maxSets;
    descriptorPoolInfo.flags = poolFlags;

    if (vkCreateDescriptorPool(Device.device(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

LavaDescriptorPool::~LavaDescriptorPool()
{
    vkDestroyDescriptorPool(Device.device(), descriptorPool, nullptr);
}

bool LavaDescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    allocInfo.descriptorSetCount = 1;

    // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
    // a new pool whenever an old pool fills up. But this is beyond our current scope
    if (vkAllocateDescriptorSets(Device.device(), &allocInfo, &descriptor) != VK_SUCCESS)
    {
        return false;
    }
    
    return true;
}

void LavaDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const
{
    vkFreeDescriptorSets(Device.device(), descriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
}

void LavaDescriptorPool::resetPool()
{
    vkResetDescriptorPool(Device.device(), descriptorPool, 0);
}

LavaDescriptorWriter::LavaDescriptorWriter(LavaDescriptorSetLayout &setLayout, LavaDescriptorPool &pool)
    : setLayout{setLayout}
    , pool{pool}
{}

LavaDescriptorWriter &LavaDescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo)
{    
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pBufferInfo = bufferInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    
    return *this;
}

LavaDescriptorWriter& LavaDescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo)
{
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto &bindingDescription = setLayout.bindings[binding];

    assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.descriptorType = bindingDescription.descriptorType;
    write.dstBinding = binding;
    write.pImageInfo = imageInfo;
    write.descriptorCount = 1;

    writes.push_back(write);
    
    return *this;
}

bool LavaDescriptorWriter::build(VkDescriptorSet &set)
{
    bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
    if (!success) {
        return false;
    }

    overwrite(set);
    
    return true;
}

void LavaDescriptorWriter::overwrite(VkDescriptorSet &set)
{
    for (auto &write : writes) {
        write.dstSet = set;
    }
    
    vkUpdateDescriptorSets(pool.Device.device(), writes.size(), writes.data(), 0, nullptr);
}

}