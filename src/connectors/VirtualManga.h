#ifndef VIRTUALMANGA_H
#define VIRTUALMANGA_H

#include "../MangaConnector.h"

class VirtualManga : public MangaConnector
{
    public: VirtualManga();
    public: virtual ~VirtualManga();

    public: /*override*/ void UpdateMangaList();
    private: /*override*/ wxArrayMCEntry GetChapterList(MCEntry* MangaEntry);
    public: /*override*/ wxArrayString GetPageLinks(wxString ChapterLink);
    public: /*override*/ wxString GetImageLink(wxString PageLink);
};

#endif // VIRTUALMANGA_H
