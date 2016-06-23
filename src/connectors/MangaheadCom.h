#ifndef MANGAHEADCOM_H
#define MANGAHEADCOM_H

#include "../MangaConnector.h"

class MangaheadCom : public MangaConnector
{
    public: MangaheadCom();
    public: virtual ~MangaheadCom();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // MANGAHEADCOM_H
