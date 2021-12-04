#include <iostream>
#include <cstring>
#include "VK_ValidationLayer.h"

VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkBool32 VK_ValidationLayer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        std::cerr << "message type:" << messageType << " validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VK_ValidationLayer::VK_ValidationLayer(bool debug):
    vkDebug(debug)
{
}

VK_ValidationLayer::~VK_ValidationLayer()
{

}

void VK_ValidationLayer::release()
{
    delete this;
}

void VK_ValidationLayer::adjustVkInstanceCreateInfo(VkInstanceCreateInfo &createInfo)
{
    if (vkDebug) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        populateDebugMessengerCreateInfo(vkDebugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &vkDebugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.pNext = nullptr;
    }
}

void VK_ValidationLayer::adjustVkDeviceCreateInfo(VkDeviceCreateInfo &createInfo)
{
    if(vkDebug) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }
}

bool VK_ValidationLayer::appendValidationLayerSupport()
{
    if(!vkDebug)
        return true;

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto& layerProperties : availableLayers) {
        if(isValidationLayer(layerProperties.layerName)) {
            char* ptr = new char[100];
            memset(ptr, 0, 100);
            memcpy(ptr, layerProperties.layerName, strlen(layerProperties.layerName));
            validationLayers.push_back(ptr);
            return true;
        }
    }

    return false;
}

void VK_ValidationLayer::cleanup(VkInstance instance)
{
    if(vkDebug) {
        destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        for(auto str : validationLayers)
            delete []str;
    }
}

void VK_ValidationLayer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

bool VK_ValidationLayer::isValidationLayer(const char *name)
{
    auto itr = validationlayerSets.begin();
    while(itr != validationlayerSets.end()) {
        if(strcmp(*itr, name) == 0)
            return true;
        itr ++;
    }
    return false;
}

bool VK_ValidationLayer::setupDebugMessenger(VkInstance instance)
{
    if (!vkDebug)
        return true;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        std::cout << "failed to set up debug messenger!" << std::endl;
        return false;
    }
    return true;
}

