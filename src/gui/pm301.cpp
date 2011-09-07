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
#include "wx/vector.h"
#include "wx/tokenzr.h"
#include "wx/socket.h"
#include "pm301.h"

//#include "sample.xpm"

////@begin XPM images
////@end XPM images

// #undef TEST_NETWORK
#define TEST_NETWORK

/*
 * PM301 type definition
 */

IMPLEMENT_CLASS( PM301, wxFrame )


/*
 * PM301 event table definition
 */

BEGIN_EVENT_TABLE( PM301, wxFrame )

////@begin PM301 event table entries
    EVT_MENU( ID_MENUITEM1, PM301::OnMenuitem1Click )

    EVT_BUTTON( ID_BUTTON3, PM301::OnButtonZeroPositionClick )

    EVT_BUTTON( ID_BUTTON_SAVEXML, PM301::OnButtonSavexmlClick )

    EVT_CHECKBOX( ID_CHECKBOX, PM301::OnCheckboxClick )

////@end PM301 event table entries

    EVT_SOCKET(SOCKET_ID, PM301::OnSocketEvent )
    
    EVT_RADIOBOX( ID_AXESRADIOBOX, PM301::OnRadioboxSelected )

END_EVENT_TABLE()

static wxMutex *s_mutex;


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
    poswidget = NULL;
    posSizer = NULL;
    jogmodelayout = NULL;
    checkjog = NULL;
    batchmodelog = NULL;
    batchmodetextctl = NULL;
////@end PM301 member initialisation
    posthread=NULL;

#ifdef TEST_NETWORK
    std::cout << "connecting to localhost" << std::endl;
    wxIPV4address addr;
    addr.Hostname(wxT("localhost"));
    addr.Service(16000);

    s = new wxSocketClient();
    //s->SetFlags(wxSOCKET_WAITALL);
			    

    //s->SetEventHandler(*this, SOCKET_ID);
    //s->SetNotify(wxSOCKET_INPUT_FLAG|wxSOCKET_LOST_FLAG);

    //s->Notify(true);

    // FIXME SetFlags WaitALL ?????????

    //Block the GUI
    if(s->Connect(addr, true))
        std::cout << "connected to localhost" << std::endl;
    else {
        std::cerr << "connection failure" << std::endl;
        exit(1);
    }
#endif
}

void PM301::OnSocketEvent(wxSocketEvent& event)
{
    wxSocketBase* sock = event.GetSocket();

    switch(event.GetSocketEvent())
    {
    case wxSOCKET_INPUT:
        sock->Read(reinterpret_cast<char*>(&reply), msglen);

        if(reply.type == MSG_ERROR) {
            std::cout << "error event received" << std::endl;
            wxString errormsg;
            errormsg.Printf(wxT("Controller ERROR: %s"), reply.msg);
            wxMessageBox(errormsg, wxT("Warning"),
                         wxOK | wxICON_INFORMATION, this);
        }

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
    itemMenu3->Append(ID_MENUITEM1, _("Batch Mode"), wxEmptyString, wxITEM_NORMAL);
    menuBar->Append(itemMenu3, _("Operations"));
    itemFrame1->SetMenuBar(menuBar);

    mainswitcher = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(mainswitcher);

    basiccontrol = new wxBoxSizer(wxHORIZONTAL);
    mainswitcher->Add(basiccontrol, 1, wxGROW, 5);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
    basiccontrol->Add(itemBoxSizer7, 1, wxALIGN_TOP|wxALL, 5);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer7->Add(itemBoxSizer8, 0, wxGROW|wxALL, 5);

    wxButton* itemButton9 = new wxButton( itemFrame1, ID_BUTTON3, _("Set current position as zero position"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(itemButton9, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton10 = new wxButton( itemFrame1, ID_BUTTON_SAVEXML, _("Save current position to xml file"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(itemButton10, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    poswidget = new wxStaticBox(itemFrame1, wxID_ANY, _("Set New Stepper Position"));
    posSizer = new wxStaticBoxSizer(poswidget, wxVERTICAL);
    itemBoxSizer7->Add(posSizer, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticLine* itemStaticLine12 = new wxStaticLine( itemFrame1, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    basiccontrol->Add(itemStaticLine12, 0, wxGROW, 5);

    jogmodelayout = new wxBoxSizer(wxVERTICAL);
    basiccontrol->Add(jogmodelayout, 0, wxGROW|wxALL, 5);

    checkjog = new wxCheckBox( itemFrame1, ID_CHECKBOX, _("Jog Mode"), wxDefaultPosition, wxDefaultSize, 0 );
    checkjog->SetValue(false);
    jogmodelayout->Add(checkjog, 0, wxGROW, 5);

    batchmodelog = new wxBoxSizer(wxVERTICAL);
    mainswitcher->Add(batchmodelog, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer16 = new wxBoxSizer(wxHORIZONTAL);
    batchmodelog->Add(itemBoxSizer16, 0, wxGROW|wxALL, 0);

    itemBoxSizer16->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton18 = new wxButton( itemFrame1, ID_BUTTON, _("Load Bach File"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemButton18, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton19 = new wxButton( itemFrame1, ID_BUTTON1, _("Quit Batch Mode"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer16->Add(itemButton19, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    batchmodetextctl = new wxTextCtrl( itemFrame1, ID_TEXTCTRL6, _("Batch Log"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    batchmodelog->Add(batchmodetextctl, 1, wxGROW|wxALL, 5);

    wxToolBar* itemToolBar21 = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL, ID_TOOLBAR );
    itemToolBar21->Show(false);
    wxButton* itemButton22 = new wxButton( itemToolBar21, ID_BUTTON2, _("Button"), wxDefaultPosition, wxDefaultSize, 0 );
    itemToolBar21->AddControl(itemButton22);
    itemToolBar21->Realize();
    itemFrame1->SetToolBar(itemToolBar21);

    // Connect events and objects
    itemButton18->Connect(ID_BUTTON, wxEVT_LEFT_DOWN, wxMouseEventHandler(PM301::LoadBatchFileDialog), NULL, this);
    itemButton19->Connect(ID_BUTTON1, wxEVT_LEFT_DOWN, wxMouseEventHandler(PM301::LeaveBatchModeButtonPressed), NULL, this);
////@end PM301 content construction
//std::locale::global(locale(setlocale(LC_ALL, NULL)));
// m_locale.Init(wxLANGUAGE_DEFAULT, wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING);

    //retreive nr. of axes and coordinate names from the server
    initaxes();

    const Position initpos = getcurpos();
    set_cp(initpos);
    wxArrayString axesradioboxStrings;

    for(size_t i=0; i < get_nraxes(); ++i) {
        axisbs.push_back(new wxBoxSizer(wxHORIZONTAL));
        axisst.push_back(new wxStaticText(posSizer->GetStaticBox(), -1,
                                          wxString::Format("%s [%s]:", *coords[i], *units[i]),
                                          wxDefaultPosition, wxDefaultSize, 0));
        axissc.push_back(new wxSpinCtrlDouble(posSizer->GetStaticBox(), ID_SPINCTRLS+i, _T("0"),
                                              wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -5000, 5000, 0));
        //axisbb.push_back(new wxBitmapButton(posSizer->GetStaticBox(), ID_BITMAPBUTTONS+i,
        //                                    wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW));

        axissc[i]->SetValue(initpos.GetCoordinate(i+1));
        axissc[i]->SetDigits(2);
        axissc[i]->SetIncrement(0.1);

        axisbs[i]->Add(axisst[i], 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        axisbs[i]->Add(axissc[i], 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        //axisbs[i]->Add(axisbb[i], 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
        posSizer->Add(axisbs[i], 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

        axissc[i]->Bind(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, &PM301::OnSpinCTRLUpdated, this);
        //axisbb[i]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &PM301::OnBitmapbuttonClick, this);

        axesradioboxStrings.Add(wxString::Format("&%s",*coords[i]));
    }

    axesradiobox = new wxRadioBox( itemFrame1, ID_AXESRADIOBOX, _("Axis"), wxDefaultPosition,
                                   wxDefaultSize, axesradioboxStrings, 1, wxRA_SPECIFY_COLS );
    axesradiobox->SetSelection(0);
    axesradiobox->Show(false);
    jogmodelayout->Add(axesradiobox, 0, wxGROW, 5);
    batchmodelog->Show(false);
    mainswitcher->Layout();
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


const wxString PM301::SendandReceive(const wxString& msgstr)
{
  msg_t msg;
  msg.type = MSG_REQUEST;
  strcpy(msg.msg, msgstr.mb_str());
  wxMutexLocker lock(m_tcpmutex);
#ifdef TEST_NETWORK
  s->Write(reinterpret_cast<char*>(&msg), msglen);
  std::cout << "msg sent" << std::endl;
  s->Read(reinterpret_cast<char*>(&msg), msglen);
  std::cout << "msg recv" << std::endl;
#endif
  if(msg.type == MSG_SUCCESS)
    return wxString("OK");
  else
    return wxString(msg.msg);
}


void PM301::SendMessage(const wxString& msgstr)
{
    request.type = MSG_REQUEST;
  strcpy(request.msg, msgstr.mb_str());
  send();
  s->Read(reinterpret_cast<char*>(&reply), msglen);
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
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_AXESRADIOBOX
 */

void PM301::OnRadioboxSelected( wxCommandEvent& event )
{
    int selected = axesradiobox->GetSelection();
    wxString text;
    text.Printf("sa%d", selected+1);
    std::cout << "Radiobox selection : " << text.c_str() << " ret: " << 
      SendandReceive(text).c_str() << std::endl;
}


/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX
 */

void PM301::OnCheckboxClick( wxCommandEvent& event )
{
    if(checkjog->IsChecked()) {
        wxString txt = SendandReceive("set jog");
        std::cout << "checkboxclick sj returned " << txt.c_str() << std::endl;
        axesradiobox->Show(true);
        txt = SendandReceive("sa1");
        std::cout << "checkboxclick sa1 returned " << txt.c_str() << std::endl;
        
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
        for(size_t i=0; i < get_nraxes(); ++i) {
            axissc[i]->SetValue(0.002); //initpos.GetCoordinate(i+1));
            axissc[i]->Enable(false);
        }
    } else
    {
        wxString txt = SendandReceive("unset jog");
        std::cout << "checkboxclick usj returned " << txt.c_str() << std::endl;
        axesradiobox->Show(false);
         if (posthread->Delete() != wxTHREAD_NO_ERROR )
            wxLogError(wxT("Can't delete the thread!"));
        else
            posthread = NULL;
            
        for(size_t i=0; i < get_nraxes(); ++i) {
            axissc[i]->Enable(true);
        }
    }

    jogmodelayout->Layout();
}

void PM301::initaxes()
{
   
#ifdef TEST_NETWORK
    wxString text = SendandReceive("ga");
#else
    wxString text;

    text.Printf("1:x:mm\n2:y:mm\n3:z:um\n4:\u0398:deg\n5:k:m\n6:l:km\n");
#endif
    std::cout << "Command \"ga\" returned: " << text.c_str() << std::endl;
    wxVector<Position::type> vec;
    wxStringTokenizer tkz(text, wxT("\n"));
    while ( tkz.HasMoreTokens() )
    {
        wxStringTokenizer line(tkz.GetNextToken(), wxT(":"));
        unsigned long id;
        line.GetNextToken().ToULong(&id);
        wxString coordname = line.GetNextToken();
        wxString unitname = line.GetNextToken();
        std::cout << "GUI registering axis with id " << id << ", coordname " << coordname
                  << "and unit " << unitname
                  << std::endl;
        coords.push_back(new wxString(coordname));
        units.push_back(new wxString(unitname));
    }

    nraxes = coords.size();
}


Position PM301::getcurpos()
{
#ifdef TEST_NETWORK
    wxString text = SendandReceive("pp");
#else
    static double pos[]={3.21,91.19,324.19,-1239.09, 9234,93,-0.2};
    wxString text;
    for(size_t i=0; i < get_nraxes(); ++i) {
        text.Append(wxString::Format(" axis%d: %lf\n", i+1, pos[i]));
        pos[i] += 0.9;
    }
#endif

    wxVector<Position::type> vec;
    wxStringTokenizer tkz(text, wxT("\n"));
    while ( tkz.HasMoreTokens() )
    {
        wxString token = tkz.GetNextToken();
        Position::type v;
        wxString axdesc  = token.BeforeFirst(':');
        double vtmp;
        token.AfterFirst(':').ToDouble(&vtmp);
        //v = floorf(vtmp*100 + 0.5)/100;
        v = vtmp;

        // std::cout << "GUI_position_parser: " << axdesc.c_str()
        //           << " " << v << std::endl;
        vec.push_back( v );
    }

    Position p;
    for(size_t i=0; i < vec.size(); ++i) {
        p.SetCoordinate(i+1,vec[i]);
    }
    return p;
}


void PM301::ToggleBatchMode(void)
{
    static bool batchstate = false;
    basiccontrol->Show(batchstate);
    if(batchstate && !checkjog->IsChecked())
        axesradiobox->Show(false);

    batchmodelog->Show(!batchstate);
    batchstate = !batchstate;
    mainswitcher->Layout();

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
        text.Printf(wxT("Position:"));
        for(size_t i = 0; i < pm301->get_nraxes(); ++i) {
            text.Append(wxString::Format("%s: %.4f %s\t", *pm301->coords[i], cp.GetCoordinate(i+1), *pm301->units[i]));
            pm301->axissc[i]->SetValue(cp.GetCoordinate(i+1));
        }
        //WriteText(text,cp, 0);

        std::cout << "PosThread: " << text.c_str() << std::endl;
        wxThread::Sleep(850);
    }
    return NULL;
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON3
 */

void PM301::OnButtonZeroPositionClick( wxCommandEvent& event )
{
    wxMessageDialog dialog( NULL,wxT("Are you sure that you want to reset the stepper" \
                                     " positions to zero?"),
                            wxT("Warning"), wxNO_DEFAULT | wxYES_NO | wxICON_INFORMATION);
    switch(dialog.ShowModal()) {
    case wxID_YES:
        // only call this if position update thread is not running
        //if(posthread == NULL) {

        for (wxVector<wxSpinCtrlDouble*>::iterator it = axissc.begin(); it != axissc.end(); ++it)
            (*it)->SetValue(0.0);
        //}
        SendandReceive("set zero");
        break;
    case wxID_NO:
        //wxLogError(wxT("no"));
        break;
    default:
        wxLogError(wxT("Unexpected wxMess Dialog return code!"));
    }
}

void PM301::OnSpinCTRLUpdated( wxCommandEvent& event ) //wxSpinDoubleEvent& event )
{
    int id = event.GetId() - ID_SPINCTRLS;
    //std::cout << "ID: " << id << std::endl;
    if(id < 0 || id >= (int)get_nraxes()) {
        wxLogError("Event ID in SpinCtrl Handler wrong");
    }

    double value = axissc[id]->GetValue();
    // std::cout << value << " sc value update " << std::endl;
    // if(!entered.ToDouble(&value)){
    //     wxMessageBox(wxT("Entered string is not a number!"), wxT("Warning"), wxOK | wxICON_INFORMATION, this);
    //     //set default value ?!?!?
    //     return;
    // }
    //std::cout << "got " << value << std::endl;

    wxString text;
    text.Printf("ma %s=%f", *coords[id], value);
    std::cout << "command to update pos: " << text.c_str();
    text = SendandReceive(text);
    std::cout  << " it returned: " << text.c_str() << std::endl;
}

void PM301::OnBitmapbuttonClick( wxCommandEvent& event )
{
    // wxBitmapButton* ptr = (wxBitmapButton*)event.GetEventObject();
    // int i = 0;
    // for (wxVector<wxBitmapButton*>::iterator it = axisbb.begin(); it != axisbb.end(); ++it, i++) {
    //     if (ptr == *it)
    //         break;
    // }

    int id = event.GetId() - ID_BITMAPBUTTONS;
    if(id < 0 || id >= (int)get_nraxes()) {
        wxLogError("Event ID in Bitmapbutton Handler wrong");
    }

    std::cout << " bitmap button " << id << " clicked " << std::endl;
    axissc[id]->SetValue(get_cp().GetCoordinate(id+1));
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SAVEXML
 */

void PM301::OnButtonSavexmlClick( wxCommandEvent& event )
{
    SendandReceive("savexml");
}

