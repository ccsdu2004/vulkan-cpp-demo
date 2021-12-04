#ifndef VK_VALIDATIONLAYER_H
#define VK_VALIDATIONLAYER_H
#include <vector>
#include <vulkan/vulkan.h>
#include "VK_Deleter.h"

class VK_ValidationLayer : public VK_Deleter
{
public:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    static inline const std::vector<const char*> validationlayerSets = {"VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_standard_validation"};
public:
    VK_ValidationLayer(bool debug = false);
    ~VK_ValidationLayer();

    void release()override;
public:
    void adjustVkInstanceCreateInfo(VkInstanceCreateInfo& createInfo);
    void adjustVkDeviceCreateInfo(VkDeviceCreateInfo& createInfo);
    bool appendValidationLayerSupport();
    bool setupDebugMessenger(VkInstance instance);

    void cleanup(VkInstance instance);
private:
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    bool isValidationLayer(const char* name);
private:
    bool vkDebug = false;
    VkDebugUtilsMessengerCreateInfoEXT vkDebugCreateInfo{};
    std::vector<const char*> validationLayers;
    VkDebugUtilsMessengerEXT debugMessenger = 0;
};

#endif // VK_VALIDATIONLAYER_H
