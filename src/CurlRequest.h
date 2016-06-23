#ifndef CURLREQUEST_H
#define CURLREQUEST_H

#include <wx/string.h>
#include <wx/statusbr.h>
#include <wx/stream.h>
#include <wx/uri.h>
#include <curl/curl.h>
#include "Logger.h"

class CurlRequest
{
    public: CurlRequest(bool Follow=true);
    public: ~CurlRequest();

    public: void SetUrl(wxString Url);
    public: void SetHeader(bool Include = true);
    // identity, deflate, gzip
    public: void SetCompression(wxString Compression);
    public: void SetAgent(wxString Agent);
    public: void SetReferer(wxString Referer);
    public: void SetCookies(wxString Cookies);
    public: void SetGetData(wxString Data = wxEmptyString);
    public: void SetPostData(wxString Data = wxEmptyString);
    public: void SetOutputStream(wxOutputStream* Stream);
    public: void SetOutputStreamFixUTF8(bool FixUTF8);
    public: void SetStatusBar(wxStatusBar* Bar, wxUint32 Field);
    public: void SetAbort(bool* Abort);
    public: bool ExecuteRequest();
    private: static size_t WriteCallback(char* data, size_t chunk_size, size_t chunk_count, void* curl_request);

    private: CURL* curl;
    private: bool* abort;
    private: wxOutputStream* stream;
    private: wxStatusBar* bar;
    private: wxUint32 bar_field;
    private: bool fix_utf;
};

#endif // CURLREQUEST_H
