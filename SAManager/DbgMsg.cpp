#include "framework.h"
// H2K
void DebugMsg(LPCWSTR lpszFormat, ...)
{
    WCHAR* pBuf = NULL;

    try
    {
        pBuf = new WCHAR[4096];
    }
    catch (const std::bad_alloc&)
    {
        return;
    }

    if (NULL == pBuf)
        return;

    memset(pBuf, 0x00, 4096 * sizeof(WCHAR));

    va_list argList;
    va_start(argList, lpszFormat);
    vswprintf_s(pBuf, 4096, lpszFormat, argList);
    va_end(argList);

    OutputDebugString(pBuf);

    if (NULL != pBuf)
    {
        delete[] pBuf;
        pBuf = NULL;
    }
}