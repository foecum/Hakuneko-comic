#ifndef MANGAPANDA_H
#define MANGAPANDA_H

#include "../MangaConnector.h"

class MangaPanda : public MangaConnector
{
    public: MangaPanda();
    public: virtual ~MangaPanda();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // MANGAPANDA_H
