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
    int ArduCAMreceived();
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
    void on_A1_clicked();
    void on_B_clicked();
    void on_C_clicked();
    void on_Leds_ON_clicked();
    void on_Leds_OFF_clicked();
    void on_Motors17_clicked();
    void on_Motors18_clicked();
    void on_CameraPort_clicked();
    void on_bCapture_clicked();
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_TopCameraButton_clicked();
    void on_BottomCameraButton_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_Pick_603_clicked();
    void on_Place_603_clicked();
    void on_Release_clicked();
    void on_A_clicked();
    void on_sendButton_2_clicked(bool checked);
};

#endif // MAINWINDOW_H
