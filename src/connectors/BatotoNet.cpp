#include "BatotoNet.h"

BatotoNet::BatotoNet(wxString User, wxString Pass)
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("Batoto");
    baseURL = wxT("http://bato.to");
    referrerURL = wxT("http://bato.to");
    mangaListFile.Assign(GetConfigurationPath(), wxT("batoto"), wxT("list"));
    LoadLocalMangaList();
    Login(User, Pass);
}

BatotoNet::~BatotoNet()
{
    //
}

void BatotoNet::UpdateMangaList()
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
    cr.SetCookies(cookies);
    cr.SetReferer(referrerURL);
    cr.SetCompression(wxT("gzip"));
    cr.SetOutputStreamFixUTF8(true);

    // self published content from batoto disabled
    /*
    for(wxUint32 i=0; i<500; i+=500)
    {
        cr.SetUrl(wxString::Format(baseURL + wxT("/comic/_/sp/?per_page=500&st=%u"), i));
        cr.ExecuteRequest();
    }
    */

    for(wxUint32 i=0; i<30000; i+=750)
    {
        wxString content;
        wxStringOutputStream sos(&content);
        cr.SetOutputStream(&sos);
        cr.SetUrl(wxString::Format(baseURL + wxT("/comic/_/comics/?per_page=750&st=%u"), i));

        //wxString content = GetHtmlContentF(baseURL + wxT("/comic/_/comics/?per_page=750&st=%u"), 0, 11250, 750, 31200, true);
        //content += GetHtmlContentF(baseURL + wxT("/comic/_/sp/?per_page=500&st=%u"), 0, 500, 500, 31200, true);

        // only update local list, if connection successful...
        if(cr.ExecuteRequest() && !content.IsEmpty())
        {
            int indexStart = 0;// = content.find(wxT("<div class=\"manga_list\">")) + 24;
            int indexEnd = 0;// = content.rfind(wxT("<div id=\"footer\">"));

            if(indexStart > -1 && indexEnd > -1)
            {
                //content = content.Mid(indexStart, indexEnd-indexStart);
                //indexEnd = 0;

                // Example Entry: <a href='http://www.batoto.net/comic/_/sp/xandria-r8964'>Xandria</a>
                while((indexStart = content.find(wxT("__topic"), indexEnd)) > -1)
                {
                    indexStart += 7;
                    indexStart = content.find(wxT("<a"), indexStart) + 9; // "<a href='"
                    indexEnd = content.find(wxT("'"), indexStart); // "'>"
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
    }
    f.Write();
    f.Close();
    LoadLocalMangaList();
}

wxArrayMCEntry BatotoNet::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString volumePrefix;
    wxString chNumber;
    wxString chEntry;
    wxString chLanguage;
    wxString chScangroup;
    wxString chTitle;
    wxString chLink;

    CurlRequest cr;
    cr.SetUrl(MangaEntry->Link);
    cr.SetCookies(cookies);
    cr.SetReferer(referrerURL);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.SetOutputStreamFixUTF8(true);
    cr.ExecuteRequest();

    int posStart, posEnd;
    int indexStart = content.find(wxT("ipb_table chapters_list")) + 23;
    int indexEnd = content.rfind(wxT("comments"));

    if(indexStart > 22 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry:
        /*
        <tr class="row lang_English chapter_row">
        <td style="border-top:0;"><a href="http://www.batoto.net/read/_/134827/one-piece_ch683_by_mangarule" title="Ch.683: A Woman like Ice | Sort: 683"><img src="http://www.batoto.net/book_open.png" style="vertical-align:middle;"/> Ch.683: A Woman like Ice</a></td>
        <td style="border-top:0;"><div title="English" style="display: inline-block; width:16px; height: 12px; margin: 2px 0; padding:0; background-image:url('http://www.batoto.net/images/all_flags.png'); background-position: -284px -173px; "></div></td>
        <td style="border-top:0;"><a href="http://www.batoto.net/group/_/mangarule-r149">Mangarule</a>        </td>
        */
        while((indexStart = content.find(wxT("class=\"row"), indexEnd)) > -1)
        {
            indexStart += 10;
            indexStart = content.find(wxT("_"), indexStart) + 1;
            indexEnd = content.find(wxT(" "), indexStart); // " "
            chLanguage = content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 1;
            indexStart = content.find(wxT("<a"), indexStart) + 9; // "<a href=\""
            indexEnd = content.find(wxT("\""), indexStart); // "\">"
            chLink = content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd;
            indexStart = content.find(wxT("title"), indexStart) + 7; // "title"
            indexEnd = content.find(wxT(" |"), indexStart); // " |"
            //indexEnd = content.find(wxT(" |"), indexEnd); // " |"
            chEntry = content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd + 4;
            indexStart = content.find(wxT("<a"), indexStart) + 9; // "<a href=\""
            indexStart = content.find(wxT(">"), indexStart) + 1; // "\">"
            indexEnd = content.find(wxT("<"), indexStart); // "</a>"
            chScangroup = content.Mid(indexStart, indexEnd-indexStart);

            // parse chEntry for: volumePrefix, chNumber, chTitle

            posStart = chEntry.find(wxT("Vol."));
            if(posStart > -1)
            {
                posEnd = chEntry.find(wxT(" "), posStart);
                volumePrefix = chEntry.Mid(posStart, posEnd-posStart);
            }

            posStart = chEntry.find(wxT("Ch.")) + 3;
            if(posStart > 2)
            {
                if(chEntry.EndsWith(wxT("Read Online")))
                {
                    posEnd = chEntry.rfind(wxT(" R"));

                    chTitle = wxEmptyString; // overwrite previous title
                }
                else
                {
                    // NOTE: wrong data when text before chapter number contains ': ' (i.e. ch.Text: Text 10: Chapter Title)
                    posEnd = chEntry.find(wxT(": "), posStart);

                    chTitle = chEntry.Mid(posEnd+2);
                }

                chNumber = chEntry.Mid(posStart, posEnd-posStart);

                FormatChapterNumberStyle(&chNumber);
            }

            // NOTE: batoto seems to use global chapter numbering, where the chapter numbers are unique (volumes are optional)
            // -> ignore volume prefix
            // unfortunately this is no longer true: http://bato.to/comic/_/comics/hidamari-sketch-r334
            // chapters are counted for each volumes...
            // this change will break the detection of chapters that has the old naming convention without the volume

            if(volumePrefix.IsEmpty())
            {
                chapterList.Add(new MCEntry(HtmlUnescapeString(chNumber + wxT(" - ") + chTitle + wxT(" [") + chLanguage + wxT("] by [") + chScangroup + wxT("]")), chLink));
            }
            else
            {
                chapterList.Add(new MCEntry(HtmlUnescapeString(wxT("[") + volumePrefix + wxT("] - ") + chNumber + wxT(" - ") + chTitle + wxT(" [") + chLanguage + wxT("] by [") + chScangroup + wxT("]")), chLink));
            }

            //wxYield();
        }
    }

    return chapterList;
}

wxArrayString BatotoNet::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    CurlRequest cr;
    ChapterLink.Replace(wxT("reader#"), wxT("areader?id="));
    ChapterLink += wxT("&p=1&supress_webtoon=t");
    cr.SetUrl(ChapterLink);
    cr.SetCookies(cookies);
    cr.SetReferer(referrerURL + wxT("/reader"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("<select name=\"page_select\"")) + 26;
    int indexEnd = content.find(wxT("</select>"), indexStart);

    if(indexStart > 25 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: <option value="http://www.batoto.net/read/_/132664/one-piece_ch682_by_mangarule/3" >page 3</option>
        while((indexStart = content.find(wxT("<option value=\""), indexEnd)) > -1)
        {
            indexStart += 15;
            indexEnd = content.find(wxT("\""), indexStart); // "\""
            pageLinks.Add(content.Mid(indexStart, indexEnd-indexStart));

            //wxYield();
        }
    }

    return pageLinks;
}

wxString BatotoNet::GetImageLink(wxString PageLink)
{
    CurlRequest cr;
    PageLink.Replace(wxT("reader#"), wxT("areader?id="));
    PageLink.Replace(wxT("_"), wxT("&p="));
    PageLink += wxT("&supress_webtoon=t");
    cr.SetUrl(PageLink);
    cr.SetCookies(cookies);
    cr.SetReferer(referrerURL + wxT("/reader"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    // Example Entry: <img id="comic_page" style="max-width: 100%;" src="http://img.batoto.net/comics/2012/09/23/o/read505ee533070ca/img000003.png" alt="One Piece - ch 682 Page 3 | Batoto!" onerror="this.src='http://img.batoto.net/comics/misc/Img-Error.jpg'" onload="adjust_page_width();" />
    int indexStart = content.find(wxT("<img id=\"comic_page\"")) + 20;
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

void BatotoNet::Login(wxString User, wxString Pass)
{
    int indexStart;
    int indexEnd;

    CurlRequest cr(false);
    cr.SetUrl(baseURL + wxT("/forums/index.php?app=core"));
    cr.SetHeader(true);
    //cr.SetAgent(agent);
    wxString response;
    wxStringOutputStream sos(&response);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    // get initial cookies
    indexStart = response.find(wxT("session_id="));
    indexEnd = response.find(wxT("\n"), indexStart);
    cookies = response.Mid(indexStart, indexEnd-indexStart);

    // get auth_key required for login
    indexStart = response.find(wxT("auth_key"), indexEnd);
    indexStart = response.find(wxT("value="), indexStart) + 7;
    indexEnd = response.find(wxT("'"), indexStart);
    wxString authKey = response.Mid(indexStart, indexEnd-indexStart);

    // submit login for this session (cookie)
    cr.SetUrl(wxT("https://bato.to/forums/index.php?app=core&module=global&section=login&do=process"));
    cr.SetCookies(cookies);
    cr.SetPostData(wxT("auth_key=") + authKey + wxT("&ips_username=") + User + wxT("&ips_password=") + Pass/* + wxT("&referer:") + referrerURL*/);
    response = wxEmptyString;
    cr.ExecuteRequest();
}
