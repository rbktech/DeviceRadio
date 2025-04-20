#include "mainwindow.h"

#include <QCloseEvent>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>
#include <QTimer>

#include <thread>

#include "glwidget.h"
#include "math/math_frequency.h"

CMainWindow::CMainWindow()
    : QMainWindow()
    , mProcess(1)
{
    this->setWindowTitle("DeviceRadio " + QString(VERSION));

    QWidget* central = new QWidget(this);

    QPushButton* btnOpen = new QPushButton("Open", central);
    QPushButton* btnClose = new QPushButton("Close", central);
    mGLWidget = new CGLWidget(central);
    mTxtQueueSize = new QLineEdit("Text", central);
    mTxtX = new QDoubleSpinBox(central);
    mTxtY = new QDoubleSpinBox(central);

    mGLWidget->resize(800, 600);
    mTxtQueueSize->setReadOnly(true);
    mTxtX->setReadOnly(true);
    mTxtX->setRange(-2000, 2000);
    mTxtX->setButtonSymbols(QAbstractSpinBox::NoButtons);
    mTxtY->setReadOnly(true);
    mTxtY->setRange(-2000, 2000);
    mTxtY->setButtonSymbols(QAbstractSpinBox::NoButtons);

    QHBoxLayout* hBox = nullptr;
    QVBoxLayout* mainBox = new QVBoxLayout(central);

    hBox = new QHBoxLayout();
    {
        hBox->addWidget(btnOpen);
        hBox->addWidget(btnClose);
        hBox->addWidget(mTxtQueueSize);
    }
    mainBox->addLayout(hBox);

    hBox = new QHBoxLayout();
    {
        hBox->addWidget(mTxtX);
        hBox->addWidget(mTxtY);
    }
    mainBox->addLayout(hBox);

    mainBox->addWidget(mGLWidget);

    this->setCentralWidget(central);

    connect(btnOpen, &QPushButton::clicked, this, &CMainWindow::OnOpen);
    connect(btnClose, &QPushButton::clicked, this, &CMainWindow::OnClose);
    connect(mGLWidget, &CGLWidget::mouseMoveEvent, this, &CMainWindow::OnMouseMoveEvent);

    QTimer::singleShot(1, this, &CMainWindow::OnProcess);
}

CMainWindow::~CMainWindow()
{
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

                int x_mouse = point.x();
                int y_mouse = point.y();

                double x_real = 0.0;
                double y_real = 0.0;

                mGLWidget->CalculatePosition(x_mouse, y_mouse, x_real, y_real);

                mTxtX->setValue(x_real);
                mTxtY->setValue(y_real);
            }
        }
    }
}

void CMainWindow::OnProcess()
{
    int result = 0;
    uint8_t buffer[SIZE_RTL_SDR_BUFFER];

    while(mProcess == 1) {

        // std::this_thread::sleep_for(std::chrono::milliseconds(1));

        mTxtQueueSize->setText(QString::number(mRtlSdr.GetSizeQueue()));

        result = mRtlSdr.ReadBuffer(buffer);
        if(result == 0) {

            CMathFrequency::Imaginary(buffer, SIZE_RTL_SDR_BUFFER);

            mGLWidget->WriteBuffer(buffer, SIZE_RTL_SDR_BUFFER);
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