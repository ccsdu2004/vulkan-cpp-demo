#include "VK_Viewports.h"
#include "VK_Util.h"

VkViewport VK_Viewports::createViewport(uint32_t width, uint32_t height, float near, float far)
{
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)width;
    viewport.height = (float)height;
    viewport.minDepth = near;
    viewport.maxDepth = far;
    return viewport;
}

VkRect2D VK_Viewports::createScissor(uint32_t width, uint32_t height)
{
    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent.width = width;
    scissor.extent.height = height;
    return scissor;
}

VK_Viewports::VK_Viewports(uint32_t width, uint32_t height)
{
    auto vp = createViewport(width, height);
    addViewport(vp);
    scissors[0] = createScissor(width, height);
}

VK_Viewports::VK_Viewports(const VK_Viewports &vKviewports):
    viewports(vKviewports.viewports),
    scissors(vKviewports.scissors)
{
}

VK_Viewports &VK_Viewports::operator =(const VK_Viewports &vkviewports)
{
    viewports = vkviewports.viewports;
    scissors = vkviewports.scissors;
    return *this;
}

VK_Viewports::VK_Viewports(const VK_Viewports &&vkViewports):
    viewports(vkViewports.viewports),
    scissors(vkViewports.scissors)
{
}

bool VK_Viewports::operator ==(const VK_Viewports &others)
{
    return isSame(viewports, others.viewports) && isSame(scissors, others.scissors);
}

bool VK_Viewports::operator !=(const VK_Viewports &others)
{
    return ! operator==(others);
}

void VK_Viewports::addViewport(const VkViewport& viewport)
{
    viewports.push_back(viewport);

    auto scissor = createScissor(viewport.width, viewport.height);
    scissors.push_back(scissor);
}

int VK_Viewports::getViewportCount() const
{
    return viewports.size();
}

const VkViewport *VK_Viewports::getViewportData() const
{
    return viewports.data();
}

const VkRect2D *VK_Viewports::getScissorData() const
{
    return scissors.data();
}

void VK_Viewports::setScissor(size_t index, const VkRect2D &scissor)
{
    if(scissors.size() > index && index >= 0)
        scissors[index] = scissor;
}

void VK_Viewports::clear()
{
    viewports.clear();
    scissors.clear();
}

