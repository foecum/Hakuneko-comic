#include "Mangago.h"

Mangago::Mangago()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("Mangago");
    baseURL = wxT("http://mangago.me");
    referrerURL = wxT("http://mangago.me");
    mangaListFile.Assign(GetConfigurationPath(), wxT("mangago"), wxT("list"));
    LoadLocalMangaList();
}

Mangago::~Mangago()
{
    //
}

void Mangago::UpdateMangaList()
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
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);

    unsigned short page_max = getChapterListPageNumber();

    //std::cout << std::endl << page_max << std::endl;

    // only update local list, if connection successful...
    for(unsigned short i = 1; i <= page_max; i++){
        //std::cout << std::endl << i << std::endl;

        cr.SetUrl(baseURL + wxT("/list/directory/all/") + wxString::Format(wxT("%i"), i));

        if(cr.ExecuteRequest() && !content.IsEmpty())
        {
            int indexStart = content.find(wxT("<ul class=\"pic_list\">")) + 23;
            int indexEnd = content.find(wxT("</ul>"), indexStart);

            if(indexStart > 22 && indexEnd > indexStart)
            {
                content = content.Mid(indexStart, indexEnd-indexStart);
                indexEnd = 0;

                //<a href="http://www.mangago.me/read-manga/hitogatana/" target="_self" class="thm-effect" title="-hitogatana-">
                while((indexStart = content.find(wxT("<a href=\""), indexEnd)) > -1)
                {
                    indexStart += 9;
                    indexEnd = content.find(wxT("\""), indexStart); // "\""
                    mangaLink = content.Mid(indexStart, indexEnd-indexStart);

                    indexStart = content.find(wxT("title=\""), indexEnd) + 7; // "\">"
                    indexEnd = content.find(wxT("\""), indexStart); // "</a>"
                    mangaLabel = content.Mid(indexStart, indexEnd-indexStart);

                    if(!mangaLabel.IsEmpty())
                    {
                        f.AddLine(HtmlUnescapeString(mangaLabel) + wxT("\t") + mangaLink);
                        //std::cout << mangaLabel.mb_str() << '\t' << mangaLink.mb_str() << std::endl;
                    }

                }
            }
        }
    }

    sos.Close();
    f.Write();
    f.Close();
    LoadLocalMangaList();
}

wxArrayMCEntry Mangago::GetChapterList(MCEntry* MangaEntry)
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

    int indexStart = content.find(wxT("<table class=\"listing\" id=\"chapter_table\">")) + 41;
    int indexEnd = content.find(wxT("</table>"), indexStart);

    if(indexStart > 40 && indexEnd >= -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;

        //<a target="_self" class="chico" href="http://www.mangago.me/read-manga/hitogatana/mf/v06/c029/">
        //                              -hitogatana- <b>Vol.6 Ch.29</b>
        while((indexStart = content.find(wxT("<a target=\"_self\" class=\"chico\" href=\""), indexEnd)) > -1)
        {
            indexStart += 38;
            indexEnd = content.find(wxT("\""), indexStart); // "\" title="
            chLink = content.Mid(indexStart, indexEnd-indexStart);

            indexStart = content.find(wxT("<b>"), indexEnd) + 3;
            indexEnd = content.find(wxT("<"), indexStart); // "</a>"
            chNumber = content.Mid(indexStart, indexEnd-indexStart);

            chTitle = wxT("");

            int index_title_start = content.find(wxT(":"), indexEnd) + 1;
            int index_title_end = content.find(wxT("</a>"), indexEnd);

            if((index_title_start > indexEnd) && (index_title_start < index_title_end)){
                chTitle = content.Mid(index_title_start, index_title_end - index_title_start);
                chTitle = chTitle.Trim(true).Trim(false);
            }


            if(!chTitle.IsEmpty()){
                chapterList.Add(new MCEntry(HtmlUnescapeString(chNumber + wxT(" - ") + chTitle), chLink));
            }else{
                chapterList.Add(new MCEntry(HtmlUnescapeString(chNumber), chLink));
            }
        }
    }

    return chapterList;
}

wxArrayString Mangago::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    CurlRequest cr;
    cr.SetUrl(ChapterLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("var imgsrcs = new Array(")) + 24;
    int indexEnd = content.find(wxT(");"), indexStart);
    if(indexStart >= 24 && indexEnd > -1){
        content = content.Mid(indexStart, indexEnd-indexStart);

        indexEnd = 0;
        while((indexStart = content.find(wxT("'"), indexEnd)) > -1)
        {
            indexStart += 1;
            indexEnd = content.find(wxT("'"), indexStart);
            if(indexEnd < 0){
                break;
            }
            pageLinks.Add(content.Mid(indexStart, indexEnd-indexStart));
            indexEnd += 1;
        }
    }

    return pageLinks;
}

wxString Mangago::GetImageLink(wxString PageLink)
{
    return PageLink;
}

unsigned short Mangago::getChapterListPageNumber(void){
    CurlRequest cr;
    cr.SetUrl(baseURL + wxT("/list/directory/all/"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    unsigned short page_nb = 0;
    long tmp_long;

    wxString nb_pg_str(wxT(""));

    int indexStart = content.find(wxT("<select onchange=\"window.location=this.value\">")) + 45;
    int indexEnd = content.find(wxT("</select>"), indexStart);

    content = content.Mid(indexStart, indexEnd-indexStart);

    //std::cout << content.mb_str() << std::endl;

    indexStart = content.rfind(wxT("<option  value='http://www.mangago.me/list/directory/all/")) + 57;
    indexEnd = content.find(wxT("/"), indexStart);

    nb_pg_str = content.Mid(indexStart, indexEnd-indexStart);

    //std::cout << nb_pg_str.mb_str() << std::endl;

    if(nb_pg_str.IsNumber()){
        if(nb_pg_str.ToLong(&tmp_long)){
            page_nb = tmp_long;
        }
    }

    return page_nb;
}

