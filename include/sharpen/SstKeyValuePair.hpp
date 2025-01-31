#pragma once
#ifndef _SHARPEN_SSTKEYVALUEPAIR_HPP
#define _SHARPEN_SSTKEYVALUEPAIR_HPP

/*
Key Value Pair
+------------------+
| Shared Key Size  | varint
+------------------+
| Uniqued Key Size | varint
+------------------+
| Uniqued Key      |
+------------------+
| Value Size       | varint
+------------------+
| Value            |
=------------------+
*/

#include <utility>
#include <cassert>
#include <stdexcept>

#include "Varint.hpp"
#include "ByteBuffer.hpp"

namespace sharpen
{
    class SstKeyValuePair
    {
    private:
        using Self = sharpen::SstKeyValuePair;
    
        sharpen::Uint64 sharedSize_;
        sharpen::Uint64 uniquedSize_;
        sharpen::ByteBuffer key_;
        sharpen::ByteBuffer value_;
    public:
    
        SstKeyValuePair();

        SstKeyValuePair(sharpen::Uint64 sharedSize,sharpen::Uint64 uniquedSize,sharpen::ByteBuffer key,sharpen::ByteBuffer value);

        SstKeyValuePair(sharpen::ByteBuffer key,sharpen::ByteBuffer value)
            :SstKeyValuePair(0,key.GetSize(),std::move(key),std::move(value))
        {
            assert(key.GetSize() != 0);
        }
    
        SstKeyValuePair(const Self &other) = default;
    
        SstKeyValuePair(Self &&other) noexcept = default;
    
        inline Self &operator=(const Self &other)
        {
            Self tmp{other};
            std::swap(tmp,*this);
            return *this;
        }
    
        inline Self &operator=(Self &&other) noexcept
        {
            if(this != std::addressof(other))
            {
                this->sharedSize_ = other.sharedSize_;
                this->uniquedSize_ = other.uniquedSize_;
                this->key_ = std::move(other.key_);
                this->value_ = std::move(other.value_);
                other.sharedSize_ = 0;
                other.uniquedSize_ = 0;
            }
            return *this;
        }
    
        ~SstKeyValuePair() noexcept = default;

        sharpen::Size LoadFrom(const char *data,sharpen::Size size);

        sharpen::Size LoadFrom(const sharpen::ByteBuffer &buf,sharpen::Size offset);

        inline sharpen::Size LoadFrom(const sharpen::ByteBuffer &buf)
        {
            return this->LoadFrom(buf,0);
        }

        sharpen::Size UnsafeStoreTo(char *data) const;

        sharpen::Size StoreTo(char *data,sharpen::Size size) const;

        sharpen::Size StoreTo(sharpen::ByteBuffer &buf,sharpen::Size offset) const;

        inline sharpen::Size StoreTo(sharpen::ByteBuffer &buf) const
        {
            return this->StoreTo(buf,0);
        }        

        sharpen::Size ComputeSize() const noexcept;

        inline const sharpen::ByteBuffer &GetKey() const noexcept
        {
            return this->key_;
        }

        sharpen::ByteBuffer &&MoveKey() && noexcept
        {
            this->SetSharedKeySize(0);
            this->SetUniquedKeySize(0);
            return std::move(this->key_);
        }

        sharpen::ByteBuffer &Value() noexcept
        {
            return this->value_;
        }

        const sharpen::ByteBuffer &Value() const noexcept
        {
            return this->value_;
        }

        inline sharpen::Uint64 GetSharedKeySize() const noexcept
        {
            return this->sharedSize_;
        }

        inline void SetSharedKeySize(sharpen::Uint64 size) noexcept
        {
            assert(this->key_.GetSize() >= size);
            this->sharedSize_ = size;
        }

        inline sharpen::Uint64 GetUniquedKeySize() const noexcept
        {
            return this->uniquedSize_;
        }

        inline void SetUniquedKeySize(sharpen::Uint64 size) noexcept
        {
            assert(this->key_.GetSize() >= size);
            this->uniquedSize_ = size;
        }

        void SetSharedKey(const char *data,sharpen::Size size)
        {
            if(size < this->GetSharedKeySize())
            {
                throw std::invalid_argument("invalid shard key");
            }
            std::memcpy(this->key_.Data(),data,this->GetSharedKeySize());
        }

        const char *GetSharedKeyBegin() const noexcept
        {
            return this->key_.Data();
        }

        const char *GetSharedKeyEnd() const noexcept
        {
            return this->key_.Data() + this->sharedSize_;
        }
    };
}

#endif