#include <cstdlib>

#include <sharpen/ExecuteContext.hpp>

thread_local bool sharpen::LocalEnableContextSwitch(false);

sharpen::ExecuteContext::ExecuteContext()
    :handle_()
{}

void sharpen::ExecuteContext::InternalEnableContextSwitch()
{
    if(!sharpen::LocalEnableContextSwitch)
    {
#ifdef SHARPEN_HAS_FIBER
      ::ConvertThreadToFiberEx(NULL,FIBER_FLAG_FLOAT_SWITCH);
#endif
      sharpen::LocalEnableContextSwitch = true;
    }
}

void sharpen::ExecuteContext::InternalDisableContextSwitch()
{
    if(sharpen::LocalEnableContextSwitch)
    {
#ifdef SHARPEN_HAS_FIBER
      ::ConvertFiberToThread();
#endif
      sharpen::LocalEnableContextSwitch = false;
    }
}

sharpen::ExecuteContext::~ExecuteContext()
{
#ifdef SHARPEN_HAS_FIBER
    if(this->handle_ != nullptr)
    {
      ::DeleteFiber(this->handle_);
    }
#else
    if(this->handle_.uc_stack.ss_sp != nullptr)
    {
      std::free(this->handle_.uc_stack.ss_up);
    }
#endif
}
