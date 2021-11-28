#ifndef VK_VIEWPORTS_H
#define VK_VIEWPORTS_H
#include <vector>
#include <vulkan/vulkan.h>

class VK_Viewports
{
public:
    static VkViewport createViewport(uint32_t width, uint32_t height, float near = 0.0f, float far = 1.0f);
    static VkRect2D createScissor(uint32_t width, uint32_t height);

    VK_Viewports() = default;
    VK_Viewports(uint32_t width, uint32_t height);
    VK_Viewports(const VK_Viewports& viewports);
    VK_Viewports& operator =(const VK_Viewports& viewports);
    VK_Viewports(const VK_Viewports&& viewports);
    ~VK_Viewports() = default;
public:
    bool operator == (const VK_Viewports& others);
    bool operator != (const VK_Viewports& others);
    void addViewport(const VkViewport&);
    void setScissor(size_t index, const VkRect2D& scissor);

    void clear();
public:
    int getViewportCount()const;
    const VkViewport* getViewportData()const;
    const VkRect2D* getScissorData()const;
private:
    typedef std::vector<VkViewport> VK_Viewport;
    typedef std::vector<VkRect2D> VK_Scissor;
    std::vector<VkViewport> viewports;
    std::vector<VkRect2D> scissors;
};

#endif // VK_VIEWPORT_H
