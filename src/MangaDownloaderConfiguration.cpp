#include "MangaDownloaderConfiguration.h"

MangaDownloaderConfiguration::MangaDownloaderConfiguration()
{
    #ifdef PORTABLE
        #ifdef __LINUX__
            ConfigurationFile.Assign(wxStandardPaths::Get().GetExecutablePath().BeforeLast('/') + wxT("/gui.conf"));
        #endif
        #ifdef __WINDOWS__
            ConfigurationFile.Assign(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\gui.conf"));
        #endif
    #else
        #ifdef __LINUX__
            wxString EnvironmentFilePath;
            wxGetEnv(wxT("XDG_CONFIG_HOME"), &EnvironmentFilePath);
            EnvironmentFilePath = EnvironmentFilePath.BeforeFirst(':');
            if(EnvironmentFilePath.IsEmpty())
            {
                EnvironmentFilePath = wxStandardPaths::Get().GetUserConfigDir() + wxT("/.config/hakuneko/gui.conf");
            }
            else
            {
                EnvironmentFilePath +=  wxT("/hakuneko/gui.conf");
            }
            ConfigurationFile.Assign(EnvironmentFilePath);
            // test if a non XDG compliant configuration exists (hakuneko < 1.0.5)
            wxString DeprecatedPath = wxStandardPaths::Get().GetUserDataDir();
            if(wxDirExists(DeprecatedPath))
            {
                // check if deprecated configuration path is different from current
                if(!DeprecatedPath.IsSameAs(ConfigurationFile.GetPath()))
                {
                    wxMessageBox(wxT("Hakuneko has detected a deprecated configuration directory that will no longer be used in:\n\n") + wxStandardPaths::Get().GetUserDataDir() + wxT("\n\nThe new configuration directory is located in:\n\n") + ConfigurationFile.GetPath() + wxT("\n\nYou can move your deprecated files into the new configuration directory and/or delete the old configuration directory."));
                }
            }
        #endif
        #ifdef __WINDOWS__
            ConfigurationFile.Assign(wxStandardPaths::Get().GetUserDataDir() + wxT("\\gui.conf"));
        #endif
    #endif
}

MangaDownloaderConfiguration::~MangaDownloaderConfiguration()
{
    //
}

void MangaDownloaderConfiguration::LoadConfiguration()
{
    wxTextFile f;
/*
    if(!ConfigurationFile.IsDirWritable())
    {
        wxMessageBox(wxT("Access denied!\nConfiguration directory: ") + ConfigurationFile.GetPath());
        return;
    }
*/
    ConfigurationFile.Mkdir(0755, wxPATH_MKDIR_FULL);

    // create file, or open if already exists
    if(!f.Create(ConfigurationFile.GetFullPath()))
    {
        f.Open(ConfigurationFile.GetFullPath());
    }

    wxString line = f.GetFirstLine();

    while(!f.Eof())
    {
        if(line.StartsWith(wxT("startupsync=")))
        {
            if(line.AfterFirst(L'=').IsSameAs(wxT("true")))
            {
                StartupSync = true;
            }
            else
            {
                StartupSync = false;
            }
        }

        if(line.StartsWith(wxT("directory=")))
        {
            BaseDirectory = line.AfterFirst(L'=');
        }

        if(line.StartsWith(wxT("connector=")))
        {
            Connector = line.AfterFirst(L'=');
        }

        if(line.StartsWith(wxT("viewjobs=")))
        {
            if(line.AfterFirst(L'=').IsSameAs(wxT("true")))
            {
                ShowJoblist = true;
            }
            else
            {
                ShowJoblist = false;
            }
        }

        if(line.StartsWith(wxT("typingsearch=")))
        {
            if(line.AfterFirst(L'=').IsSameAs(wxT("true")))
            {
                TypingSearch = true;
            }
            else
            {
                TypingSearch = false;
            }
        }

        if(line.StartsWith(wxT("compresschapters=")))
        {
            if(line.AfterFirst(L'=').IsSameAs(wxT("true")))
            {
                CompressChapters = true;
            }
            else
            {
                CompressChapters = false;
            }
        }

        if(line.StartsWith(wxT("filter=")))
        {
            FilterSelected = line.AfterFirst(L'=');
        }

        if(line.StartsWith(wxT("filter[]=")))
        {
            FilterList.Add(line.AfterFirst(L'='));
        }

        line = f.GetNextLine();
    }

    f.Close();
}

void MangaDownloaderConfiguration::SaveConfiguration()
{
    wxTextFile f;
/*
    if(!ConfigurationFile.IsDirWritable())
    {
        wxMessageBox(wxT("Access denied!\nConfiguration directory: ") + ConfigurationFile.GetPath());
        return;
    }
*/
    ConfigurationFile.Mkdir(0755, wxPATH_MKDIR_FULL);

    // create file, or open if already exists
    if(!f.Create(ConfigurationFile.GetFullPath()))
    {
        f.Open(ConfigurationFile.GetFullPath());
        f.Clear();
    }

    if(StartupSync)
    {
        f.AddLine(wxT("startupsync=true"));
    }
    else
    {
        f.AddLine(wxT("startupsync=false"));
    }
    f.AddLine(wxT("directory=") + BaseDirectory);
    f.AddLine(wxT("connector=") + Connector);
    if(ShowJoblist)
    {
        f.AddLine(wxT("viewjobs=true"));
    }
    else
    {
        f.AddLine(wxT("viewjobs=false"));
    }
    if(TypingSearch)
    {
        f.AddLine(wxT("typingsearch=true"));
    }
    else
    {
        f.AddLine(wxT("typingsearch=false"));
    }
    if(CompressChapters)
    {
        f.AddLine(wxT("compresschapters=true"));
    }
    else
    {
        f.AddLine(wxT("compresschapters=false"));
    }
    f.AddLine(wxT("filter=") + FilterSelected);
    for(unsigned int i=0; i<FilterList.GetCount(); i++)
    {
        f.AddLine(wxT("filter[]=") + FilterList[i]);
    }

    f.Write();
    f.Close();
}
