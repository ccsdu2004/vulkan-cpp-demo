#ifndef VK_IMAGE_HPP
#define VK_IMAGE_HPP
#include "VK_Deleter.h"

class VK_Image : public VK_Deleter
{
public:
    virtual int getWidth()const = 0;
    virtual int getHeight()const = 0;
};

#endif // VK_IMAGE_HPP
