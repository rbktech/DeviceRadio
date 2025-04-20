#include "MainFrame.h"

#include <wx/button.h>
#include <wx/sizer.h>

#include <thread>

#define MAIN_THEME wxColor(0x40, 0x40, 0x40)
#define SUB_THEME wxColor(0x60, 0x60, 0x60)

CMainFrame::CMainFrame(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, wxT("Device Radio"))
    , mAppProcess(1)
{
    SetIcon(wxICON(radio));
    this->SetBackgroundColour(*wxWHITE);

    wxButton* btnCheckHid = new wxButton(this, NewControlId(), wxT("Поиск"));
    wxButton* btnLink = new wxButton(this, NewControlId(), wxT("Соединить"));
    // m_cmbListHid = new wxComboBox(this, NewControlId(), wxT(""), wxDefaultPosition, wxSize(800, 25));
    m_cmbListHid = new wxComboBox(this, NewControlId());
    mTxtDiff = new wxStaticText(this, NewControlId(), "");

    wxBoxSizer* hBox = nullptr;
    wxBoxSizer* main_box = new wxBoxSizer(wxVERTICAL);

    hBox = new wxBoxSizer(wxHORIZONTAL);
    {
        hBox->Add(btnCheckHid);
        hBox->Add(m_cmbListHid);
        hBox->Add(btnLink);
        hBox->Add(mTxtDiff);
    }
    main_box->Add(hBox);

    {
        mGraphPanel = new gpPanel(this, NewControlId());
        mGraphPanel->SetDoubleBuffered(true);

        // InitPlot();
    }
    main_box->Add(mGraphPanel, 1, wxEXPAND);

    SetSizer(main_box);
    // SetSizerAndFit(main_box);

    Bind(wxEVT_CLOSE_WINDOW, &CMainFrame::OnClose, this);
    Bind(wxEVT_BUTTON, &CMainFrame::OnCheckHid, this, btnCheckHid->GetId());
    Bind(wxEVT_BUTTON, &CMainFrame::OnLink, this, btnLink->GetId());

    this->CallAfter(&CMainFrame::OnProcess);
}

CMainFrame::~CMainFrame()
{
    mSDRBlunt.close();
    mGraphPanel->DelLayer(mLineLayer);
}

void CMainFrame::OnClose(wxCloseEvent& WXUNUSED(event))
{
    mAppProcess = 0;
}

void CMainFrame::OnCheckHid(wxCommandEvent& WXUNUSED(event))
{
    int i = 0;

    char* devices[150] = { nullptr };

    /*DWORD res = m_hid.SearchHid(vid, pid, devices);
    if(res != ERROR_NO_MORE_ITEMS && res != ERROR_SUCCESS) {
        wxMessageBox(wxString::Format(wxT("GetLastError %d"), res));
        return;
    }*/

    m_cmbListHid->Clear();
    while(devices[i] != nullptr)
        m_cmbListHid->Append(wxString::FromUTF8(devices[i++]));
}

void CMainFrame::OnLink(wxCommandEvent& WXUNUSED(event))
{
    /*DWORD res = m_hid.Connect(m_cmbListHid->GetValue().mb_str());
    if(res != ERROR_SUCCESS) {
        wxMessageBox(wxString::Format(wxT("GetLastError %d"), res));
        return;
    }*/
}

void CMainFrame::RefreshData(const Coords* coords)
{
    mSeriesData->DataClear();

    for(int i = 0; i < SIZE_BUFFER; i++)
        mSeriesData->DataPush(coords[i].x, coords[i].y);

    mLineLayer->RefreshChart();
    mGraphPanel->Fit(mLineLayer);
}

void CMainFrame::OnProcess()
{
    int result = 0;

    // std::vector<Coords> data;

    result = mSDRBlunt.open();

    result = mSDRBlunt.setCenterFrequency(100.3e6);
    result = mSDRBlunt.setSampleRate(1.024e6);
    result = mSDRBlunt.setAutoGain();
    result = mSDRBlunt.setBandwidth(0);

    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Coords coords[SIZE_BUFFER];

    while(mAppProcess == 1) {

        const std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();

        result = mSDRBlunt.recv(coords);
        if(result == 0) {
            mSeriesData->DataClear();

            for(auto & coord : coords)
                mSeriesData->DataPush(coord.x, coord.y);

            mLineLayer->RefreshChart();
            mGraphPanel->Refresh();
        }

        const std::chrono::system_clock::time_point end = std::chrono::system_clock::now();

        mTxtDiff->SetLabelText(wxString::Format(wxT("%lld"), (end - begin).count()));
        wxYield();
    }

    Destroy();
}

int CMainFrame::InitPlot()
{
    mLineLayer = new gpLineLayer(wxT("Line"), wxT("MHz"), wxT("dBm"));
    mLineLayer->ShowGrid(false);

    mSeriesData = mLineLayer->AddSeriesLayer("data");

    mGraphPanel->AddLayer(mLineLayer, POPUP_FILE | POPUP_CHART | POPUP_EDIT | POPUP_HELP | POPUP_FIT);

    mGraphPanel->GetWindowByLayer(mLineLayer)->SetColourTheme(MAIN_THEME, *wxBLACK, *wxWHITE);
    mGraphPanel->GetWindowByLayer(mLineLayer)->SetGradienBackColour(false);

    mSeriesData->SetPen(wxPen(wxColor(0xFF, 0xFF, 0x00), 1));      // Yellow

    mGraphPanel->Refresh(mLineLayer);

    return 0;
}