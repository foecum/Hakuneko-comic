#ifndef ERRORLOGFRAME_H
#define ERRORLOGFRAME_H

//(*Headers(ErrorLogFrame)
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/frame.h>
//*)

#include <wx/icon.h>

class ErrorLogFrame: public wxFrame
{
    public:

        ErrorLogFrame(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~ErrorLogFrame();

    public:

        void SetLog(wxArrayString ErrorLog);

        //(*Declarations(ErrorLogFrame)
        wxListCtrl* ListCtrlLog;
        //*)

    protected:

        //(*Identifiers(ErrorLogFrame)
        static const long ID_LISTCTRL2;
        //*)

    private:

        //(*Handlers(ErrorLogFrame)
        void OnResize(wxSizeEvent& event);
        //*)

        DECLARE_EVENT_TABLE()
};

#endif
