#include "stdafx.h"
#include "CStringToChar.h"


CCStringToChar::CCStringToChar(void)
{
    m_buffer = new char[256];
}


CCStringToChar::~CCStringToChar(void)
{
    delete[] m_buffer;
}


CCStringToChar::CCStringToChar(CString& cstring)
{
    m_buffer = new char[256];
    m_result = WideCharToMultiByte(CP_UTF8, 0, cstring, cstring.GetLength(), m_buffer, 256, NULL, NULL);
    if(m_result > 0)
    {
        m_buffer[m_result] = '\0';
    }
}


char* CCStringToChar::GetCStyleString(void)
{
    if(m_result > 0)
    {
        return m_buffer;
    }
    else
    {
        return NULL;
    }
}
