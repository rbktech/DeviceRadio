#include "glwidget.h"

#include <QMouseEvent>

#include <iostream>

#include "rtlsdr.h"

#define SIZE_MAX_QUEUE 100

#define DRAW_X (n + 0)
#define DRAW_Y (n + 1)
#define DRAW_Z (n + 2)

CGLWidget::CGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , mMouseButton(Qt::NoButton)
{
    connect(this, &CGLWidget::mouseMoveEvent, this, &CGLWidget::OnMouseMoveEvent);
    connect(this, &CGLWidget::wheelEvent, this, &CGLWidget::OnMouseWheelEvent);

    this->setMouseTracking(true);
}

CGLWidget::~CGLWidget()
{
    while(mQueue.isEmpty() == false) {
        std::cout << "";
    }

    for(auto& item : mMap) {
        if(item != nullptr) {
            delete item;
            item = nullptr;
        }
    }
}

void CGLWidget::paintGL()
{
    for(auto& item : mMap)
        if(item != nullptr)
            mQueue.push_back(*item);

    while(mQueue.isEmpty() == false) {
        int item = mQueue.front();
        std::cout << item << std::endl;
        mQueue.pop_front();
    }

    mGraph.Draw();
}

void CGLWidget::initializeGL()
{
    mGraph.Init(SCREEN_WIDTH, SCREEN_HEIGHT);
    mGraph.SetBackground(0x00, 0x31, 0x53);
    mGraph.SetGrid(SIZE_RTL_SDR_BUFFER, 500);
    mGraph.Create("frequency");
    mGraph.Set("frequency", glm::vec3 { 0.0f, 1.0f, 0.0f });
}

void CGLWidget::resizeGL(int w, int h)
{
    mGraph.SetSize(w, h);
}

QSize CGLWidget::sizeHint() const
{
    return this->size();
}

void CGLWidget::OnUp(const float& step)
{
    (void)step;
    this->update();
}

void CGLWidget::OnDown(const float& step)
{
    (void)step;
    this->update();
}

void CGLWidget::OnLeft(const float& step)
{
    (void)step;
    this->update();
}

void CGLWidget::OnRight(const float& step)
{
    (void)step;
    this->update();
}

void CGLWidget::OnPlus(const float& step)
{
    (void)step;
    this->update();
}

void CGLWidget::OnMinus(const float& step)
{
    (void)step;
    this->update();
}

void CGLWidget::mousePressEvent(QMouseEvent* e)
{
    mMouseButton |= e->button();
}

void CGLWidget::mouseReleaseEvent(QMouseEvent* e)
{
    mMouseButton &= ~(e->button());
}

bool CGLWidget::IsMousePresses(const QFlags<Qt::MouseButton>& mouseButton)
{
    return mMouseButton & mouseButton;
}

void CGLWidget::WriteBuffer(const RTL_DATA* data, const int& size)
{
    float* buffer = new float[size * 3];

    float x = 0.0f;
    for(int i = 0, n = 0; i < size; i++, n = i * 3) {
        buffer[DRAW_X] = (x += 0.01);
        buffer[DRAW_Y] = data[i];
        buffer[DRAW_Z] = 0.0f;
    }

    this->makeCurrent();

    mGraph.Set("frequency", buffer, size * 3);

    this->doneCurrent();

    this->update();

    delete[] buffer;
}

void CGLWidget::OnMouseWheelEvent(QWheelEvent* event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if(!numPixels.isNull() == true) {
    } else if(!numDegrees.isNull() == true) {
        QPoint numSteps = numDegrees / 15;
        mGraph.Scroll(numSteps.x(), numSteps.y());
        this->update();
    }

    event->accept();
}

void CGLWidget::OnMouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);

    if(event != nullptr) {

        if(event->type() == QEvent::MouseMove) {

            QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
            if(mouseEvent != nullptr) {

                QPoint point = mouseEvent->pos();

                float x_real = 0.0f;
                float y_real = 0.0f;

                const int x_mouse = point.x();
                const int y_mouse = point.y();

                mGraph.Cursor(x_mouse, y_mouse, x_real, y_real);

                this->setProperty("cursor_x", x_real);
                this->setProperty("cursor_y", y_real);

                switch(mMouseButton) {

                    case Qt::LeftButton: {
                        mGraph.Move(mXPosition - x_mouse, y_mouse - mYPosition);
                        break;
                    }
                    case Qt::RightButton:
                    case Qt::MiddleButton:
                    case Qt::NoButton: {
                        break;
                    }
                }

                this->update();

                mXPosition = x_mouse;
                mYPosition = y_mouse;
            }
        }
    }
}