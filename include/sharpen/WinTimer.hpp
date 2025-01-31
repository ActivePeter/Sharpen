#pragma once
#ifndef _SHARPEN_WINTIMER_HPP
#define _SHARPEN_WINTIMER_HPP

#include "SystemMacro.hpp"
#ifdef SHARPEN_IS_WIN

#include "ITimer.hpp"
#include "Noncopyable.hpp"
#include "Nonmovable.hpp"

#define SHARPEN_HAS_WAITABLETIMER
namespace sharpen
{
     class WinTimer:public sharpen::ITimer,public sharpen::Noncopyable,public sharpen::Nonmovable
     {
     private:
          using Myhandle = void *;
          using Mybase = sharpen::ITimer;
          
          Myhandle handle_;
          std::atomic<sharpen::Future<bool>*> future_;

          static void WINAPI CompleteFuture(void *arg, DWORD, DWORD);
     public:
          WinTimer();

          virtual ~WinTimer() noexcept;

          virtual void WaitAsync(sharpen::Future<bool> &future, sharpen::Uint64 waitMs) override;

          virtual void Cancel() override;
     };
}

#endif
#endif