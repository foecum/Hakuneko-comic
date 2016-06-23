#include "MangareaderNet.h"

MangareaderNet::MangareaderNet()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("MangaReader");
    baseURL = wxT("http://www.mangareader.net");
    referrerURL = wxT("http://www.mangareader.net");
    mangaListFile.Assign(GetConfigurationPath(), wxT("mangareader"), wxT("list"));
    LoadLocalMangaList();
}

MangareaderNet::~MangareaderNet()
{
    //
}

void MangareaderNet::UpdateMangaList()
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
    cr.SetUrl(baseURL + wxT("/alphabetical"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);

    // only update local list, if connection successful...
    if(cr.ExecuteRequest() && !content.IsEmpty())
    {
        int indexStart = content.find(wxT("<div class=\"series_col\">")) + 24;
        int indexEnd = content.rfind(wxT("<div id=\"adfooter\">"));

        if(indexStart > 23 && indexEnd > indexStart)
        {
            content = content.Mid(indexStart, indexEnd-indexStart);
            indexEnd = 0;

            // Example Entry: <li><a href="/manga">Label</a></li>
            while((indexStart = content.find(wxT("<li><a href=\""), indexEnd)) > -1)
            {
                indexStart += 13;
                indexEnd = content.find(wxT("\""), indexStart); // "\">"
                mangaLink = baseURL + content.Mid(indexStart, indexEnd-indexStart);

                indexStart = indexEnd + 2;
                indexEnd = content.find(wxT("</"), indexStart); // "</a>" | find "</" instead "<" because mangareader uses < > symbols in labels instead of html entities (&gt; &lt;)
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

wxArrayMCEntry MangareaderNet::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString volumePrefix;
    wxString chNumber;
    wxString chTitle;
    wxString chLink;

    CurlRequest cr;
    cr.SetUrl(MangaEntry->Link);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("<table id=\"listing\">")) + 20;
    int indexEnd = content.find(wxT("</table>"), indexStart);

    if(indexStart > 19 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <a href="/id/manga/chapter.html">Chapter 1</a> : Chapter Name</td>
        while((indexStart = content.find(wxT("<a href=\""), indexEnd)) > -1)
        {
            indexStart += 9;
            indexEnd = content.find(wxT("\""), indexStart); // "\">"
            chLink = baseURL + content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 2;
            indexEnd = content.find(wxT("</"), indexStart); // "</a> : " | find "</" instead "<" because mangareader uses < > symbols in labels instead of html entities (&gt; &lt;)
            chNumber = content.Mid(indexStart, indexEnd-indexStart).AfterLast(' ');

            FormatChapterNumberStyle(&chNumber);

            indexStart = indexEnd + 7;
            indexEnd = content.find(wxT("<"), indexStart); // "</td>"
            chTitle = content.Mid(indexStart, indexEnd-indexStart);

            // NOTE: mangareader uses global chapter numbering, where the chapter numbers are unique (there aren't volumes anyway)
            // -> ignore volume prefix

            // in case chTitle is NULL or whitespaced, whitespaces will be removed bei MCEntry::SetSafeLabel()
            chapterList.Add(new MCEntry(HtmlUnescapeString(chNumber + wxT(" - ") + chTitle), chLink));

            //wxYield();
        }
    }

    return chapterList;
}

wxArrayString MangareaderNet::GetPageLinks(wxString ChapterLink)
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

    int indexStart = content.find(wxT("<select id=\"pageMenu\" name=\"pageMenu\">")) + 37;
    int indexEnd = content.find(wxT("</select>"), indexStart);

    if(indexStart > 36 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <option value="/id/manga/chapter.html">39</option>
        while((indexStart = content.find(wxT("<option value=\""), indexEnd)) > -1)
        {
            indexStart += 15;
            indexEnd = content.find(wxT("\""), indexStart); // "\">"
            pageLinks.Add(baseURL + content.Mid(indexStart, indexEnd-indexStart));

            //wxYield();
        }
    }

    return pageLinks;
}

wxString MangareaderNet::GetImageLink(wxString PageLink)
{
    CurlRequest cr;
    cr.SetUrl(PageLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    //wxString content = GetHtmlContent(PageLink);

    // Example Entry: <img id="img" width="800" height="1214" src="http://i30.mangareader.net/manga/chapter/image.jpg" alt="label" name="img" />
    int indexStart = content.find(wxT("<img id=\"img\"")) + 13;
    indexStart = content.find(wxT("src=\""), indexStart) + 5;
    int indexEnd = content.find(wxT("\""), indexStart);

    if(indexStart > 4 && indexEnd >= -1)
    {
        return content.Mid(indexStart, indexEnd-indexStart);
    }
    else
    {
        return wxEmptyString;
    }
}
