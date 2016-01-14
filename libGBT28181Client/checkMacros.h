#pragma once
#ifndef _LIBGBT28181_CHECK_MACROS_H_
#define _LIBGBT28181_CHECK_MACROS_H_

#define CHECK_NULL_AND_RETURN(pointer)  \
if(NULL == (pointer))                   \
{                                       \
    return GBT28181_NOMEM;              \
}                                       \
else                                    \
{                                       \
    return GBT28181_SUCCESS;            \
}

#define CHECK_NULL_PARAMETER(pointer)   \
if(NULL == (pointer))                   \
{                                       \
    return GBT28181_BADPARAMETER;       \
}

#define CHECK_INITIALED(bool_initialed) \
if(!(bool_initialed))                   \
{                                       \
    return GBT28181_CLIENT_UNINITIALED; \
}

#define CHECK_CALLBACK(pointer)         \
if(NULL == (pointer))                   \
{                                       \
    return GBT28181_CALLBACK_NOTSET;    \
}

#define CHECK_ONLINE_NO_SET(bool_online)    \
if((bool_online))                           \
{                                           \
    return GBT28181_ALREADY_ONLINE;         \
}

#define CHECK_MUST_ON_LINE(bool_online) \
if(!(bool_online))                      \
{                                       \
    return GBT28181_MUST_ONLINE;        \
}

#define CHECK_NOT_STREAMING(bool_streaming) \
if(bool_streaming)                          \
{                                           \
    return GBT28181_ALREADY_STREAMING;      \
}

#endif // !_LIBGBT28181_CHECK_MACROS_H_