#include "glwidget.h"

// #include <GL/glu.h>
// #include <GL/glew.h>

#include <QMouseEvent>
#include <QMutexLocker>

#include <cmath>

#include <core/error.h>
#include <core/initialization.h>
#include <graph/graph.h>

#include "rtlsdr.h"

#define SIZE_MAX_QUEUE 100
#define STEP 0.1f

CGLWidget::CGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , mInit(0)
    , m_fovy(0.0)
    , m_aspect(0.0)
    , m_zNear(0.0)
    , m_zFar(0.0)
    , m_x_camera(-150.0f)
    , m_y_camera(-100.0f)
    , m_z_camera(-500.0f)
{
    this->setMouseTracking(true);
}

CGLWidget::~CGLWidget()
{
    while(mQueue.isEmpty() == false) {

        core::CVAO* vao = mQueue.front();
        if(vao != nullptr) {
            delete vao;
            vao = nullptr;
        }
        mQueue.pop_front();
    }
}

void CGLWidget::paintGL()
{
    std::lock_guard<std::mutex> lck(mMutex);

    // DrawScene();
    e(glEnable(GL_DEPTH_TEST));
    e(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
    e(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    e(glClearColor(0.2f, 0.2f, 0.2f, 0.0f));

    // SettingView
    e(glMatrixMode(GL_PROJECTION));
    e(glLoadIdentity());
    // e(gluPerspective(m_fovy, m_aspect, m_zNear, m_zFar));
    graph::Perspective(m_fovy, m_aspect, m_zNear, m_zFar);

    // DrawObject();
    e(glMatrixMode(GL_MODELVIEW));

    // DrawCamera();
    e(glLoadIdentity());
    e(glTranslatef(m_x_camera, m_y_camera, m_z_camera));
    e(glRotatef(0.0f, 0.0f, 0.0f, 0.0f));

    // DrawObject();
    if(mInit == 1) {
        for(auto& vbo : mQueue)
            if(vbo != nullptr)
                vbo->Draw();
    }
}

void CGLWidget::initializeGL()
{
    GLenum result = 0;
    core::Init();
    if(result == 0) { // GLEW_OK
        mInit = 1;

        // InitScene();
        int maxVertexAttribs;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

        // InitObject();

        float color[3] = { 1.0f, 0.0f, 0.0f };
        float line[SIZE_RTL_SDR_BUFFER * 5] = { 0 };

        float x = 0.0f;
        for(int i = 0, n = 0; i < SIZE_RTL_SDR_BUFFER; i++, n = i * 5) {
            line[n + 0] = (x += 0.01);
            line[n + 1] = 50;
            line[n + 2] = color[0];
            line[n + 3] = color[1];
            line[n + 4] = color[2];
        }

        core::CVAOBuffer* vao_buffer = nullptr;
        vao_buffer = new core::CVAOBuffer();
        vao_buffer->SetDrawParams(GL_LINE_STRIP, 0, SIZE_RTL_SDR_BUFFER);
        vao_buffer->SetBuffer(nullptr, sizeof(line) / sizeof(line[0]));
        vao_buffer->EnableVertex(2, 5, 0);
        vao_buffer->EnableColor(3, 5, 2);

        mQueue.push_back(vao_buffer);
    }
}

void CGLWidget::resizeEvent(QResizeEvent* event)
{
    QOpenGLWidget::resizeEvent(event);

    QSize size = event->size();

    GLfloat w = (GLfloat)size.width();
    GLfloat h = (GLfloat)size.height();

    GLfloat aspect = w / h;

    SetPerspective(30.0f, aspect, 0.1f, 1000.0f);

    glViewport(0, 0, size.width(), size.height());
}

QSize CGLWidget::sizeHint() const
{
    return this->size();
}

void CGLWidget::OnUp()
{
    m_y_camera -= STEP * m_z_camera;
    this->update();
}

void CGLWidget::OnDown()
{
    m_y_camera += STEP * m_z_camera;
    this->update();
}

void CGLWidget::OnLeft()
{
    m_x_camera += STEP * m_z_camera;
    this->update();
}

void CGLWidget::OnRight()
{
    m_x_camera -= STEP * m_z_camera;
    this->update();
}

void CGLWidget::OnPlus()
{
    m_z_camera += STEP;
    this->update();
}

void CGLWidget::OnMinus()
{
    m_z_camera -= STEP;
    this->update();
}

void CGLWidget::mousePressEvent(QMouseEvent* e)
{
}

void CGLWidget::mouseReleaseEvent(QMouseEvent* e)
{
}

void CGLWidget::SetPerspective(const GLdouble& fovy,
    const GLdouble& aspect,
    const GLdouble& zNear,
    const GLdouble& zFar)
{
    m_fovy = fovy;
    m_aspect = aspect;
    m_zNear = zNear;
    m_zFar = zFar;
}

void CGLWidget::CalculatePosition(GLint& x_mouse, GLint& y_mouse, GLdouble& x_real, GLdouble& y_real)
{
    QSize wSize = this->size();

    int w = wSize.width();
    int h = wSize.height();

    int w2 = w >> 1;
    int h2 = h >> 1;

    y_mouse = h - y_mouse;

    double K = h2 / (m_z_camera * std::tan(m_fovy / 2 * M_PI / 180.0));

    x_real = (x_mouse - w2 + m_x_camera * K) / K;
    y_real = (y_mouse - h2 + m_y_camera * K) / K;
}

void CGLWidget::WriteBuffer(const uint8_t* data, const int& size)
{
    float color[3] = { 0.0f, 1.0f, 0.0f };
    float* buffer = new float[size * 5];

    float x = 0.0f;
    for(int i = 0, n = 0; i < size; i++, n = i * 5) {
        buffer[n + 0] = (x += 0.01);
        buffer[n + 1] = data[i];
        buffer[n + 2] = color[0];
        buffer[n + 3] = color[1];
        buffer[n + 4] = color[2];
    }

    std::lock_guard<std::mutex> lck(mMutex);

    if(mQueue.isEmpty() == false) {

        core::CVAOBuffer* vao = mQueue.front();
        if(vao != nullptr) {
            vao->SetBuffer(buffer, size * 5);
        }
    }

    delete[] buffer;

    this->update();
}