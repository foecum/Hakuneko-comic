#ifndef KISSMANGA_H
#define KISSMANGA_H

#include "../MangaConnector.h"

class KissMangaCom : public MangaConnector
{
    public: KissMangaCom();
    public: virtual ~KissMangaCom();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);

    private: wxUint32 GetMangaListPageCount();
    private: void Activate(wxString Link);
    // initialize the global cookies variable, required for all requests
    private: void CFCookie(wxInt32 RetryCount);
    // run a javascript using NodeJS service and return the result
    private: wxString JSRiddle(wxString Script);

    private: wxString agent;
    private: wxString cookies;
};

#endif // KISSANIME_H
