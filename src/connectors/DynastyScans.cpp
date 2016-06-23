#include "DynastyScans.h"

//std::cout << wxString.mb_str() << std::endl;

DynastyScans::DynastyScans()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("DynastyScans");
    baseURL = wxT("http://dynasty-scans.com");
    referrerURL = wxT("http://dynasty-scans.com");
    mangaListFile.Assign(GetConfigurationPath(), wxT("dynastyscans"), wxT("list"));
    LoadLocalMangaList();
}

DynastyScans::~DynastyScans()
{
    //
}

//TODO Add also anthologies, doujins, issues, etc.
void DynastyScans::UpdateMangaList()
{
    wxTextFile f;

    mangaListFile.Mkdir(0755, wxPATH_MKDIR_FULL);

    // create file, or open if already exists
    if(!f.Create(mangaListFile.GetFullPath()))
    {
        f.Open(mangaListFile.GetFullPath());
        f.Clear();
    }

    wxString mangasDirectory[5];
    wxString mangasDirectoryLabel[5];
    
    mangasDirectory[0] = wxT("/series");
    mangasDirectoryLabel[0] = wxT("[SERI]");
    mangasDirectory[1] = wxT("/doujins");
    mangasDirectoryLabel[1] = wxT("[DOUJ]");
    mangasDirectory[2] = wxT("/anthologies");
    mangasDirectoryLabel[2] = wxT("[ANTH]");
    mangasDirectory[3] = wxT("/issues");
    mangasDirectoryLabel[3] = wxT("[ISSU]");
    mangasDirectory[4] = wxT("/pairings");
    mangasDirectoryLabel[4] = wxT("[PAIR]");

    wxString mangaLink;
    wxString mangaLabel;

    for(unsigned char dir=0; dir<=4; dir++){

        CurlRequest cr;
        cr.SetUrl(baseURL + mangasDirectory[dir]);//cr.SetUrl(baseURL + wxT("/series"));
        cr.SetCompression(wxT("gzip"));
        wxString content;
        wxStringOutputStream sos(&content);
        cr.SetOutputStream(&sos);

            // only update local list, if connection successful...
            if(cr.ExecuteRequest() && !content.IsEmpty())
            {
                int indexStart = content.find(wxT("<dl class='tag-list'>"));
                int indexEnd = content.rfind(wxT("</dl>"));
                int listPrefixIndexCurrent, listPrefixIndexNext;
                wxString listPrefix;
                
                if(indexStart > -1 && indexEnd > indexStart)
                {
                    content = content.Mid(indexStart, indexEnd-indexStart);
                    indexEnd = 0;
                    
	            	listPrefixIndexCurrent = -1;
	            	listPrefixIndexNext = content.find(wxT("<dt>"));
	            	listPrefix = wxT("");
		            
                    //<dd><a href="/series/akuma_no_riddle">Akuma no Riddle</a></dd>
                    while((indexStart = content.find(wxT("<dd><a href=\""), indexEnd)) > -1)
                    {
                    	if(listPrefixIndexNext > -1 && listPrefixIndexNext < indexStart)
						{
						    // get the volume name of current volume
						    listPrefixIndexCurrent = listPrefixIndexNext + 4;
						    listPrefixIndexNext = content.find(wxT("<"), listPrefixIndexCurrent);
						    listPrefix = content.Mid(listPrefixIndexCurrent, listPrefixIndexNext - listPrefixIndexCurrent);
						    // goto next volume
						    listPrefixIndexNext = content.find(wxT("<dt>"), listPrefixIndexNext);
						}
                    
                        indexStart += 13;
                        indexEnd = content.find(wxT("\""), indexStart);
                        mangaLink = content.Mid(indexStart, indexEnd-indexStart);
		                mangaLink.Prepend(baseURL);

                        indexStart = content.find(wxT(">"), indexEnd) + 1;
                        indexEnd = content.find(wxT("<"), indexStart);
                        mangaLabel = content.Mid(indexStart, indexEnd-indexStart);

                        if(!mangaLabel.IsEmpty())
                        {
                        	if(dir>=3){
                        		mangaLabel.Prepend(wxT("[")+HtmlUnescapeString(listPrefix)+wxT("]"));
                        	}
                            mangaLabel.Prepend(mangasDirectoryLabel[dir]);
                            f.AddLine(HtmlUnescapeString(mangaLabel) + wxT("\t") + mangaLink);
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

wxArrayMCEntry DynastyScans::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    wxString volumePrefix = wxT("");
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

    int indexStart = content.find(wxT("<dl class='chapter-list'>"));
    int indexEnd = content.find(wxT("</dl>"), indexStart);
    int volumeIndexCurrent = 0;
    int volumeIndexNext = 0;

    if(indexStart > -1 && indexEnd > -1)
    {
        content = content.Mid(indexStart, indexEnd-indexStart);
        indexEnd = 0;
        volumeIndexNext = content.find(wxT("<dt>"), volumeIndexNext);

        //<dt>Volume 1</dt>
        //<a href="/chapters/akuma_no_riddle_ch00" class="name">Chapter 0: Prologue</a>
        while((indexStart = content.find(wxT("<dd>"), indexEnd)) > -1)
        {
            // automatically true on first iteration
            if(volumeIndexNext > -1 && volumeIndexNext < indexStart)
            {
                // get the volume name of current volume
                volumeIndexCurrent = volumeIndexNext + 4;
                volumeIndexNext = content.find(wxT("<"), volumeIndexCurrent);
                volumePrefix = content.Mid(volumeIndexCurrent, volumeIndexNext - volumeIndexCurrent);
                // goto next volume
                volumeIndexNext = content.find(wxT("<dt>"), volumeIndexNext);
            }
            
            indexStart = content.find(wxT("<a href=\""), indexEnd);

            indexStart += 9;
            indexEnd = content.find(wxT("\""), indexStart);
            chLink = content.Mid(indexStart, indexEnd-indexStart);
            chLink.Prepend(baseURL);

            chNumber = wxT("");	//TODO In most of the case, chNumber can be guessed

            indexStart = content.find(wxT(">"), indexEnd) + 1;
            indexEnd = content.find(wxT("<"), indexStart);
            chTitle = content.Mid(indexStart, indexEnd-indexStart);

            if(volumePrefix.IsEmpty()){
                chapterList.Add(new MCEntry(HtmlUnescapeString(chTitle), chLink));
            }else{
                chapterList.Add(new MCEntry(HtmlUnescapeString(wxT("[") + volumePrefix + wxT("] - ") + chTitle), chLink));
            }

            indexEnd = content.find(wxT("</dd>"), indexStart);
        }
    }

    return chapterList;
}

wxArrayString DynastyScans::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    CurlRequest cr;
    cr.SetUrl(ChapterLink);
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    int indexStart = content.find(wxT("<div class='container' id='content'>"));
    int indexEnd = content.find(wxT("</script>"), indexStart);
    if(indexStart > -1 && indexEnd > -1){
        content = content.Mid(indexStart, indexEnd-indexStart);

        indexStart = content.find(wxT("["));
        //indexEnd = content.find(wxT("</script>"), indexStart);

        if(indexStart > -1 && indexEnd > -1){
            content = content.Mid(indexStart, indexEnd-indexStart);
            indexEnd = indexStart = 0;
            //{"image":"/system/releases/000/008/837/Akuma_001_001.png","name":"001"}
            while((indexStart = content.find(wxT("\"image\":\""), indexEnd)) > -1)
            {
                indexStart += 9;
                indexEnd = content.find(wxT("\""), indexStart);
                pageLinks.Add(baseURL + content.Mid(indexStart, indexEnd-indexStart));
		//pageLinks.Add(baseURL + HtmlUnescapeString(content.Mid(indexStart, indexEnd-indexStart)));
            }
        }
    }

    return pageLinks;
}

wxString DynastyScans::GetImageLink(wxString PageLink)
{
    return PageLink;
}
