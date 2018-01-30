#include "glwidget.h"
#include <GL/glut.h>
#include <GL/glu.h>

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    connect(&timer,SIGNAL(timeout()),this,SLOT(updateGL()));
    timer.start(20);
}

void GLWidget::initializeGL()
{
    glClearColor(0.2,0.2,0.2,1);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0,0.0,1.0);
    glRotatef(1,0.2,0.2,0.2);
    glutWireSphere(0.8,15,15);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0.0,0.0,h,h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,1, 0,0,0, 0,1,0);
}
