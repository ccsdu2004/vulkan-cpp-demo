#ifndef VK_DELETER_H
#define VK_DELETER_H

class VK_Deleter
{
public:
    virtual ~VK_Deleter(){}
    virtual void release() = 0;
};

#endif // VK_DELETER_H
