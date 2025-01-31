#pragma once
#ifndef _SHARPEN_TCPSERVER_HPP
#define _SHARPEN_TCPSERVER_HPP

#include <atomic>

#include "TcpAcceptor.hpp"
#include "AwaitableFuture.hpp"

namespace sharpen
{
    class TcpServer:public sharpen::Noncopyable,public sharpen::Nonmovable
    {
    private:

        sharpen::TcpAcceptor acceptor_;
        std::atomic_bool running_;
        sharpen::AwaitableFuture<void> waiter_;
    protected:
        sharpen::EventEngine *engine_;

        virtual void OnNewChannel(sharpen::NetStreamChannelPtr channel) = 0;
    public:
        TcpServer(sharpen::AddressFamily af,const sharpen::IEndPoint &endpoint,sharpen::EventEngine &engine);

        virtual ~TcpServer() noexcept
        {
            this->Stop();
        }

        void RunAsync();

        void Stop() noexcept;

        inline void GetLocalEndPoint(sharpen::IEndPoint &endpoint) const
        {
            this->acceptor_.GetLocalEndPoint(endpoint);
        }

        inline void GetRemoteEndPoint(sharpen::IEndPoint &endpoint) const
        {
            this->acceptor_.GetRemoteEndPoint(endpoint);
        }

        inline bool IsRunning() const noexcept
        {
            return this->running_;
        }
    };
}

#endif