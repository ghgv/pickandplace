#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QGraphicsScene>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene = new QGraphicsScene ();

private slots:
    void Zup();
    void Zdown();
    void Xleft();
    void Xright();
    void Yplus();
    void Yminus();
    void portconnect();
    void Get();
    void Release();
    void rotorright();
    void rotorleft();
    void zoom();
    int serialreceived();
    void command();
    void handleFrame(QImage);
    void Slider(int);
    void SetOrigin();
    void Goto();
    void Pick();
    void Place();
    void HomeX();
    void HomeY();
    void HomeZ();
    void BMP_GetPixelRGB(QImage imageObject,int x,int y, int  *r1, int *g1, int *b1);
    void BMP_SetPixelRGB(QImage imageObject,int x,int y,int  r1,int g1,int  b1);


    void on_PLCC28_clicked();
    void on_TQFP44_clicked();
};

#endif // MAINWINDOW_H
