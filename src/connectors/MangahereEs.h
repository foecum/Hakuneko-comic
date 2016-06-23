#ifndef MANGAHERE_ES_H
#define MANGAHERE_ES_H

#include "../MangaConnector.h"

class MangahereEs : public MangaConnector
{
    public: MangahereEs();
    public: virtual ~MangahereEs();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // MANGAHERE_ES_H
