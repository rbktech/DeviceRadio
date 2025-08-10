#include "mainwindow.h"

#include <QCloseEvent>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>
#include <QTimer>

#include <iostream>
#include <thread>

#include "glwidget.h"

#include <core/matrixglm.h>

CMainWindow::CMainWindow()
    : QMainWindow()
    , mProcess(1)
{
    this->setWindowTitle("DeviceRadio " + QString(VERSION));

    QWidget* central = new QWidget(this);

    QPushButton* btnOpen = new QPushButton("Open", central);
    QPushButton* btnClose = new QPushButton("Close", central);
    QPushButton* btnClear = new QPushButton("Clear", central);
    mGLWidget = new CGLWidget(central);
    mLineChart = new CLineChart(central);
    mTxtQueueSize = new QLineEdit("Text", central);
    mTxtX = new QLineEdit(central);
    mTxtY = new QLineEdit(central);

    mGLWidget->resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    mLineChart->resize(SCREEN_WIDTH, 300.0f);
    mTxtQueueSize->setReadOnly(true);
    mTxtX->setReadOnly(true);
    mTxtY->setReadOnly(true);

    QHBoxLayout* hBox = nullptr;
    QVBoxLayout* mainBox = new QVBoxLayout(central);

    hBox = new QHBoxLayout();
    {
        hBox->addWidget(btnOpen);
        hBox->addWidget(btnClose);
        hBox->addWidget(btnClear);
        hBox->addWidget(mTxtQueueSize);
        hBox->addWidget(mTxtX);
        hBox->addWidget(mTxtY);
    }
    mainBox->addLayout(hBox);

    mainBox->addWidget(mLineChart);
    mainBox->addWidget(mGLWidget);

    this->setCentralWidget(central);

    connect(btnOpen, &QPushButton::clicked, this, &CMainWindow::OnOpen);
    connect(btnClose, &QPushButton::clicked, this, &CMainWindow::OnClose);
    connect(btnClear, &QPushButton::clicked, this, &CMainWindow::OnClear);
    connect(mGLWidget, &CGLWidget::mouseMoveEvent, this, &CMainWindow::OnMouseMoveEvent);

    QTimer::singleShot(1, this, &CMainWindow::OnProcess);

    int result = 0;
    result += mRtlSdr.SetCenterFrequency(908.5e6); // 100.3e6
    result += mRtlSdr.SetSampleRate(1.024e6);
    result += mRtlSdr.SetAutoGain();
    result += mRtlSdr.SetBandwidth(3.0e6);
    std::cout << "Set RTL STD: " << result << std::endl;

    // mReadProcess = new std::thread(&CMainWindow::ReadProcess, this);
}

CMainWindow::~CMainWindow()
{
    // mReadProcess->join();
    mRtlSdr.Close();
}

void CMainWindow::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);

    if(mProcess == 1) {

        QString title = tr("Подтверждение закрытия");
        QString text = tr("Вы уверены, что хотите закрыть окно?");
        auto buttons = QMessageBox::Yes | QMessageBox::No;
        auto defaultButton = QMessageBox::No;
        QMessageBox::StandardButton result = QMessageBox::question(this, title, text, buttons, defaultButton);
        if(result == QMessageBox::Yes) {
            mProcess = 0;
        }

        event->ignore();
    } else
        event->accept();
}

void CMainWindow::OnMouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);

    if(event != nullptr) {

        if(event->type() == QEvent::MouseMove) {

            QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
            if(mouseEvent != nullptr) {

                QPoint point = mouseEvent->pos();

                const int x_mouse = point.x();
                const int y_mouse = point.y();

                float x_real = mGLWidget->property("cursor_x").toFloat();
                float y_real = mGLWidget->property("cursor_y").toFloat();

                mTxtX->setText(QString("X: %1 = %2").arg(x_mouse).arg(core::p(x_real)));
                mTxtY->setText(QString("Y: %1 = %2").arg(y_mouse).arg(core::p(y_real)));

                mLineChart->AddValue(x_mouse, y_mouse, Qt::red);
                mLineChart->AddValue(x_real, y_real, Qt::green);
            }
        }
    }
}

void CMainWindow::OnClear()
{
    mLineChart->Clear();
}

/*void CMainWindow::ReadProcess()
{
    while(mProcess == 1) {}
}*/

void CMainWindow::OnProcess()
{
    int result = 0;

    RTL_DATA buffer[SIZE_RTL_SDR_BUFFER] = { 0 };
    // RTL_DATA REX[SIZE_RTL_SDR_BUFFER] = { 0 };
    // RTL_DATA IMX[SIZE_RTL_SDR_BUFFER] = { 0 };

    while(mProcess == 1) {

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        mTxtQueueSize->setText(QString::number(mRtlSdr.GetSizeQueue()));

        result = mRtlSdr.ReadBuffer(buffer);
        if(result == 0) {

            mGLWidget->WriteBuffer(buffer, SIZE_RTL_SDR_BUFFER);

            // CMathFrequency::Imaginary(buffer, SIZE_RTL_SDR_BUFFER, REX, IMX);
            // CMathFrequency::DTF(REX, IMX, SIZE_RTL_SDR_BUFFER);
            // mGLWidget->WriteBuffer(REX, SIZE_RTL_SDR_BUFFER);
            // mGLWidget->WriteGauss(REX, SIZE_RTL_SDR_BUFFER);
            // mGLWidget->WriteFFT(buffer, SIZE_RTL_SDR_BUFFER);
        }

        QCoreApplication::processEvents();
    }

    this->close();
}

void CMainWindow::OnOpen()
{
    mRtlSdr.Open();
}

void CMainWindow::OnClose()
{
    mRtlSdr.Close();
}