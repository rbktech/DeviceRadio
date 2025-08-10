#pragma once

#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QTextEdit>

#include "glwidget.h"
#include "linechart.h"

class CMainWindow : public QMainWindow
{
    Q_OBJECT

private:
    CRtlSdr mRtlSdr;
    QAtomicInt mProcess;
    QLineEdit* mTxtQueueSize;
    QLineEdit* mTxtX;
    QLineEdit* mTxtY;

    CLineChart* mLineChart;
    CGLWidget* mGLWidget;

    void OnProcess();

    void closeEvent(QCloseEvent* event) override;

    // std::thread* mReadProcess;

    // void ReadProcess();

private slots:
    void OnOpen();
    void OnClose();
    void OnClear();
    void OnMouseMoveEvent(QMouseEvent* event);

public:
    CMainWindow();
    ~CMainWindow() override;
};