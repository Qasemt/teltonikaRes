/*********************************************************************
*
*   HEADER NAME:
*       FileStream.h
*
* Copied from http://msdn.microsoft.com/en-us/library/windows/desktop/ms752876(v=vs.85).aspx
*---------------------------------------------------------------------
* $NoKeywords$
*********************************************************************/
#pragma once
#include "stdafx.h"

//----------------------------------------------------------------------
//! \brief File-based implementation of IStream interface
//! \details This is taken directly from the Microsoft SDK examples site
//!    for use with the MSXML XSL transformation module
//----------------------------------------------------------------------
class FileStream :
    public IStream
    {
    public:
        FileStream(HANDLE hFile);
        virtual ~FileStream(void);

        HRESULT static OpenFile(LPCWSTR pName, IStream ** ppStream, bool fWrite);
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
        virtual ULONG STDMETHODCALLTYPE AddRef(void);
        virtual ULONG STDMETHODCALLTYPE Release(void);

    // ISequentialStream Interface
    public:
        virtual HRESULT STDMETHODCALLTYPE Read(void* pv, ULONG cb, ULONG* pcbRead);
        virtual HRESULT STDMETHODCALLTYPE Write(void const* pv, ULONG cb, ULONG* pcbWritten);

    // IStream Interface
    public:
        virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER)                                               { return E_NOTIMPL; }
        virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*)    { return E_NOTIMPL; }
        virtual HRESULT STDMETHODCALLTYPE Commit(DWORD)                                                         { return E_NOTIMPL; }
        virtual HRESULT STDMETHODCALLTYPE Revert(void)                                                          { return E_NOTIMPL; }
        virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)                     { return E_NOTIMPL; }
        virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)                   { return E_NOTIMPL; }
        virtual HRESULT STDMETHODCALLTYPE Clone(IStream **)                                                     { return E_NOTIMPL; }

        virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER liDistanceToMove, DWORD dwOrigin, ULARGE_INTEGER* lpNewFilePointer);

        virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* pStatstg, DWORD grfStatFlag);

    private:
        HANDLE _hFile;
        LONG _refcount;
    };
