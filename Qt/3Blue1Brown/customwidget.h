#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QGLWidget>

class CustomWidget : public QGLWidget
{

public:
    explicit CustomWidget(QWidget *parent = 0);

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

/*signals:

public slots:*/
};

#endif // CUSTOMWIDGET_H
