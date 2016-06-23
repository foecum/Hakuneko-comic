#include "Fakku.h"

//std::cout << wxString.mb_str() << std::endl;

Fakku::Fakku()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("Fakku");
    baseURL = wxT("https://www.fakku.net");
    referrerURL = wxT("https://www.fakku.net");
    mangaListFile.Assign(GetConfigurationPath(), wxT("fakku"), wxT("list"));
    LoadLocalMangaList();
}

Fakku::~Fakku()
{
}

void Fakku::UpdateMangaList()
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
    cr.SetUrl(baseURL + wxT("/series"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);

    // only update local list, if connection successful...
    if(cr.ExecuteRequest() && !content.IsEmpty())
    {
        int indexStart = content.find(wxT("<div id=\"content\" class=\"attribute-list\">")) + 41;
        int indexEnd = content.rfind(wxT("<div class=\"footer-tail\">"));

        if(indexStart > 40 && indexEnd > indexStart)
        {
            content = content.Mid(indexStart, indexEnd-indexStart);
            indexEnd = 0;

            // Example Entry: <li><a href="http://mangafox.me/manga/name/" rel="8894" class="series_preview manga_open">Label</a></li>
            while((indexStart = content.find(wxT("<a class=\"attribute-row\" href=\""), indexEnd)) > -1)
            {
                indexStart += 31;
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

wxArrayMCEntry Fakku::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString chTitle;
    wxString chLink;

	unsigned short curr_page_nb = 0;
	unsigned short max_page_nb = 1;
	
	int indexStart;
	int indexEnd;

	do{

		CurlRequest cr;
		cr.SetUrl(MangaEntry->Link.Append(wxString::Format(wxT("/page/%i"), curr_page_nb+1)));
		cr.SetCompression(wxT("gzip"));
		wxString content;
		wxStringOutputStream sos(&content);
		cr.SetOutputStream(&sos);
		cr.ExecuteRequest();
		
		//Get the page number
		{
			long tmplong;
		
			indexStart = content.find(wxT("Page <b>")) + 8;
			indexEnd = content.find(wxT("</b>"), indexStart);
			wxString pgstr = content.Mid(indexStart, indexEnd-indexStart);
			
			if(pgstr.IsNumber()){
				curr_page_nb = -1;
				if(pgstr.ToLong(&tmplong)){
					curr_page_nb = tmplong;
				}
			}
			
			indexStart = content.find(wxT("<b>"), indexEnd) + 3;
			indexEnd = content.find(wxT("</b>"), indexStart);
			pgstr = content.Mid(indexStart, indexEnd-indexStart);
			
			if(pgstr.IsNumber()){
				max_page_nb = 0;
				if(pgstr.ToLong(&tmplong)){
					max_page_nb = tmplong;
				}
			}
		}

		indexStart = content.find(wxT("<div class=\"table row\">"));
		indexEnd = content.find(wxT("<div id=\"pagination\" class=\"responsive-menu hidden-mobile\">"), indexStart);

		if(indexStart > -1 && indexEnd > -1)
		{
		    content = content.Mid(indexStart, indexEnd-indexStart);
		    indexEnd = 0;

		    //<dt>Volume 1</dt>
		    //<a href="/chapters/akuma_no_riddle_ch00" class="name">Chapter 0: Prologue</a>
		    while((indexStart = content.find(wxT("<div class=\"content-row doujinshi row\">"), indexEnd)) > -1)
		    {   
		    	indexEnd = indexStart;
		        indexStart = content.find(wxT("<a href=\""), indexEnd);

		        indexStart += 9;
		        indexEnd = content.find(wxT("\""), indexStart);
		        chLink = content.Mid(indexStart, indexEnd-indexStart);
		        chLink.Prepend(baseURL);
		        chLink.Append(wxT("/read"));

		        indexStart = content.find(wxT("title=\""), indexEnd) + 7;
		        indexEnd = content.find(wxT("\""), indexStart);
		        chTitle = content.Mid(indexStart, indexEnd-indexStart);

		        chapterList.Add(new MCEntry(HtmlUnescapeString(chTitle), chLink));
		    }
		}
    
    }while(curr_page_nb < max_page_nb);

    return chapterList;
}

wxArrayString Fakku::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    CurlRequest cr;
    cr.SetUrl(ChapterLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();
    
    wxString pgLink = wxT("");
    
    //std::cout << ChapterLink.mb_str() << std::endl;

    int indexStart = content.find(wxT("window.params.thumbs = [\""));
    int indexEnd = content.find(wxT("];"), indexStart);

    if(indexStart > -1 && indexEnd > -1){
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = indexStart = 0;
        //{"image":"/system/releases/000/008/837/Akuma_001_001.png","name":"001"}
        while((indexStart = content.find(wxT("\""), indexEnd)) > -1)
        {
            indexStart += 1;
            indexEnd = content.find(wxT("\""), indexStart);
            
            pgLink = content.Mid(indexStart, indexEnd-indexStart);
            indexEnd += 1;
            
            //content = content.Mid(indexEnd);
            
            pgLink.Replace(wxT("\\"), wxT(""));
            pgLink.Replace(wxT(".thumb"), wxT(""));
            pgLink.Replace(wxT("/thumbs/"), wxT("/images/"));

            pageLinks.Add(wxT("https:") + pgLink);
            
            //std::cout << wxString(wxT("https:") + pgLink).mb_str() << std::endl;
        }
    }

    return pageLinks;
}

wxString Fakku::GetImageLink(wxString PageLink)
{
	//std::cout << PageLink.mb_str() << std::endl;
    return PageLink;
}
