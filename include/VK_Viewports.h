#ifndef VK_VIEWPORTS_H
#define VK_VIEWPORTS_H
#include <vulkan/vulkan.h>
#include <vector>

class VK_Viewports
{
public:
    static VkViewport createViewport(int width, int height);
    static VkRect2D createScissor(int width, int height);

    VK_Viewports() = default;
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
