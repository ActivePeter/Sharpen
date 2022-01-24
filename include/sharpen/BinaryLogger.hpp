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

namespace sharpen
{
    class BinaryLogger:public sharpen::Noncopyable
    {
    private:
        using Self = sharpen::BinaryLogger;
    
        sharpen::FileChannelPtr channel_;
        std::string logName_;
        sharpen::Uint64 offset_;
    public:
        BinaryLogger(const char *logName,sharpen::EventEngine &engine);
    
        BinaryLogger(Self &&other) noexcept = default;
    
        Self &operator=(Self &&other) noexcept;
    
        ~BinaryLogger() noexcept;

        void Log(const sharpen::WriteBatch &batch);

        std::list<sharpen::WriteBatch> GetWriteBatchs();

        void Clear();

        void Remove();
    };
}

#endif