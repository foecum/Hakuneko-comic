#ifndef MANGADOWNLOADERMAIN_H
#define MANGADOWNLOADERMAIN_H

//(*Headers(MangaDownloaderFrame)
#include <wx/bmpbuttn.h>
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/dirdlg.h>
#include <wx/statusbr.h>
#include <wx/statline.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

//#include <wx/icon.h>
#include <wx/imaglist.h>
#include <wx/mstream.h>
#include <wx/timer.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/aboutdlg.h>
#include <wx/progdlg.h>
#include <wx/xml/xml.h>

#include "version.h"
#include "resource.h"
#include "MangaConnectorCollection.h"
//#include "MangaDownloaderConfiguration.h"
#include "ErrorLogFrame.h"

enum ResourceImage
{
    BTN_BROWSE_NORM,
    BTN_BROWSE_HOVER,
    BTN_BROWSE_DISABLE,

    BTN_SYNC_NORM,
    BTN_SYNC_HOVER,
    BTN_SYNC_DISABLE,

    BTN_BOOK_NORM,
    BTN_BOOK_HOVER,
    BTN_BOOK_DISABLE,

    BTN_BOOKDEL_NORM,
    BTN_BOOKDEL_HOVER,
    BTN_BOOKDEL_DISABLE,

    BTN_DOWN_NORM,
    BTN_DOWN_HOVER,
    BTN_DOWN_DISABLE,

    BTN_ABORT_NORM,
    BTN_ABORT_HOVER,
    BTN_ABORT_DISABLE,

    CHECK_ENABLED,
    CHECK_DISABLED,

    APP_ICO
};

class MangaDownloaderFrame: public wxFrame
{
    public:

        MangaDownloaderFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~MangaDownloaderFrame();

    private:

        //(*Handlers(MangaDownloaderFrame)
        void OnChoiceSourceSelect(wxCommandEvent& event);
        void OnButtonUpdateClick(wxCommandEvent& event);
        void OnCheckBoxChaptersClick(wxCommandEvent& event);
        void OnButtonBrowseClick(wxCommandEvent& event);
        void OnButtonDownloadClick(wxCommandEvent& event);
        void OnButtonAbortClick(wxCommandEvent& event);
        void OnListCtrlMangasItemSelect(wxListEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnSearchPattern(wxCommandEvent& event);
        void OnBitmapButtonBookmarkClick(wxCommandEvent& event);
        void OnListCtrlChaptersItemSelect(wxListEvent& event);
        void OnResize(wxSizeEvent& event);
        void OnCheckBoxShowJobListClick(wxCommandEvent& event);
        void OnListCtrlJobsItemActivated(wxListEvent& event);
        void OnListCtrlJobsItemRClick(wxListEvent& event);
        void OnListCtrlMangasItemDeselect(wxListEvent& event);
        //*)
        void OnMenuJobContextClick(wxCommandEvent& event);
        void OnMenuMainClick(wxCommandEvent& event);
        void OnMainWindowRClick(wxMouseEvent& event);

        // load a bitmap from generated resource into an enumerated bitmap array
        void LoadImageResource(RCDATA RCImage, wxBitmapType Type, ResourceImage ImageLabel);
        // load the resources from the generated resources.h(& .cpp)
        void LoadResources();
        // check if a new version of hakuneko is available
        void UpdateCheck();
        // initialize configuration file
        void InitConfigurationFile();
        // load and apply gui settings from configuration file
        void LoadConfiguration(bool NoGUI = false);
        // save gui settings to configuration file
        void SaveConfiguration();
        // change bookmark button depending on search value in bookmarks or not
        void UpdateBookmarkButton();
        // load manga list from the local file and filter by the given pattern
        void LoadMangaList(wxString Pattern = wxEmptyString);
        // change the color of all manga list items depending on their directory existence
        void ColorifyMangaList();
        // load the chapter list for the selected manga and filter by the given pattern
        void LoadChapterList(wxString Pattern = wxEmptyString);
        // change the color of all chapter list items depending on their directory existence
        void ColorifyChapterList();
        // get the checked state of a chapter in the chapter list
        bool GetChapterCheckedState(long ChapterIndex);
        // set the checked state of a chapter in the chapter list
        void SetChapterCheckedState(long ChapterIndex, bool State, bool UpdateJobList);
        void EnableControls(bool DisableAbortButton = false);
        void EnableListCtrl(wxListCtrl* ListCtrl);
        void DisableControls(bool EnableAbortButton = false);
        void DisableListCtrl(wxListCtrl* ListCtrl);

        // FUNCTIONS FOR SELFTEST
        long Random(long Min, long Max);
        public: void ErrorDetectionTest();

        //(*Identifiers(MangaDownloaderFrame)
        static const long ID_STATICLINE1;
        static const long ID_STATICTEXT5;
        static const long ID_TEXTCTRL2;
        static const long ID_BITMAPBUTTON3;
        static const long ID_STATICTEXT1;
        static const long ID_COMBOBOX2;
        static const long ID_BITMAPBUTTON1;
        static const long ID_STATICTEXT2;
        static const long ID_COMBOBOX1;
        static const long ID_BITMAPBUTTON2;
        static const long ID_STATICLINE2;
        static const long ID_STATICTEXT3;
        static const long ID_CHECKBOX1;
        static const long ID_LISTCTRL1;
        static const long ID_LISTCTRL2;
        static const long ID_CHECKBOX2;
        static const long ID_LISTCTRL3;
        static const long ID_STATICLINE3;
        static const long ID_BITMAPBUTTON6;
        static const long ID_BITMAPBUTTON5;
        static const long ID_STATUSBAR1;
        //*)
        static const long ID_JOBMENUITEM_REMOVEALL;
        static const long ID_JOBMENUITEM_REMOVESELECTED;
        static const long ID_MenuHelp;
        static const long ID_MenuAbout;
        static const long ID_MenuAuth;
        static const long ID_MenuStartUpSync;
        static const long ID_MenuTypingSearch;
        static const long ID_MenuChapterAutoSelect;
        static const long ID_MenuNewChapterNotification;
        static const long ID_MenuCompressChapters;
        static const long ID_MenuDeleteCompletedJobs;

        //(*Declarations(MangaDownloaderFrame)
        wxCheckBox* CheckBoxChapters;
        wxCheckBox* CheckBoxShowJobList;
        wxTextCtrl* TextCtrlDirectoryBase;
        wxStaticLine* StaticLine2;
        wxListCtrl* ListCtrlJobs;
        wxStaticText* StaticTextBaseDirectory;
        wxStaticText* StaticTextSearch;
        wxBitmapButton* BitmapButtonAbort;
        wxStaticText* StaticTextMangaList;
        wxStaticLine* StaticLine1;
        wxFlexGridSizer* FlexGridSizerLists;
        wxListCtrl* ListCtrlMangas;
        wxBitmapButton* BitmapButtonBrowse;
        wxComboBox* ComboBoxSource;
        wxStaticText* StaticTextSource;
        wxStaticLine* StaticLine3;
        wxBitmapButton* BitmapButtonUpdate;
        wxListCtrl* ListCtrlChapters;
        wxMenu MenuJobContext;
        wxBitmapButton* BitmapButtonBookmark;
        wxStatusBar* StatusBar;
        wxBitmapButton* BitmapButtonDownload;
        wxDirDialog* DirDialogBrowse;
        wxComboBox* ComboBoxSearchPattern;
        //*)
        wxMenu* MenuMain;

        //private: MangaDownloaderConfiguration Configuration;
        private: wxFileName ConfigurationFile;
        private: wxBitmap* ResourceImages;
        private: MangaConnectorCollection* MCC;
        private: wxArrayMCEntry CurrentMangaList;
        private: wxArrayMCEntry CurrentChapterList;
        private: wxString Username;
        private: wxString Password;
        private: wxString ChapterPattern;
        private: bool AbortDownload;
        private: bool StartupSync;
        private: bool TypingSearch;
        private: bool ChapterAutoSelect;
        private: bool NewChapterNotification;
        private: bool CompressChapters;
        private: bool DeleteCompletedJobs;

        DECLARE_EVENT_TABLE()
};

#endif // MANGADOWNLOADERMAIN_H
