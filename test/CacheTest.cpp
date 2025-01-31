#include <cstdio>
#include <cassert>
#include <sharpen/SegmentedCircleCache.hpp>

int main(int argc, char const *argv[])
{
    std::puts("circle cache test begin");
    {
        sharpen::CircleCache<sharpen::Size> caches{8};
        for (sharpen::Size i = 0; i != caches.GetSize(); ++i)
        {
            std::string key{"key"};
            key.push_back(static_cast<char>('0' + i));
            assert(!caches.Get(key));
            caches.GetOrEmplace(key,i);   
        }
        for (sharpen::Size i = 0; i != caches.GetSize(); ++i)
        {
            std::string key{"key"};
            key.push_back(static_cast<char>('0' + i));
            assert(*caches.Get(key) == i);
        }
    }
    std::puts("pass");
    std::puts("delete test");
    {
        sharpen::CircleCache<int> cache{8};
        auto p{cache.GetOrEmplace("1",1)};
        cache.Delete("1");
        p = cache.Get("1");
        assert(p == nullptr);
    }
    std::puts("pass");
    std::puts("segmented cache test begin");
    {
        sharpen::SegmentedCircleCache<sharpen::Size> caches{64};
        for (sharpen::Size i = 0; i != caches.GetSize(); ++i)
        {
            std::string key{"key"};
            key.push_back(static_cast<char>('0' + i));
            assert(!caches.Get(key));
            caches.GetOrEmplace(key,i);
        }
        for (sharpen::Size i = 0; i != caches.GetSize(); ++i)
        {
            std::string key{"key"};
            key.push_back(static_cast<char>('0' + i));
            assert(*caches.Get(key) == i);
        }
    }
    std::puts("pass");
    std::puts("delete test");
    {
        sharpen::SegmentedCircleCache<int> cache{64};
        auto p{cache.GetOrEmplace("1",1)};
        cache.Delete("1");
        p = cache.Get("1");
        assert(p == nullptr);
    }
    std::puts("pass");
    return 0;
}