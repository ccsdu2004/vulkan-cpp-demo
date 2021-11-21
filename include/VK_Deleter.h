#ifndef VK_DELETER_H
#define VK_DELETER_H

class VK_Deleter
{
public:
    virtual ~VK_Deleter() {}
public:
    void setRemoveFromContainerWhenRelease(bool remove)
    {
        removeFromContainerWhenRelease = remove;
    }
    bool isRemoveFromContainerWhenRelease()const
    {
        return removeFromContainerWhenRelease;
    }
    virtual void release() = 0;
private:
    bool removeFromContainerWhenRelease = true;
};

#endif // VK_DELETER_H
