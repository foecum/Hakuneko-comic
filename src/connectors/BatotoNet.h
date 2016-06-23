#ifndef BATOTO_H
#define BATOTO_H

#include "../MangaConnector.h"

class BatotoNet : public MangaConnector
{
    public: BatotoNet(wxString User = wxEmptyString, wxString Pass = wxEmptyString);
    public: virtual ~BatotoNet();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);

    // initialize the global cookies variable, required for all requests
    private: void Login(wxString User = wxEmptyString, wxString Pass = wxEmptyString);

    private: wxString cookies;
};

#endif // BATOTO_H
