#include "MeinmangaCom.h"

MeinmangaCom::MeinmangaCom()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("MeinManga");
    baseURL = wxT("http://meinmanga.com");
    referrerURL = wxT("http://meinmanga.com");
    mangaListFile.Assign(GetConfigurationPath(), wxT("meinmanga"), wxT("list"));
    LoadLocalMangaList();
}

MeinmangaCom::~MeinmangaCom()
{
    //
}

void MeinmangaCom::UpdateMangaList()
{
    wxTextFile f;
/*
    if(!mangaListFile.IsDirWritable())
    {
        wxMessageBox(wxT("Access denied!\nConfiguration directory: ") + mangaListFile.GetPath());
        return;
    }
*/
    mangaListFile.Mkdir(0755, wxPATH_MKDIR_FULL);

    // create file, or open if already exists
    if(!f.Create(mangaListFile.GetFullPath()))
    {
        f.Open(mangaListFile.GetFullPath());
        f.Clear();
    }

    wxString mangaLink;
    wxString mangaLabel;

    CurlRequest cr;
    cr.SetUrl(baseURL + wxT("/directory/all/"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);

    // only update local list, if connection successful...
    if(cr.ExecuteRequest() && !content.IsEmpty())
    {
        int indexStart = content.find(wxT("<th>Serienname</th>")) + 19;
        int indexEnd = content.rfind(wxT("</table>"));

        if(indexStart > 18 && indexEnd > indexStart)
        {
            content = content.Mid(indexStart, indexEnd-indexStart);
            indexEnd = 0;

            // Example Entry: <a href="/manga">Label</a>
            while((indexStart = content.find(wxT("<tr>"), indexEnd)) > -1)
            {
                indexStart += 4;
                indexStart = content.find(wxT("<a href=\""), indexStart) + 9; // "\">"
                indexEnd = content.find(wxT("\""), indexStart); // "\">"
                mangaLink = content.Mid(indexStart, indexEnd-indexStart);

                indexStart = indexEnd + 2;
                indexEnd = content.find(wxT("<"), indexStart); // "</a>"
                mangaLabel = content.Mid(indexStart, indexEnd-indexStart);

                if(!mangaLabel.IsEmpty())
                {
                    f.AddLine(HtmlUnescapeString(mangaLabel) + wxT("\t") + mangaLink);
                }

                //wxYield();
            }
        }
    }
    sos.Close();
    f.Write();
    f.Close();
    LoadLocalMangaList();
}

wxArrayMCEntry MeinmangaCom::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;
    return chapterList;
}

wxArrayString MeinmangaCom::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;
    return pageLinks;
}

wxString MeinmangaCom::GetImageLink(wxString PageLink)
{
    return wxEmptyString;
}
