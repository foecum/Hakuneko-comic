#include "MangaConnectorCollection.h"

MangaConnectorCollection::MangaConnectorCollection(wxString User, wxString Pass)
{
    MangaConnectors.Add(new BatotoNet(User, Pass));
    MangaConnectors.Add(new DynastyScans());
    //MangaConnectors.Add(new Fakku());
    MangaConnectors.Add(new KissAnimeCom());
    MangaConnectors.Add(new KissMangaCom());
    MangaConnectors.Add(new MangafoxMe());
    MangaConnectors.Add(new Mangago());
    MangaConnectors.Add(new MangaheadCom());
    MangaConnectors.Add(new MangahereCom());
    MangaConnectors.Add(new MangahereEs());
    MangaConnectors.Add(new MangaPanda());
    MangaConnectors.Add(new MangareaderNet());
    //MangaConnectors.Add(new MeinmangaCom());
    //MangaConnectors.Add(new SubmangaCom());
    #if wxDEBUG_LEVEL > 1
    MangaConnectors.Add(new VirtualManga());
    #endif
}

MangaConnectorCollection::~MangaConnectorCollection()
{
    //
}

wxArrayString MangaConnectorCollection::GetConnectorLabels()
{
    wxArrayString labels;

    for(size_t i=0; i<MangaConnectors.GetCount(); i++)
    {
        labels.Add(MangaConnectors[i]->GetLabel());
    }

    return labels;
}

int MangaConnectorCollection::GetConnectorIndexFromLabel(wxString ConnectorLabel)
{
    for(int i=0; i<(int)MangaConnectors.GetCount(); i++)
    {
        if(ConnectorLabel.IsSameAs(MangaConnectors[i]->GetLabel()))
        {
            return i;
        }
    }
    return -1;
}

void MangaConnectorCollection::UpdateMangaLists()
{
    for(size_t i=0; i<MangaConnectors.GetCount(); i++)
    {
        MangaConnectors[i]->UpdateMangaList();
    }
}

wxArrayMCEntry MangaConnectorCollection::GetMangaList(wxString ConnectorLabel)
{
    int index = GetConnectorIndexFromLabel(ConnectorLabel);
    if(index > -1)
    {
        return MangaConnectors[index]->GetMangaList();
    }
    else
    {
        return wxArrayMCEntry();
    }
}

wxArrayMCEntry MangaConnectorCollection::GetChapterList(wxString ConnectorLabel, MCEntry* MangaEntry)
{
    int index = GetConnectorIndexFromLabel(ConnectorLabel);
    if(index > -1)
    {
        return MangaConnectors[index]->GetChapterList(MangaEntry);
    }
    else
    {
        return wxArrayMCEntry();
    }
}

MCJob MangaConnectorCollection::CreateJob(wxString ConnectorLabel, MCEntry* MangaEntry, MCEntry* ChapterEntry)
{
    int index = GetConnectorIndexFromLabel(ConnectorLabel);
    if(index > -1)
    {
        return MangaConnectors[index]->CreateJob(MangaEntry, ChapterEntry);
    }
    else
    {
        return MCJob();
    }
}

unsigned long MangaConnectorCollection::GenerateJobID(MCEntry* ChapterEntry)
{
    return MangaConnector::GenerateJobID(ChapterEntry);
}

bool MangaConnectorCollection::AddJob(MCJob Job)
{
    int index = GetConnectorIndexFromLabel(Job.Connector);
    if(index > -1)
    {
        return MangaConnectors[index]->AddJob(Job);
    }
    else
    {
        return false;
    }
}

void MangaConnectorCollection::RemoveJob(unsigned long JobID)
{
    // remove in all connectors
    for(size_t i=0; i<MangaConnectors.GetCount(); i++)
    {
        MangaConnectors[i]->RemoveJob(JobID);
    }
}

void MangaConnectorCollection::RemoveAllJobs()
{
    // remove in all connectors
    for(size_t i=0; i<MangaConnectors.GetCount(); i++)
    {
        MangaConnectors[i]->RemoveAllJobs();
    }
}

bool MangaConnectorCollection::ContainsJob(unsigned long JobID)
{
    // search in all connectors
    for(size_t i=0; i<MangaConnectors.GetCount(); i++)
    {
        if(MangaConnectors[i]->ContainsJob(JobID))
        {
            return true;
        }
    }
    return false;
}

size_t MangaConnectorCollection::GetJobCount()
{
    size_t n = 0;

    for(size_t i=0; i<MangaConnectors.GetCount(); i++)
    {
        n += MangaConnectors[i]->GetJobCount();
    }

    return n;
}

wxArrayJobID MangaConnectorCollection::GetCompletedJobIDs()
{
    wxArrayJobID completed;
    wxArrayJobID temp;

    for(size_t i=0; i<MangaConnectors.GetCount(); i++)
    {
        temp = MangaConnectors[i]->GetCompletedJobIDs();
        completed.Alloc(completed.GetCount() + temp.GetCount());
        for(size_t j=0; j<temp.GetCount(); j++)
        {
            completed.Add(temp[j]);
        }
    }

    return completed;
}

void MangaConnectorCollection::SetJobDownloadCompleted(unsigned long JobID, bool Value)
{
    for(size_t i=0; i<MangaConnectors.GetCount(); i++)
    {
        if(MangaConnectors[i]->ContainsJob(JobID))
        {
            MangaConnectors[i]->SetJobDownloadCompleted(JobID, Value);
        }
    }
}

wxArrayString MangaConnectorCollection::DownloadJobs(wxFileName BaseDirectory, wxStatusBar* StatusBar, bool* Abort, bool CompressChapters)
{
    wxArrayString temp;
    wxArrayString errorLog;

    if(!BaseDirectory.IsDirWritable())
    {
        wxMessageBox(wxT("Access denied!\nBase directory: ") + BaseDirectory.GetPath());
        return errorLog;
    }

    for(size_t i=0; i<MangaConnectors.GetCount(); i++)
    {
        temp = MangaConnectors[i]->DownloadJobs(BaseDirectory, StatusBar, Abort, CompressChapters);

        for(size_t n=0; n<temp.GetCount(); n++)
        {
            errorLog.Add(temp[n]);
        }
    }

    return errorLog;
}
