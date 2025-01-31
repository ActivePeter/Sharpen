#pragma once
#ifndef _SHARPEN_SEGMENTEDCIRCLECACHE_HPP
#define _SHARPEN_SEGMENTEDCIRCLECACHE_HPP

#include "CircleCache.hpp"
#include "BufferOps.hpp"

namespace sharpen
{
    template<typename _T>
    class SegmentedCircleCache:public sharpen::Noncopyable
    {
    private:
        using Self = sharpen::SegmentedCircleCache<_T>;
    
        sharpen::Size size_;
        sharpen::CircleCache<_T> *caches_;

        constexpr static sharpen::Size cacheSize_{3};

        inline sharpen::Size HashKey(const std::string &key) const noexcept
        {
            return Self::HashKey(key.begin(),key.end());
        }

        template<typename _Iterator,typename _Check = decltype(static_cast<char>(0) == *std::declval<_Iterator>())>
        inline sharpen::Size HashKey(_Iterator begin,_Iterator end) const noexcept
        {
            sharpen::Size hash{sharpen::BufferHash32(begin,end)};
            return hash % this->size_;
        }

    public:
        explicit SegmentedCircleCache(sharpen::Size cacheSize)
            :size_(0)
            ,caches_(nullptr)
        {
            cacheSize >>= cacheSize_;
            if(!cacheSize)
            {
                cacheSize = 1;
            }
            this->caches_ = reinterpret_cast<sharpen::CircleCache<_T>*>(std::calloc(cacheSize,sizeof(*this->caches_)));
            if(!this->caches_)
            {
                throw std::bad_alloc();
            }
            for (;this->size_ != cacheSize; this->size_++)
            {
                try
                {
                    new (this->caches_ + this->size_) sharpen::CircleCache<_T>{1 << cacheSize_};
                }
                catch(const std::exception&)
                {
                    this->Release();
                    throw;   
                }
            }
        }

        SegmentedCircleCache(Self &&other) noexcept
            :size_(other.size_)
            ,caches_(other.caches_)
        {
            other.caches_ = nullptr;
        }
    
        ~SegmentedCircleCache() noexcept
        {
            this->Release();
        }

        Self &operator=(Self &&other) noexcept
        {
            if(this != std::addressof(other))
            {
                this->Release();
                this->caches_ = other.caches_;
                this->size_ = other.size_;
            }
            return *this;
        }

        inline sharpen::Size GetSize() const noexcept
        {
            return this->size_ << cacheSize_;
        }

        inline std::shared_ptr<_T> Get(const std::string &key) const noexcept
        {
            assert(!key.empty());
            assert(this->caches_);
            return this->caches_[this->HashKey(key)].Get(key);
        }

        template<typename _Iterator,typename _Check = decltype(static_cast<char>(0) == *std::declval<_Iterator>())>
        inline std::shared_ptr<_T> Get(_Iterator begin,_Iterator end) const noexcept
        {
            assert(begin != end);
            assert(this->caches_);
            return this->caches_[this->HashKey(begin,end)].Get(begin,end);
        }

        template <typename... _Args,typename _Check = decltype(new _T{std::declval<_Args>()...})>
        inline std::shared_ptr<_T> GetOrEmplace(const std::string &key, _Args &&...args)
        {
            assert(!key.empty());
            assert(this->caches_);
            return this->caches_[this->HashKey(key)].GetOrEmplace(key,std::forward<_Args>(args)...);
        }

        template <typename _Iterator,typename... _Args,typename _Check = decltype(new _T{std::declval<_Args>()...},static_cast<char>(0) == *std::declval<_Iterator>())>
        inline std::shared_ptr<_T> GetOrEmplace(_Iterator begin,_Iterator end, _Args &&...args)
        {
            assert(begin != end);
            assert(this->caches_);
            return this->caches_[this->HashKey(begin,end)].GetOrEmplace(begin,end,std::forward<_Args>(args)...);
        }

        template<typename _Iterator,typename _Check = decltype(static_cast<char>(0) == *std::declval<_Iterator>())>
        inline void Delete(_Iterator begin,_Iterator end) noexcept
        {
            assert(begin != end);
            assert(this->caches_);
            return this->caches_[this->HashKey(begin,end)].Delete(begin,end);
        }

        inline void Delete(const std::string &key) noexcept
        {
            assert(!key.empty());
            assert(this->caches_);
            return this->caches_[this->HashKey(key)].Delete(key);
        }

        void Release() noexcept
        {
            if(this->caches_)
            {
                for (sharpen::Size i = 0; i != this->size_; ++i)
                {
                    this->caches_[i].~CircleCache<_T>();   
                }
                this->size_ = 0;
                this->caches_ = nullptr;
            }
        }
    };
}

#endif