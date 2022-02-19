#include <sharpen/IoUringQueue.hpp>

#ifdef SHARPEN_HAS_IOURING

#include <cstring>

#include <fcntl.h>

#include <sharpen/IteratorOps.hpp>

sharpen::IoUringQueue::IoUringQueue()
    :eventFd_(0,O_CLOEXEC | O_NONBLOCK)
    ,ring_(Self::queueLength,0,0,0,0)
    ,compQueue_()
    ,subQueue_()
{
    this->subQueue_.reserve(32);
    this->compQueue_.reserve(32);
    this->ring_.RegisterEventFd(this->eventFd_);
}

sharpen::IoUringQueue::~IoUringQueue() noexcept
{
    this->ring_.UnregisterEventFd();
}

void sharpen::IoUringQueue::Submit()
{
    if(this->subQueue_.empty())
    {
        return;
    }
    sharpen::Size moved{0};
    while (this->ring_.Requestable())
    {
        this->ring_.SubmitToSring(&this->subQueue_.back() - moved);
        ++moved;
    }
    this->subQueue_.erase(sharpen::IteratorBackward(this->subQueue_.end(),moved),this->subQueue_.end());
    this->ring_.Enter(moved,0,0);
}

void sharpen::IoUringQueue::SubmitIoRequest(const Sqe &sqe)
{
    if(this->ring_.Requestable())
    {
        this->ring_.SubmitToSring(&sqe);
        this->ring_.Enter(1,0,0);
        return;
    }
    this->subQueue_.emplace_back(sqe);
    return;
}

sharpen::Size sharpen::IoUringQueue::GetCompletionStatus(Cqe *cqes,sharpen::Size size)
{
    if(!this->compQueue_.empty())
    {
        size = (std::min)(this->compQueue_.size(),size);
        auto begin = this->compQueue_.rbegin();
        auto end = sharpen::IteratorForward(begin,size);
        sharpen::Size index{0};
        while (begin != end)
        {
            std::memcpy(cqes + index,std::addressof(*begin),sizeof(*begin));
            ++begin;
            ++index;
        }
        this->compQueue_.erase(sharpen::IteratorBackward(this->compQueue_.end(),size),this->compQueue_.end());
        this->Submit();
        return size;
    }
    sharpen::Size index{0};
    while (index != size && this->ring_.GetFromCring(cqes + index))
    {
        ++index;
    }
    size = size - index;
    while (size != 0)
    {
        this->compQueue_.emplace_back();
        if(!this->ring_.GetFromCring(&this->compQueue_.back()))
        {
            break;
        }
        --size;
    }
    this->Submit();
    return index;
}

bool sharpen::TestIoUring() noexcept
{
    static int status{0};
    if(!status)
    {
        try
        {
            sharpen::IoUringQueue queue;
            status = 1;
            static_cast<void>(queue);
        }
        catch(const std::exception&)
        {
            status = 2;   
        }
    }
    return status == 1;
}
#endif