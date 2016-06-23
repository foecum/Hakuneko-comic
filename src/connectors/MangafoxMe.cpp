#include "MangafoxMe.h"

MangafoxMe::MangafoxMe()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("MangaFox");
    baseURL = wxT("http://mangafox.me");
    referrerURL = wxT("http://mangafox.me");
    mangaListFile.Assign(GetConfigurationPath(), wxT("mangafox"), wxT("list"));
    LoadLocalMangaList();
}

MangafoxMe::~MangafoxMe()
{
    //
}

void MangafoxMe::UpdateMangaList()
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
    cr.SetUrl(baseURL + wxT("/manga"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);

    // only update local list, if connection successful...
    if(cr.ExecuteRequest() && !content.IsEmpty())
    {
        int indexStart = content.find(wxT("<div class=\"manga_list\">")) + 24;
        int indexEnd = content.rfind(wxT("<div id=\"footer\">"));

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

wxArrayMCEntry MangafoxMe::GetChapterList(MCEntry* MangaEntry)
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

    //int indexStart = content.find(wxT("<div id=\"chapters\" >")) + 20;
    int indexStart = content.find(wxT("</h2><hr/>")) + 10;
    int indexEnd = content.find(wxT("<div id=\"discussion\""), indexStart);
    int volumeIndexCurrent = 0;
    int volumeIndexNext = 0;

    if(indexStart > 9 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;
        volumeIndexNext = content.find(wxT("<h3 class=\"volume\">"), volumeIndexNext);

        // Example Volume: <h3 class="volume">Volume 02 <span>Chapter 5 - 8</h3>
        // Example Entry: <a href="http://mangafox.me/manga/manga/v02/c008/1.html" title="thx" class="tips">Manga 8</a>         <span class="title nowrap">Label</span>
        while((indexStart = content.find(wxT("<a href=\""), indexEnd)) > -1)
        {
            // automatically true on first iteration
            if(volumeIndexNext > -1 && volumeIndexNext < indexStart)
            {
                // get the volume name of current volume
                volumeIndexCurrent = volumeIndexNext + 19;
                volumeIndexNext = content.find(wxT("<"), volumeIndexCurrent);
                volumePrefix = content.Mid(volumeIndexCurrent, volumeIndexNext - volumeIndexCurrent);
                // goto next volume
                volumeIndexNext = content.find(wxT("<h3 class=\"volume\">"), volumeIndexNext);
            }

            indexStart += 9;
            indexEnd = content.find(wxT("\""), indexStart); // "\" title="
            chLink = content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 2;
            indexEnd = content.find(wxT("<"), indexStart); // "</a>"
            chNumber = content.Mid(indexStart, indexEnd-indexStart).AfterLast(' ');

            FormatChapterNumberStyle(&chNumber);

            // optional, some chapters don't have title <span>...
            indexStart = content.find(wxT(">"), indexEnd+5) + 1; // ">"
            if(content.Mid(indexStart-5, 5).IsSameAs(wxT("</h3>")))
            {
                chTitle = wxEmptyString;
            }
            else  // <span class="title nowrap">
            {
                indexEnd = content.find(wxT("<"), indexStart); // "</span>"
                chTitle = content.Mid(indexStart, indexEnd-indexStart);
            }

            // NOTE: mangafox sometimes uses relative chapter numbering, where the chapter numbers are not unique (i.e. vol1-ch1, vol2-ch1)
            // -> add volume prefix

            // in case chTitle is NULL or whitespaced, whitespaces will be removed bei MCEntry::SetSafeLabel()
            chapterList.Add(new MCEntry(HtmlUnescapeString(wxT("[") + volumePrefix + wxT("] - ") + chNumber + wxT(" - ") + chTitle), chLink));

            //wxYield();
        }
    }

    return chapterList;
}

wxArrayString MangafoxMe::GetPageLinks(wxString ChapterLink)
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

    int indexStart = content.find(wxT("<select onchange=\"change_page(this)\" class=\"m\">")) + 48;
    // ignore last option (comments -> value="0")
    int indexEnd = content.find(wxT("<option value=\"0\""), indexStart);

    if(indexStart > 47 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <option value="1" selected="selected">1</option>
        while((indexStart = content.find(wxT("<option value=\""), indexEnd)) > -1)
        {
            indexStart += 15;
            indexEnd = content.find(wxT("\""), indexStart); // "\""
            pageLinks.Add(ChapterLink.BeforeLast('/') + wxT("/") + content.Mid(indexStart, indexEnd-indexStart) + wxT(".html"));

            //wxYield();
        }
    }

    return pageLinks;
}

wxString MangafoxMe::GetImageLink(wxString PageLink)
{
    CurlRequest cr;
    cr.SetUrl(PageLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    //wxString content = GetHtmlContent(PageLink);

    // Example Entry: <img src="http://c.mfcdn.net/store/manga/10235/01-001.0/compressed/pimg001.jpg" onerror="this.src='http://l.mfcdn.net/store/manga/10235/01-001.0/compressed/pimg001.jpg'" width="728" id="image" alt="Tenshin Ranman: Lucky or Unlucky!? 1: Unlucky Nature &amp;amp; Kiss at MangaFox.me"/>
    // use the onerror-server instead the src-server (to save bandwith for web-browsing users which will read from src-server)
    int indexStart = content.find(wxT("this.src='")) + 10;
    int indexEnd = content.find(wxT("'"), indexStart);

    if(indexStart > 9 && indexEnd >= -1)
    {
        return content.Mid(indexStart, indexEnd-indexStart);
    }
    else
    {
        return wxEmptyString;
    }
}
