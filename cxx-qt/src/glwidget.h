#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <QDate>
#include <QMap>
#include <QOpenGLWidget>
#include <QQueue>

#include <mutex>

#include <graph/graph.h>

#include "math/math_frequency.h"
#include "rtlsdr.h"

#define SCREEN_WIDTH 800.0f
#define SCREEN_HEIGHT 600.0f

#define STEP 0.1f

class CGLWidget : public QOpenGLWidget
{
    Q_OBJECT

private:
    QQueue<int> mQueue;
    QMap<QString, int*> mMap;

    graph::CGraph mGraph;

    // std::mutex m_mutex; // std::lock_guard<std::mutex> lck(m_mutex);

    QFlags<Qt::MouseButton> mMouseButton;

    int mXPosition = 0.0f;
    int mYPosition = 0.0f;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    QSize sizeHint() const override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

public slots:
    void OnUp(const float& step = STEP);
    void OnDown(const float& step = STEP);
    void OnLeft(const float& step = STEP);
    void OnRight(const float& step = STEP);
    void OnPlus(const float& step = STEP);
    void OnMinus(const float& step = STEP);

    void OnMouseMoveEvent(QMouseEvent* event);
    void OnMouseWheelEvent(QWheelEvent* event);

public:
signals:
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

public:
    explicit CGLWidget(QWidget* parent);
    ~CGLWidget() override;

    GLvoid WriteBuffer(const RTL_DATA* data, const int& size);

    bool IsMousePresses(const QFlags<Qt::MouseButton>& mouseButton);
};