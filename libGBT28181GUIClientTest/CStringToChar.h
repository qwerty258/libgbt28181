#pragma once
class CCStringToChar
{
public:
    CCStringToChar(void);
    CCStringToChar(CString& cstring);
    ~CCStringToChar(void);
    char* GetCStyleString(void);
private:
    char* m_buffer;
    int m_result;
};

