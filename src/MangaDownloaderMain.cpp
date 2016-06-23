#include "MangaDownloaderMain.h"

//(*InternalHeaders(MangaDownloaderFrame)
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/intl.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(MangaDownloaderFrame)
const long MangaDownloaderFrame::ID_STATICLINE1 = wxNewId();
const long MangaDownloaderFrame::ID_STATICTEXT5 = wxNewId();
const long MangaDownloaderFrame::ID_TEXTCTRL2 = wxNewId();
const long MangaDownloaderFrame::ID_BITMAPBUTTON3 = wxNewId();
const long MangaDownloaderFrame::ID_STATICTEXT1 = wxNewId();
const long MangaDownloaderFrame::ID_COMBOBOX2 = wxNewId();
const long MangaDownloaderFrame::ID_BITMAPBUTTON1 = wxNewId();
const long MangaDownloaderFrame::ID_STATICTEXT2 = wxNewId();
const long MangaDownloaderFrame::ID_COMBOBOX1 = wxNewId();
const long MangaDownloaderFrame::ID_BITMAPBUTTON2 = wxNewId();
const long MangaDownloaderFrame::ID_STATICLINE2 = wxNewId();
const long MangaDownloaderFrame::ID_STATICTEXT3 = wxNewId();
const long MangaDownloaderFrame::ID_CHECKBOX1 = wxNewId();
const long MangaDownloaderFrame::ID_LISTCTRL1 = wxNewId();
const long MangaDownloaderFrame::ID_LISTCTRL2 = wxNewId();
const long MangaDownloaderFrame::ID_CHECKBOX2 = wxNewId();
const long MangaDownloaderFrame::ID_LISTCTRL3 = wxNewId();
const long MangaDownloaderFrame::ID_STATICLINE3 = wxNewId();
const long MangaDownloaderFrame::ID_BITMAPBUTTON6 = wxNewId();
const long MangaDownloaderFrame::ID_BITMAPBUTTON5 = wxNewId();
const long MangaDownloaderFrame::ID_STATUSBAR1 = wxNewId();
//*)
const long MangaDownloaderFrame::ID_JOBMENUITEM_REMOVEALL = wxNewId();
const long MangaDownloaderFrame::ID_JOBMENUITEM_REMOVESELECTED = wxNewId();
const long MangaDownloaderFrame::ID_MenuHelp = wxNewId();
const long MangaDownloaderFrame::ID_MenuAbout = wxNewId();
const long MangaDownloaderFrame::ID_MenuAuth = wxNewId();
const long MangaDownloaderFrame::ID_MenuStartUpSync = wxNewId();
const long MangaDownloaderFrame::ID_MenuTypingSearch = wxNewId();
const long MangaDownloaderFrame::ID_MenuChapterAutoSelect = wxNewId();
const long MangaDownloaderFrame::ID_MenuNewChapterNotification = wxNewId();
const long MangaDownloaderFrame::ID_MenuCompressChapters = wxNewId();
const long MangaDownloaderFrame::ID_MenuDeleteCompletedJobs = wxNewId();

BEGIN_EVENT_TABLE(MangaDownloaderFrame,wxFrame)
    //(*EventTable(MangaDownloaderFrame)
    //*)
END_EVENT_TABLE()

static int CompareStringCaseInsensitive(const wxString& First, const wxString& Second)
{
    return First.CmpNoCase(Second);
}

MangaDownloaderFrame::MangaDownloaderFrame(wxWindow* parent, wxWindowID id)
{
    wxProgressDialog Progress(wxT("HakuNeko"), wxT("Initialiting HakuNeko, please wait ..."), 10);
    Progress.Update(1, wxT("Initializing Manga Connectors ..."));
    AbortDownload = false;
    // preload configuration without GUI settings to get connector logins
    InitConfigurationFile();
    LoadConfiguration(true);
    MCC = new MangaConnectorCollection(Username, Password);

    Progress.Update(3, wxT("Initializing GUI Components ..."));
    //(*Initialize(MangaDownloaderFrame)
    wxFlexGridSizer* FlexGridSizerFilter;
    wxFlexGridSizer* FlexGridSizerControls;
    wxFlexGridSizer* FlexGridSizerContainer;
    wxFlexGridSizer* FlexGridSizerButtons;
    wxFlexGridSizer* FlexGridSizerMangaChapter;

    Create(parent, id, _("HakuNeko - Manga Downloader"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));
    SetToolTip(_("Right click to open menu"));
    FlexGridSizerContainer = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizerContainer->AddGrowableCol(0);
    FlexGridSizerContainer->AddGrowableRow(3);
    StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
    FlexGridSizerContainer->Add(StaticLine1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerControls = new wxFlexGridSizer(0, 3, 0, 0);
    FlexGridSizerControls->AddGrowableCol(1);
    StaticTextBaseDirectory = new wxStaticText(this, ID_STATICTEXT5, _("Base Directory:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizerControls->Add(StaticTextBaseDirectory, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrlDirectoryBase = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    TextCtrlDirectoryBase->Disable();
    FlexGridSizerControls->Add(TextCtrlDirectoryBase, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButtonBrowse = new wxBitmapButton(this, ID_BITMAPBUTTON3, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
    BitmapButtonBrowse->SetDefault();
    BitmapButtonBrowse->SetToolTip(_("Select the base directory of your manga collection\nMangas will be downloaded into this folder"));
    FlexGridSizerControls->Add(BitmapButtonBrowse, 1, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticTextSource = new wxStaticText(this, ID_STATICTEXT1, _("Source Connector:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizerControls->Add(StaticTextSource, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerFilter = new wxFlexGridSizer(0, 4, 0, 0);
    FlexGridSizerFilter->AddGrowableCol(0);
    FlexGridSizerFilter->AddGrowableCol(3);
    ComboBoxSource = new wxComboBox(this, ID_COMBOBOX2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_READONLY, wxDefaultValidator, _T("ID_COMBOBOX2"));
    ComboBoxSource->Append(MCC->GetConnectorLabels());
    FlexGridSizerFilter->Add(ComboBoxSource, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButtonUpdate = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    BitmapButtonUpdate->SetToolTip(_("Synchronize all local manga lists with internet"));
    FlexGridSizerFilter->Add(BitmapButtonUpdate, 1, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticTextSearch = new wxStaticText(this, ID_STATICTEXT2, _("Search:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizerFilter->Add(StaticTextSearch, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    ComboBoxSearchPattern = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_SORT|wxCB_DROPDOWN, wxDefaultValidator, _T("ID_COMBOBOX1"));
    FlexGridSizerFilter->Add(ComboBoxSearchPattern, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerControls->Add(FlexGridSizerFilter, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButtonBookmark = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
    BitmapButtonBookmark->SetDefault();
    FlexGridSizerControls->Add(BitmapButtonBookmark, 1, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerContainer->Add(FlexGridSizerControls, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, _T("ID_STATICLINE2"));
    FlexGridSizerContainer->Add(StaticLine2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerLists = new wxFlexGridSizer(0, 1, 0, 0);
    FlexGridSizerLists->AddGrowableCol(0);
    FlexGridSizerLists->AddGrowableRow(0);
    FlexGridSizerLists->AddGrowableRow(1);
    FlexGridSizerMangaChapter = new wxFlexGridSizer(0, 2, 0, 0);
    FlexGridSizerMangaChapter->AddGrowableCol(0);
    FlexGridSizerMangaChapter->AddGrowableCol(1);
    FlexGridSizerMangaChapter->AddGrowableRow(1);
    StaticTextMangaList = new wxStaticText(this, ID_STATICTEXT3, _("Manga List:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizerMangaChapter->Add(StaticTextMangaList, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxChapters = new wxCheckBox(this, ID_CHECKBOX1, _("Chapter List:"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    CheckBoxChapters->SetValue(false);
    CheckBoxChapters->SetToolTip(_("Select / Deselect all chapters in the list"));
    FlexGridSizerMangaChapter->Add(CheckBoxChapters, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    ListCtrlMangas = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxSUNKEN_BORDER, wxDefaultValidator, _T("ID_LISTCTRL1"));
    ListCtrlMangas->SetMinSize(wxSize(380,160));
    ListCtrlMangas->InsertColumn(0, wxT("Mangas"));
    ListCtrlMangas->SetToolTip(_("Holding the CTRL key while selecting a manga\nwill popup a dialog to filter the chapter list"));
    FlexGridSizerMangaChapter->Add(ListCtrlMangas, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ListCtrlChapters = new wxListCtrl(this, ID_LISTCTRL2, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_NO_HEADER|wxLC_SINGLE_SEL|wxSUNKEN_BORDER, wxDefaultValidator, _T("ID_LISTCTRL2"));
    ListCtrlChapters->SetMinSize(wxSize(380,160));
    ListCtrlChapters->InsertColumn(0, wxT("Chapters"));
    FlexGridSizerMangaChapter->Add(ListCtrlChapters, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    CheckBoxShowJobList = new wxCheckBox(this, ID_CHECKBOX2, _("Show / Hide Job List"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    CheckBoxShowJobList->SetValue(true);
    FlexGridSizerMangaChapter->Add(CheckBoxShowJobList, 1, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerLists->Add(FlexGridSizerMangaChapter, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    ListCtrlJobs = new wxListCtrl(this, ID_LISTCTRL3, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxSUNKEN_BORDER, wxDefaultValidator, _T("ID_LISTCTRL3"));
    ListCtrlJobs->SetMinSize(wxSize(-1,140));
    ListCtrlJobs->InsertColumn(0, wxT("ID"), wxLIST_FORMAT_LEFT, 90);
    ListCtrlJobs->InsertColumn(1, wxT("Connector"), wxLIST_FORMAT_LEFT, 120);
    ListCtrlJobs->InsertColumn(2, wxT("Manga"));
    ListCtrlJobs->InsertColumn(3, wxT("Chapter"));
    FlexGridSizerLists->Add(ListCtrlJobs, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerContainer->Add(FlexGridSizerLists, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticLine3 = new wxStaticLine(this, ID_STATICLINE3, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL, _T("ID_STATICLINE3"));
    FlexGridSizerContainer->Add(StaticLine3, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerButtons = new wxFlexGridSizer(0, 3, 0, 0);
    BitmapButtonDownload = new wxBitmapButton(this, ID_BITMAPBUTTON6, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON6"));
    BitmapButtonDownload->SetToolTip(_("Start downloading all selected manga chapters to the base directory"));
    FlexGridSizerButtons->Add(BitmapButtonDownload, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerButtons->Add(8,32,1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButtonAbort = new wxBitmapButton(this, ID_BITMAPBUTTON5, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON5"));
    BitmapButtonAbort->Disable();
    BitmapButtonAbort->SetToolTip(_("Abort the current download"));
    FlexGridSizerButtons->Add(BitmapButtonAbort, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizerContainer->Add(FlexGridSizerButtons, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(FlexGridSizerContainer);
    StatusBar = new wxStatusBar(this, ID_STATUSBAR1, wxST_SIZEGRIP, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[3] = { -10, -3, -4 };
    int __wxStatusBarStyles_1[3] = { wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL };
    StatusBar->SetFieldsCount(3,__wxStatusBarWidths_1);
    StatusBar->SetStatusStyles(3,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar);
    StatusBar->SetStatusText(wxT("Selected Chapters: 0"), 2);
    DirDialogBrowse = new wxDirDialog(this, _("Select directory"), wxEmptyString, wxDD_DEFAULT_STYLE, wxDefaultPosition, wxDefaultSize, _T("wxDirDialog"));
    MenuJobContext.Append(ID_JOBMENUITEM_REMOVESELECTED, wxT("Remove Selected"));
    MenuJobContext.Append(ID_JOBMENUITEM_REMOVEALL, wxT("Remove All Jobs"));
    MenuJobContext.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MangaDownloaderFrame::OnMenuJobContextClick, NULL, this);

    Progress.Update(4, wxT("Initializing GUI Events ..."));
    Connect(ID_BITMAPBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MangaDownloaderFrame::OnButtonBrowseClick);
    Connect(ID_COMBOBOX2,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&MangaDownloaderFrame::OnChoiceSourceSelect);
    Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MangaDownloaderFrame::OnButtonUpdateClick);
    Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&MangaDownloaderFrame::OnSearchPattern);
    Connect(ID_COMBOBOX1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&MangaDownloaderFrame::OnSearchPattern);
    Connect(ID_COMBOBOX1,wxEVT_COMMAND_TEXT_ENTER,(wxObjectEventFunction)&MangaDownloaderFrame::OnSearchPattern);
    Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MangaDownloaderFrame::OnBitmapButtonBookmarkClick);
    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&MangaDownloaderFrame::OnCheckBoxChaptersClick);
    Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&MangaDownloaderFrame::OnListCtrlMangasItemSelect);
    Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_DESELECTED,(wxObjectEventFunction)&MangaDownloaderFrame::OnListCtrlMangasItemDeselect);
    Connect(ID_LISTCTRL2,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&MangaDownloaderFrame::OnListCtrlChaptersItemSelect);
    Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&MangaDownloaderFrame::OnCheckBoxShowJobListClick);
    Connect(ID_LISTCTRL3,wxEVT_COMMAND_LIST_ITEM_ACTIVATED,(wxObjectEventFunction)&MangaDownloaderFrame::OnListCtrlJobsItemActivated);
    Connect(ID_LISTCTRL3,wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK,(wxObjectEventFunction)&MangaDownloaderFrame::OnListCtrlJobsItemRClick);
    Connect(ID_BITMAPBUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MangaDownloaderFrame::OnButtonDownloadClick);
    Connect(ID_BITMAPBUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MangaDownloaderFrame::OnButtonAbortClick);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&MangaDownloaderFrame::OnClose);
    Connect(wxEVT_SIZE,(wxObjectEventFunction)&MangaDownloaderFrame::OnResize);
    //*)
    Connect(wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&MangaDownloaderFrame::OnMainWindowRClick);

    Progress.Update(5, wxT("Initializing Resources ..."));
    LoadResources();
    //FlexGridSizerContainer->Fit(this); // will be done by SetSizeHints()
    FlexGridSizerContainer->SetSizeHints(this); // fit window minwidth, in case combobox->minwidth increased by LoadConfiguration()
    Center();

    StartupSync = false;
    TypingSearch = true;
    ChapterAutoSelect = false;
    NewChapterNotification = false;
    CompressChapters = false;
    DeleteCompletedJobs = true;

    Progress.Update(6, wxT("Initializing Configuration ..."));
    // LoadConfiguration() may show the window immediately, so call it last...
    // it also may change initial minwidth of comboboxes
    LoadConfiguration();
    Progress.Update(8, wxT("Checking for Updates ..."));
    UpdateCheck();

    MenuMain = new wxMenu(_("Main Menu"));
    MenuMain->Append(ID_MenuAuth, _("Batoto Login..."));
    MenuMain->AppendCheckItem(ID_MenuStartUpSync, _("Enable Start Up Sync"), wxT("Manga lists will be updated, every time when HakuNeko starts."))->Check(StartupSync);
    MenuMain->AppendCheckItem(ID_MenuTypingSearch, _("Enable Smart Search"), wxT("Search manga list while typing (disable this on slow machines)."))->Check(TypingSearch);
    MenuMain->AppendCheckItem(ID_MenuChapterAutoSelect, _("Enable Chapter Auto-Selection"), wxT("Search manga list while typing (disable this on slow machines)."))->Check(ChapterAutoSelect);
    MenuMain->AppendCheckItem(ID_MenuNewChapterNotification, _("Show Missing Chapter Notification"), wxT("When selecting a manga, a notification window will appear checking for missing chapters."))->Check(NewChapterNotification);
    MenuMain->AppendCheckItem(ID_MenuCompressChapters, _("Compress Chapters (Comic Book Archive)"), wxT("Create comic book archives (*.cbz) instead of downloading separate images."))->Check(CompressChapters);
    MenuMain->AppendCheckItem(ID_MenuDeleteCompletedJobs, _("Show 'Delete Completed Jobs' Dialog"), wxT("Shows a dialog after downloading, asking to remove completed jobs from the list."))->Check(DeleteCompletedJobs);
    MenuMain->AppendSeparator();
    MenuMain->Append(ID_MenuHelp, _("Help (Online)"));
    MenuMain->Append(ID_MenuAbout, _("About"));
    MenuMain->Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&MangaDownloaderFrame::OnMenuMainClick, NULL, this);

    Progress.Close();
}

MangaDownloaderFrame::~MangaDownloaderFrame()
{
    //(*Destroy(MangaDownloaderFrame)
    //*)
    wxDELETE(MCC);
    wxDELETEA(ResourceImages);
}

void MangaDownloaderFrame::LoadImageResource(RCDATA RCImage, wxBitmapType Type, ResourceImage ImageLabel)
{
    wxMemoryInputStream* ResourceMemoryStream = new wxMemoryInputStream(RCImage.data, RCImage.length);
    ResourceImages[ImageLabel] = wxBitmap(wxImage(*ResourceMemoryStream, Type));
    wxDELETE(ResourceMemoryStream);
    ResourceMemoryStream = NULL;
}

void MangaDownloaderFrame::LoadResources()
{
    ResourceImages = new wxBitmap[21];

    LoadImageResource(RESOURCES::APP_ICO, wxBITMAP_TYPE_ICO, APP_ICO);
    wxIcon ico;
    ico.CopyFromBitmap(ResourceImages[APP_ICO]);
    SetIcon(ico);

    LoadImageResource(RESOURCES::BTN_BROWSE_NORMAL_PNG, wxBITMAP_TYPE_PNG, BTN_BROWSE_NORM);
    BitmapButtonBrowse->SetBitmapLabel(ResourceImages[BTN_BROWSE_NORM]);
    LoadImageResource(RESOURCES::BTN_BROWSE_HOVER_PNG, wxBITMAP_TYPE_PNG, BTN_BROWSE_HOVER);
    BitmapButtonBrowse->SetBitmapHover(ResourceImages[BTN_BROWSE_HOVER]);
    LoadImageResource(RESOURCES::BTN_BROWSE_DISABLED_PNG, wxBITMAP_TYPE_PNG, BTN_BROWSE_DISABLE);
    BitmapButtonBrowse->SetBitmapDisabled(ResourceImages[BTN_BROWSE_DISABLE]);

    LoadImageResource(RESOURCES::BTN_SYNC_NORMAL_PNG, wxBITMAP_TYPE_PNG, BTN_SYNC_NORM);
    BitmapButtonUpdate->SetBitmapLabel(ResourceImages[BTN_SYNC_NORM]);
    LoadImageResource(RESOURCES::BTN_SYNC_HOVER_PNG, wxBITMAP_TYPE_PNG, BTN_SYNC_HOVER);
    BitmapButtonUpdate->SetBitmapHover(ResourceImages[BTN_SYNC_HOVER]);
    LoadImageResource(RESOURCES::BTN_SYNC_DISABLED_PNG, wxBITMAP_TYPE_PNG, BTN_SYNC_DISABLE);
    BitmapButtonUpdate->SetBitmapDisabled(ResourceImages[BTN_SYNC_DISABLE]);

    LoadImageResource(RESOURCES::BTN_BOOKMARK_NORMAL_PNG, wxBITMAP_TYPE_PNG, BTN_BOOK_NORM);
    BitmapButtonBookmark->SetBitmapLabel(ResourceImages[BTN_BOOK_NORM]);
    LoadImageResource(RESOURCES::BTN_BOOKMARK_HOVER_PNG, wxBITMAP_TYPE_PNG, BTN_BOOK_HOVER);
    BitmapButtonBookmark->SetBitmapHover(ResourceImages[BTN_BOOK_HOVER]);
    LoadImageResource(RESOURCES::BTN_BOOKMARK_DISABLED_PNG, wxBITMAP_TYPE_PNG, BTN_BOOK_DISABLE);
    BitmapButtonBookmark->SetBitmapDisabled(ResourceImages[BTN_BOOK_DISABLE]);

    LoadImageResource(RESOURCES::BTN_BOOKREMOVE_NORMAL_PNG, wxBITMAP_TYPE_PNG, BTN_BOOKDEL_NORM);
    LoadImageResource(RESOURCES::BTN_BOOKREMOVE_HOVER_PNG, wxBITMAP_TYPE_PNG, BTN_BOOKDEL_HOVER);
    LoadImageResource(RESOURCES::BTN_BOOKREMOVE_DISABLED_PNG, wxBITMAP_TYPE_PNG, BTN_BOOKDEL_DISABLE);
    BitmapButtonBookmark->SetToolTip(wxT("Add entry to search list\nHold right mouse button + left click\nto bookmark without connector"));
    if(ComboBoxSearchPattern->FindString(ComboBoxSearchPattern->GetValue()) > -1)
    {
        BitmapButtonBookmark->SetBitmapLabel(ResourceImages[BTN_BOOKDEL_NORM]);
        BitmapButtonBookmark->SetBitmapHover(ResourceImages[BTN_BOOKDEL_HOVER]);
        BitmapButtonBookmark->SetBitmapDisabled(ResourceImages[BTN_BOOKDEL_DISABLE]);
        BitmapButtonBookmark->SetToolTip(wxT("Remove entry from search list"));
    }

    LoadImageResource(RESOURCES::BTN_DOWNLOAD_NORMAL_PNG, wxBITMAP_TYPE_PNG, BTN_DOWN_NORM);
    BitmapButtonDownload->SetBitmapLabel(ResourceImages[BTN_DOWN_NORM]);
    LoadImageResource(RESOURCES::BTN_DOWNLOAD_HOVER_PNG, wxBITMAP_TYPE_PNG, BTN_DOWN_HOVER);
    BitmapButtonDownload->SetBitmapHover(ResourceImages[BTN_DOWN_HOVER]);
    LoadImageResource(RESOURCES::BTN_DOWNLOAD_DISABLED_PNG, wxBITMAP_TYPE_PNG, BTN_DOWN_DISABLE);
    BitmapButtonDownload->SetBitmapDisabled(ResourceImages[BTN_DOWN_DISABLE]);

    LoadImageResource(RESOURCES::BTN_ABORT_NORMAL_PNG, wxBITMAP_TYPE_PNG, BTN_ABORT_NORM);
    BitmapButtonAbort->SetBitmapLabel(ResourceImages[BTN_ABORT_NORM]);
    LoadImageResource(RESOURCES::BTN_ABORT_HOVER_PNG, wxBITMAP_TYPE_PNG, BTN_ABORT_HOVER);
    BitmapButtonAbort->SetBitmapHover(ResourceImages[BTN_ABORT_HOVER]);
    LoadImageResource(RESOURCES::BTN_ABORT_DISABLED_PNG, wxBITMAP_TYPE_PNG, BTN_ABORT_DISABLE);
    BitmapButtonAbort->SetBitmapDisabled(ResourceImages[BTN_ABORT_DISABLE]);

    // create image list for checked/unchecked images from resource
    wxImageList* checkIcons = new wxImageList(12, 12);
    LoadImageResource(RESOURCES::UNTICK_PNG, wxBITMAP_TYPE_PNG, CHECK_DISABLED);
    checkIcons->Add(ResourceImages[CHECK_DISABLED]);
    LoadImageResource(RESOURCES::TICK_PNG, wxBITMAP_TYPE_PNG, CHECK_ENABLED);
    checkIcons->Add(ResourceImages[CHECK_ENABLED]);
    ListCtrlChapters->SetImageList(checkIcons, wxIMAGE_LIST_SMALL);
}

void MangaDownloaderFrame::UpdateCheck()
{
    this->Disable();
    wxString latest;
    wxString version = VERSION;
    Logger::Log(wxT("NOTE +++ MangaDownloaderFrame::UpdateCheck() -> current version = ") + version);
    wxMemoryOutputStream mos;
    CurlRequest cr;
    // cr.SetUrl(wxT("http://sourceforge.net/projects/hakuneko/files/list"));
    cr.SetUrl(wxT("http://sourceforge.net/projects/hakuneko/rss?path=/"));
    cr.SetOutputStream(&mos);
    if(cr.ExecuteRequest())
    {
        wxMemoryInputStream mis(mos);
        wxXmlDocument xml_feed(mis);
        wxXmlNode* node = xml_feed.GetRoot()->GetChildren()->GetChildren();
        while(node)
        {
            if(node->GetName().IsSameAs(wxT("item"), false))
            {
                latest = node->GetChildren()->GetNodeContent();
                #ifdef __LINUX__
                    if(latest.Contains(wxT("hakuneko")) && !latest.Contains(wxT("windows")) && !latest.Contains(wxT("src")))
                #endif
                #ifdef __WINDOWS__
                    if(latest.Contains(wxT("hakuneko")) && latest.Contains(wxT("windows")))
                #endif
                {
                    Logger::Log(wxT("NOTE +++ MangaDownloaderFrame::UpdateCheck() -> latest package = ") + latest);
                    if(!latest.Contains(version))
                    {
                        wxAboutDialogInfo about;
                        about.SetName(wxT("HakuNeko Update"));
                        about.SetDescription(wxT("Your version: ") + version + wxT("\nLatest version: ") + latest.AfterLast('/').AfterFirst('_').BeforeFirst('_'));
                        about.SetWebSite(wxT("http://sourceforge.net/projects/hakuneko/files/"), wxT("Download Page"));
                        wxAboutBox(about);
                    }
                    else
                    {
                        //wxMessageBox(wxT("Your launcher (") + version + wxT(") is the latest version."));
                    }
                    break;
                }
            }
            node = node->GetNext();
        }
    }
    else
    {
        Logger::Log(wxT("ERROR +++ MangaDownloaderFrame::UpdateCheck() -> server connection failed"));
    }
    mos.Close();
    this->Enable();
}

void MangaDownloaderFrame::InitConfigurationFile()
{
    /*
    wxStandardPaths::GetExecutablePath
    Return the directory and the filename for the current executable
    Win7: .\HakuNeko\Hakuneko.exe
    Unix: /usr/local/bin/hakuneko

    wxStandardPaths::GetDataDir()
    Return the location of the applications global, i.e. not user-specific, data files
    Win7: .\HakuNeko
    Unix: /usr/local/share/hakuneko

    wxStandardPaths::GetConfigDir()
    Return the directory containing the system config files
    // Win7: C:\ProgramData\HakuNeko
    // Unix: /etc

    wxStandardPaths::GetLocalDataDir()
    Return the location for application data files which are host-specific and can't, or shouldn't, be shared with the other machines
    // Win7: .\HakuNeko
    // Unix: /etc/hakuneko

    wxStandardPaths::GetUserConfigDir()
    Return the directory for the user config files
    Win7: C:\User\UserName\AppData\Roaming
    Unix: /home/username

    wxStandardPaths::GetUserDataDir()
    Return the directory for the user-dependent application data files
    Win7: C:\User\UserName\AppData\Roaming\HakuNeko
    Unix: /home/username/.hakuneko

    wxStandardPaths::GetUserLocalDataDir()
    Return the directory for user data files which shouldn't be shared with the other machines
    Win7: C:\User\UserName\AppData\Local\HakuNeko
    Unix: /home/username/.hakuneko
    */

    #ifdef PORTABLE
        #ifdef __LINUX__
            ConfigurationFile.Assign(wxStandardPaths::Get().GetExecutablePath().BeforeLast('/') + wxT("/gui.conf"));
        #endif
        #ifdef __WINDOWS__
            ConfigurationFile.Assign(wxStandardPaths::Get().GetExecutablePath().BeforeLast('\\') + wxT("\\gui.conf"));
        #endif
    #else
        #ifdef __LINUX__
            wxString EnvironmentFilePath;
            wxGetEnv(wxT("XDG_CONFIG_HOME"), &EnvironmentFilePath);
            EnvironmentFilePath = EnvironmentFilePath.BeforeFirst(':');
            if(EnvironmentFilePath.IsEmpty())
            {
                EnvironmentFilePath = wxStandardPaths::Get().GetUserConfigDir() + wxT("/.config/hakuneko/gui.conf");
            }
            else
            {
                EnvironmentFilePath +=  wxT("/hakuneko/gui.conf");
            }
            ConfigurationFile.Assign(EnvironmentFilePath);
            // test if a non XDG compliant configuration exists (hakuneko < 1.0.5)
            wxString DeprecatedPath = wxStandardPaths::Get().GetUserDataDir();
            if(wxDirExists(DeprecatedPath))
            {
                // check if deprecated configuration path is different from current
                if(!DeprecatedPath.IsSameAs(ConfigurationFile.GetPath()))
                {
                    wxMessageBox(wxT("HakuNeko has detected a deprecated configuration directory that will no longer be used in:\n\n") + wxStandardPaths::Get().GetUserDataDir() + wxT("\n\nThe new configuration directory is located in:\n\n") + ConfigurationFile.GetPath() + wxT("\n\nYou can move your deprecated files into the new configuration directory and/or delete the old configuration directory."));
                }
            }
        #endif
        #ifdef __WINDOWS__
            ConfigurationFile.Assign(wxStandardPaths::Get().GetUserDataDir() + wxT("\\gui.conf"));
        #endif
    #endif
}

void MangaDownloaderFrame::LoadConfiguration(bool NoGUI)
{
    // NOTE: to prevent LoadMangaList when applying configuration parameters, we disable comboboxes
    if(!NoGUI) {
        ComboBoxSource->Disable();
        ComboBoxSearchPattern->Disable();
    }

    wxTextFile f;
/*
    if(!ConfigurationFile.IsDirWritable())
    {
        wxMessageBox(wxT("Access denied!\nConfiguration directory: ") + ConfigurationFile.GetPath());
        return;
    }
*/

    ConfigurationFile.Mkdir(0755, wxPATH_MKDIR_FULL);

    // create file, or open if already exists
    if(!f.Create(ConfigurationFile.GetFullPath()))
    {
        f.Open(ConfigurationFile.GetFullPath());
    }

    wxString line = f.GetFirstLine();
    while(!f.Eof())
    {
        if(line.StartsWith(wxT("username=")))
        {
            Username = line.AfterFirst(L'=');
        }

        if(line.StartsWith(wxT("password=")))
        {
            wxString enc = line.AfterFirst(L'=');
            Password = wxT("");
            unsigned int n = 0;
            for (wxString::const_iterator i = enc.begin(); i != enc.end(); i++)
            {
                Password += wxUniChar((int)*i + ((7*n+3) % 13));
                n++;
            }
        }

        if(!NoGUI) {
            if(line.StartsWith(wxT("startupsync=")))
            {
                if(line.AfterFirst(L'=').IsSameAs(wxT("true")))
                {
                    StartupSync = true;
                    Show();
                    wxCommandEvent ce;
                    OnButtonUpdateClick(ce);
                }
                else
                {
                    StartupSync = false;
                }
            }

            if(line.StartsWith(wxT("directory=")))
            {
                TextCtrlDirectoryBase->SetValue(line.AfterFirst(L'='));
            }

            if(line.StartsWith(wxT("connector=")))
            {
                //ComboBoxSource->SetValue(line.AfterFirst(L'='));
                // don't trigger event and don't set SelectedIndex
                ComboBoxSource->SetSelection(ComboBoxSource->FindString(line.AfterFirst(L'=')));
            }

            if(line.StartsWith(wxT("viewjobs=")))
            {
                if(line.AfterFirst(L'=').IsSameAs(wxT("true")))
                {
                    CheckBoxShowJobList->SetValue(true);
                }
                else
                {
                    CheckBoxShowJobList->SetValue(false);
                }
                wxCommandEvent ce;
                OnCheckBoxShowJobListClick(ce);
            }
        }

        if(line.StartsWith(wxT("typingsearch=")))
        {
            if(line.AfterFirst(L'=').IsSameAs(wxT("true")))
            {
                TypingSearch = true;
            }
            else
            {
                TypingSearch = false;
            }
        }

        if(line.StartsWith(wxT("chapterautoselect=")))
        {
            ChapterAutoSelect = line.AfterFirst(L'=').IsSameAs(wxT("true"));
        }

        if(line.StartsWith(wxT("chapternotification=")))
        {
            if(line.AfterFirst(L'=').IsSameAs(wxT("true")))
            {
                NewChapterNotification = true;
            }
            else
            {
                NewChapterNotification = false;
            }
        }

        if(line.StartsWith(wxT("compresschapters=")))
        {
            if(line.AfterFirst(L'=').IsSameAs(wxT("true")))
            {
                CompressChapters = true;
            }
            else
            {
                CompressChapters = false;
            }
        }

        if(line.StartsWith(wxT("deletejobs=")))
        {
            if(line.AfterFirst(L'=').IsSameAs(wxT("true")))
            {
                DeleteCompletedJobs = true;
            }
            else
            {
                DeleteCompletedJobs = false;
            }
        }

        if (!NoGUI) {
            if(line.StartsWith(wxT("filter=")))
            {
                wxString pattern = line.AfterFirst(L'=');
                ComboBoxSearchPattern->SetValue(pattern);
                if(pattern.EndsWith(wxT(">")) && (int)pattern.rfind(wxT(" <")) > -1)
                {
                    // don't trigger event
                    ComboBoxSource->SetSelection(ComboBoxSource->FindString(pattern.AfterLast('<').BeforeLast('>')));
                    if(ComboBoxSource->GetSelection() < 0)
                    {
                        ComboBoxSource->SetValue(wxEmptyString);
                    }
                }
            }

            if(line.StartsWith(wxT("filter[]=")))
            {
                // NOTE: combobox has autocomplete/overwrite behaviour when an item from the list starts with the current value
                // this problem only occurs in wxMSW not in wxGTK
                ComboBoxSearchPattern->Append(line.AfterFirst(L'='));
                // NOTE: on wxGTK SelectedIndex is -1, on wxMSW it might be set when the value is in the list...
                /*
                wxString before = ComboBoxSearchPattern->GetValue();
                wxYield(); // trigger combobox internal text<->choice matching search
                wxString after = ComboBoxSearchPattern->GetValue();
                if(after.Upper().StartsWith(before.Upper()))
                {
                    ComboBoxSearchPattern->SetValue(before);
                    ComboBoxSearchPattern->SetLabel(previous);
                    wxMessageBox(wxT("New item has \"overwrite behaviour\" to the current item:\n\nValue: ") + before + wxT("\nNew Value: ") + choice);
                }
                */
            }
        }
        line = f.GetNextLine();
    }

    f.Close();

    if(!NoGUI) {
        UpdateBookmarkButton();

        // trigger manga load list when connector is selected
        if(ComboBoxSource->GetSelection() > -1)
        {
            LoadMangaList(ComboBoxSearchPattern->GetValue());
        }

        ComboBoxSource->Enable();
        ComboBoxSearchPattern->Enable();
    }
}

void MangaDownloaderFrame::SaveConfiguration()
{
    wxTextFile f;
/*
    if(!ConfigurationFile.IsDirWritable())
    {
        wxMessageBox(wxT("Access denied!\nConfiguration directory: ") + ConfigurationFile.GetPath());
        return;
    }
*/
    ConfigurationFile.Mkdir(0755, wxPATH_MKDIR_FULL);

    // create file, or open if already exists
    if(!f.Create(ConfigurationFile.GetFullPath()))
    {
        f.Open(ConfigurationFile.GetFullPath());
        f.Clear();
    }

    if(Username != wxEmptyString)
    {
        f.AddLine(wxT("username=") + Username);
    }

    if(Password != wxEmptyString)
    {
        wxString enc = wxT("");
        unsigned int n = 0;
        for (wxString::const_iterator i = Password.begin(); i != Password.end(); i++)
        {
            enc += wxUniChar((int)*i - ((7*n+3) % 13));
            n++;
        }
        f.AddLine(wxT("password=") + enc);
    }

    if(StartupSync)
    {
        f.AddLine(wxT("startupsync=true"));
    }
    else
    {
        f.AddLine(wxT("startupsync=false"));
    }

    f.AddLine(wxT("directory=") + TextCtrlDirectoryBase->GetValue());
    f.AddLine(wxT("connector=") + ComboBoxSource->GetValue());

    if(CheckBoxShowJobList->GetValue())
    {
        f.AddLine(wxT("viewjobs=true"));
    }
    else
    {
        f.AddLine(wxT("viewjobs=false"));
    }

    if(TypingSearch)
    {
        f.AddLine(wxT("typingsearch=true"));
    }
    else
    {
        f.AddLine(wxT("typingsearch=false"));
    }

    if(ChapterAutoSelect)
    {
        f.AddLine(wxT("chapterautoselect=true"));
    }
    else
    {
        f.AddLine(wxT("chapterautoselect=false"));
    }

    if(NewChapterNotification)
    {
        f.AddLine(wxT("chapternotification=true"));
    }
    else
    {
        f.AddLine(wxT("chapternotification=false"));
    }

    if(CompressChapters)
    {
        f.AddLine(wxT("compresschapters=true"));
    }
    else
    {
        f.AddLine(wxT("compresschapters=false"));
    }

    if(DeleteCompletedJobs)
    {
        f.AddLine(wxT("deletejobs=true"));
    }
    else
    {
        f.AddLine(wxT("deletejobs=false"));
    }

    f.AddLine(wxT("filter=") + ComboBoxSearchPattern->GetValue());
    for(unsigned int i=0; i<ComboBoxSearchPattern->GetCount(); i++)
    {
        f.AddLine(wxT("filter[]=") + ComboBoxSearchPattern->GetString(i));
    }

    f.Write();
    f.Close();
}

void MangaDownloaderFrame::LoadMangaList(wxString Pattern)
{
    ListCtrlMangas->DeleteAllItems();
    ListCtrlChapters->DeleteAllItems();

    if(ComboBoxSource->GetSelection() > -1)
    {
        CurrentMangaList = MCC->GetMangaList(ComboBoxSource->GetValue());
        wxArrayMCEntry temp;
        long n = 0; // counter for pattern match results

        if(Pattern.EndsWith(wxT(">")) && (int)Pattern.rfind(wxT(" <")) > -1)
        {
            Pattern = Pattern.BeforeLast('<').BeforeLast(' ');
        }
        Pattern.MakeLower();

        ListCtrlMangas->Freeze();
        for(size_t i=0; i<CurrentMangaList.GetCount(); i++)
        {
            if(CurrentMangaList[i]->Label.Lower().Find(Pattern) > -1)
            {
                temp.Add(CurrentMangaList[i]);
                // TODO: even when list is frozen, insert operations are really slow in wxGTK
                ListCtrlMangas->InsertItem(n, CurrentMangaList[i]->Label);
                n++;
            }
        }

        // update current manga list BEFORE calling colorify !!!
        CurrentMangaList = temp;

        // set manga font color depending on their existence in base directory
        ColorifyMangaList();

        // select chapters from exactly matching manga or when there is only one manga in the list
        if(ChapterAutoSelect && CurrentMangaList.GetCount() < 16) // for performance reasons: only crawl list when < 16 items
        {
            for(size_t i=0; i<CurrentMangaList.GetCount(); i++)
            {
                if(CurrentMangaList.GetCount() == 1 || CurrentMangaList[i]->Label.Lower().IsSameAs(Pattern))
                {
                    // TODO: improve live search (only load chapter list when no key pressed between 1~2 seconds?)
                    ComboBoxSearchPattern->Disable();
                    ListCtrlMangas->SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                    ListCtrlMangas->EnsureVisible(i);
                    ComboBoxSearchPattern->Enable();
                    ComboBoxSearchPattern->SetFocus();
                    ComboBoxSearchPattern->SetSelection(Pattern.Len()+4, Pattern.Len()+4);
                    break;
                }
            }
        }
        ListCtrlMangas->Thaw();
    }
}

void MangaDownloaderFrame::ColorifyMangaList()
{
    wxFileName baseDirectory(TextCtrlDirectoryBase->GetValue(), wxEmptyString);

    if(ComboBoxSource->GetSelection() > -1)
    {
        ListCtrlMangas->Freeze();
        for(long i=0; i<(long)CurrentMangaList.GetCount(); i++)
        {
            if(wxDirExists(baseDirectory.GetPathWithSep() + CurrentMangaList[i]->SafeLabel))
            {
                ListCtrlMangas->SetItemTextColour(i, wxColor(0, 150, 250));
            }
            else
            {
                // reset item specific color to listctrl default background
                //ListCtrlMangas->SetItemTextColour(i, wxNullColour);
                ListCtrlMangas->SetItemTextColour(i, ListCtrlMangas->GetTextColour());
            }
        }
        ListCtrlMangas->Thaw();
    }
}

void MangaDownloaderFrame::LoadChapterList(wxString Pattern)
{
    ListCtrlChapters->DeleteAllItems();
    CheckBoxChapters->SetValue(false);

    bool AllChaptersChecked = true;
    wxString connectorLabel = ComboBoxSource->GetValue();
    long mangaIndex = ListCtrlMangas->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if(ComboBoxSource->GetSelection() > -1 && mangaIndex > -1)
    {
        wxArrayMCEntry temp = MCC->GetChapterList(connectorLabel, CurrentMangaList[mangaIndex]);
        CurrentChapterList.Clear();
        long n = 0; // counter for pattern match results

        Pattern.MakeLower();

        ListCtrlChapters->Freeze();
        for(long i=0; i<(long)temp.GetCount(); i++)
        {
            if(temp[i]->Label.Lower().Find(Pattern) > -1)
            {
                CurrentChapterList.Add(temp[i]);

                ListCtrlChapters->InsertItem(n, CurrentChapterList[n]->Label, 0);

                if(MCC->ContainsJob(MCC->GenerateJobID(CurrentChapterList[n])))
                {
                    SetChapterCheckedState(n, true, false); // job is already in joblist
                }
                if(CheckBoxChapters->IsChecked() && !GetChapterCheckedState(n))
                {
                    SetChapterCheckedState(n, true, true); // need to add this job to joblist
                }
                if(AllChaptersChecked && !GetChapterCheckedState(n))
                {
                    AllChaptersChecked = false;
                }
                n++;
            }
        }

        if(CurrentChapterList.GetCount() > 0)
        {
            CheckBoxChapters->SetValue(AllChaptersChecked);
        }

        ListCtrlChapters->Thaw();

        // set chapter font color depending on their existence in base directory
        ColorifyChapterList();
    }
}

void MangaDownloaderFrame::ColorifyChapterList()
{
    long mangaIndex = ListCtrlMangas->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    wxFileName baseDirectory(TextCtrlDirectoryBase->GetValue(), wxEmptyString);

    if(ComboBoxSource->GetSelection() > -1 && mangaIndex > -1)
    {
        baseDirectory.AppendDir(CurrentMangaList[mangaIndex]->SafeLabel);

        bool addNewChaptersToJoblist = false;
        if(wxDirExists(baseDirectory.GetPath()) && NewChapterNotification)
        {
            // problem when using different connectors for same manga (different chapter names!)
            // also not reliable, because count based analysis can be wrong
            // i.e.
            // * additional files in directory
            // * chapters differs in name because downloaded from different connectors
            // * existing chapters checked for redownload
            // NOTE: a one by one comparsion should be performed in advance, but this is to costly...

            // count the chapters in the manga directory
            size_t currentChapterCount = 0;
            wxDir mangaDirectory(baseDirectory.GetPath());
            wxString chapterItem;
            if(mangaDirectory.GetFirst(&chapterItem))
            {
                if(wxDir::Exists(baseDirectory.GetPathWithSep() + chapterItem) || chapterItem.EndsWith(wxT(".cbz")) || chapterItem.EndsWith(wxT(".mp4")))
                {
                    currentChapterCount++;
                }
                while(mangaDirectory.GetNext(&chapterItem))
                {
                    if(wxDir::Exists(baseDirectory.GetPathWithSep() + chapterItem) || chapterItem.EndsWith(wxT(".cbz")) || chapterItem.EndsWith(wxT(".mp4")))
                    {
                        currentChapterCount++;
                    }
                }
            }

            // count the chapters that are available from the connector
            // FIXME: if chapter list was filtered with a search pattern, the count does not represent the number of all available chapters
            // solution: store the overall chapter count in another global variable when loading chapter list...
            // this behaviour maybe correct i.e. you always use filter: "english" to ignore other languages...
            size_t availableChapterCount = CurrentChapterList.GetCount();

            // count the chapters that are already checked for download
            size_t checkedChapterCount = 0;
            for(long c=0; c<(long)CurrentChapterList.GetCount(); c++)
            {
                if(MCC->ContainsJob(MCC->GenerateJobID(CurrentChapterList[c])))
                {
                    checkedChapterCount++;
                }
            }

            //wxPrintf(wxString::Format(wxT("%lu, %lu, %lu"), (unsigned long)currentChapterCount, (unsigned long)availableChapterCount, (unsigned long)checkedChapterCount));

            if(/*currentChapterCount > 0 && */currentChapterCount < availableChapterCount-checkedChapterCount)
            {
                if(wxMessageBox(wxT("Add missing/new chapters to job list?"), wxT("Loading Chapters"), wxYES_NO) == wxYES)
                {
                    addNewChaptersToJoblist = true;
                }
            }
        }

        ListCtrlChapters->Freeze();
        for(long i=0; i<(long)CurrentChapterList.GetCount(); i++)
        {
            if(wxDirExists(baseDirectory.GetPathWithSep() + CurrentChapterList[i]->SafeLabel) && !wxDir::FindFirst(baseDirectory.GetPathWithSep() + CurrentChapterList[i]->SafeLabel, wxEmptyString, wxDIR_FILES).IsEmpty())
            {
                ListCtrlChapters->SetItemTextColour(i, wxColor(0, 150, 250));
            }
            else if(wxFileExists(baseDirectory.GetPathWithSep() + CurrentChapterList[i]->SafeLabel + wxT(".cbz")))
            {
                ListCtrlChapters->SetItemTextColour(i, wxColor(0, 150, 250));
            }
            else if(wxFileExists(baseDirectory.GetPathWithSep() + CurrentChapterList[i]->SafeLabel))
            {
                ListCtrlChapters->SetItemTextColour(i, wxColor(0, 150, 250));
            }
            else
            {
                // reset item specific color to listctrl default background
                ListCtrlChapters->SetItemTextColour(i, ListCtrlChapters->GetTextColour());
                // add chapter to joblist, if user selected this capability
                if(addNewChaptersToJoblist)
                {
                    SetChapterCheckedState(i, true, true);
                }
            }
        }
        ListCtrlChapters->Thaw();
    }
}

void MangaDownloaderFrame::OnButtonBrowseClick(wxCommandEvent& event)
{
    if(DirDialogBrowse->ShowModal() == wxID_OK)
    {
        wxBeginBusyCursor();
        StatusBar->SetStatusText(wxT("Changing base directory..."));

        TextCtrlDirectoryBase->SetValue(DirDialogBrowse->GetPath());
/*
        unsigned int chapterTopIndex = ListCtrlChapters->GetTopItem();
        unsigned int mangaTopIndex = ListCtrlMangas->GetTopItem();
        unsigned int mangaSelectedIndex = ListCtrlMangas->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

        LoadMangaList(ComboBoxSearchPattern->GetValue());

        ListCtrlMangas->SetItemState(mangaSelectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        ListCtrlMangas->EnsureVisible(mangaTopIndex + ListCtrlMangas->GetCountPerPage()-1);
        ListCtrlChapters->EnsureVisible(chapterTopIndex + ListCtrlChapters->GetCountPerPage()-1);
*/
        ColorifyMangaList();
        ColorifyChapterList();

        StatusBar->SetStatusText(wxEmptyString);
        wxEndBusyCursor();
    }
}

void MangaDownloaderFrame::OnChoiceSourceSelect(wxCommandEvent& event)
{
    wxBeginBusyCursor();
    StatusBar->SetStatusText(wxT("Loading manga list..."));

    wxYield();
    LoadMangaList(ComboBoxSearchPattern->GetValue());

    StatusBar->SetStatusText(wxEmptyString);
    wxEndBusyCursor();
}

void MangaDownloaderFrame::OnButtonUpdateClick(wxCommandEvent& event)
{
    if(wxMessageBox(wxT("This operation may take 1~5 minutes!\n\nUpdate now?"), wxT("Hint"), wxYES_NO) == wxYES)
    {
        wxBeginBusyCursor();
        StatusBar->SetStatusText(wxT("Synchronizing local manga lists with internet..."));
        wxLongLong start = wxGetLocalTimeMillis();

        DisableControls();
        MCC->UpdateMangaLists();
        LoadMangaList(ComboBoxSearchPattern->GetValue());
        EnableControls(true);

        wxLongLong time = (wxGetLocalTimeMillis() - start)/1000;
        StatusBar->SetStatusText(wxT("Manga lists updated (") + time.ToString() + wxT("sec)"));
        wxEndBusyCursor();

        wxMessageBox(wxT("Local manga lists have been updated."));
    }
}

void MangaDownloaderFrame::UpdateBookmarkButton()
{
    wxString pattern = ComboBoxSearchPattern->GetValue();

    int index = ComboBoxSearchPattern->FindString(pattern);

    if(index > -1)
    {
        BitmapButtonBookmark->SetBitmapLabel(ResourceImages[BTN_BOOKDEL_NORM]);
        BitmapButtonBookmark->SetBitmapHover(ResourceImages[BTN_BOOKDEL_HOVER]);
        BitmapButtonBookmark->SetBitmapDisabled(ResourceImages[BTN_BOOKDEL_DISABLE]);
        BitmapButtonBookmark->SetToolTip(wxT("Remove entry from search list"));
    }
    else
    {
        BitmapButtonBookmark->SetBitmapLabel(ResourceImages[BTN_BOOK_NORM]);
        BitmapButtonBookmark->SetBitmapHover(ResourceImages[BTN_BOOK_HOVER]);
        BitmapButtonBookmark->SetBitmapDisabled(ResourceImages[BTN_BOOK_DISABLE]);
        BitmapButtonBookmark->SetToolTip(wxT("Add entry to search list\nHold right mouse button + left click\nto bookmark without connector"));
    }
}

void MangaDownloaderFrame::OnBitmapButtonBookmarkClick(wxCommandEvent& event)
{
    wxString pattern = ComboBoxSearchPattern->GetValue();
    // Do not allow bookmarking of blank entry
    if(!pattern.IsEmpty())
    {
        int index = ComboBoxSearchPattern->FindString(pattern);

        if(index > -1)
        {
            // remove bookmark
            ComboBoxSearchPattern->Delete(index);
            // remove the connector part from the pattern
            if(pattern.EndsWith(wxT(">")) && (int)pattern.rfind(wxT(" <")) > -1)
            {
                ComboBoxSearchPattern->SetValue(pattern.BeforeLast(' '));
            }
        }
        else
        {
            if(!wxGetMouseState().ControlDown())
            {
                if(pattern.EndsWith(wxT(">")) && (int)pattern.rfind(wxT(" <")) > -1)
                {
                    // replace the connector from the pattern with the connector from the combobox
                    pattern = pattern.BeforeLast('<');
                }
                else
                {
                    // append the connector from the combobox to the pattern
                    pattern += wxT(" ");
                }
                pattern += wxT("<") + ComboBoxSource->GetValue() + wxT(">");
                ComboBoxSearchPattern->SetValue(pattern);
                index = ComboBoxSearchPattern->FindString(pattern);
            }

            // re-check if modified pattern not already in list (prevent duplicates)
            if(index < 0)
            {
                // add bookmark
                ComboBoxSearchPattern->Append(pattern);

                // NOTE: wxGTK bug: entries not sorted automatically
                //{
                    wxArrayString bookmarks = ComboBoxSearchPattern->GetStrings();
                    bookmarks.Sort(CompareStringCaseInsensitive);
                    ComboBoxSearchPattern->Clear();
                    ComboBoxSearchPattern->Append(bookmarks);
                    ComboBoxSearchPattern->SetValue(pattern);
                //}

                GetSizer()->SetSizeHints(this); // fit window minwidth, in case combobox->minwidth increased
                //Center();
            }
        }

        UpdateBookmarkButton();
    }
}

void MangaDownloaderFrame::OnSearchPattern(wxCommandEvent& event)
{
    wxString pattern = ComboBoxSearchPattern->GetValue();

    UpdateBookmarkButton();

    wxEventType evtType = event.GetEventType();
    // prevent double search: don't perform search on EVT_COMBOBOBOX, when liveSearch is true, because EVT_COMBOBOBOX also triggers EVT_TEXT which will process search
    if( (evtType == wxEVT_COMMAND_TEXT_ENTER) || (TypingSearch && evtType == wxEVT_COMMAND_TEXT_UPDATED) || (!TypingSearch && evtType == wxEVT_COMMAND_COMBOBOX_SELECTED) )
    {
        wxBeginBusyCursor();
        StatusBar->SetStatusText(wxT("Searching in manga list..."));

        if(pattern.EndsWith(wxT(">")) && (int)pattern.rfind(wxT(" <")) > -1)
        {
            // don't trigger event
            ComboBoxSource->SetSelection(ComboBoxSource->FindString(pattern.AfterLast('<').BeforeLast('>')));
            if(ComboBoxSource->GetSelection() < 0)
            {
                ComboBoxSource->SetValue(wxEmptyString);
            }
        }

        LoadMangaList(pattern);

        StatusBar->SetStatusText(wxEmptyString);
        wxEndBusyCursor();
    }
}

void MangaDownloaderFrame::OnListCtrlMangasItemSelect(wxListEvent& event)
{
    wxBeginBusyCursor();
    StatusBar->SetStatusText(wxT("Loading chapters from website..."));
    wxLongLong start = wxGetLocalTimeMillis();
/*
    wxEventType evtType = event.GetEventType();

    if(evtType == wxEVT_COMMAND_LIST_ITEM_SELECTED)
    {
        wxMessageBox(wxT("SELECT"));
    }

    if(evtType == wxEVT_COMMAND_LIST_ITEM_DESELECTED)
    {
        wxMessageBox(wxT("DESELECT"));
    }
*/
    DisableListCtrl(ListCtrlMangas);
    //Disconnect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&MangaDownloaderFrame::OnListCtrlMangasItemSelect);

    wxString pattern = wxEmptyString;
    if(wxGetMouseState().ControlDown())
    {
        wxTextEntryDialog win(NULL, wxT("Please enter a search pattern to filter the chapter list.\n\ni.e. '043', 'english', 'scanlator-group', ..."), wxT("Chapter Filter"));
        win.SetValue(ChapterPattern);
        if(win.ShowModal() == wxID_OK&& !win.GetValue().IsEmpty())
        {
            pattern = win.GetValue();
            ChapterPattern = pattern;
        }
    }
    LoadChapterList(pattern);

    //Connect(ID_LISTCTRL1,wxEVT_COMMAND_LIST_ITEM_SELECTED,(wxObjectEventFunction)&MangaDownloaderFrame::OnListCtrlMangasItemSelect);
    EnableListCtrl(ListCtrlMangas);

    // NOTE: there seems to be problems with focus, so we force focus when item selected
    ListCtrlMangas->SetFocus();

    wxLongLong time = wxGetLocalTimeMillis() - start;
    StatusBar->SetStatusText(wxT("Chapters loaded (") + time.ToString() + wxT("ms)"));
    wxEndBusyCursor();
}

void MangaDownloaderFrame::OnListCtrlMangasItemDeselect(wxListEvent& event)
{
    CheckBoxChapters->SetValue(false);
    ListCtrlChapters->DeleteAllItems();
    CurrentChapterList.Clear();
}

void MangaDownloaderFrame::OnCheckBoxChaptersClick(wxCommandEvent& event)
{
    wxBeginBusyCursor();
    StatusBar->SetStatusText(wxT("Selecting chapters..."));

    ListCtrlChapters->Freeze();
    ListCtrlJobs->Freeze();
    for(long c=0; c<ListCtrlChapters->GetItemCount(); c++)
    {
        if(CheckBoxChapters->IsChecked() != GetChapterCheckedState(c))
        {
            if(CheckBoxChapters->IsChecked())
            {
                SetChapterCheckedState(c, true, true);
            }
            else
            {
                SetChapterCheckedState(c, false, true);
            }
        }
    }
    ListCtrlJobs->Thaw();
    ListCtrlChapters->Thaw();

    StatusBar->SetStatusText(wxString::Format(wxT("Selected Chapters: %lu"), (unsigned long)MCC->GetJobCount()), 2);

    StatusBar->SetStatusText(wxEmptyString);
    wxEndBusyCursor();
}

void MangaDownloaderFrame::OnListCtrlChaptersItemSelect(wxListEvent& event)
{
    int x = ListCtrlChapters->ScreenToClient(wxGetMousePosition()).x;
    long index = ListCtrlChapters->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    // deselect item to remove selection and make them selectable again
    ListCtrlChapters->SetItemState(index, 0, wxLIST_STATE_SELECTED);

    // mouse must be in checkbox area, themes can change position of the checkbox image so we use a safezone (+ 4px)
    if(x > 0 && x < 14+4)
    {
        if(GetChapterCheckedState(index))
        {
            SetChapterCheckedState(index, false, true);
        }
        else
        {
            SetChapterCheckedState(index, true, true);
        }

        bool AllChaptersChecked = true;
        for(long i=0; i<ListCtrlChapters->GetItemCount(); i++)
        {
            if(!GetChapterCheckedState(i))
            {
                AllChaptersChecked = false;
                break;
            }
        }
        CheckBoxChapters->SetValue(AllChaptersChecked);

        StatusBar->SetStatusText(wxString::Format(wxT("Selected Chapters: %lu"), (unsigned long)MCC->GetJobCount()), 2);
    }
}

bool MangaDownloaderFrame::GetChapterCheckedState(long ChapterIndex)
{
    wxListItem chapterItem;
    chapterItem.SetMask(wxLIST_MASK_IMAGE);
    chapterItem.SetId(ChapterIndex);

    ListCtrlChapters->GetItem(chapterItem);

    if(chapterItem.GetImage() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MangaDownloaderFrame::SetChapterCheckedState(long ChapterIndex, bool State, bool UpdateJobList)
{
    wxString connectorLabel = ComboBoxSource->GetValue();
    long mangaIndex = ListCtrlMangas->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    if(ComboBoxSource->GetSelection() > -1 && mangaIndex > -1 && ChapterIndex > -1)
    {
        if(State && !GetChapterCheckedState(ChapterIndex))
        {
            ListCtrlChapters->SetItemImage(ChapterIndex, 1);
            if(UpdateJobList)
            {
                MCJob job = MCC->CreateJob(connectorLabel, CurrentMangaList[mangaIndex], CurrentChapterList[ChapterIndex]);
                if(MCC->AddJob(job))
                {
                    long index = ListCtrlJobs->GetItemCount();
                    wxString jobID = wxString::Format(wxT("%lu"), job.ID);
                    ListCtrlJobs->InsertItem(index, jobID);
                    ListCtrlJobs->SetItem(index, 0, jobID);
                    ListCtrlJobs->SetItem(index, 1, job.Connector);
                    ListCtrlJobs->SetItem(index, 2, job.MangaLabel);
                    ListCtrlJobs->SetItem(index, 3, job.ChapterLabel);
                }
                else
                {
                    wxMessageBox(wxString::Format(wxT("A job with this ID already exist!\n\nJobID: %lu\nChapter: ") + job.ChapterLabel, job.ID));
                    // leave checkbox checked, maybe this job is the one who is already in job list...
                    //ListCtrlChapters->SetItemImage(ChapterIndex, 0);
                    //CheckBoxChapters->SetValue(false);
                }
            }
        }

        if(!State && GetChapterCheckedState(ChapterIndex))
        {
            ListCtrlChapters->SetItemImage(ChapterIndex, 0);
            if(UpdateJobList)
            {
                unsigned long job = MCC->GenerateJobID(CurrentChapterList[ChapterIndex]);
                MCC->RemoveJob(job);

                wxString jobID = wxString::Format(wxT("%lu"), job);
                // it works correct even if find returns -1
                ListCtrlJobs->DeleteItem(ListCtrlJobs->FindItem(-1, jobID));
            }
        }
    }
}

void MangaDownloaderFrame::OnButtonDownloadClick(wxCommandEvent& event)
{
    //wxBeginBusyCursor(wxHOURGLASS_CURSOR);
    StatusBar->SetStatusText(wxEmptyString);
    wxFileName baseDirectory(TextCtrlDirectoryBase->GetValue(), wxEmptyString);

    AbortDownload = false;

    if(baseDirectory.DirExists())
    {
        DisableControls(true);

        wxArrayString errorLog = MCC->DownloadJobs(baseDirectory, StatusBar, &AbortDownload, CompressChapters);
/*
        for(unsigned int i=0; i<errorLog.Count(); i++)
        {
            Log(wxT("MangaDownloaderFrame::OnButtonDownloadClick() -> ERROR downloading (SOURCE|TARGET): ") + errorLog[i]);
        }
*/
        if(errorLog.Count() > 0)
        {
            ErrorLogFrame* LogFrame = new ErrorLogFrame(0);
            LogFrame->SetLog(errorLog);
            LogFrame->Show();
        }

        wxArrayJobID completed = MCC->GetCompletedJobIDs();
        if(DeleteCompletedJobs && completed.GetCount() > 0 && wxMessageBox(wxT("Remove completed jobs from download list?"), wxT("Operation Complete"), wxYES_NO) == wxYES)
        {
            // remove jobs from joblist
            ListCtrlJobs->Freeze();
            for(size_t i=0; i<completed.GetCount(); i++)
            {
                MCC->RemoveJob(completed[i]);
                wxString jobID = wxString::Format(wxT("%lu"), completed[i]);
                // it works correct even if find returns -1
                ListCtrlJobs->DeleteItem(ListCtrlJobs->FindItem(-1, jobID));
            }
            ListCtrlJobs->Thaw();

            // update checkboxes for current chapters
            bool AllChaptersChecked = true;
            ListCtrlChapters->Freeze();
            for(long i=0; i<(long)CurrentChapterList.GetCount(); i++)
            {
                if(GetChapterCheckedState(i) && !MCC->ContainsJob(MCC->GenerateJobID(CurrentChapterList[i])))
                {
                    SetChapterCheckedState(i, false, false);
                    if(AllChaptersChecked)
                    {
                        AllChaptersChecked = false;
                    }
                }
            }
            ListCtrlChapters->Thaw();

            if(!AllChaptersChecked)
            {
                CheckBoxChapters->SetValue(false);
            }

            // update job count in statusbar
            StatusBar->SetStatusText(wxString::Format(wxT("Selected Chapters: %lu"), (unsigned long)MCC->GetJobCount()), 2);
        }
        else
        {
            // mark all completed jobs as not completed
            for(size_t i=0; i<completed.GetCount(); i++)
            {
                MCC->SetJobDownloadCompleted(completed[i], false);
            }
        }

        StatusBar->SetStatusText(wxEmptyString, 1);
        EnableControls(true);
    }
    else
    {
        wxMessageBox(wxT("Base directory not found!"));
    }

    ColorifyMangaList();
    ColorifyChapterList();

    StatusBar->SetStatusText(wxEmptyString);
    //wxEndBusyCursor();
}

void MangaDownloaderFrame::OnButtonAbortClick(wxCommandEvent& event)
{
    AbortDownload = true;
}

void MangaDownloaderFrame::EnableControls(bool DisableAbortButton)
{
    ComboBoxSource->Enable();
    BitmapButtonUpdate->Enable();
    ComboBoxSearchPattern->Enable();
    BitmapButtonBookmark->Enable();
    EnableListCtrl(ListCtrlMangas);
    CheckBoxChapters->Enable();
    EnableListCtrl(ListCtrlChapters);
    BitmapButtonBrowse->Enable();
    CheckBoxShowJobList->Enable();
    EnableListCtrl(ListCtrlJobs);
    BitmapButtonDownload->Enable();
    if(DisableAbortButton)
    {
        BitmapButtonAbort->Disable();
    }
    else
    {
        BitmapButtonAbort->Enable();
    }
}

void MangaDownloaderFrame::EnableListCtrl(wxListCtrl* ListCtrl)
{
    ListCtrl->Freeze();
    long visibleTopItem = ListCtrl->GetTopItem();
    long visableItemCount = wxMin(ListCtrl->GetCountPerPage()+1, ListCtrl->GetItemCount()-visibleTopItem);
    for(long i=visibleTopItem; i<visibleTopItem+visableItemCount; i++)
    {
        ListCtrl->SetItemBackgroundColour(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    }
    ListCtrl->Thaw();
    ListCtrl->Enable();
}

void MangaDownloaderFrame::DisableControls(bool EnableAbortButton)
{
    ComboBoxSource->Disable();
    BitmapButtonUpdate->Disable();
    ComboBoxSearchPattern->Disable();
    BitmapButtonBookmark->Disable();
    DisableListCtrl(ListCtrlMangas);
    CheckBoxChapters->Disable();
    DisableListCtrl(ListCtrlChapters);
    BitmapButtonBrowse->Disable();
    CheckBoxShowJobList->Disable();
    DisableListCtrl(ListCtrlJobs);
    BitmapButtonDownload->Disable();
    if(EnableAbortButton)
    {
        BitmapButtonAbort->Enable();
    }
    else
    {
        BitmapButtonAbort->Disable();
    }
}

void MangaDownloaderFrame::DisableListCtrl(wxListCtrl* ListCtrl)
{
    ListCtrl->Disable();
    ListCtrl->Freeze();
    long visibleTopItem = ListCtrl->GetTopItem();
    long visableItemCount = wxMin(ListCtrl->GetCountPerPage()+1, ListCtrl->GetItemCount()-visibleTopItem);
    for(long i=visibleTopItem; i<visibleTopItem+visableItemCount; i++)
    {
        ListCtrl->SetItemBackgroundColour(i, wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
    }
    ListCtrl->Thaw();
}

void MangaDownloaderFrame::OnCheckBoxShowJobListClick(wxCommandEvent& event)
{
    if(CheckBoxShowJobList->GetValue())
    {
        ListCtrlJobs->Show();
    }
    else
    {
        ListCtrlJobs->Hide();
    }
    // force all children to update their sizes
    Layout();
}

void MangaDownloaderFrame::OnListCtrlJobsItemActivated(wxListEvent& event)
{
    long i = event.GetIndex();
    if(i > -1)
    {
        // get connector name
        wxString connector;
        wxString manga;
        wxString chapter;
        wxListItem jobItem;
        jobItem.SetId(i);
        jobItem.SetMask(wxLIST_MASK_TEXT);

        jobItem.SetColumn(1);
        ListCtrlJobs->GetItem(jobItem);
        connector = jobItem.GetText();

        jobItem.SetColumn(2);
        ListCtrlJobs->GetItem(jobItem);
        manga = jobItem.GetText();

        jobItem.SetColumn(3);
        ListCtrlJobs->GetItem(jobItem);
        chapter = jobItem.GetText();

        // check if the connector is selected
        if(!connector.IsSameAs(ComboBoxSource->GetValue()))
        {
            // SetValue() don't trigger event and don't set SelectedIndex
            //ComboBoxSource->SetValue(connector);
            ComboBoxSource->SetSelection(ComboBoxSource->FindString(connector));
            LoadMangaList(ComboBoxSearchPattern->GetValue());
        }

        // check if the manga is selected
        long mangaIndex = ListCtrlMangas->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if(mangaIndex < 0 || !manga.IsSameAs(CurrentMangaList[mangaIndex]->Label))
        {
            // check if the manga is in the current filtered manga list
            wxString pattern = ComboBoxSearchPattern->GetValue().Lower();
            if(manga.Lower().Find(pattern) < 0)
            {
                // clear search / reload mangalist
                ComboBoxSearchPattern->SetValue(wxEmptyString);
                LoadMangaList();
            }

            // find correct manga index
            for(long m=0; m<(long)CurrentMangaList.GetCount(); m++)
            {
                if(manga.IsSameAs(CurrentMangaList[m]->Label))
                {
                    // triggers chapter loading automatically
                    mangaIndex = m;
                    ListCtrlMangas->SetItemState(m, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                    break;
                }
            }
        }

        ListCtrlMangas->EnsureVisible(mangaIndex);

        long chapterIndex = -1;
        for(size_t c=0; c<CurrentChapterList.GetCount(); c++)
        {
            if(chapter.IsSameAs(CurrentChapterList[c]->Label))
            {
                chapterIndex = c;
                break;
            }
        }

        ListCtrlChapters->EnsureVisible(chapterIndex);
    }
}

void MangaDownloaderFrame::OnMenuJobContextClick(wxCommandEvent& event)
{
    wxBeginBusyCursor();
    StatusBar->SetStatusText(wxT("Removing jobs..."));

    ListCtrlJobs->Freeze();
    ListCtrlChapters->Freeze();

    if(event.GetId() == ID_JOBMENUITEM_REMOVEALL)
    {
        MCC->RemoveAllJobs();
        ListCtrlJobs->DeleteAllItems();
        CheckBoxChapters->SetValue(false);
        // instead of reloading chapters we set all chapter checked state to false
        for(long c=0; c<ListCtrlChapters->GetItemCount(); c++)
        {
            SetChapterCheckedState(c, false, false);
        }
    }

    if(event.GetId() == ID_JOBMENUITEM_REMOVESELECTED)
    {
        // remove jobs from joblist & MCC
        wxListItem jobItem;
        unsigned long jobID;
        long i = ListCtrlJobs->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        while(i > -1)
        {
            // get job id
            jobItem.SetId(i);
            jobItem.SetMask(wxLIST_MASK_TEXT);
            jobItem.SetColumn(0);
            ListCtrlJobs->GetItem(jobItem);
            jobItem.GetText().ToULong(&jobID);
            MCC->RemoveJob(jobID);
            ListCtrlJobs->DeleteItem(i);
            i--;
            i = ListCtrlJobs->GetNextItem(i, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        }

        // update selected state with respect to removed jobs
        bool AllChaptersChecked = CheckBoxChapters->IsChecked();
        for(long i=0; i<(long)CurrentChapterList.GetCount(); i++)
        {
            if(GetChapterCheckedState(i) && !MCC->ContainsJob(MCC->GenerateJobID(CurrentChapterList[i])))
            {
                SetChapterCheckedState(i, false, false);
                if(AllChaptersChecked)
                {
                    CheckBoxChapters->SetValue(false);
                    AllChaptersChecked = false;
                }
            }
        }
    }

    ListCtrlChapters->Thaw();
    ListCtrlJobs->Thaw();

    StatusBar->SetStatusText(wxString::Format(wxT("Selected Chapters: %lu"), (unsigned long)MCC->GetJobCount()), 2);
    StatusBar->SetStatusText(wxEmptyString);
    wxEndBusyCursor();
}

void MangaDownloaderFrame::OnListCtrlJobsItemRClick(wxListEvent& event)
{
    ListCtrlJobs->PopupMenu(&MenuJobContext);
}

void MangaDownloaderFrame::OnResize(wxSizeEvent& event)
{
    Layout();

    // prevent flickering when changing column size...
    ListCtrlMangas->Freeze();
    ListCtrlChapters->Freeze();
    ListCtrlJobs->Freeze();

    // always assume scrollbar is visible, never use GetClientSize
    ListCtrlMangas->SetColumnWidth(0, ListCtrlMangas->GetSize().GetWidth()-24);
    ListCtrlChapters->SetColumnWidth(0, ListCtrlChapters->GetSize().GetWidth()-24);

    // hidden controls don't resize, so Control->GetSize() will be wrong...
    // use the width of the parent sizer (assuming control expands)
    // subtract 2x5px border and 24px safe zone for vscroll
    int jobsWidth = FlexGridSizerLists->GetSize().GetWidth()-34 - ListCtrlJobs->GetColumnWidth(0) - ListCtrlJobs->GetColumnWidth(1);
    ListCtrlJobs->SetColumnWidth(2, jobsWidth/2);
    ListCtrlJobs->SetColumnWidth(3, jobsWidth/2);

    ListCtrlJobs->Thaw();
    ListCtrlChapters->Thaw();
    ListCtrlMangas->Thaw();
}

void MangaDownloaderFrame::OnClose(wxCloseEvent& event)
{
    SaveConfiguration();
    // for dialogs we can't use Close()
    Destroy();
}

void MangaDownloaderFrame::OnMenuMainClick(wxCommandEvent& event)
{
    if(event.GetId() == ID_MenuAuth)
    {
        wxString user= wxGetTextFromUser(wxT("Please enter your Batoto username"), wxT("Batoto Login"), Username);
        if(user != wxEmptyString) {
            Username = user;
        }
        wxString pass = wxGetPasswordFromUser(wxT("Please enter your Batoto password"), wxT("Batoto Login"), Password);
        if(pass != wxEmptyString) {
            Password = pass;
        }
    }

    if(event.GetId() == ID_MenuStartUpSync)
    {
        StartupSync = event.IsChecked();
    }

    if(event.GetId() == ID_MenuTypingSearch)
    {
        TypingSearch = event.IsChecked();
    }

    if(event.GetId() == ID_MenuChapterAutoSelect)
    {
        ChapterAutoSelect = event.IsChecked();
    }

    if(event.GetId() == ID_MenuNewChapterNotification)
    {
        NewChapterNotification = event.IsChecked();
    }

    if(event.GetId() == ID_MenuCompressChapters)
    {
        CompressChapters = event.IsChecked();
    }

    if(event.GetId() == ID_MenuDeleteCompletedJobs)
    {
        DeleteCompletedJobs = event.IsChecked();
    }

    if(event.GetId() == ID_MenuHelp)
    {
        wxLaunchDefaultBrowser(wxT("http://sourceforge.net/p/hakuneko/doc/"));
    }

    if(event.GetId() == ID_MenuAbout)
    {
        wxAboutDialogInfo about;
        about.SetName(wxT("HakuNeko"));
        about.SetVersion(VERSION);
        about.SetDescription(wxT("A manga downloader for Linux & Windows."));
        about.SetWebSite(wxT("http://sourceforge.net/projects/hakuneko/"));
        about.SetCopyright(COPYRIGHT);
        about.AddDeveloper(wxT("Ronny Wegener"));
        about.AddDeveloper(wxT("mytenmo"));
        about.AddDeveloper(wxT("Neogeek"));
        wxAboutBox(about);
    }
}

void MangaDownloaderFrame::OnMainWindowRClick(wxMouseEvent& event)
{
    this->PopupMenu(MenuMain);
}

// #################################
// ### FUNCTIONS FOR SELFTESTING ###
// #################################

long MangaDownloaderFrame::Random(long Min, long Max)
{
    if(Min > Max)
    {
        long temp = Min;
        Min = Max;
        Max = temp;
    }

    // prevent division by zero
    if(Min == Max)
    {
        return Min;
    }
    else
    {
        long rd = (wxGetLocalTimeMillis() % (Max-Min) + Min).ToLong();

        if(rd < Min)
        {
            return Min;
        }

        if(rd > Max)
        {
            return Max;
        }

        return rd;
    }
}

void MangaDownloaderFrame::ErrorDetectionTest()
{
    wxString bkp_base_dir = TextCtrlDirectoryBase->GetValue();
    wxString bkp_search_pattern = ComboBoxSearchPattern->GetValue();

    wxTextFile dbg_log_file;
    // create file, or open if already exists
    if(!dbg_log_file.Create(ConfigurationFile.GetPathWithSep() + wxT("selftest.log")))
    {
        dbg_log_file.Open(ConfigurationFile.GetPathWithSep() + wxT("selftest.log"));
        dbg_log_file.Clear();
    }

    TextCtrlDirectoryBase->SetValue(ConfigurationFile.GetPathWithSep() + wxT("temp"));
    ComboBoxSearchPattern->SetValue(wxEmptyString);

    if(!wxDir::Exists(TextCtrlDirectoryBase->GetValue()))
    {
        wxMkdir(TextCtrlDirectoryBase->GetValue());
    }

    wxCommandEvent ce;
    MCEntry *dbg_mc_entry;
    long dbg_mangalist_length;
    long dbg_manga_index;
    long dbg_chapterlist_length;
    long dbg_chapter_index;
    long dbg_joblist_length;
    long dbg_job_index;
    long mode;
    wxListItem dbg_job_item;
    unsigned long dbg_job_id;

    // STAGE 1:
    dbg_log_file.AddLine(wxEmptyString);
    dbg_log_file.AddLine(wxT("STAGE 1"));
    dbg_log_file.AddLine(wxEmptyString);
    // update & verify: manga/chapters

        // emulate update button click
        OnButtonUpdateClick(ce);

        // verify each connector->manga->chapter (link & label)
        for(size_t i=0; i<ComboBoxSource->GetCount(); i++)
        {
            // emulate sequential connector selection
            ComboBoxSource->SetSelection(i);
            OnChoiceSourceSelect(ce);

            wxYield();

            dbg_mangalist_length = CurrentMangaList.GetCount();
            dbg_log_file.AddLine(wxT("Selected Connector: ") + ComboBoxSource->GetValue());
            dbg_log_file.AddLine(wxString::Format(wxT("Manga Count: %ld"), dbg_mangalist_length));
            if(dbg_mangalist_length > 0)
            {
                dbg_log_file.AddLine(wxT("First Manga: ") + CurrentMangaList[0]->Label);
                dbg_log_file.AddLine(wxT("Last Manga: ") + CurrentMangaList[dbg_mangalist_length-1]->Label);

                // analyze each manga entry (i.e. link starts exactly with http://)
                for(long m=0; m<dbg_mangalist_length; m++)
                {
                    dbg_mc_entry = CurrentMangaList[m];
                    if(dbg_mc_entry->Label.IsSameAs(wxEmptyString))
                    {
                        dbg_log_file.AddLine(wxT("SUSPICIOUS Manga Label NULL: ") + dbg_mc_entry->Label);
                    }
                    if(dbg_mc_entry->Label.Len() > 128)
                    {
                        dbg_log_file.AddLine(wxT("SUSPICIOUS Manga Label Size: ") + dbg_mc_entry->Label);
                    }
                    if(dbg_mc_entry->Label.Find(wxT("<")) > -1 || dbg_mc_entry->Label.Find(wxT(">")) > -1)
                    {
                        dbg_log_file.AddLine(wxT("SUSPICIOUS Manga Label Entities(<>): ") + dbg_mc_entry->Label);
                    }
                    if(dbg_mc_entry->Link.IsSameAs(wxEmptyString))
                    {
                        dbg_log_file.AddLine(wxT("SUSPICIOUS Manga Link NULL: ") + dbg_mc_entry->Link);
                    }
                    if(dbg_mc_entry->Link.Len() > 128)
                    {
                        dbg_log_file.AddLine(wxT("SUSPICIOUS Manga Link Size: ") + dbg_mc_entry->Link);
                    }
                    if(dbg_mc_entry->Link.rfind(wxT("http://")) != 0)
                    {
                        dbg_log_file.AddLine(wxT("SUSPICIOUS Manga Link http://: ") + dbg_mc_entry->Link);
                    }
                    if(dbg_mc_entry->Link.Find(wxT("<")) > -1 || dbg_mc_entry->Link.Find(wxT(">")) > -1)
                    {
                        dbg_log_file.AddLine(wxT("SUSPICIOUS Manga Link Entities(<>): ") + dbg_mc_entry->Link);
                    }
                    //wxDELETE(dbg_mc_entry);
                }
            }

            // emulate manga selection (10)

            // todo check chapters from all mangas instead of 10 random mangas (~8 hours runtime)
            // for(long m=0; m<dbg_mangalist_length; m++)
            for(long m=0; m<10; m++)
            {
                //dbg_manga_index = m;
                dbg_manga_index = Random(0, dbg_mangalist_length-1);
                ListCtrlMangas->SetItemState(dbg_manga_index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

                wxYield();

                dbg_chapterlist_length = CurrentChapterList.GetCount();
                if(dbg_chapterlist_length > 0)
                {
                    // analyze each chapter entry
                    for(long c=0; c<dbg_chapterlist_length; c++)
                    {
                        dbg_mc_entry = CurrentChapterList[c];
                        if(dbg_mc_entry->Label.IsSameAs(wxEmptyString))
                        {
                            dbg_log_file.AddLine(wxT("SUSPICIOUS Chapter Label NULL: ") + dbg_mc_entry->Label);
                        }
                        if(dbg_mc_entry->Label.Len() > 128)
                        {
                            dbg_log_file.AddLine(wxT("SUSPICIOUS Chapter Label Size: ") + dbg_mc_entry->Label);
                        }
                        if(dbg_mc_entry->Label.Find(wxT("<")) > -1 || dbg_mc_entry->Label.Find(wxT(">")) > -1)
                        {
                            dbg_log_file.AddLine(wxT("SUSPICIOUS Chapter Label Entities(<>): ") + dbg_mc_entry->Label);
                        }
                        if(dbg_mc_entry->Link.IsSameAs(wxEmptyString))
                        {
                            dbg_log_file.AddLine(wxT("SUSPICIOUS Chapter Link NULL: ") + dbg_mc_entry->Link);
                        }
                        if(dbg_mc_entry->Link.Len() > 128)
                        {
                            dbg_log_file.AddLine(wxT("SUSPICIOUS Chapter Link Size: ") + dbg_mc_entry->Link);
                        }
                        if(dbg_mc_entry->Link.rfind(wxT("http://")) != 0)
                        {
                            dbg_log_file.AddLine(wxT("SUSPICIOUS Chapter Link http://: ") + dbg_mc_entry->Link);
                        }
                        if(dbg_mc_entry->Link.Find(wxT("<")) > -1 || dbg_mc_entry->Link.Find(wxT(">")) > -1)
                        {
                            dbg_log_file.AddLine(wxT("SUSPICIOUS Chapter Link Entities(<>): ") + dbg_mc_entry->Link);
                        }
                        //wxDELETE(dbg_mc_entry);
                    }
                }
            }
        }

    // STAGE 2:
    dbg_log_file.AddLine(wxEmptyString);
    dbg_log_file.AddLine(wxT("STAGE 2"));
    dbg_log_file.AddLine(wxEmptyString);
    // emulate job enable/disable events

        for(int j=0; j<50; j++)
        {
            mode = Random(0, 19);

            dbg_joblist_length = ListCtrlJobs->GetItemCount();

            // 5% probability to clear joblist
            if(mode < 1 && dbg_joblist_length > 0)
            {
                // remove all jobs from joblist
                ce.SetId(ID_JOBMENUITEM_REMOVEALL);
                OnMenuJobContextClick(ce);

                dbg_log_file.AddLine(wxT("CLEAR"));
            }

            // 25% probability to remove job
            if(mode >= 1 && mode < 6)
            {
                if(dbg_joblist_length > 0)
                {
                    // remove 1 chapters from joblist
                    dbg_job_index = Random(0, dbg_joblist_length-1);
                    ListCtrlJobs->SetItemState(dbg_job_index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

                    wxString log_job = wxT("--Job: ");

                    dbg_job_item.SetId(dbg_job_index);
                    dbg_job_item.SetMask(wxLIST_MASK_TEXT);

                    dbg_job_item.SetColumn(1);
                    ListCtrlJobs->GetItem(dbg_job_item);
                    log_job += dbg_job_item.GetText() + wxT(" | ");

                    dbg_job_item.SetColumn(2);
                    ListCtrlJobs->GetItem(dbg_job_item);
                    log_job += dbg_job_item.GetText() + wxT(" | ");

                    dbg_job_item.SetColumn(3);
                    ListCtrlJobs->GetItem(dbg_job_item);
                    log_job += dbg_job_item.GetText();

                    ce.SetId(ID_JOBMENUITEM_REMOVESELECTED);
                    OnMenuJobContextClick(ce);

                    dbg_log_file.AddLine(log_job);
                }
                else
                {
                    dbg_log_file.AddLine(wxT("--Job: Empty List"));
                }
            }

            // 70% probability to add job
            if(mode >= 6)
            {
                // select random connector (except virtual manga)
                ComboBoxSource->SetSelection(Random(0, ComboBoxSource->GetCount()-2));
                OnChoiceSourceSelect(ce);
                wxYield();

                // select random manga whith at least 1 chapter
                do
                {
                    dbg_manga_index = Random(0, CurrentMangaList.GetCount()-1);
                    ListCtrlMangas->SetItemState(dbg_manga_index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                    //wxYield();
                }
                while(CurrentChapterList.GetCount() < 1);

                // add 1 chapter from selected manga to joblist
                dbg_chapter_index = Random(0, CurrentChapterList.GetCount()-1);
                SetChapterCheckedState(dbg_chapter_index, true, true);
                //wxYield();

                dbg_log_file.AddLine(wxT("++Job: ") + ComboBoxSource->GetValue() + wxT(" | ") + CurrentMangaList[dbg_manga_index]->Label + wxT(" | ") + CurrentChapterList[dbg_chapter_index]->Label);
            }

            dbg_joblist_length = ListCtrlJobs->GetItemCount();

            // test if jobs in joblist are equal to those in MCC
            if((long)MCC->GetJobCount() > dbg_joblist_length)
            {
                dbg_log_file.AddLine(wxT("ERROR: MCCJobCount > JobListCount"));
            }
            if((long)MCC->GetJobCount() < dbg_joblist_length)
            {
                dbg_log_file.AddLine(wxT("ERROR: MCCJobCount < JobListCount"));
            }
            for(long j=0; j<dbg_joblist_length; j++)
            {
                // get job id
                dbg_job_item.SetId(j);
                dbg_job_item.SetMask(wxLIST_MASK_TEXT);
                dbg_job_item.SetColumn(0);
                ListCtrlJobs->GetItem(dbg_job_item);
                dbg_job_item.GetText().ToULong(&dbg_job_id);

                if(!MCC->ContainsJob(dbg_job_id))
                {
                    dbg_log_file.AddLine(wxT("ERROR: Missing MCC Job ") + dbg_job_item.GetText());
                }
            }
            wxYield();
        }

        // clear joblist
        ce.SetId(ID_JOBMENUITEM_REMOVEALL);
        OnMenuJobContextClick(ce);

    // STAGE 3:
    dbg_log_file.AddLine(wxEmptyString);
    dbg_log_file.AddLine(wxT("STAGE 3"));
    dbg_log_file.AddLine(wxEmptyString);

        // emulate
        for(size_t i=0; i<ComboBoxSource->GetCount(); i++)
        {
            // emulate sequential connector selection
            ComboBoxSource->SetSelection(i);
            OnChoiceSourceSelect(ce);

            wxYield();

            long dbg_mangalist_length = CurrentMangaList.GetCount();
            if(dbg_mangalist_length > 0)
            {
                // emulate random manga selection
                long dbg_manga_index = Random(0, dbg_mangalist_length-1);
                ListCtrlMangas->EnsureVisible(dbg_manga_index);
                ListCtrlMangas->SetItemState(dbg_manga_index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

                wxYield();

                long dbg_chapterlist_length = CurrentChapterList.GetCount();
                if(dbg_chapterlist_length > 0)
                {
                    // emulate random chapter selection
                    long dbg_chapter_index = Random(0, dbg_chapterlist_length-1);
                    ListCtrlChapters->EnsureVisible(dbg_chapter_index);
                    SetChapterCheckedState(dbg_chapter_index, true, true);

                    wxYield();

                    dbg_log_file.AddLine(wxT("Selected: ") + ComboBoxSource->GetValue() + wxT(" | ") + CurrentMangaList[dbg_manga_index]->Label + wxT(" | ") + CurrentChapterList[dbg_chapter_index]->Label);
                }
                else
                {
                    dbg_log_file.AddLine(wxT("Selected: ") + ComboBoxSource->GetValue() + wxT(" | ") + CurrentMangaList[dbg_manga_index]->Label + wxT(" | NULL"));
                }
            }
        }

        // emulate download click
        OnButtonDownloadClick(ce);

    dbg_log_file.Write();
    dbg_log_file.Close();

    TextCtrlDirectoryBase->SetValue(bkp_base_dir);
    ComboBoxSearchPattern->SetValue(bkp_search_pattern);
}
