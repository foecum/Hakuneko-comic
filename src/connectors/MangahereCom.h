#ifndef MANGAHERE_H
#define MANGAHERE_H

#include "../MangaConnector.h"

class MangahereCom : public MangaConnector
{
    public: MangahereCom();
    public: virtual ~MangahereCom();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // MANGAHERE_H
