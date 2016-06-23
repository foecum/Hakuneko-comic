#include "VirtualManga.h"

VirtualManga::VirtualManga()
{
    type = CONNECTOR_TYPE_MANGA;
    label = wxT("VirtualManga");
    baseURL = wxT("http://virtualmanga.null");
    referrerURL = wxT("http://virtualmanga.null");
    mangaListFile.Assign(GetConfigurationPath(), wxT("virtualmanga"), wxT("list"));
    LoadLocalMangaList();
}

VirtualManga::~VirtualManga()
{
    //
}

void VirtualManga::UpdateMangaList()
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

    f.AddLine(wxT("Manga_01\thttp://virtualmanga.null/manga_01"));
    f.AddLine(wxT("Manga_02\thttp://virtualmanga.null/manga_02"));

    f.Write();
    f.Close();

    LoadLocalMangaList();
}

wxArrayMCEntry VirtualManga::GetChapterList(MCEntry* MangaEntry)
{
    wxArrayMCEntry chapterList;

    chapterList.Add(new MCEntry(wxT("1 - Chapter"), MangaEntry->Link + wxT("/chapter_1")));
    chapterList.Add(new MCEntry(wxT("2.5 - Chapter"), MangaEntry->Link + wxT("/chapter_2.5")));
    chapterList.Add(new MCEntry(wxT("3v2 - Chapter"), MangaEntry->Link + wxT("/chapter_3v2")));
    chapterList.Add(new MCEntry(wxT("4.5v2 - Chapter"), MangaEntry->Link + wxT("/chapter_4.5v2")));

    return chapterList;
}

wxArrayString VirtualManga::GetPageLinks(wxString ChapterLink)
{
    wxArrayString pageLinks;

    // http://konachan.com/post/random
    pageLinks.Add(ChapterLink + wxT("/page_001.html"));
    pageLinks.Add(ChapterLink + wxT("/page_002.html"));
    pageLinks.Add(ChapterLink + wxT("/page_003.html"));
    pageLinks.Add(ChapterLink + wxT("/page_004.html"));
    pageLinks.Add(ChapterLink + wxT("/page_005.html"));
    pageLinks.Add(ChapterLink + wxT("/page_006.html"));
    pageLinks.Add(ChapterLink + wxT("/page_007.html"));

    return pageLinks;
}

wxString VirtualManga::GetImageLink(wxString PageLink)
{
    CurlRequest cr;
    cr.SetUrl(wxT("http://www.goodfon.com/catalog/anime/rand/"));
    cr.SetCompression(wxT("gzip"));
    wxString content;
    wxStringOutputStream sos(&content);
    cr.SetOutputStream(&sos);
    cr.ExecuteRequest();

    //wxString content = GetHtmlContent(wxT("http://www.goodfon.com/catalog/anime/rand/"));

    int indexStart = content.find(wxT("tabl_td")) + 7;
    indexStart = content.find(wxT("<img src=\""), indexStart) + 10;
    int indexEnd = content.find(wxT("\""), indexStart);

    if(indexStart > 9 && indexEnd >= -1)
    {
        return content.Mid(indexStart, indexEnd-indexStart);
    }
    else
    {
        return PageLink.BeforeLast('.') + wxT(".jpg");
    }
}
