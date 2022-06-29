#ifndef VK_COMMANDPOOL_H
#define VK_COMMANDPOOL_H
#include <vulkan/vulkan.h>
#include <VK_Util.h>
#include <VK_Deleter.h>
#include <VK_Context.h>

class VK_SecondaryCommandBuffer;

class VK_CommandPool : public VK_Deleter
{
public:
    VK_CommandPool(VK_Context *vkContext, const QueueFamilyIndices &index);
public:
    void release()override;

    [[nodiscard]] VkCommandBuffer beginSingleTimeCommands(uint32_t flag =
                                                              VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue);
    [[nodiscard]] VkCommandPool getCommandPool()const;

    [[nodiscard]] VK_SecondaryCommandBuffer *createSecondaryCommand(uint32_t count);
private:
    VK_Context *context = nullptr;
    VkCommandPool pool = nullptr;
};

#endif // VK_COMMANDPOOL_H
