#include "KissMangaCom.h"

KissMangaCom::KissMangaCom()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("KissManga");
    baseURL = wxT("http://kissmanga.com");
    referrerURL = wxT("http://kissmanga.com");
    mangaListFile.Assign(GetConfigurationPath(), wxT("kissmanga"), wxT("list"));
    LoadLocalMangaList();
    agent = wxT("Mozilla");
    CFCookie(3);
}

KissMangaCom::~KissMangaCom()
{
    //
}

void KissMangaCom::UpdateMangaList()
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
    cr.SetAgent(agent);
    cr.SetCookies(cookies);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);

    wxUint32 pageMax = GetMangaListPageCount() + 1;

    for(wxUint32 i=1; i<pageMax; i++)
    {
        cr.SetUrl(wxString::Format(baseURL + wxT("/MangaList?page=%u"), i));
        content = wxEmptyString;

        if(cr.ExecuteRequest() && !content.IsEmpty())
        {
            int indexStart = content.find(wxT("class=\"listing\"")) + 15;
            int indexEnd = indexStart;//content.rfind(wxT("</table>"));

            if(indexStart > 14 && indexEnd > 14)
            {
                // Example Entry: <a class="bigChar" href="/Manga/–-and-–">& – and –</a>
                while((indexStart = content.find(wxT("class=\"bigChar\""), indexEnd)) > -1)
                {
                    indexStart += 15;
                    indexStart = content.find(wxT("=\""), indexStart) + 2; // "<a href='"
                    indexEnd = content.find(wxT("\""), indexStart); // "\">"
                    mangaLink = baseURL + content.Mid(indexStart, indexEnd-indexStart);

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

wxArrayMCEntry KissMangaCom::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString volumePrefix;
    wxString chNumber;
    wxString chEntry;
    wxString chTitle;
    wxString chLink;

    CurlRequest cr;
    cr.SetUrl(MangaEntry->Link);
    cr.SetAgent(agent);
    cr.SetCookies(cookies);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("class=\"listing\"")) + 15;
    int indexEnd = content.find(wxT("</table>"), indexStart);

    if(indexStart > 14 && indexEnd > -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        //Example Entry:
        /*
        <a href="/Manga/–-and-–/Vol-003-Ch-014-Read-Online?id=209953" title="Read &amp; – and – Vol.003 Ch.014 Read Online online">& – and – Vol.003 Ch.014 Read Online</a>
        <a href="/Manga/One-Piece/823---A-World-Abuzz?id=270460" title="Read One Piece 823 - A World Abuzz online">One Piece 823 - A World Abuzz</a>
        */
        while((indexStart = content.find(wxT("<a href=\""), indexEnd)) > -1)
        {
            indexStart += 9;
            indexEnd = content.find(wxT("\""), indexStart); // "\">"
            chLink = baseURL + content.Mid(indexStart, indexEnd-indexStart);

            indexStart = indexEnd;
            indexStart = content.find(wxT(">"), indexStart) + 1; // ">"
            indexEnd = content.find(wxT("<"), indexStart); // "<"
            chEntry = content.Mid(indexStart, indexEnd-indexStart);

            // parse chEntry for: volumePrefix, chNumber, chTitle is impossible,
            // because KissManga naming is inconsistent
            // volumePrefix = ...
            // chNumber = ...
            // chTitle = ...
            chapterList.Add(new MCEntry(HtmlUnescapeString(chEntry), chLink));
        }
    }

    return chapterList;
}

wxArrayString KissMangaCom::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    CurlRequest cr;
    cr.SetUrl(ChapterLink);
    cr.SetAgent(agent);
    cr.SetCookies(cookies);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("var lstImages")) + 13;
    int indexEnd = content.find(wxT("var lstImagesLoaded"), indexStart);

    if(indexStart > 12 && indexEnd > -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        // Example Entry: lstImages.push("http://2.bp.blogspot.com/-m2wUve_VBR8/VsFJRUDLJiI/AAAAAAAFMGc/7HfBG-sFqVA/s16000-Ic42/000.png");
        while((indexStart = content.find(wxT("lstImages.push(\""), indexEnd)) > -1)
        {
            indexStart += 16;
            indexEnd = content.find(wxT("\""), indexStart); // "\""
            pageLinks.Add(content.Mid(indexStart, indexEnd-indexStart));

            //wxYield();
        }
    }

    return pageLinks;
}

wxString KissMangaCom::GetImageLink(wxString PageLink)
{
    return PageLink;
}

wxUint32 KissMangaCom::GetMangaListPageCount()
{
    unsigned long result = 500;

    CurlRequest cr;
    cr.SetUrl(baseURL + wxT("/MangaList"));
    cr.SetAgent(agent);
    cr.SetCookies(cookies);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("class=\"pager\"")) + 13;
    indexStart = content.find(wxT("</ul>"), indexStart);
    indexStart = content.rfind(wxT("page="), indexStart) + 6;
    int indexEnd = content.find(wxT("\""), indexStart);
    if(indexStart > 5 && indexEnd > -1) {
        content = content.Mid(indexStart, indexEnd-indexStart);
        if(content.IsNumber() && content.ToULong(&result)){
            return (wxUint32)result;
        }
    }

    Logger::Log(wxT("ERROR +++ KissMangaCom::GetMangaListPageCount() -> Cannot find last page of manga list"));
    return (wxUint32)result;
}

void KissMangaCom::Activate(wxString Link)
{
    wxString tmp = Link + wxT("0xb"); // TODO: get offset from online source in case it changes periodically
    wxString hash = wxEmptyString;
    unsigned char sum[32];
    SHA256((const unsigned char*)tmp.ToAscii().data(), tmp.Len(), sum); // Scripts/google.js (= CryptoJS)
    for(int i = 0; i < 32; i++)
    {
        hash.Append(wxString::Format(wxT("%02x"), sum[i]));
    }
    CurlRequest cr;
    cr.SetUrl(baseURL + wxT("/0xba93?key=") + hash);
    cr.SetReferer(Link);
    cr.SetPostData(wxT("key=") + hash);
    wxString response;
    wxStringOutputStream sos(&response);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();
}

wxString KissMangaCom::JSRiddle(wxString Script)
{
    CurlRequest cr(false);
    cr.SetUrl(wxT("http://jsapp.us/ajax"));
    cr.SetPostData(wxT("{\"actions\":[{\"action\":\"test\",\"code\":\"") + Script + wxT("\"}]}"));
    wxString response;
    wxStringOutputStream sos(&response);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    // open url from response
    cr.SetUrl(response.BeforeLast('"').AfterLast('"'));
    response = wxEmptyString;
    cr.ExecuteRequest();

    return response;
}

void KissMangaCom::CFCookie(wxInt32 RetryCount)
{
    if(RetryCount < 0) {
        //wxMessageBox::Show(wxT("Initialization of KissManga failed!"));
        return;
    }

    int indexStart;
    int indexEnd;

    CurlRequest cr(false);
    cr.SetUrl(baseURL);
    cr.SetHeader(true);
    cr.SetAgent(agent);
    wxString response;
    wxStringOutputStream sos(&response);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    // get initial cookies
    indexStart = response.find(wxT("__cfduid="));
    indexEnd = response.find(wxT(";"), indexStart);
    cookies = response.Mid(indexStart, indexEnd-indexStart);

    // get jschl_answer equation script
    indexStart = response.find(wxT("setTimeout(function(){"), indexEnd) + 22;
    indexStart = response.find(wxT("={"), indexStart);
    indexStart = response.rfind(wxT(","), indexStart) + 1;
    indexEnd = response.find(wxT(".submit();"), indexStart) + 10;
    wxString temp = response.Mid(indexStart, indexEnd-indexStart).Trim(false);
    wxString key = temp.BeforeFirst('=');
    wxString jschl_script = wxT("require('http').createServer(function(req,res){");
    while(temp.Contains(wxT(";")))
    {
        if(temp.StartsWith(key) || temp.StartsWith(wxT("a.value"))) {
            jschl_script += temp.BeforeFirst(';').Trim(false) + wxT(";");
        }
        temp = temp.AfterFirst(';').Trim(false);
    }
    jschl_script.Replace(wxT("a.value = "), wxT("res.writeHead(200);res.end(("));
    jschl_script.Replace(wxT("t.length"), wxString::Format(wxT("%lu)+'');}).listen()"), (unsigned long)baseURL.AfterLast('/').Len()));
    jschl_script.Replace(wxT("\""), wxT("'"));
    jschl_script.Replace(wxT(" "), wxT(""));
    wxString jschl_answer = JSRiddle(jschl_script);

    // get chk_jschl url
    indexStart = response.find(wxT("challenge-form"), indexEnd) + 14;
    indexStart = response.find(wxT("action=\""), indexStart) + 8;
    indexEnd = response.find(wxT("\""), indexStart);
    wxString jschl_chk = baseURL + response.Mid(indexStart, indexEnd-indexStart);

    // get jschl_vc
    indexStart = response.find(wxT("name=\"jschl_vc\""), indexEnd) + 23;
    indexEnd = response.find(wxT("\""), indexStart);
    wxString jschl_vc = response.Mid(indexStart, indexEnd-indexStart);

    // get pass
    indexStart = response.find(wxT("name=\"pass\"")) + 19;
    indexEnd = response.find(wxT("\""), indexStart);
    wxString pass = response.Mid(indexStart, indexEnd-indexStart);

    // wait a bit before sending solution of jsriddle (bypass cloudflare's "to fast solved" protection)
    wxSleep(4);
    cr.SetUrl(jschl_chk + wxT("?jschl_vc=") + jschl_vc + wxT("&pass=") + pass + wxT("&jschl_answer=") + jschl_answer);
    cr.SetCookies(cookies);
    response = wxEmptyString;
    cr.ExecuteRequest();

    // get cf_clearance cookie
    // FIXME: it happens regulary that the cookie is not returned by the server,
    // the reason is unknown since the riddle always seems to be solved correctly
    indexStart = response.find(wxT("cf_clearance="));
    indexEnd = response.find(wxT(";"), indexStart);
    cookies = response.Mid(indexStart, indexEnd-indexStart) + wxT(";vns_readType1=1");
    if(cookies.StartsWith(wxT("cf_clearance"))) {
        Logger::Log(wxT("NOTE +++ KissMangaCom::CFCookie() -> cookie = ") + cookies);
    } else {
        Logger::Log(wxT("ERROR +++ KissMangaCom::CFCookie() -> Missing cf_clearance cookie"));
        // retry
        CFCookie(RetryCount-1);
    }
}
