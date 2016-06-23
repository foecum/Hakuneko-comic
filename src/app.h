#ifndef MANGADOWNLOADERAPP_H
#define MANGADOWNLOADERAPP_H

#include <wx/app.h>
#include <wx/image.h>
#include "MangaDownloaderMain.h"

class MangaDownloaderApp : public wxApp
{
    MangaDownloaderFrame* Frame;

    public: virtual bool OnInit();
    public: virtual int OnRun();
    public: virtual int OnExit();
};

#endif // MANGADOWNLOADERAPP_H
