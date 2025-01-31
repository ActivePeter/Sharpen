#pragma once
#ifndef _SHARPEN_EVENTFD_HPP
#define _SHARPEN_EVENTFD_HPP

#include "TypeDef.hpp"
#include "Noncopyable.hpp"
#include "FileTypeDef.hpp"

//event fd only supported by linux
#ifdef SHARPEN_IS_LINUX

#define SHARPEN_HAS_EVENTFD

namespace sharpen
{
    class EventFd:public sharpen::Noncopyable
    {
    private:
        using EventFdValue = sharpen::Uint64;
        using Self = sharpen::EventFd;

        sharpen::FileHandle handle_;
    public:
        EventFd(sharpen::Uint32 initVal,int flags);
        
        EventFd(Self &&other) noexcept;
        
        ~EventFd() noexcept;
        
        Self &operator=(Self &&other) noexcept;
        
        EventFdValue Read();
        
        void Write(EventFdValue value);
        
        sharpen::FileHandle GetHandle() const noexcept;
    };
}

#endif
#endif
