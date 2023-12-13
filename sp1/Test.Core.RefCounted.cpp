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

class Entity : public Base
{
public:
    Entity() {}
};

TEST(Core, RefCounted)
{
    EXPECT_EQ(g_instanceCount, 0);
    {
        rad::Ref<Base> e1 = new Entity();
        rad::Ref<Base> e2 = new Entity();
        EXPECT_EQ(g_instanceCount, 2);
        EXPECT_EQ(e1->GetRefCount(), 1);
        EXPECT_EQ(e2->GetRefCount(), 1);
        rad::Ref<Base> r1 = e1;
        rad::Ref<Base> r2 = e2;
        EXPECT_EQ(g_instanceCount, 2);
        EXPECT_EQ(e1->GetRefCount(), 2);
        EXPECT_EQ(e2->GetRefCount(), 2);
        r1 = r2;
        EXPECT_EQ(g_instanceCount, 2);
        EXPECT_EQ(e1->GetRefCount(), 1); // e1
        EXPECT_EQ(e2->GetRefCount(), 3); // e2, r1, r2
    }
    EXPECT_EQ(g_instanceCount, 0);
}
