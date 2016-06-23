#ifndef KISSANIME_H
#define KISSANIME_H

#include "../MangaConnector.h"

class KissAnimeCom : public MangaConnector
{
    public: KissAnimeCom();
    public: virtual ~KissAnimeCom();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);

    private: wxUint32 GetAnimeListPageCount();
    // Get all resolutions for a given episode and add the entries to the chapter list
    private: void GetResolutionList(wxString ChapterTitle, wxString ChapterLink, wxArrayMCEntry* ChapterList);
    private: void Activate(wxString Link);
    // initialize the global cookies variable, required for all requests
    private: void CFCookie(wxInt32 RetryCount);
    // run a javascript using NodeJS service and return the result
    private: wxString JSRiddle(wxString Script);

    private: wxString agent;
    private: wxString cookies;
};

#endif // KISSANIME_H
