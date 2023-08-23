#include "gtest/gtest.h"
#include "rad/Core/RefCounted.h"

static size_t g_instanceCount = 0;

class Base : public rad::RefCounted<Base>
{
public:
    Base()
    {
        ++g_instanceCount;
    }
    virtual ~Base()
    {
        --g_instanceCount;
    }
};

class A : public Base
{
public:
    A()
    {
    }
    ~A()
    {
        EXPECT_EQ(GetRefCount(), 0);
    }
};

TEST(Core, RefCounted)
{
    // Lifetime Scope
    {
        rad::Ref<Base> a1 = new A();
        rad::Ref<Base> a2 = new A();
        rad::Ref<Base> r1 = a1;
        rad::Ref<Base> r2 = a2;
        EXPECT_EQ(g_instanceCount, 2);
        a1 = a2;
        r1 = r2;
        EXPECT_EQ(g_instanceCount, 1);
    }
    EXPECT_EQ(g_instanceCount, 0);
}
