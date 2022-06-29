#pragma once
#include <cassert>
#include <VK_BufferView.h>
#include <VK_Context.h>

class VK_BufferViewImpl : public VK_BufferView
{
public:
    VK_BufferViewImpl() = delete;
    VK_BufferViewImpl(VK_Context *inputContext, VkBuffer buffer, VkFormat format, VkDeviceSize size);

    ~VK_BufferViewImpl();
public:
    VkBufferView getBufferView() override;
private:
    VK_Context *vkContext = nullptr;
    VkBufferView bufferView = nullptr;
};
