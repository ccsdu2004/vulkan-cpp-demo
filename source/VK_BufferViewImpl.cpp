#include <VK_BufferViewImpl.h>

VK_BufferViewImpl::VK_BufferViewImpl(VK_Context *inputContext, VkBuffer buffer, VkFormat format,
                                     VkDeviceSize size)
{
    VkBufferViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    viewInfo.pNext = NULL;
    viewInfo.buffer = buffer;
    viewInfo.format = format;
    viewInfo.offset = 0;
    viewInfo.range = size;

    vkContext = inputContext;
    auto result = vkCreateBufferView(vkContext->getDevice(), &viewInfo, vkContext->getAllocation(),
                                     &bufferView);
    assert(result == VK_SUCCESS);
}

VK_BufferViewImpl::~VK_BufferViewImpl()
{
    vkDestroyBufferView(vkContext->getDevice(), bufferView, vkContext->getAllocation());
}

VkBufferView VK_BufferViewImpl::getBufferView()
{
    return bufferView;
}
