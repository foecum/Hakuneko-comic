#ifndef MANGAGO_H
#define MANGAGO_H

#include "../MangaConnector.h"

class Mangago : public MangaConnector
{
    public: Mangago();
    public: virtual ~Mangago();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);

    private: unsigned short getChapterListPageNumber(void);
};

#endif // MANGAGO_H
