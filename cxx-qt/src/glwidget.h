#pragma once

// #include <GL/glew.h>

#include <core/vao.h>

#include <QOpenGLWidget>
#include <QQueue>

#include <mutex>

#include "rtlsdr.h"

class CGLWidget : public QOpenGLWidget
{
    Q_OBJECT

private:
    QAtomicInt mInit;

    QQueue<core::CVAOBuffer*> mQueue;

    std::mutex mMutex;

    GLdouble m_fovy;
    GLdouble m_aspect;
    GLdouble m_zNear;
    GLdouble m_zFar;

    GLfloat m_x_camera;
    GLfloat m_y_camera;
    GLfloat m_z_camera;

    GLvoid paintGL() override;
    GLvoid resizeEvent(QResizeEvent* event) override;
    GLvoid initializeGL() override;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

public slots:
    void OnUp();
    void OnDown();
    void OnLeft();
    void OnRight();
    void OnPlus();
    void OnMinus();

public:
signals:
    void mouseMoveEvent(QMouseEvent* event) override;

public:
    explicit CGLWidget(QWidget* parent);
    ~CGLWidget() override;

    GLvoid WriteBuffer(const uint8_t* data, const int& size);
    GLvoid CalculatePosition(GLint& x_mouse, GLint& y_mouse, GLdouble& x_real, GLdouble& y_real);
    GLvoid SetPerspective(const GLdouble& fovy, const GLdouble& aspect, const GLdouble& zNear, const GLdouble& zFar);
};