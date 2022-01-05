#pragma once
#ifndef _SHARPEN_SSTINDEXBLOCK_HPP
#define _SHARPEN_SSTINDEXBLOCK_HPP

/*
Index Block
+---------------------+
|Data Block1 Key Size | 8 bytes
+---------------------+
|Data Block1 Key      |
+---------------------+
|Offset of Data Block1| 8 bytes
+---------------------+
|Size of Data Block1  | 8 bytes
+---------------------+
|        .....        |
+---------------------+
|Data BlockN Key Size | 8 bytes
+---------------------+
|Data BlockN Key      |
+---------------------+
|Offset of Data BlockN| 8 bytes
+---------------------+
|Size of Data BlockN  | 8 bytes
+---------------------+
*/

#include <vector>
#include <algorithm>

#include "SstBlockHandle.hpp"

namespace sharpen
{
    class SstIndexBlock
    {
    private:
        using Self = SstIndexBlock;
        using DataBlockHandles = std::vector<sharpen::SstBlockHandle>;
        using Iterator = typename DataBlockHandles::iterator;
        using ConstIterator = typename DataBlockHandles::const_iterator;
    
        DataBlockHandles dataBlocks_;

        Iterator Find(const sharpen::ByteBuffer &key) noexcept;
    public:
    
        SstIndexBlock() noexcept = default;

        explicit SstIndexBlock(DataBlockHandles blocks) noexcept
            :dataBlocks_(std::move(blocks))
        {}
    
        SstIndexBlock(const Self &other) = default;
    
        SstIndexBlock(Self &&other) noexcept = default;
    
        inline Self &operator=(const Self &other)
        {
            Self tmp{other};
            std::swap(tmp,*this);
            return *this;
        }
    
        Self &operator=(Self &&other) noexcept
        {
            if(this != std::addressof(other))
            {
                this->dataBlocks_ = std::move(other.dataBlocks_);
            }
            return *this;
        }
    
        ~SstIndexBlock() noexcept = default;

        const DataBlockHandles &Blocks() const noexcept
        {
            return this->dataBlocks_;
        }

        void LoadFrom(const sharpen::ByteBuffer &buf,sharpen::Size size,sharpen::Size offset);

        inline void LoadFrom(const sharpen::ByteBuffer &buf,sharpen::Size size)
        {
            this->LoadFrom(buf,size,0);
        }

        sharpen::Size StoreTo(sharpen::ByteBuffer &buf,sharpen::Size offset) const;

        inline sharpen::Size StoreTo(sharpen::ByteBuffer &buf) const
        {
            return this->StoreTo(buf,0);
        }

        ConstIterator Find(const sharpen::ByteBuffer &key) const noexcept;

        void Sort() noexcept
        {
            std::sort(this->dataBlocks_.begin(),this->dataBlocks_.end());
        }

        inline Iterator Begin()
        {
            return this->dataBlocks_.begin();
        }

        inline ConstIterator Begin() const
        {
            return this->dataBlocks_.cbegin();
        }

        inline Iterator End()
        {
            return this->dataBlocks_.end();
        }

        inline ConstIterator End() const
        {
            return this->dataBlocks_.cend();
        }

        void Put(sharpen::SstBlockHandle block)
        {
            auto ite = this->Find(block.Key());
            if (ite == this->End() || ite->Key() != block.Key())
            {
                this->dataBlocks_.push_back(std::move(block));
                this->Sort();
                return;
            }
            *ite = std::move(block);
        }

        template<typename ..._Args>
        auto Emplace(_Args &&...args) -> decltype(this->dataBlocks_.emplace_back(std::forward<_Args>(args)...))
        {
            auto ite = this->Find(block.Key());
            if (ite == this->End() || ite->Key() != block.Key())
            {
                this->dataBlocks_.emplace_back(std::forward<_Args>(args)...);
                this->Sort();
                return;
            }
            this->dataBlocks_.emplace(ite,std::forward<_Args>(args)...);
        }
    };
}

#endif