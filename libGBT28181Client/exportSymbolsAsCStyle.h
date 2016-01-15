#pragma once
#ifndef _EXPORT_SYMBOLS_AS_C_STYLE_H_
#define _EXPORT_SYMBOLS_AS_C_STYLE_H_


#ifdef __cplusplus
#define C_EXPORT_BEGIN extern "C" {
#define C_EXPORT_END }
#else
#define C_EXPORT_BEGIN
#define C_EXPORT_END
#endif // __cplusplus


#if defined _WIN32 || defined __CYGWIN__
#define _SYMBOLS_IMPORT __declspec(dllimport)
#define _SYMBOLS_EXPORT __declspec(dllexport)
#else
#if __GNUC__ >= 4
#define _SYMBOLS_IMPORT __attribute__((visibility ("default")))
#define _SYMBOLS_EXPORT __attribute__((visibility ("default")))
#else
#define _SYMBOLS_IMPORT
#define _SYMBOLS_EXPORT
#endif // __GNUC__ >= 4
#endif // defined _WIN32 || defined __CYGWIN__


#ifdef EXPORT_SYMBOLS
#define EXPORT_SYMBOLS_API _SYMBOLS_EXPORT
#else
#define EXPORT_SYMBOLS_API _SYMBOLS_IMPORT
#endif // EXPORT_SYMBOLS


#endif // !_EXPORT_SYMBOLS_AS_C_STYLE_H_