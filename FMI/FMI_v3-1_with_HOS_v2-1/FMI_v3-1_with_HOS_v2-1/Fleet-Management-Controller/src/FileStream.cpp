/*********************************************************************
*
*   MODULE NAME:
*       FileStream.h
*
* Copied from http://msdn.microsoft.com/en-us/library/windows/desktop/ms752876(v=vs.85).aspx
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/

//----------------------------------------------------------------------
//! \brief File-based implementation of IStream interface
//! \details This is taken directly from the Microsoft SDK examples site
//!    for use with the MSXML XSL transformation module
//----------------------------------------------------------------------
#include "FileStream.h"

FileStream::FileStream(HANDLE hFile)
    {
        _refcount = 1;
        _hFile = hFile;
    }

FileStream::~FileStream(void)
    {
        if (_hFile != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(_hFile);
        }
    }

HRESULT FileStream::OpenFile(LPCWSTR pName, IStream ** ppStream, bool fWrite)
    {
        HANDLE hFile = ::CreateFileW(pName, fWrite ? GENERIC_WRITE : GENERIC_READ, FILE_SHARE_READ,
            NULL, fWrite ? CREATE_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile == INVALID_HANDLE_VALUE)
            return HRESULT_FROM_WIN32(GetLastError());

        *ppStream = new FileStream(hFile);

        if(*ppStream == NULL)
            CloseHandle(hFile);

        return S_OK;
    }

HRESULT STDMETHODCALLTYPE FileStream::QueryInterface(REFIID iid, void ** ppvObject)
    {
        if (iid == __uuidof(IUnknown)
            || iid == __uuidof(IStream)
            || iid == __uuidof(ISequentialStream))
        {
            *ppvObject = static_cast<IStream*>(this);
            AddRef();
            return S_OK;
        } else
            return E_NOINTERFACE;
    }

ULONG STDMETHODCALLTYPE FileStream::AddRef(void)
    {
        return (ULONG)InterlockedIncrement(&_refcount);
    }

ULONG STDMETHODCALLTYPE FileStream::Release(void)
    {
        ULONG res = (ULONG) InterlockedDecrement(&_refcount);
        if (res == 0)
            delete this;
        return res;
    }

HRESULT STDMETHODCALLTYPE FileStream::Read(void* pv, ULONG cb, ULONG* pcbRead)
    {
        BOOL rc = ReadFile(_hFile, pv, cb, pcbRead, NULL);
        return (rc) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
    }

HRESULT STDMETHODCALLTYPE FileStream::Write(void const* pv, ULONG cb, ULONG* pcbWritten)
    {
        BOOL rc = WriteFile(_hFile, pv, cb, pcbWritten, NULL);
        return rc ? S_OK : HRESULT_FROM_WIN32(GetLastError());
    }

HRESULT STDMETHODCALLTYPE FileStream::Seek(LARGE_INTEGER liDistanceToMove, DWORD dwOrigin,
    ULARGE_INTEGER* lpNewFilePointer)
    {
        DWORD dwMoveMethod;

        switch(dwOrigin)
        {
        case STREAM_SEEK_SET:
            dwMoveMethod = FILE_BEGIN;
            break;
        case STREAM_SEEK_CUR:
            dwMoveMethod = FILE_CURRENT;
            break;
        case STREAM_SEEK_END:
            dwMoveMethod = FILE_END;
            break;
        default:
            return STG_E_INVALIDFUNCTION;
            break;
        }

        if (SetFilePointerEx(_hFile, liDistanceToMove, (PLARGE_INTEGER) lpNewFilePointer,
                             dwMoveMethod) == 0)
            return HRESULT_FROM_WIN32(GetLastError());
        return S_OK;
    }

HRESULT STDMETHODCALLTYPE FileStream::Stat(STATSTG* pStatstg, DWORD /*grfStatFlag*/)
    {
        if (GetFileSizeEx(_hFile, (PLARGE_INTEGER) &pStatstg->cbSize) == 0)
            return HRESULT_FROM_WIN32(GetLastError());
        return S_OK;
    }
