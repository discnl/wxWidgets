/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Minimal wxWidgets sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dataview.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButton(wxCommandEvent &event)
    {
        if (event.GetId() == wxID_ADD)
        {
            wxString itemText = m_edit->GetValue();
            if ( m_dataview->GetItemCount() )
            {
                m_dataview->SetTextValue(itemText, 0, 0);
            }
            else
            {
                wxVector<wxVariant> data;
                data.push_back(itemText);
                data.push_back("123");
                m_dataview->AppendItem(data);
            }
        }
        else
            m_dataview->SendSizeEvent();
}

private:
    wxDataViewListCtrl *m_dataview;
    wxTextCtrl *m_edit;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("Minimal wxWidgets App");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUBAR
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#elif 0 // !wxUSE_MENUBAR
    // If menus are not available add a button to access the about box
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
    SetSizer(sizer);
#endif // wxUSE_MENUBAR/!wxUSE_MENUBAR

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR

// Use test 1 by default (default width + autosize).
#define COL_TEST 1

// Regardless of test type the right column should always autosize and, because
// wxALIGN_RIGHT is used (but not part of the problem), the text should be
// on the right side of the window. If the column width doesn't update, then
// pressing the "SendSizeEvent()"" button ensures a correct update.

    int colWidths[2] = {
#if COL_TEST == 0

// No explicit auto-sizing columns. The last column should autosize and does
// correctly.
        wxCOL_WIDTH_DEFAULT, wxCOL_WIDTH_DEFAULT

#elif COL_TEST == 1

// Only column 1 autosizes.

// Simplest case that bugs for me anywhere as it doesn't autosize column 1
// when appending or setting. Instead the column is an exact fit.
        wxCOL_WIDTH_DEFAULT, wxCOL_WIDTH_AUTOSIZE

#elif COL_TEST == 2

// Same behaviour as 1. Column 0 does autosize correctly.
        wxCOL_WIDTH_AUTOSIZE, wxCOL_WIDTH_AUTOSIZE

#elif COL_TEST == 3

// Like Igor's sample: Only first column autosizes.

// While the original sample bugs for me on (only) actual machines, I can't
// get it to bug here as-is. But if I create the below button before
// the DVC and change sizer stuff then it does bug... (see two more
// "COL_TEST == 3" checks below).

// Behaviour:
// Column 1 is autosized but isn't wide enough, it's off by about column 0's
// width. Making column 0's text wider makes column 1 shift to the right, and
// making column 0 shorter makes column 1 move into view more.
        wxCOL_WIDTH_AUTOSIZE, wxCOL_WIDTH_DEFAULT

#else
#   error Nope
#endif
    };

    wxPanel *panel = new wxPanel(this);

#if COL_TEST == 3
    // MUST create in this order for this test to be able to bug (?).
    wxButton *btn = new wxButton(panel, wxID_ADD, "Set Col 0 Text");
    m_dataview = new wxDataViewListCtrl(panel, wxID_ANY);
#else
    m_dataview = new wxDataViewListCtrl(panel, wxID_ANY);
    wxButton *btn = new wxButton(panel, wxID_ADD, "Set Col 0 Text");
#endif

    m_dataview->AppendTextColumn("", wxDATAVIEW_CELL_INERT, colWidths[0]);
    m_dataview->AppendTextColumn("", wxDATAVIEW_CELL_INERT, colWidths[1], wxALIGN_RIGHT);

    Bind(wxEVT_BUTTON, &MyFrame::OnButton, this);

    const wxString col0Text = "abc";

#if 0
    // Enable to test with initial value (works).
    wxVector<wxVariant> data;
    data.push_back(col0Text);
    data.push_back("123");
    m_dataview->AppendItem( data );
#endif

    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(m_dataview, wxSizerFlags().Expand());

    wxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);
#if COL_TEST == 3
        // MUST add to sizer and not hsizer for this test to be able to bug (?).
        sizer->Add(btn);
#else
        hsizer->Add(btn);
#endif
        m_edit = new wxTextCtrl(panel, wxID_ANY, col0Text);
        hsizer->Add(m_edit, wxSizerFlags(1));
        hsizer->AddStretchSpacer();
        hsizer->Add(new wxButton(panel, wxID_ANY, "SendSizeEvent()"));

    sizer->Add(hsizer, wxSizerFlags().Expand());
    panel->SetSizer(sizer);
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                    "Welcome to %s!\n"
                    "\n"
                    "This is the minimal wxWidgets sample\n"
                    "running under %s.",
                    wxVERSION_STRING,
                    wxGetOsDescription()
                 ),
                 "About wxWidgets minimal sample",
                 wxOK | wxICON_INFORMATION,
                 this);
}
