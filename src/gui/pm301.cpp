/////////////////////////////////////////////////////////////////////////////
// Name:        pm301.cpp
// Purpose:
// Author:      Thomas Hisch
// Modified by:
// Created:     Mon 21 Mar 2011 16:38:47 CET
// RCS-ID:
// Copyright:
// Licence:
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes


#include <vector>
#include "wx/tokenzr.h"
#include "wx/socket.h"
#include "pm301.h"
#include "unitconversionconstants.h"

//#include "sample.xpm"

////@begin XPM images
////@end XPM images

#undef TEST_NETWORK

/*
 * PM301 type definition
 */

IMPLEMENT_CLASS( PM301, wxFrame )


/*
 * PM301 event table definition
 */

BEGIN_EVENT_TABLE( PM301, wxFrame )

////@begin PM301 event table entries
    EVT_MENU( ID_MENUITEM, PM301::ClickUnitConvConsts )

    EVT_MENU( ID_MENUITEM1, PM301::OnMenuitem1Click )

    EVT_MENU( ID_MENUITEM2, PM301::OnPositionUpdateClick )

    EVT_BUTTON( ID_BITMAPBUTTON, PM301::OnBitmapbuttonClick )

    EVT_BUTTON( ID_BITMAPBUTTON1, PM301::OnBitmapbutton1Click )

    EVT_BUTTON( ID_BITMAPBUTTON2, PM301::OnBitmapbutton2Click )

    EVT_BUTTON( ID_BUTTON3, PM301::OnButtonZeroPositionClick )

    EVT_CHECKBOX( ID_CHECKBOX, PM301::OnCheckboxClick )

    EVT_RADIOBOX( ID_RADIOBOX, PM301::OnRadioboxSelected )

    EVT_UPDATE_UI( ID_STATUSBAR, PM301::OnStatusbarUpdate )

////@end PM301 event table entries

    EVT_SPINCTRLDOUBLE( ID_SPINCTRL, PM301::OnSpinctrlUpdated )
    EVT_SPINCTRLDOUBLE( ID_SPINCTRL1, PM301::OnSpinctrl1Updated )
    EVT_SPINCTRLDOUBLE( ID_SPINCTRL2, PM301::OnSpinctrl2Updated )
    EVT_SOCKET(SOCKET_ID, PM301::OnSocketEvent )

END_EVENT_TABLE()

/*
 * PM301 constructors
 */

PM301::PM301()
{
    Init();
}

PM301::PM301( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style ) :
   m_mutex()
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * PM301 creator
 */

bool PM301::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PM301 creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();
////@end PM301 creation
    return true;
}


/*
 * PM301 destructor
 */

PM301::~PM301()
{
////@begin PM301 destruction
////@end PM301 destruction
}


/*
 * Member initialisation
 */

void PM301::Init()
{
////@begin PM301 member initialisation
    mainswitcher = NULL;
    basiccontrol = NULL;
    xlabel = NULL;
    xSpinCtrl = NULL;
    ylabel = NULL;
    ySpinCtrl = NULL;
    tlabel = NULL;
    tSpinCtrl = NULL;
    jogmodelayout = NULL;
    checkjog = NULL;
    axradiobox = NULL;
    batchmodelog = NULL;
    batchmodetextctl = NULL;
    statusbar = NULL;
////@end PM301 member initialisation
    posthread=NULL;

#ifdef TEST_NETWORK
    std::cout << "connecting to localhost" << std::endl;
    wxIPV4address addr;
    addr.Hostname(wxT("localhost"));
    addr.Service(15000);

    s = new wxSocketClient();

    s->SetEventHandler(*this, SOCKET_ID);
    s->SetNotify(wxSOCKET_INPUT_FLAG|wxSOCKET_LOST_FLAG);

    s->Notify(true);

    //Block the GUI
    s->Connect(addr, true);
    std::cout << "connected to localhost" << std::endl;
#endif
}

void PM301::OnSocketEvent(wxSocketEvent& event)
{
    wxSocketBase* sock = event.GetSocket();

    switch(event.GetSocketEvent())
    {
    case wxSOCKET_INPUT:
        std::cout << "INPUT event received" << std::endl;
        sock->Read(reinterpret_cast<char*>(&reply), msglen);
        break;
    case wxSOCKET_LOST:
    default:
        std::cerr << "lost network connection to server" << std::endl;
        sock->Destroy();
        break;

    }


}


/*
 * Control creation for PM301
 */

void PM301::CreateControls()
{
////@begin PM301 content construction
    PM301* itemFrame1 = this;

    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* itemMenu3 = new wxMenu;
    itemMenu3->Append(ID_MENUITEM, _("Change Unit-Conversion-Constants"), wxEmptyString, wxITEM_NORMAL);
    itemMenu3->Enable(ID_MENUITEM, false);
    itemMenu3->Append(ID_MENUITEM1, _("Batch Mode"), wxEmptyString, wxITEM_NORMAL);
    itemMenu3->Append(ID_MENUITEM2, _("Position Update Thread"), wxEmptyString, wxITEM_CHECK);
    menuBar->Append(itemMenu3, _("Operations"));
    itemFrame1->SetMenuBar(menuBar);

    mainswitcher = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(mainswitcher);

    basiccontrol = new wxBoxSizer(wxHORIZONTAL);
    mainswitcher->Add(basiccontrol, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer9 = new wxBoxSizer(wxVERTICAL);
    basiccontrol->Add(itemBoxSizer9, 1, wxALIGN_TOP|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer10Static = new wxStaticBox(itemFrame1, wxID_ANY, _("Set New Stepper Position"));
    wxStaticBoxSizer* itemStaticBoxSizer10 = new wxStaticBoxSizer(itemStaticBoxSizer10Static, wxVERTICAL);
    itemBoxSizer9->Add(itemStaticBoxSizer10, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer10->Add(itemBoxSizer11, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    xlabel = new wxStaticText( itemFrame1, wxID_STATIC, _("x:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer11->Add(xlabel, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    xSpinCtrl = new wxSpinCtrlDouble( itemFrame1, ID_SPINCTRL, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -5000, 5000, 0 );
    itemBoxSizer11->Add(xSpinCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBitmapButton* itemBitmapButton14 = new wxBitmapButton( itemFrame1, ID_BITMAPBUTTON, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    itemBoxSizer11->Add(itemBitmapButton14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer10->Add(itemBoxSizer15, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    ylabel = new wxStaticText( itemFrame1, wxID_STATIC, _("y:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(ylabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    ySpinCtrl = new wxSpinCtrlDouble( itemFrame1, ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -5000, 5000, 0 );
    itemBoxSizer15->Add(ySpinCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBitmapButton* itemBitmapButton18 = new wxBitmapButton( itemFrame1, ID_BITMAPBUTTON1, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    itemBoxSizer15->Add(itemBitmapButton18, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer10->Add(itemBoxSizer19, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    tlabel = new wxStaticText( itemFrame1, wxID_STATIC, _("t:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer19->Add(tlabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    tSpinCtrl = new wxSpinCtrlDouble( itemFrame1, ID_SPINCTRL2, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -5000, 5000, 0 );
    itemBoxSizer19->Add(tSpinCtrl, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBitmapButton* itemBitmapButton22 = new wxBitmapButton( itemFrame1, ID_BITMAPBUTTON2, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    itemBoxSizer19->Add(itemBitmapButton22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton23 = new wxButton( itemFrame1, ID_BUTTON3, _("Set current position as zero position"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer9->Add(itemButton23, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticLine* itemStaticLine24 = new wxStaticLine( itemFrame1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    basiccontrol->Add(itemStaticLine24, 0, wxGROW, 5);

    jogmodelayout = new wxBoxSizer(wxVERTICAL);
    basiccontrol->Add(jogmodelayout, 0, wxGROW|wxRIGHT|wxTOP|wxBOTTOM, 5);

    checkjog = new wxCheckBox( itemFrame1, ID_CHECKBOX, _("Jog Mode"), wxDefaultPosition, wxDefaultSize, 0 );
    checkjog->SetValue(false);
    jogmodelayout->Add(checkjog, 0, wxALIGN_LEFT, 5);

    wxArrayString axradioboxStrings;
    axradioboxStrings.Add(_("&Axis 1"));
    axradioboxStrings.Add(_("&Axis 2"));
    axradioboxStrings.Add(_("&Axis 3"));
    axradiobox = new wxRadioBox( itemFrame1, ID_RADIOBOX, _("Axes"), wxDefaultPosition, wxDefaultSize, axradioboxStrings, 1, wxRA_SPECIFY_COLS );
    axradiobox->SetSelection(0);
    axradiobox->Show(false);
    jogmodelayout->Add(axradiobox, 0, wxGROW, 5);

    batchmodelog = new wxBoxSizer(wxVERTICAL);
    mainswitcher->Add(batchmodelog, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxHORIZONTAL);
    batchmodelog->Add(itemBoxSizer29, 0, wxGROW|wxALL, 0);

    itemBoxSizer29->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton31 = new wxButton( itemFrame1, ID_BUTTON, _("Load Bach File"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer29->Add(itemButton31, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton32 = new wxButton( itemFrame1, ID_BUTTON1, _("Quit Batch Mode"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer29->Add(itemButton32, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    batchmodetextctl = new wxTextCtrl( itemFrame1, ID_TEXTCTRL6, _("Batch Log"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    batchmodelog->Add(batchmodetextctl, 1, wxGROW|wxALL, 5);

    statusbar = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
    statusbar->SetFieldsCount(1);
    itemFrame1->SetStatusBar(statusbar);

    wxToolBar* itemToolBar35 = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL, ID_TOOLBAR );
    itemToolBar35->Show(false);
    wxButton* itemButton36 = new wxButton( itemToolBar35, ID_BUTTON2, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
    itemToolBar35->AddControl(itemButton36);
    itemToolBar35->Realize();
    itemFrame1->SetToolBar(itemToolBar35);

    // Connect events and objects
    itemButton31->Connect(ID_BUTTON, wxEVT_LEFT_DOWN, wxMouseEventHandler(PM301::LoadBatchFileDialog), NULL, this);
    itemButton32->Connect(ID_BUTTON1, wxEVT_LEFT_DOWN, wxMouseEventHandler(PM301::LeaveBatchModeButtonPressed), NULL, this);
////@end PM301 content construction
//std::locale::global(locale(setlocale(LC_ALL, NULL)));
// m_locale.Init(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING);

    // wxBoxSizer* axe4itemBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    // itemStaticBoxSizer10->Add(axe4itemBoxSizer, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // wxStaticText* axe4label = new wxStaticText( itemStaticBoxSizer10->GetStaticBox(), wxID_STATIC, _("y:"), wxDefaultPosition, wxDefaultSize, 0 );
    // axe4itemBoxSizer->Add(axe4label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // wxSpinCtrlDouble* axe4SpinCtrl = new wxSpinCtrlDouble( itemStaticBoxSizer10->GetStaticBox(), ID_SPINCTRL1, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -5000, 5000, 0 );
    // axe4itemBoxSizer->Add(axe4SpinCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // wxBitmapButton* axe4BitmapButton = new wxBitmapButton( itemStaticBoxSizer10->GetStaticBox(), ID_BITMAPBUTTON1, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    // axe4itemBoxSizer->Add(axe4BitmapButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //SendMessage("gp1");
    //FIXME unit conversion
    //new_x_pos->SetValue(wxString::Format(wxT("%f"), atof(reply.msg));
    batchmodelog->Show(false);
    mainswitcher->Layout();

    const Position initpos = getcurpos();
    set_cp(initpos);
    
    xlabel->SetLabel(wxT("x [\u03BCm]:"));
    ylabel->SetLabel(wxT("y [\u03BCm]:"));
    tlabel->SetLabel(wxT("\u0398 [\u03BCm]:"));
    // axe4label->SetLabel(wxT("lala"));
    
    xSpinCtrl->SetValue(initpos.GetCoordinate(1));
    ySpinCtrl->SetValue(initpos.GetCoordinate(2));
    tSpinCtrl->SetValue(initpos.GetCoordinate(3));
    xSpinCtrl->SetDigits(2);
    ySpinCtrl->SetDigits(2);
    tSpinCtrl->SetDigits(2);
    
    //wxString text;
    //text.Printf(wxT("Position: x: %.2f,  y: %.2f,  \u0398: %.2f"),
    //           initpos.GetCoordinate(1), initpos.GetCoordinate(2), initpos.GetCoordinate(3));
    //statusbar->SetStatusText(text, 0);
}

/*
 * Should we show tooltips?
 */

bool PM301::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PM301::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PM301 bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PM301 bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PM301::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PM301 icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PM301 icon retrieval
}

void PM301::send(void)
{
#ifdef TEST_NETWORK
    s->Write(reinterpret_cast<char*>(&request), msglen);
#endif
}


void PM301::SendMessage(const wxString& msgstr)
{
    request.type = MSG_REQUEST;
    strcpy(request.msg, msgstr.mb_str());
    send();
}

void PM301::SendMessage(const std::string& msgstr)
{
    request.type = MSG_REQUEST;
    strcpy(request.msg, msgstr.c_str());
    send();
}

void PM301::SendMessage(const char *msgstr)
{
    request.type = MSG_REQUEST;
    strcpy(request.msg, msgstr);
    send();
}


/*
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIOBOX
 */

void PM301::OnRadioboxSelected( wxCommandEvent& event )
{
    char buf[4];
    int selected = axradiobox->GetSelection();
    sprintf(buf,"sa%d",selected);
    SendMessage(buf);
}



int PM301::getIdxFromCoord(const wxString &coord)
{
    if(coord == wxT("x"))
        return 0;
    else if(coord == wxT("y"))
        return 1;
    else if(coord == wxT("phi"))
        return 2;
    else
        return -1;
}


void PM301::check_and_update_position(wxSpinCtrlDouble* ctrl, const wxString& coord, const double curval)
{
    int idx = getIdxFromCoord(coord);

    if(idx < 0) {
        wxLogError(wxT("yes"));
        return;
    }

    wxString text;
    text.Printf(wxT("sa%d"),idx);
    SendMessage(text);
    text.Printf(wxT("1MA%d"), (int)curval);
    SendMessage(text);
}


/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX
 */

void PM301::OnCheckboxClick( wxCommandEvent& event )
{
    if(checkjog->IsChecked()) {
        SendMessage("1AR");
        axradiobox->Show(true);
        SendMessage("sa0");

    } else
    {
        SendMessage("1IR");
        axradiobox->Show(false);
    }

    jogmodelayout->Layout();
}


Position PM301::getcurpos()
{
    SendMessage("pp");
#ifdef TEST_NETWORK
    s->Read(reinterpret_cast<char*>(&reply), msglen);
    wxString text(reply.msg,wxConvUTF8);
#else
    static double a=10.46;
    static double b=20.51;
    static double c=30.58;
    wxString text;
    text.Printf(_T(" axis1: %lf\n axis2: %lf\n axis3: %lf\n"), a,b,c);
    c+=0.1;
    b+=0.9;
    a+=1.2;
#endif

//    wxRegEx rePos(_T("[[:digit:]]+\n"));
//    wxString num = rePos.GetMatch(text, 1);

    std::vector<Position::type> vec;
    wxStringTokenizer tkz(text, wxT("\n"));
    while ( tkz.HasMoreTokens() )
    {
        wxString token = tkz.GetNextToken();
        Position::type v;
        int d;
        sscanf(token.mb_str()," axis%d: %f", &d, &v);
        //std::cout << v << std::endl;
        vec.push_back( v );
    }

    //{

    //}
    // if ( reEmail.Matches(text) )
    // {
    //     wxString text = reEmail.GetMatch(email);
    //     wxString username = reEmail.GetMatch(email, 1);
    // }

    // or we could do this to hide the email address
//    size_t count = reEmail.ReplaceAll(text, "HIDDEN@\\2\\3");
//    printf("text now contains %u hidden addresses", count);


    //std::string b = wxT("abc");
    //wxString a = b.c_str();
    //wxLogError(wxT("bca"));
    Position p;
    p.SetCoordinate(1,vec[0]);
    p.SetCoordinate(2,vec[1]);
    p.SetCoordinate(3,vec[2]);
    return p;
}

/*
 * wxEVT_UPDATE_UI event handler for ID_STATUSBAR
 */

void PM301::OnStatusbarUpdate( wxUpdateUIEvent& event )
{
////@begin wxEVT_UPDATE_UI event handler for ID_STATUSBAR in PM301.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_UPDATE_UI event handler for ID_STATUSBAR in PM301.
}


void PM301::ToggleBatchMode(void)
{
    static bool batchstate = false;
    basiccontrol->Show(batchstate);
    if(batchstate && !checkjog->IsChecked())
        axradiobox->Show(false);

    batchmodelog->Show(!batchstate);
    batchstate = !batchstate;
    mainswitcher->Layout();

}

/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENUITEM
 */

void PM301::ClickUnitConvConsts( wxCommandEvent& event )
{
    ToggleBatchMode();
}


/*
 * wxEVT_LEFT_DOWN event handler for ID_BUTTON1
 */

void PM301::LeaveBatchModeButtonPressed( wxMouseEvent& event )
{
    ToggleBatchMode();
}


/*
 * wxEVT_LEFT_DOWN event handler for ID_BUTTON
 */

void PM301::LoadBatchFileDialog( wxMouseEvent& event )
{
    wxFileDialog dialog(GetParent(), _T("choose a file"), wxEmptyString, wxEmptyString,
                        _T("Batch Files (*.bat)|*.bat"), wxFD_OPEN);
    if(dialog.ShowModal() == wxID_OK)
    {
        BatchThread *thread = new BatchThread(dialog.GetPath(),batchmodetextctl, this);
        if(thread->Create() != wxTHREAD_NO_ERROR )
        {
            wxLogError(wxT("Can't create thread"));
        }else {
            thread->Run();
        }
    }
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENUITEM1
 */

void PM301::OnMenuitem1Click( wxCommandEvent& event )
{
    ToggleBatchMode();
}

void *BatchThread::Entry()
{
    for(size_t i = 0; i < file.GetLineCount(); i++)
    {
        batchmodetextctl_->AppendText(file[i] + _T("\n"));
        if(file[i].length() > 5 && !file[i].substr(0,5).compare(_T("sleep"))) {
            wxString tmp(file[i].substr(6));
            sleep(wxAtoi(tmp));
        }

        pm301->SendMessage(file[i]);
    }
    batchmodetextctl_->AppendText(_T("Finished executing batch file"));
    
    return NULL;
}


void PositionUpdateThread::WriteText(const wxString& text, const Position &pos, const size_t n) const
{
    wxString msg;

    // before doing any GUI calls we must ensure that this thread is the only
    // one doing it!

    wxMutexGuiEnter();

    msg << text;
    pm301->statusbar->SetStatusText(msg, n);

    //FIXME element under focus should not get updated
    //pm301->xSpinCtrl->SetValue(pos.GetCoordinate(1));
    //pm301->ySpinCtrl->SetValue(pos.GetCoordinate(2));
    //pm301->tSpinCtrl->SetValue(pos.GetCoordinate(3));

    wxMutexGuiLeave();
}

void* PositionUpdateThread::Entry()
{
    while(true) {
        if ( TestDestroy() )
            break;

        wxString text;
        Position cp = pm301->getcurpos();
        pm301->set_cp(cp); // so that the main frame can use it

        //TODO only output BarePosition to statusbar
        // or ???
        text.Printf(wxT("Position: x: %.2f,  y: %.2f,  \u0398: %.2f"),
                    cp.GetCoordinate(1), cp.GetCoordinate(2), cp.GetCoordinate(3));
        WriteText(text,cp, 0);
        wxThread::Sleep(900);
    }
    return NULL;
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_MENUITEM2
 */

void PM301::OnPositionUpdateClick( wxCommandEvent& event )
{
    if(!event.IsChecked()) {
        if (posthread->Delete() != wxTHREAD_NO_ERROR )
            wxLogError(wxT("Can't delete the thread!"));
        else
            posthread = NULL;
            statusbar->SetStatusText(wxEmptyString, 0);
            
    }
    else
    {
        posthread = new PositionUpdateThread(this);
        if(posthread->Create() != wxTHREAD_NO_ERROR )
        {
            wxLogError(wxT("Can't create thread"));
        }else {
            posthread->SetPriority(20);
            if (posthread->Run() != wxTHREAD_NO_ERROR) {
                wxLogError(wxT("Can't run thread"));
            }
        }
    }
}


void PM301::GeneralSpinCtrlUpdate(const wxString& coord)
{
    wxSpinCtrlDouble *cur = NULL;
    //wxSpinCtrlDouble *tmp = NULL;
    if(coord == wxT("x"))
        cur = xSpinCtrl;
    else if(coord == wxT("y"))
        cur = ySpinCtrl;
    else if(coord == wxT("phi"))
        cur = tSpinCtrl;
    else
        return;

    int entered = cur->GetValue();
    double value = entered;
    //if(!entered.ToDouble(&value)){
    //    wxMessageBox(wxT("Entered string is not a number!"), wxT("Warning"), wxOK | wxICON_INFORMATION, this);
        //set default value ?!?!?
    //    return;
    //}

    check_and_update_position(cur, coord, value);

}



/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON3
 */

void PM301::OnButtonZeroPositionClick( wxCommandEvent& event )
{    
    wxMessageDialog dialog( NULL, wxT("Are you sure that you want to reset the stepper positions to zero?"),
                            wxT("Warning"), wxNO_DEFAULT | wxYES_NO | wxICON_INFORMATION);
        switch(dialog.ShowModal()) {
        case wxID_YES:
            // only call this if position update thread is not running
            //if(posthread == NULL) {
                xSpinCtrl->SetValue(0.0);
                ySpinCtrl->SetValue(0.0);
                tSpinCtrl->SetValue(0.0);
            //}
            SendMessage("set zero");
            break;
        case wxID_NO:
            //wxLogError(wxT("no"));
            break;
        default:
            wxLogError(wxT("Unexpected wxMess Dialog return code!"));
        }
}

void PM301::OnSpinctrlUpdated( wxSpinDoubleEvent& event )
{
    std::cout << "update" << std::endl;
    GeneralSpinCtrlUpdate(wxT("x"));
}

void PM301::OnSpinctrl1Updated( wxSpinDoubleEvent& event )
{
    std::cout << "update" << std::endl;
    GeneralSpinCtrlUpdate(wxT("y"));
}

void PM301::OnSpinctrl2Updated( wxSpinDoubleEvent& event )
{
    std::cout << "update" << std::endl;
    GeneralSpinCtrlUpdate(wxT("phi"));
}

/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON
 */

void PM301::OnBitmapbuttonClick( wxCommandEvent& event )
{
    xSpinCtrl->SetValue(get_cp().GetCoordinate(1));

}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON1
 */

void PM301::OnBitmapbutton1Click( wxCommandEvent& event )
{
    ySpinCtrl->SetValue(get_cp().GetCoordinate(2));
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON2
 */

void PM301::OnBitmapbutton2Click( wxCommandEvent& event )
{
        tSpinCtrl->SetValue(get_cp().GetCoordinate(3));
}



