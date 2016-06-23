#ifndef MEINMANGA_H
#define MEINMANGA_H

#include "../MangaConnector.h"

class MeinmangaCom : public MangaConnector
{
    public: MeinmangaCom();
    public: virtual ~MeinmangaCom();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // MEINMANGA_H
