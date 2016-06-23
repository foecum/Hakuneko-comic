#include "ErrorLogFrame.h"

//(*InternalHeaders(ErrorLogFrame)
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/image.h>
#include <wx/artprov.h>
//*)

//(*IdInit(ErrorLogFrame)
const long ErrorLogFrame::ID_LISTCTRL2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ErrorLogFrame,wxFrame)
    //(*EventTable(ErrorLogFrame)
    //*)
END_EVENT_TABLE()

ErrorLogFrame::ErrorLogFrame(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(ErrorLogFrame)
    wxBoxSizer* BoxSizer1;

    Create(parent, wxID_ANY, _("Error Log"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    {
        wxIcon FrameIcon;
        FrameIcon.CopyFromBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_INFORMATION")),wxART_FRAME_ICON));
        SetIcon(FrameIcon);
    }
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    ListCtrlLog = new wxListCtrl(this, ID_LISTCTRL2, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_EDIT_LABELS, wxDefaultValidator, _T("ID_LISTCTRL2"));
    ListCtrlLog->SetMinSize(wxSize(640,320));
    wxListItem col0;
    col0.SetId(0);
    col0.SetWidth(320);
    col0.SetText( wxT("Source Page") );
    ListCtrlLog->InsertColumn(0, col0);

    wxListItem col1;
    col1.SetId(1);
    col1.SetWidth(280);
    col1.SetText( wxT("Target File") );
    ListCtrlLog->InsertColumn(1, col1);
    BoxSizer1->Add(ListCtrlLog, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);
    Center();

    Connect(wxEVT_SIZE,(wxObjectEventFunction)&ErrorLogFrame::OnResize);
    //*)
}

ErrorLogFrame::~ErrorLogFrame()
{
    //(*Destroy(ErrorLogFrame)
    //*)
}

void ErrorLogFrame::SetLog(wxArrayString ErrorLog)
{
    wxString target;
    wxString source;

    //ListCtrlLog->SetItemState(-1, !wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

    ListCtrlLog->Freeze();

    for(unsigned int i=0; i<ErrorLog.Count(); i++)
    {
        source = ErrorLog[i].Before('|');
        target= ErrorLog[i].After('|');

        ListCtrlLog->InsertItem(i, ErrorLog[i]);
        ListCtrlLog->SetItem(i, 0, source);
        ListCtrlLog->SetItem(i, 1, target);
    }

    ListCtrlLog->Thaw();
}

void ErrorLogFrame::OnResize(wxSizeEvent& event)
{
    Layout();

    // prevent flickering when changing column size...
    ListCtrlLog->Freeze();

    // always assume scrollbar is visible, never use GetClientSize
    int width = ListCtrlLog->GetSize().GetWidth()-24;
    ListCtrlLog->SetColumnWidth(0, width/2);
    ListCtrlLog->SetColumnWidth(1, width/2);

    ListCtrlLog->Thaw();
}
