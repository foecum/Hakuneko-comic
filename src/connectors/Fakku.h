#ifndef FAKKU_H
#define FAKKU_H

#include "../MangaConnector.h"

class Fakku : public MangaConnector
{
    public: Fakku();
    public: virtual ~Fakku();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // FAKKU_H
