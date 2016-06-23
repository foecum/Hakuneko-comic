#ifndef MANGAFOXME_H
#define MANGAFOXME_H

#include "../MangaConnector.h"

class MangafoxMe : public MangaConnector
{
    public: MangafoxMe();
    public: virtual ~MangafoxMe();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // MANGAFOXME_H
