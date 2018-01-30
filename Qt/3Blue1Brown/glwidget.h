#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QObject>
#include <QGLWidget>
#include <QTimer>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);

    void initializeGL();
    void paintGL();
    void resizeGL(int h, int w);

private:
    QTimer timer;

};


#endif // GLWIDGET_H
