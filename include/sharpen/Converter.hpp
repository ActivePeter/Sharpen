#pragma once
#ifndef _SHARPEN_CONVERTER_HPP
#define _SHARPEN_CONVERTER_HPP

#include <type_traits>
#include <cassert>
#include <stdexcept>

#include "TypeDef.hpp"

namespace sharpen
{
    template<typename _T,typename _RawType = typename std::remove_const<typename std::remove_reference<_T>::type>::type,typename _IsNum = typename std::enable_if<std::is_integral<_RawType>::value || std::is_floating_point<_RawType>::value>::type,typename _IsUnsig = typename std::enable_if<std::is_unsigned<_RawType>::value>::type>
    auto InternalGetAbs(_T &&v,int) -> decltype(std::forward<_T>(v))
    {
        return std::forward<_T>(v);
    }

    template<typename _T,typename _RawType = typename std::remove_const<typename std::remove_reference<_T>::type>::type,typename _IsNum = typename std::enable_if<std::is_integral<_RawType>::value || std::is_floating_point<_RawType>::value>::type>
    auto InternalGetAbs(_T &&v,...) ->decltype(v < 0 ? -v:v)
    {
        return v < 0 ? -v:v;
    }

    template<typename _T>
    auto GetAbs(_T &&v) ->decltype(sharpen::InternalGetAbs(v,0))
    {
        return sharpen::InternalGetAbs(v,0);
    }

    template<typename _T,typename _Check = typename std::enable_if<(std::is_integral<_T>::value && std::is_signed<_T>::value) || std::is_floating_point<_T>::value>::type>
    constexpr bool InternalIsNegative(const _T &value,...) noexcept
    {
        return value < 0;
    }

    template<typename _T,typename _Check = typename std::enable_if<std::is_integral<_T>::value && std::is_unsigned<_T>::value>::type>
    constexpr bool InternalIsNegative(const _T &value,int) noexcept
    {
        return false;
    }

    template<typename _T>
    constexpr auto IsNegative(const _T &value) noexcept -> decltype(sharpen::InternalIsNegative(value,0))
    {
        return sharpen::IsNegative(value,0);
    }

    //unsafe
    //bufSize must be checked by user
    template<typename _T,typename _RawType = typename std::remove_const<typename std::remove_reference<_T>::type>::type,typename _IsNum = typename std::enable_if<std::is_integral<_RawType>::value>::type>
    sharpen::Size Itoa(_T &&val,sharpen::Byte radix,char *buf)
    {
        if (!buf)
        {
            return 0;
        }
        const char *index = "0123456789ABCDEF";
        sharpen::Size i{0};
        sharpen::Size s{0};
        _RawType num{sharpen::GetAbs(val)};
        if (val < 0)
        {
            buf[0] = '-';
            i++;
        }
        if(num == 0)
        {
            buf[0] = '0';
            return 1;
        }
        while (num)
        {
            buf[i] = index[num % radix];
            i++;
            num /= radix;
        }
        s = i;
        i--;
        if (buf[0] == '-')
        {
            buf += 1;
            i--;
        }
        for (sharpen::Size t = 0,size = i + 1,count = size/2;t < count;++t)
        {
            char tmp = buf[t];
            buf[t] = buf[i - t];
            buf[i - t] = tmp;
        }
        return s;
    }

    template<typename _T,typename _RawType = typename std::remove_const<typename std::remove_reference<_T>::type>::type,typename _IsNum = typename std::enable_if<std::is_integral<_RawType>::value && !std::is_unsigned<_RawType>::value>::type>
    _T InternalAtoi(const char *str,sharpen::Size size,sharpen::Size radix,int)
    {
        _T data{0};
        bool n{false};
        const char *end = str + size;
        while (str != end)
        {
            data *= static_cast<_T>(radix);
            if(*str == '-')
            {
                n = true;
            }
            else if(radix == 16 && *str >= 'A' && *str <= 'F')
            {
                data += *str - 'A' + 10;
            }
            else if(radix == 16 && *str >= 'a' && *str <= 'f')
            {
                data += *str - 'a' + 10;
            }
            else if(*str >= '0' && *str <= '9')
            {
                data += *str - '0';
            }
            else
            {
                throw std::invalid_argument("string is not a number");
            }
            ++str;
        }
        if(n)
        {
            data = -data;
        }
        return data;
    }

    template<typename _T,typename _RawType = typename std::remove_const<typename std::remove_reference<_T>::type>::type,typename _IsNum = typename std::enable_if<std::is_integral<_RawType>::value>::type>
    _T InternalAtoi(const char *str,sharpen::Size size,sharpen::Size radix,...)
    {
        _T data{0};
        const char *end = str + size;
        while (str != end)
        {
            data *= static_cast<_T>(radix);
            if(*str == '-')
            {
                throw std::invalid_argument("string is not a unsigned number");
            }
            else if(radix == 16 && *str >= 'A' && *str <= 'F')
            {
                data += *str - 'A' + 10;
            }
            else if(radix == 16 && *str >= 'a' && *str <= 'f')
            {
                data += *str - 'a' + 10;
            }
            else if(*str >= '0' && *str <= '9')
            {
                data += *str - '0';
            }
            else
            {
                throw std::invalid_argument("string is not a number");
            }
            ++str;
        }
        return data;
    }

    template<typename _T,typename _RawType = typename std::remove_const<typename std::remove_reference<_T>::type>::type,typename _IsNum = typename std::enable_if<std::is_integral<_RawType>::value>::type>
    _T Atoi(const char *str,sharpen::Size size,sharpen::Size radix = 10)
    {
        return sharpen::InternalAtoi<_T>(str,size,radix,0);
    }
}
#endif