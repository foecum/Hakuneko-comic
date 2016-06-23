#include "MangaheadCom.h"

MangaheadCom::MangaheadCom()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("MangaHead");
    baseURL = wxT("http://www.mangahead.com");
    referrerURL = wxT("http://www.mangahead.com");
    mangaListFile.Assign(GetConfigurationPath(), wxT("mangahead"), wxT("list"));
    LoadLocalMangaList();
}

MangaheadCom::~MangaheadCom()
{
    //
}

void MangaheadCom::UpdateMangaList()
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
    cr.SetCompression(wxT("gzip"));
    
    for(wxUint32 i=0; i<=1; i++)
    {
        if(i==0)
        {
            cr.SetUrl(baseURL + wxT("/Manga-English-Scan"));
        }
        else
        {
            cr.SetUrl(baseURL + wxT("/Manga-Raw-Scan"));
        }
        wxString content;
        wxStringOutputStream sos(&content);
        cr.SetOutputStream(&sos);

        // only update local list, if connection successful...
        if(cr.ExecuteRequest() && !content.IsEmpty())
        {
            int indexStart = content.find(wxT("<h1>Manga Series</h1><table id=\"updates\">")) + 40;
            int indexEnd = content.rfind(wxT("<h1>Latest Updates</h1><table id=\"updates\">"));

            if(indexStart > 39 && indexEnd > indexStart)
            {
                content = content.Mid(indexStart, indexEnd-indexStart);
                indexEnd = 0;

                // Example Entry: <a href=/Manga-Raw-Scan/Aiki title="Read Aiki Online">Aiki</a>
                while((indexStart = content.find(wxT("<a href="), indexEnd)) > -1)
                {
                    indexStart += 8;
                    indexEnd = content.find(wxT(" "), indexStart);
                    mangaLink = baseURL + content.Mid(indexStart, indexEnd-indexStart);

                    indexStart = content.find(wxT(">"), indexEnd);
                    indexStart += 1;
                    indexEnd = content.find(wxT("</"), indexStart);
                    mangaLabel = content.Mid(indexStart, indexEnd-indexStart);

                    if(!mangaLabel.IsEmpty())
                    {
                        if (i==0)
                        {
                            f.AddLine(HtmlUnescapeString(mangaLabel) + wxT("\t") + mangaLink);
                        }
                        else
                        {
                            f.AddLine(HtmlUnescapeString(mangaLabel) + wxT(" [raw]\t") + mangaLink);
                        }
                    }
                }
            }
        }
        sos.Close();
    }
    f.Write();
    f.Close();
    LoadLocalMangaList();
}

wxArrayMCEntry MangaheadCom::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString volumePrefix;
    wxString chTitle;
    wxString chLink;

    CurlRequest cr;
    cr.SetUrl(MangaEntry->Link);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("<table id=\"home_scan\">")) + 22;
    int indexEnd = content.find(wxT("<div id=\"bottom_ads\">"), indexStart);

    if(indexStart > 21 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <a href="/Manga-Raw-Scan/One-Piece/One-Piece-740-Raw-Scan">One Piece 740 Raw Scan</a>
        while((indexStart = content.find(wxT("<a href=\""), indexEnd)) > -1)
        {
            indexStart += 9;
            indexEnd = content.find(wxT("\""), indexStart);
            chLink = baseURL + content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 2;
            indexEnd = content.find(wxT("</"), indexStart);
            chTitle = content.Mid(indexStart, indexEnd-indexStart);

            if (!chTitle.IsEmpty())
            {
                chapterList.Add(new MCEntry(HtmlUnescapeString(chTitle), chLink));
            }
        }
    }

    return chapterList;
}

wxArrayString MangaheadCom::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    CurlRequest cr;
    cr.SetUrl(ChapterLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("class=\"mangahead_thumbnail_cell\"")) + 32;
    int indexEnd = content.find(wxT("</table>"), indexStart);

    if(indexStart > 31 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <a ref="nofollow" name="01.jpg" href="/index.php/Manga-Raw-Scan/One-Piece/One-Piece-754-Raw-Scan/01.jpg?action=big&amp;size=original&amp;fromthumbnail=true">
        while((indexStart = content.find(wxT("href=\""), indexEnd)) > -1)
        {
            indexStart += 6;
            indexEnd = content.find(wxT("\""), indexStart);
            pageLinks.Add(baseURL + content.Mid(indexStart, indexEnd-indexStart));
        }
    }

    return pageLinks;
}

wxString MangaheadCom::GetImageLink(wxString PageLink)
{
    CurlRequest cr;
    cr.SetUrl(PageLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    // Example Entry: <img id="img" width="800" height="1214" src="http://i30.mangareader.net/manga/chapter/image.jpg" alt="label" name="img" />
    int indexStart = content.find(wxT("<img id= \"mangahead_image\"")) + 26;
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
