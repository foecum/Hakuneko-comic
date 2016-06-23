#ifndef SUBMANGA_H
#define SUBMANGA_H

#include "../MangaConnector.h"

class SubmangaCom : public MangaConnector
{
    public: SubmangaCom();
    public: virtual ~SubmangaCom();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // SUBMANGA_H
