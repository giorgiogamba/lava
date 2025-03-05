/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */

#include "LavaBuffer.hpp"

// std
#include <cassert>
#include <cstring>

namespace lava
{

    VkDeviceSize LavaBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
    {
        if (minOffsetAlignment > 0)
        {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        
        return instanceSize;
    }

    LavaBuffer::LavaBuffer(
        LavaDevice &device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment)
        : Device{device},
        instanceSize{instanceSize},
        instanceCount{instanceCount},
        usageFlags{usageFlags},
        memoryPropertyFlags{memoryPropertyFlags}
    {
        alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;
        device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
    }

    LavaBuffer::~LavaBuffer()
    {
        unmap();
        vkDestroyBuffer(Device.device(), buffer, nullptr);
        vkFreeMemory(Device.device(), memory, nullptr);
    }

    VkResult LavaBuffer::map(VkDeviceSize size, VkDeviceSize offset)
    {
        assert(buffer && memory && "Called map on buffer before create");
        
        const bool bWholeSize = size == VK_WHOLE_SIZE;
        const VkDeviceSize& CurrentSize = bWholeSize ? bufferSize : size;
        const VkDeviceSize& CurrentOffset = bWholeSize ? 0 : offset;

        return vkMapMemory(Device.device(), memory, CurrentOffset, CurrentSize, 0, &mapped);
    }

    void LavaBuffer::unmap()
    {
        if (mapped)
        {
            vkUnmapMemory(Device.device(), memory);
            mapped = nullptr;
        }
    }

    void LavaBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset)
    {
        assert(mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE)
        {
            memcpy(mapped, data, bufferSize);
        }
        else
        {
            char *memOffset = (char *)mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    VkResult LavaBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;

        return vkFlushMappedMemoryRanges(Device.device(), 1, &mappedRange);
    }

    VkResult LavaBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        
        return vkInvalidateMappedMemoryRanges(Device.device(), 1, &mappedRange);
    }

    VkDescriptorBufferInfo LavaBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset)
    {
        return VkDescriptorBufferInfo{buffer,offset,size};
    }

    void LavaBuffer::writeToIndex(void *data, int index)
    {
        writeToBuffer(data, instanceSize, index * alignmentSize);
    }

    VkResult LavaBuffer::flushIndex(int index)
    {
        return flush(alignmentSize, index * alignmentSize);
    }

    VkDescriptorBufferInfo LavaBuffer::descriptorInfoForIndex(int index)
    {
        return descriptorInfo(alignmentSize, index * alignmentSize);
    }

    VkResult LavaBuffer::invalidateIndex(int index)
    {
        return invalidate(alignmentSize, index * alignmentSize);
    }

}  // namespace lve