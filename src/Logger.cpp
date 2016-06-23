#include "Logger.h"

Logger::Logger()
{
    //
}

Logger::~Logger()
{
    //
}

wxFileName Logger::LogFile()
{
	wxFileName LogFile;
    #ifdef PORTABLE
        #ifdef __LINUX__
            LogFile.Assign(wxStandardPaths::Get().GetExecutablePath() + wxT(".log"));
        #endif
        #ifdef __WINDOWS__
            LogFile.Assign(wxStandardPaths::Get().GetExecutablePath() + wxT(".log"));
        #endif
    #else
        #ifdef __LINUX__
            wxString EnvironmentFilePath;
            wxGetEnv(wxT("XDG_CONFIG_HOME"), &EnvironmentFilePath);
            EnvironmentFilePath = EnvironmentFilePath.BeforeFirst(':');
            if(EnvironmentFilePath.IsEmpty())
            {
                EnvironmentFilePath = wxStandardPaths::Get().GetUserConfigDir() + wxT("/.config/hakuneko/hakuneko.log");
            }
            else
            {
                EnvironmentFilePath +=  wxT("/hakuneko/hakuneko.log");
            }
            LogFile.Assign(EnvironmentFilePath);
        #endif
        #ifdef __WINDOWS__
            LogFile.Assign(wxStandardPaths::Get().GetUserDataDir() + wxT("\\hakuneko.log"));
        #endif
    #endif
    return LogFile;
}

void Logger::Init()
{
    wxFileName LogFile = Logger::LogFile();
    LogFile.Mkdir(0755, wxPATH_MKDIR_FULL);
    if(LogFile.Exists())
    {
        bool reset = false;
        wxFile log(LogFile.GetFullPath(), wxFile::read);
        if(log.Length() > 5242880)
        {
            reset = true;
        }
        log.Close();

        if(reset)
        {
            wxRemoveFile(LogFile.GetFullPath());
        }
    }

    Logger::Log(wxEmptyString);
    Logger::Log(wxT("+++++++++++++++++++++++++++++++++"));
    Logger::Log(wxT("+++ ") + wxDateTime::Now().Format(wxT("%Y-%m-%dT%H:%M:%S +0000"), wxDateTime::UTC) + wxT(" +++"));
    Logger::Log(wxT("+++++++++++++++++++++++++++++++++"));
    Logger::Log(wxEmptyString);
}

void Logger::Log(wxString Text)
{
    wxFileName LogFile = Logger::LogFile();
    // TODO: check if log file exist / was initialized
    wxFile log(LogFile.GetFullPath(), wxFile::write_append);
    #ifdef __LINUX__
        log.Write(Text + wxT("\n"));
    #endif
    #ifdef __WINDOWS__
        log.Write(Text + wxT("\r\n"));
    #endif
    log.Close();
}
