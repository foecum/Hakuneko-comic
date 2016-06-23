#ifndef MANGAREADERNET_H
#define MANGAREADERNET_H

#include "../MangaConnector.h"

class MangareaderNet : public MangaConnector
{
    public: MangareaderNet();
    public: virtual ~MangareaderNet();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // MANGAREADERNET_H
