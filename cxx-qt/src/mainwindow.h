#pragma once

#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QMainWindow>

#include "glwidget.h"

class CMainWindow : public QMainWindow
{
    Q_OBJECT

private:
    CRtlSdr mRtlSdr;
    QAtomicInt mProcess;
    QLineEdit* mTxtQueueSize;
    QDoubleSpinBox* mTxtX;
    QDoubleSpinBox* mTxtY;

    CGLWidget* mGLWidget;

    void OnProcess();

    void closeEvent(QCloseEvent* event) override;

    void ImaginaryFrequency(const uint8_t* data, const int& size);

private slots:
    void OnOpen();
    void OnClose();
    void OnMouseMoveEvent(QMouseEvent* event);

public:
    CMainWindow();
    ~CMainWindow() override;
};