#include <iostream>
#include <cstring>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include "VK_UniformBuffer.h"
#include "VK_Context.h"
#include "VK_Image.h"
#include "VK_Texture.h"
#include "VK_Pipeline.h"
#include "VK_DynamicState.h"
#include "VK_CommandPool.h"
#include <fstream>

using namespace std;

const std::vector<float> vertices = {
    -0.5f, -0.5, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.05f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.5f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.5f, 1.5f
    };

const std::vector<uint32_t> indices = {
    0, 1, 2, 2, 3, 0
};

VK_Context *context = nullptr;
VK_Pipeline *pipeline = nullptr;

uint32_t updateUniformBufferData(char *&data, uint32_t size)
{
    glm::mat4 model = glm::identity<glm::mat4>();
    memcpy(data, &model[0][0], size);
    return sizeof(model);
}

void onFrameSizeChanged(int width, int height)
{
    pipeline->getDynamicState()->applyDynamicViewport({0, 0, (float)width, (float)height, 0, 1});
}

void clear_(VkImage image, VkCommandBuffer command);



int main()
{
    VK_ContextConfig config;
    config.debug = true;
    config.name = "Texure Push Descriptor";

    context = createVkContext(config);
    context->createWindow(480, 480, true);
    context->setOnFrameSizeChanged(onFrameSizeChanged);

    VK_Context::VK_Config vkConfig;
    context->initVulkanDevice(vkConfig);

    auto shaderSet = context->createShaderSet();
    shaderSet->addShader("../shader/texture/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addShader("../shader/texture/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    shaderSet->appendVertexAttributeDescription(0, sizeof (float) * 3, VK_FORMAT_R32G32B32_SFLOAT, 0);
    shaderSet->appendVertexAttributeDescription(1, sizeof (float) * 4, VK_FORMAT_R32G32B32A32_SFLOAT,
            sizeof(float) * 3);
    shaderSet->appendVertexAttributeDescription(2, sizeof (float) * 2, VK_FORMAT_R32G32_SFLOAT,
            sizeof(float) * 7);

    shaderSet->appendVertexInputBindingDescription(9 * sizeof(float), 0, VK_VERTEX_INPUT_RATE_VERTEX);

    VkDescriptorSetLayoutBinding uniformBinding = VK_ShaderSet::createDescriptorSetLayoutBinding(0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    shaderSet->addDescriptorSetLayoutBinding(uniformBinding);

    auto samplerBinding = VK_ShaderSet::createDescriptorSetLayoutBinding(1,
                          VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                          VK_SHADER_STAGE_FRAGMENT_BIT);
    auto samplerCreateInfo  = VK_Sampler::createSamplerCreateInfo();
    auto samplerPtr = context->createSampler(samplerCreateInfo);
    VkSampler sampler = samplerPtr->getSampler();
    samplerBinding.pImmutableSamplers = &sampler;

    shaderSet->addDescriptorSetLayoutBinding(samplerBinding);

    if (!shaderSet->isValid()) {
        std::cerr << "invalid shaderSet" << std::endl;
        shaderSet->release();
        context->release();
        return -1;
    }

    auto ubo = shaderSet->addUniformBuffer(0, sizeof(float) * 16);
    ubo->setWriteDataCallback(updateUniformBufferData);

    auto image = context->createImage("../images/cat.png");

    // transitionImageLayout(image->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);

    auto command = context->getCommandPool()->beginSingleTimeCommands();
    clear_(image->getImage(), command);
    context->getCommandPool()->endSingleTimeCommands(command, context->getGraphicQueue());

    auto imageViewCreateInfo = VK_ImageView::createImageViewCreateInfo(image->getImage(),
                               VK_FORMAT_R8G8B8A8_SRGB);
    auto imageView = context->createImageView(imageViewCreateInfo);
    shaderSet->addImageView(imageView);

    context->initVulkanContext();
    pipeline = context->createPipeline(shaderSet);
    pipeline->getDynamicState()->addDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
    pipeline->create();
    pipeline->getDynamicState()->applyDynamicViewport({0, 0, 480, 480, 0, 1});

    auto buffer = context->createVertexBuffer(vertices, 9, indices);
    pipeline->addRenderBuffer(buffer);

    context->createCommandBuffers();

    context->run();
    context->release();

    return 0;
}

void clear_(VkImage image, VkCommandBuffer command)
{
    /*VkImageMemoryBarrier textureBarrier = {};
    textureBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    textureBarrier.pNext = NULL;
    textureBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    textureBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    textureBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    textureBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;//VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    textureBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    textureBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    textureBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    textureBarrier.subresourceRange.baseMipLevel = 0;
    textureBarrier.subresourceRange.levelCount = 1;
    textureBarrier.subresourceRange.baseArrayLayer = 0;
    textureBarrier.subresourceRange.layerCount = 1;
    textureBarrier.image = image;
    vkCmdPipelineBarrier(command, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1,
                         &textureBarrier);

    std::cout << 111 << std::endl;*/

    VkImageSubresourceRange srRange = {};
    srRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    srRange.baseMipLevel = 0;
    srRange.levelCount = VK_REMAINING_MIP_LEVELS;
    srRange.baseArrayLayer = 0;
    srRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

    VkClearColorValue clearColor;
    clearColor.float32[0] = 0.0f;
    clearColor.float32[1] = 1.0f;
    clearColor.float32[2] = 0.0f;
    clearColor.float32[3] = 1.0f;
    vkCmdClearColorImage(command, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &srRange);

}

/*
void set_image_layout(VkCommandBuffer command, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout,
                      VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages)
{
    VkImageMemoryBarrier image_memory_barrier = {};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.pNext = NULL;
    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask = 0;
    image_memory_barrier.oldLayout = old_image_layout;
    image_memory_barrier.newLayout = new_image_layout;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange.aspectMask = aspectMask;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;

    switch (old_image_layout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        default:
            break;
    }

    switch (new_image_layout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        default:
            break;
    }

    vkCmdPipelineBarrier(command, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
}


void write_ppm(VK_Context* context, VkImage image)
{
    string filename;
    VkResult res;

    auto width = context->getSwapChainExtent().width;
    auto height = context->getSwapChainExtent().height;
    auto format = context->getSwapChainFormat();
    auto device = context->getDevice();

    VkImageCreateInfo image_create_info = {};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.pNext = NULL;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = context->getSwapChainFormat();
    image_create_info.extent.width = width;
    image_create_info.extent.height = height;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_LINEAR;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices = NULL;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.flags = 0;

    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = NULL;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;

    VkImage mappableImage;
    VkDeviceMemory mappableMemory;

    res = vkCreateImage(device, &image_create_info, NULL, &mappableImage);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device, mappableImage, &mem_reqs);

    mem_alloc.allocationSize = mem_reqs.size;

    res = vkAllocateMemory(device, &mem_alloc, NULL, &(mappableMemory));
    assert(res == VK_SUCCESS);


    res = vkBindImageMemory(device, mappableImage, mappableMemory, 0);
    assert(res == VK_SUCCESS);

    auto command = context->getCommandPool()->beginSingleTimeCommands();

    set_image_layout(command, mappableImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

    set_image_layout(command, image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

    VkImageCopy copy_region;
    copy_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_region.srcSubresource.mipLevel = 0;
    copy_region.srcSubresource.baseArrayLayer = 0;
    copy_region.srcSubresource.layerCount = 1;
    copy_region.srcOffset.x = 0;
    copy_region.srcOffset.y = 0;
    copy_region.srcOffset.z = 0;
    copy_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_region.dstSubresource.mipLevel = 0;
    copy_region.dstSubresource.baseArrayLayer = 0;
    copy_region.dstSubresource.layerCount = 1;
    copy_region.dstOffset.x = 0;
    copy_region.dstOffset.y = 0;
    copy_region.dstOffset.z = 0;
    copy_region.extent.width = width;
    copy_region.extent.height = height;
    copy_region.extent.depth = 1;

    vkCmdCopyImage(command, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mappableImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

    set_image_layout(command, mappableImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
                     VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT);
*/

/*
res = vkEndCommandBuffer(command);
assert(res == VK_SUCCESS);
const VkCommandBuffer cmd_bufs[] = {command};
VkFenceCreateInfo fenceInfo;
VkFence cmdFence;
fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
fenceInfo.pNext = NULL;
fenceInfo.flags = 0;
vkCreateFence(device, &fenceInfo, NULL, &cmdFence);

VkSubmitInfo submit_info[1] = {};
submit_info[0].pNext = NULL;
submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
submit_info[0].waitSemaphoreCount = 0;
submit_info[0].pWaitSemaphores = NULL;
submit_info[0].pWaitDstStageMask = NULL;
submit_info[0].commandBufferCount = 1;
submit_info[0].pCommandBuffers = cmd_bufs;
submit_info[0].signalSemaphoreCount = 0;
submit_info[0].pSignalSemaphores = NULL;

res = vkQueueSubmit(context->getGraphicQueue(), 1, submit_info, cmdFence);
assert(res == VK_SUCCESS);

do {
    res = vkWaitForFences(device, 1, &cmdFence, VK_TRUE, ~0);
} while (res == VK_TIMEOUT);
assert(res == VK_SUCCESS);
*/
/*
context->getCommandPool()->endSingleTimeCommands(command, context->getGraphicQueue());

//vkDestroyFence(device, cmdFence, NULL);

filename = "2.ppm";

VkImageSubresource subres = {};
subres.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
subres.mipLevel = 0;
subres.arrayLayer = 0;
VkSubresourceLayout sr_layout;
vkGetImageSubresourceLayout(device, mappableImage, &subres, &sr_layout);

char *ptr;
res = vkMapMemory(device, mappableMemory, 0, mem_reqs.size, 0, (void **)&ptr);
assert(res == VK_SUCCESS);

ptr += sr_layout.offset;
std::ofstream file(filename.c_str(), ios::binary);

file << "P6\n";
file << width << " ";
file << height << "\n";
file << 255 << "\n";

for (size_t y = 0; y < height; y++)
{
    const int *row = (const int *)ptr;
    int swapped;

    if (format == VK_FORMAT_B8G8R8A8_UNORM || format == VK_FORMAT_B8G8R8A8_SRGB) {
        for (size_t x = 0; x < width; x++) {
            swapped = (*row & 0xff00ff00) | (*row & 0x000000ff) << 16 | (*row & 0x00ff0000) >> 16;
            file.write((char *)&swapped, 3);
            row++;
        }
    } else if (format == VK_FORMAT_R8G8B8A8_UNORM) {
        for (size_t x = 0; x < width; x++) {
            file.write((char *)row, 3);
            row++;
        }
    } else {
        printf("Unrecognized image format - will not write image files");
        break;
    }

    ptr += sr_layout.rowPitch;
}

file.close();
vkUnmapMemory(device, mappableMemory);
vkDestroyImage(device, mappableImage, NULL);
vkFreeMemory(device, mappableMemory, NULL);
}
*/

