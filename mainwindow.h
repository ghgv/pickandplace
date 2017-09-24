#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

};

#endif // MAINWINDOW_H
