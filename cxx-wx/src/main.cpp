#include <wx/app.h>
#include <thread>

#include "mainframe.h"

class CApp : public wxApp
{
private:
    std::thread* mThread;
    wxAtomicInt mProcess;

    void process()
    {
        while(mProcess == true)
        {

        }
    }

public:
    CApp()
        : mThread(nullptr)
        , mProcess(false)
    {
    }

    bool OnInit() override {

        mProcess = (new CMainFrame())->Show();

        // mThread = new std::thread(&CApp::process, this);

        return mProcess;
    }
};

IMPLEMENT_APP(CApp)