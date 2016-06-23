#ifndef MANGACONNECTORCOLLECTION_H
#define MANGACONNECTORCOLLECTION_H

#include <wx/dir.h>

#include "connectors/VirtualManga.h"
#include "connectors/MangareaderNet.h"
#include "connectors/MangafoxMe.h"
#include "connectors/MangahereCom.h"
#include "connectors/MangahereEs.h"
#include "connectors/BatotoNet.h"
#include "connectors/SubmangaCom.h"
#include "connectors/MeinmangaCom.h"
#include "connectors/KissAnimeCom.h"
#include "connectors/MangaheadCom.h"
#include "connectors/DynastyScans.h"
#include "connectors/KissMangaCom.h"
#include "connectors/Fakku.h"
#include "connectors/MangaPanda.h"
#include "connectors/Mangago.h"
#include "MangaConnector.h"

class MangaConnectorCollection
{
    public: MangaConnectorCollection(wxString User = wxEmptyString, wxString Pass = wxEmptyString);
    public: virtual ~MangaConnectorCollection();

    private: wxArrayMangaConnector MangaConnectors;

    public: wxArrayString GetConnectorLabels();
    private: int GetConnectorIndexFromLabel(wxString ConnectorLabel);
    public: void UpdateMangaLists();
    public: wxArrayMCEntry GetMangaList(wxString ConnectorLabel);
    public: wxArrayMCEntry GetChapterList(wxString ConnectorLabel, MCEntry* MangaEntry);
    public: MCJob CreateJob(wxString ConnectorLabel, MCEntry* MangaEntry, MCEntry* ChapterEntry);
    public: unsigned long GenerateJobID(MCEntry* ChapterEntry);
    public: bool AddJob(MCJob Job);
    public: void RemoveJob(unsigned long JobID);
    public: void RemoveAllJobs();
    public: bool ContainsJob(unsigned long JobID);
    public: size_t GetJobCount();
    public: wxArrayJobID GetCompletedJobIDs();
    public: void SetJobDownloadCompleted(unsigned long JobID, bool Value);
    public: wxArrayString DownloadJobs(wxFileName BaseDirectory, wxStatusBar* StatusBar, bool* Abort, bool CompressChapters = false);
};

#endif // MANGACONNECTORCONNECTION_H
