#pragma once
#ifndef _SHARPEN_VARINT_HPP
#define _SHARPEN_VARINT_HPP

#include <cassert>

#include "ByteBuffer.hpp"
#include "Optional.hpp"
#include "IntOps.hpp"

namespace sharpen
{
    template<typename _T>
    class Varint
    {
    private:
        using Self = sharpen::Varint<_T>;

        constexpr static sharpen::Size bytes_ = sizeof(_T)*8/7 + ((sizeof(_T)*8 % 7)?1:0);
        constexpr static unsigned char mask_ = static_cast<unsigned char>(~(1 << 7));
        constexpr static unsigned char signBit_ = static_cast<unsigned char>(~mask_);

        mutable sharpen::Optional<_T> cache_;
        char data_[Self::bytes_];
    public:
        explicit Varint(_T intValue)
            :cache_(sharpen::EmptyOpt)
            ,data_()
        {
            this->Set(intValue);
        }

        explicit Varint(sharpen::ByteBuffer data)
            :cache_(sharpen::EmptyOpt)
            ,data_()
        {
            this->Set(data);
        }

        Varint(const char *data,sharpen::Size size)
            :cache_(sharpen::EmptyOpt)
            ,data_()
        {
            this->Set(data,size);
        }
    
        Varint(const Self &other) noexcept
            :cache_(other.cache_)
            ,data_()
        {
            std::memcpy(this->data_,other.data_,bytes_);
        }
    
        Varint(Self &&other) noexcept
            :cache_(std::move(other.cache_))
            ,data_()
        {
            std::memcpy(this->data_,other.data_,bytes_);
        }
    
        Self &operator=(const Self &other) noexcept
        {
            if (this != std::addressof(other))
            {
                this->cache_ = other.cache_;
                std::memcpy(this->data_,other.data_,bytes_);
            }
            return *this;
        }
    
        Self &operator=(Self &&other) noexcept
        {
            if(this != std::addressof(other))
            {
                this->cache_ = std::move(other.cache_);
                std::memcpy(this->data_,other.data_,bytes_);
            }
            return *this;
        }

        Self &operator=(_T val) noexcept
        {
            this->Set(val);
            return *this;
        }
    
        ~Varint() noexcept = default;

        _T Get() const noexcept
        {
            if(!this->cache_.Exist())
            {
                _T value{0};
                const char *ite = this->data_;
                sharpen::Size counter{0};
                while (*ite & signBit_)
                {
                    value |= (*ite++ & mask_) << 7*counter++;
                }
                value  |= (*ite & mask_) << 7*counter;
                this->cache_.Construct(value);
            }
            return this->cache_.Get();
        }

        void Set(_T value)
        {
            if(this->cache_.Exist() && this->cache_.Get() == value)
            {
                return;
            }
            this->cache_.Construct(value);
            using UnsignedValue = typename std::make_unsigned<_T>::type;
            UnsignedValue val{static_cast<UnsignedValue>(value)};
            unsigned char *ite = reinterpret_cast<unsigned char*>(this->data_ + 0);
            for (;val > mask_; ++ite)
            {
                *ite = static_cast<unsigned char>(val) | signBit_;
                val >>= 7;
            }
            *ite = static_cast<unsigned char>(val);
        }

        void Set(const sharpen::ByteBuffer &data)
        {
            this->cache_.Reset();
            sharpen::Size size = (std::min)(data.GetSize(),sizeof(this->data_));
            std::memcpy(this->data_,data.Data(),size);
            this->data_[size] &= mask_;
        }

        void Set(const char *data,sharpen::Size size)
        {
            this->cache_.Reset();
            size = (std::min)(size,sizeof(this->data_));
            std::memcpy(this->data_,data,size);
            this->data_[size] &= mask_;
        }

        inline operator _T() const noexcept
        {
            return this->Get();
        }

        inline bool operator==(const Self &other)
        {
            return this->Get() == other.Get();
        }

        inline bool operator!=(const Self &other)
        {
            return this->Get() != other.Get();
        }

        inline bool operator>=(const Self &other)
        {
            return this->Get() >= other.Get();
        }

        inline bool operator<=(const Self &other)
        {
            return this->Get() <= other.Get();
        }

        inline bool operator>(const Self &other)
        {
            return this->Get() > other.Get();
        }

        inline bool operator<(const Self &other)
        {
            return this->Get() < other.Get();
        }

        const char *Data() const noexcept
        {
            return this->data_;
        }

        sharpen::Size ComputeSize() const noexcept
        {
            sharpen::Size size{1};
            const char *ite = this->data_;
            while (*ite & signBit_)
            {
                ++ite;
                ++size;
            }
            return size;
        }

        static constexpr sharpen::Size GetMaxSize() noexcept
        {
            return sizeof(_T)*8/7 + ((sizeof(_T)*8 % 7)?1:0);
        }
    };

    template<typename _T>
    inline bool operator==(const sharpen::Varint<_T> &varint,_T obj)
    {
        return varint.Get() == obj;
    }

    template<typename _T>
    inline bool operator==(_T obj,const sharpen::Varint<_T> &varint)
    {
        return varint.Get() == obj;
    }

    template<typename _T>
    inline bool operator!=(const sharpen::Varint<_T> &varint,_T obj)
    {
        return varint.Get() != obj;
    }

    template<typename _T>
    inline bool operator!=(_T obj,const sharpen::Varint<_T> &varint)
    {
        return varint.Get() != obj;
    }

    template<typename _T>
    inline bool operator>=(const sharpen::Varint<_T> &varint,_T obj)
    {
        return varint.Get() >= obj;
    }

    template<typename _T>
    inline bool operator>=(_T obj,const sharpen::Varint<_T> &varint)
    {
        return obj >= varint.Get();
    }

    template<typename _T>
    inline bool operator>(const sharpen::Varint<_T> &varint,_T obj)
    {
        return varint.Get() > obj;
    }

    template<typename _T>
    inline bool operator>(_T obj,const sharpen::Varint<_T> &varint)
    {
        return obj > varint.Get();
    }

    template<typename _T>
    inline bool operator<=(const sharpen::Varint<_T> &varint,_T obj)
    {
        return varint.Get() <= obj;
    }

    template<typename _T>
    inline bool operator<=(_T obj,const sharpen::Varint<_T> &varint)
    {
        return obj <= varint.Get();
    }   

    template<typename _T>
    inline bool operator<(const sharpen::Varint<_T> &varint,_T obj)
    {
        return varint.Get() < obj;
    }

    template<typename _T>
    inline bool operator<(_T obj,const sharpen::Varint<_T> &varint)
    {
        return obj < varint.Get();
    }

    using Varint32 = sharpen::Varint<sharpen::Int32>;
    using Varuint32 = sharpen::Varint<sharpen::Uint32>;

    using Varint64 = sharpen::Varint<sharpen::Int64>;
    using Varuint64 = sharpen::Varint<sharpen::Uint64>;

    using Varsize = sharpen::Varint<sharpen::Size>;
}

#endif