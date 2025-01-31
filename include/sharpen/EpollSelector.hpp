#pragma once
#ifndef _SHARPEN_EPOLLSELECTOR_HPP
#define _SHARPEN_EPOLLSELECTOR_HPP

#include "Epoll.hpp"

#ifdef SHARPEN_HAS_EPOLL

#include <map>
#include <vector>

#include "ISelector.hpp"
#include "EventFd.hpp"
#include "Nonmovable.hpp"
#include "EpollEventStruct.hpp"
#include "IoUringQueue.hpp"
#include "IoUringStruct.hpp"
#include "SpinLock.hpp"

namespace sharpen
{
    class EpollSelector:public sharpen::ISelector,public sharpen::Noncopyable,public sharpen::Nonmovable
    {
    private:
        using Event = sharpen::EpollEventStruct;
        using Map = std::map<sharpen::FileHandle,Event>;
        using EventBuf = std::vector<sharpen::Epoll::Event>;

        sharpen::Epoll epoll_;
        sharpen::EventFd eventfd_;
        Map map_;
        EventBuf eventBuf_;
        sharpen::SpinLock lock_;
#ifdef SHARPEN_HAS_IOURING
        std::unique_ptr<sharpen::IoUringQueue> ring_;
        std::vector<io_uring_cqe> cqes_;
#endif

        static bool CheckChannel(sharpen::ChannelPtr channel) noexcept;

        void RegisterInternalEventFd(int fd,char internalVal);
    public:

        EpollSelector();

        ~EpollSelector() noexcept = default;

        virtual void Select(EventVector &events) override;
        
        virtual void Notify() override;
        
        virtual void Resister(WeakChannelPtr channel) override;

#ifdef SHARPEN_HAS_IOURING
        sharpen::IoUringQueue *GetRing() const noexcept;
#endif
    };
}

#endif
#endif
