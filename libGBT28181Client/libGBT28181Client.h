#ifndef _LIBGBT28181CLIENT_H_
#define _LIBGBT28181CLIENT_H_

#ifdef __cplusplus
#define C_EXPORT_BEGIN extern "C" {
#define C_EXPORT_END }
#else
#define C_EXPORT_BEGIN
#define C_EXPORT_END
#endif // __cplusplus

#ifdef LIBGBT28181CLIENT_EXPORTS
#define LIBGBT28181CLIENT_API __declspec(dllexport)
#else
#define LIBGBT28181CLIENT_API __declspec(dllimport)
#endif

C_EXPORT_BEGIN

LIBGBT28181CLIENT_API int GBT28181_client_initial(int local_port, char* SIP_user_name, char* SIP_user_ID, char* SIP_password);

C_EXPORT_END

#endif // !_LIBGBT28181CLIENT_H_



