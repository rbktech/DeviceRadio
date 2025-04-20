#pragma once

#include <wx/frame.h>
#include <wx/combobox.h>
#include <wx/atomic.h>
#include <wx/stattext.h>

#include <gpPanel.h>
#include <gpLineLayer.h>
#include <queue>

#include "sdrblunt.h"

// #include "../qt/Console/hidlink.h"

class CMainFrame : public wxFrame
{
private:
    std::queue<Coords> mQueue;

    wxAtomicInt mAppProcess;

    gpPanel* mGraphPanel;
    gpLineLayer* mLineLayer;
    gpSeries* mSeriesData;
    // gpSeries* mSeriesDataTrace;

    // CHidLink m_hid;
    CSDRBlunt mSDRBlunt;

    wxComboBox* m_cmbListHid;
    wxStaticText* mTxtDiff;

    void OnClose(wxCloseEvent& event);
    void OnCheckHid(wxCommandEvent& event);
    void OnLink(wxCommandEvent& event);

    void OnProcess();
    int InitPlot();

    void RefreshData(const Coords* coords);

public:
    explicit CMainFrame(wxWindow* parent = nullptr);
    ~CMainFrame() override;
};