#pragma once
#ifndef _SHARPEN_BINARYLOGGER_HPP
#define _SHARPEN_BINARYLOGGER_HPP

/*
+---------------------+
| Batch Size1         | 8 bytes
+---------------------+
| Batch1              |
+---------------------+
|    ...              |
+---------------------+
| Batch SizeN         | 8 bytes
+---------------------+
| BatchN              |
+---------------------+
*/

#include "Noncopyable.hpp"
#include "IFileChannel.hpp"
#include "CompilerInfo.hpp"
#include "ByteOrder.hpp"
#include "WriteBatch.hpp"
#include "IntOps.hpp"
#include "AsyncMutex.hpp"

namespace sharpen
{
    class BinaryLogger:public sharpen::Noncopyable
    {
    private:
        using Self = sharpen::BinaryLogger;
    
        std::unique_ptr<sharpen::AsyncMutex> lock_;
        sharpen::FileChannelPtr channel_;
        sharpen::Uint64 offset_;
    public:
        BinaryLogger(sharpen::FileChannelPtr channel);
    
        BinaryLogger(Self &&other) noexcept = default;
    
        Self &operator=(Self &&other) noexcept;
    
        ~BinaryLogger() noexcept = default;

        void Log(const sharpen::WriteBatch &batch);

        std::vector<sharpen::WriteBatch> GetWriteBatchs();

        void Clear();
    };
}

#endif