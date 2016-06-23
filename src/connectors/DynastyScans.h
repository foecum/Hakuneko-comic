#ifndef DYNASTYSCANS_H
#define DYNASTYSCANS_H

#include "../MangaConnector.h"

class DynastyScans : public MangaConnector
{
    public: DynastyScans();
    public: virtual ~DynastyScans();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // DYNASTYSCANS_H
