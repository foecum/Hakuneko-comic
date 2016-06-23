#include "SubmangaCom.h"

SubmangaCom::SubmangaCom()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("Submanga");
    baseURL = wxT("http://submanga.com");
    referrerURL = wxT("http://submanga.com");
    mangaListFile.Assign(GetConfigurationPath(), wxT("submanga"), wxT("list"));
    LoadLocalMangaList();
}

SubmangaCom::~SubmangaCom()
{
    //
}

void SubmangaCom::UpdateMangaList()
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
    cr.SetUrl(baseURL + wxT("/series/n"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);

    // only update local list, if connection successful...
    if(cr.ExecuteRequest() && !content.IsEmpty())
    {
        int indexStart = content.find(wxT("<div id=\"tabla\">")) + 16;
        int indexEnd = content.rfind(wxT("<div id=\"inferior\">"));

        if(indexStart > 15 && indexEnd > indexStart)
        {
            content = content.Mid(indexStart, indexEnd-indexStart);
            indexEnd = 0;

            // Example Entry: <td><a href="http://submanga.com/zzz"><b class="xs">11.936.</b> zzz</a></td>
            while((indexStart = content.find(wxT("<td><a href=\""), indexEnd)) > -1)
            {
                indexStart += 13;
                indexEnd = content.find(wxT("\""), indexStart); // "\">"
                mangaLink = content.Mid(indexStart, indexEnd-indexStart) + wxT("/completa");

                indexStart = indexEnd + 2;
                indexStart = content.find(wxT("b>"), indexStart) + 3; // "</b> "
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

wxArrayMCEntry SubmangaCom::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString volumePrefix;
    wxString chNumber;
    wxString chScangroup;
    wxString chTitle;
    wxString chLink;

    CurlRequest cr;
    cr.SetUrl(MangaEntry->Link);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("<div id=\"tabla\">")) + 16;
    int indexEnd = content.find(wxT("<div id=\"paginacion\">"), indexStart);

    if(indexStart > 15 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry:
        // Headline: <tr><td colspan="3"><a href="#">Especiales</a></td></tr>
        // Valid:   <tr><td><a href="http://submanga.com/Distance/oneshot/236360">Distance<span class="label label-primary">OneShot</span></a></td><td><a href="http://submanga.com/scanlation/hentai-series">hentai-series</a></td><td><a href="#">15.714</a></td></tr>
        // Valid:   <tr><td><a href="http://submanga.com/Gantz/383/195163">Gantz <strong>383</strong></a></td><td><a href="http://submanga.com/scanlation/Mangatopia">Mangatopia</a></td><td><a href="#">114.958</a></td></tr>
        while((indexStart = content.find(wxT("<tr><td><a href=\""), indexEnd)) > -1)
        {
            indexStart += 17;
            indexEnd = content.find(wxT("\""), indexStart); // "\">"
            chLink = content.Mid(indexStart, indexEnd-indexStart);
            chLink = baseURL + wxT("/c/") + chLink.AfterLast('/');

            indexStart = indexEnd + 2;
            indexEnd = content.find(wxT("<"), indexStart); // "<"
            chTitle = content.Mid(indexStart, indexEnd-indexStart).Trim();

            indexStart = content.find(wxT(">"), indexEnd) + 1; // ">"
            indexEnd = content.find(wxT("<"), indexStart); // "</"
            chNumber = content.Mid(indexStart, indexEnd-indexStart);

            FormatChapterNumberStyle(&chNumber);

            indexStart = content.find(wxT("</td><td><a"), indexEnd) + 11; // "</td><td><a href="
            indexStart = content.find(wxT(">"), indexStart) + 1; // ">"
            indexEnd = content.find(wxT("<"), indexStart); // "</a>"
            chScangroup = content.Mid(indexStart, indexEnd-indexStart);

            // NOTE: submanga uses global chapter numbering, where the chapter numbers are unique (there aren't volumes anyway)
            // -> ignore volume prefix

            chapterList.Add(new MCEntry(HtmlUnescapeString(chNumber + wxT(" - ") + chTitle + wxT(" by [") + chScangroup + wxT("]")), chLink));

            //wxYield();
        }
    }

    return chapterList;
}

wxArrayString SubmangaCom::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    CurlRequest cr;
    cr.SetUrl(ChapterLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    //wxString content = GetHtmlContent(ChapterLink);

    int indexStart = content.find(wxT("<select")) + 7;
    int indexEnd = content.find(wxT("</select>"), indexStart);

    if(indexStart > 6 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <option selected value="2">2</option>
        while((indexStart = content.find(wxT("<option"), indexEnd)) > -1)
        {
            indexStart += 7;
            indexStart = content.find(wxT("\""), indexStart) + 1; // "\""
            indexEnd = content.find(wxT("\""), indexStart); // "\""
            pageLinks.Add(ChapterLink + wxT("/") + content.Mid(indexStart, indexEnd-indexStart));

            //wxYield();
        }
    }

    return pageLinks;
}

wxString SubmangaCom::GetImageLink(wxString PageLink)
{
    CurlRequest cr;
    cr.SetUrl(PageLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    //wxString content = GetHtmlContent(PageLink);

    // Example Entry: <img src="http://img6.submanga.com/pages/105/1056821bb/2.jpg"/>
    int indexStart = content.rfind(wxT("<img src=\"")) + 10;
    int indexEnd = content.find(wxT("\""), indexStart);

    if(indexStart > 9 && indexEnd >= -1)
    {
        return content.Mid(indexStart, indexEnd-indexStart);
    }
    else
    {
        return wxEmptyString;
    }
}
