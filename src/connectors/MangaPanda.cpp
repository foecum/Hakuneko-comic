#include "MangaPanda.h"

MangaPanda::MangaPanda()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("MangaPanda");
    baseURL = wxT("http://www.mangapanda.com");
    referrerURL = wxT("http://www.mangapanda.com");
    mangaListFile.Assign(GetConfigurationPath(), wxT("mangapanda"), wxT("list"));
    LoadLocalMangaList();
}

MangaPanda::~MangaPanda()
{
    //
}

void MangaPanda::UpdateMangaList()
{
    wxTextFile f;

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

            // Example Entry: <li><a href="http://mangafox.me/manga/name/" rel="8894" class="series_preview manga_open">Label</a></li>
            while((indexStart = content.find(wxT("<li><a href=\""), indexEnd)) > -1)
            {
                indexStart += 13;
                indexEnd = content.find(wxT("\""), indexStart); // "\""
                mangaLink = content.Mid(indexStart, indexEnd-indexStart);
                mangaLink.Prepend(baseURL);

                indexStart = content.find(wxT(">"), indexEnd) + 1; // "\">"
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

wxArrayMCEntry MangaPanda::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

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

    int indexStart = content.find(wxT("<div id=\"chapterlist\">")) + 22;
    int indexEnd = content.find(wxT("<div id=\"adfooter\">"), indexStart);

    if(indexStart > 22 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Volume: <h3 class="volume">Volume 02 <span>Chapter 5 - 8</h3>
        // Example Entry: <a href="http://mangafox.me/manga/manga/v02/c008/1.html" title="thx" class="tips">Manga 8</a>         <span class="title nowrap">Label</span>
        while((indexStart = content.find(wxT("<a href=\""), indexEnd)) > -1)
        {
            indexStart += 9;
            indexEnd = content.find(wxT("\""), indexStart); // "\" title="
            chLink = content.Mid(indexStart, indexEnd-indexStart);
            chLink.Prepend(baseURL);

            indexStart = indexEnd + 2;
            indexEnd = content.find(wxT("<"), indexStart); // "</a>"
            chNumber = content.Mid(indexStart, indexEnd-indexStart).AfterLast(' ');

            //FormatChapterNumberStyle(&chNumber);

            // optional, some chapters don't have title <span>...
            indexStart = content.find(wxT(": "), indexEnd+4) + 2; // ">"
            indexEnd = content.find(wxT("<"), indexStart); // "</span>"
            chTitle = content.Mid(indexStart, indexEnd-indexStart);

            // NOTE: mangafox sometimes uses relative chapter numbering, where the chapter numbers are not unique (i.e. vol1-ch1, vol2-ch1)
            // -> add volume prefix

            // in case chTitle is NULL or whitespaced, whitespaces will be removed bei MCEntry::SetSafeLabel()
            chapterList.Add(new MCEntry(HtmlUnescapeString(chNumber + wxT(" - ") + chTitle), chLink));

            //wxYield();
        }
    }

    return chapterList;
}

wxArrayString MangaPanda::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    CurlRequest cr;
    cr.SetUrl(ChapterLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    wxString chapLink;

    //wxString content = GetHtmlContent(ChapterLink);

    int indexStart = content.find(wxT("<select id=\"pageMenu\" name=\"pageMenu\">")) + 38;
    // ignore last option (comments -> value="0")
    int indexEnd = content.find(wxT("</select>"), indexStart);

    if(indexStart >= 38 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <option value="1" selected="selected">1</option>
        while((indexStart = content.find(wxT("<option value=\""), indexEnd)) > -1)
        {
            indexStart += 15;
            indexEnd = content.find(wxT("\""), indexStart); // "\""
            chapLink = content.Mid(indexStart, indexEnd-indexStart);
            chapLink.Prepend(baseURL);

            pageLinks.Add(chapLink);

            //wxYield();
        }
    }

    return pageLinks;
}

wxString MangaPanda::GetImageLink(wxString PageLink)
{
    CurlRequest cr;
    cr.SetUrl(PageLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    //wxString content = GetHtmlContent(PageLink);

    //<img id="img" width="800" height="1162" src="http://i997.mangapanda.com/bleach/657/bleach-6748189.jpg" alt="Bleach 657 - Page 1" name="img" />
    int indexStart = content.find(wxT("<img id=\"img\"")) + 13;
    int indexEnd = content.find(wxT("/>"), indexStart);

    content = content.Mid(indexStart, indexEnd-indexStart);

    indexStart = content.find(wxT("src=\"")) + 5;
    indexEnd = content.find(wxT("\""), indexStart);

    return content.Mid(indexStart, indexEnd-indexStart);
}
