#ifndef MANGADOWNLOADERCONFIGURATION_H
#define MANGADOWNLOADERCONFIGURATION_H

#include <wx/string.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/utils.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>

class MangaDownloaderConfiguration
{
    public: MangaDownloaderConfiguration();
    public: ~MangaDownloaderConfiguration();

    public: void LoadConfiguration();
    public: void SaveConfiguration();

    private: wxFileName ConfigurationFile;
    public: wxString BaseDirectory;
    public: wxString Connector;
    public: bool ShowJoblist;
    public: bool StartupSync;
    public: bool TypingSearch;
    public: bool CompressChapters;
    public: wxString FilterSelected;
    public: wxArrayString FilterList;
};

#endif // MANGADOWNLOADERCONFIGURATION_H
