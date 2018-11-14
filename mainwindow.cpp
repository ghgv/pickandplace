#include <QApplication>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QCamera>
#include <qtDebug>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTimer>
#include <string.h>
#include <QtMath>
#include <vector>
#include <QSerialPortInfo>
#include <windows.h>
#include "cameraframegrabber.h"
#include "rectangle.h"
#include "serial.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace std;


QSerialPort *serial,*bCamera;
QVSerialPort *Kam;
QString str,deltaf;
QCamera *mCamera;
QCameraViewfinder *mCameraViewfinder;
QVBoxLayout *mLayout;
float camera_angle=0;
float offsetX=-42.1;
float offsetY=-6;
float offsetZ=3;
float package_width=12.573;
float package_large=12.573;
float delta=0.1;
std::vector<unsigned char> buffer (409600);
QByteArray buffy;
bool is_header=false;
bool topCamera=true;
bool bottomCamera=false;

QCamera *_camera;
CameraFrameGrabber *_cameraFrameGrabber;

float X,Y,Z;
float X1,Y1,X2,Y2,X3,Y3;
float catOp=0,hypo=1,desfase=0;
float W=0;//positions of the grip

int   r1, g1, b1;
int   ra, ga, ba;
int   rb, gb, bb;
int   rc, gc, bc;
int   rd, gd, bd;
int   re, ge, be;
int   rf, gf, bf;
int   rg, gg, bg;
int   rh, gh, bh;
int   ri, gi, bi;
float zr,zg,zb;

struct blur
{
    float a=2,b=2,c=2;
    float d=2,e=4,f=2;
    float g=2,h=2,i=2;
    float div=(a+b+c+d+e+f+g+h+i);
};

struct sobelx
{
    float a=-1,b=0,c=1;
    float d=-2,e=0,f=2;
    float g=-1,h=0,i=1;
};

struct sobely
{
    float a=-1,b=-2,c=-1;
    float d=0,e=0,f=0;
    float g=1,h=2,i=1;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //cv::Mat srcImage = cv::imread("C://Users//German//Documents//programacion//serialarduino//1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat srcImage = cv::imread("C://Users//German//Documents//programacion//serialarduino//1.jpg", 1);
    //Then define your mask image
    cv::Mat mask = cv::Mat::zeros(srcImage.size(), srcImage.type());
    //Define your destination image
    cv::Mat dstImage = cv::Mat::zeros(srcImage.size(), srcImage.type());
    //I assume you want to draw the circle at the center of your image, with a radius of 50
    cv::circle(mask, cv::Point(srcImage.rows/2, srcImage.cols/2), 100, cv::Scalar(255, 255, 255), -1, 8, 0);
    //Now you can copy your source image to destination image with masking
    srcImage.copyTo(dstImage, mask);

    //cv::Mat image = cv::imread("C://Users//German//Documents//programacion//serialarduino//1.jpg", 1);
    cvtColor(dstImage, dstImage, CV_BGR2RGB);
    ui->pic->setPixmap(QPixmap::fromImage(QImage(dstImage.data, dstImage.cols, dstImage.rows, dstImage.step, QImage::Format_RGB888)));

    ui->PosX->setText("0");
    ui->PosY->setText("0");
    ui->PosZ->setText("0");
    ui->PosW->setText("0");
    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    QSerialPortInfo port;
    foreach(port, com_ports)
    {
        ui->PortcomboBox->addItem(port.portName());
    }
    bCamera= new QSerialPort(this);
    Kam=new QVSerialPort(this);
    serial= new QSerialPort(this);
    str=ui->port->text();
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    serial->setPortName(str);

    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);




    connect(ui->Zup, SIGNAL(pressed()),this, SLOT(Zup( )));
    connect(ui->Zdown, SIGNAL(pressed()),this, SLOT(Zdown( )));
    connect(ui->Yplus, SIGNAL(pressed()),this, SLOT(Yplus( )));
    connect(ui->Yminus, SIGNAL(pressed()),this, SLOT(Yminus( )));
    connect(ui->Xright, SIGNAL(pressed()),this, SLOT(Xright( )));
    connect(ui->Xleft, SIGNAL(pressed()),this, SLOT(Xleft( )));
    connect(ui->Get, SIGNAL(pressed()),this, SLOT(Get( )));
    connect(ui->Release, SIGNAL(pressed()),this, SLOT(Release( )));
    connect(ui->rotorright, SIGNAL(pressed()),this, SLOT(rotorright( )));
    connect(ui->rotorleft, SIGNAL(pressed()),this, SLOT(rotorleft( )));
    connect(serial,SIGNAL(readyRead()),this,SLOT(serialreceived()));
    connect(bCamera,SIGNAL(readyRead()),this,SLOT(ArduCAMreceived()));
    connect(ui->zoom,SIGNAL(pressed()),this,SLOT(zoom()));
    connect(ui->sendButton, SIGNAL(pressed()),this, SLOT(command( )));
    connect(ui->slider, SIGNAL(valueChanged(int)), this, SLOT(Slider(int)));
    connect(ui->SetOrigin, SIGNAL(pressed()), this, SLOT(SetOrigin()));
    connect(ui->Goto, SIGNAL(pressed()), this, SLOT(Goto()));
    connect(ui->Pick, SIGNAL(pressed()), this, SLOT(Pick()));
    connect(ui->Place, SIGNAL(pressed()), this, SLOT(Place()));
    connect(ui->HomeX, SIGNAL(pressed()), this, SLOT(HomeX()));
    connect(ui->HomeY, SIGNAL(pressed()), this, SLOT(HomeY()));
    connect(ui->HomeZ, SIGNAL(pressed()), this, SLOT(HomeZ()));

    _camera = new QCamera();
    _cameraFrameGrabber = new CameraFrameGrabber();
    _camera->setViewfinder(_cameraFrameGrabber);
    connect(_cameraFrameGrabber, SIGNAL(frameAvailable(QImage)), this, SLOT(handleFrame(QImage)));
    _camera->start();


    serial->write("G92 X0 Y0 X0\r");


}

MainWindow::~MainWindow()
{
    delete ui;
       serial->close();
}

void MainWindow::Zup()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    Z=Z+delta;
    QString s= QString::number(Z);
    s="G1 Z"+s+"\r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
    ui->PosZ->setText(s);
}

void MainWindow::Zdown()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    Z=Z-delta;
    QString s= QString::number(Z);
    ui->PosZ->setText(s);
    s="G1 Z"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;

}

void MainWindow::Yminus()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    Y=Y-delta;
    QString s= QString::number(Y);
    ui->PosY->setText(s);
    s="G1 Y"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;

}

void MainWindow::Yplus()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    Y=Y+delta;
    QString s= QString::number(Y);
    ui->PosY->setText(s);
    s="G1 Y"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Xleft()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    X=X-delta;
    QString s= QString::number(X);
    ui->PosX->setText(s);
    s="G1 X"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Xright()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    X=X+delta;
    QString s= QString::number(X);
    ui->PosX->setText(s);
    s="G1 X"+s+"\r";
     serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Get()
{

     //serial->write("M106\r"); For Marlin
     serial->write("M3\r"); //Laser/Pump On

}
void MainWindow::Release()
{
    // serial->write("M107\r"); For Marlin
    serial->write("M4\r"); //Laser/Pump Off
}

void MainWindow::rotorright()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    W=W+delta;
    camera_angle=W;
    QString s= QString::number(W);
    ui->PosW->setText(s);
    s="G1 E"+s+"\r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}
void MainWindow::rotorleft()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    W=W-delta;
    camera_angle=W;
    QString s= QString::number(W);
    ui->PosW->setText(s);
    s="G1 E"+s+"\r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::portconnect()
{
    str=ui->port->text();
    serial->setPortName(str);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);


}


int MainWindow::serialreceived()
{

    QByteArray data = serial->readAll();
    ui->Messages->appendPlainText(data);

   // parse((char *)data.constData());

// printf("command %s %s %s %s %s %s\n",argv[0],argv[1],argv[2],argv[3],argv[4],argv[5]);
    printf("%s",(char *)data.constData());

 }

int MainWindow::ArduCAMreceived()
{
    int n=bCamera->bytesAvailable();
    QByteArray data = bCamera->read(n);
    buffy.append(data);

    if (buffy.size() >= 2)
     {
         if (((unsigned char)buffy.data()[0] == 0xFF) && ((unsigned char)buffy.data()[1] == 0xD8) && (is_header == false))
         {
             is_header = true;
                     ui->Messages->appendPlainText("JPEG header found\n");
             //break;
         }
         else if (((unsigned char)buffy.data()[buffy.size() - 2] == 0xFF) && ((unsigned char)buffy.data()[buffy.size() - 1] == 0xD9) && (is_header == true))
         {
             //Display the image data in the picture box
             //DispPictureBox.Image = BytesToBitmap(buffer);
             QImage foto;
             foto.loadFromData((unsigned char *)buffy.constData(),buffy.size(),"JPG");
             ui->Messages->appendPlainText("tail");
             ui->pic->setPixmap(QPixmap::fromImage(foto));


             ///Pipeline
             ///
             QImage temp = foto.copy();

             cv::Mat res(temp.height(),temp.width(),CV_8UC4,(uchar*)temp.bits(),temp.bytesPerLine());
             cvtColor(res, res,CV_RGB2BGR);
             cvtColor(res, res,CV_BGR2RGB);
             //cv::Mat srcImage = cv::imread("C://Users//German//Documents//programacion//serialarduino//1.jpg", 1);
             //Then define your mask image
             cv::Mat mask = cv::Mat::zeros(res.size(), res.type());
             cv::Mat boxy = cv::Mat::zeros(res.size(), res.type());
             cv::Mat dstImage = cv::Mat::zeros(res.size(), res.type());
             cv::circle(mask, cv::Point(640/2, 480/2), 200, cv::Scalar(255, 255, 255), -1, 8, 0);
             //Now you can copy your source image to destination image with masking
             res.copyTo(dstImage, mask);
             cv::imwrite( "C://Users//German//Documents//programacion//serialarduino//Orig.jpg",dstImage );
             cvtColor(dstImage,dstImage, CV_BGR2HSV_FULL);//CV_BGR2HSV
             //cvtColor(dstImage,dstImage, CV_BGR2HSV);//CV_BGR2HSV

             cv::Mat imgThreshold = cv::Mat::zeros(res.size(), res.type());
             cv::Mat img2 = cv::Mat::zeros(res.size(), res.type());
             //cv::Mat imgThreshold= cv::imread("C://Users//German//Documents//programacion//serialarduino//2.jpg", 1);
             //cv::inRange(dstImage, cv::Scalar(60-15,61, 100),cv::Scalar(60+15, 100,255), imgThreshold); //GOOD
             cv::inRange(dstImage, cv::Scalar(97,4, 0),cv::Scalar(185, 255,255), imgThreshold); //GOOD
             unsigned char h,s,v;
             unsigned char  hh,ss,vv;
             h=ui->HL->tickPosition();
             s=ui->SL->tickPosition();
             v=ui->HL->tickPosition();
             hh=ui->HU->tickPosition();
             ss=ui->SU->tickPosition();
             vv=ui->HU->tickPosition();
             ui->Messages->appendPlainText("hh: ");
             ui->Messages->appendPlainText(QString::number(hh));
             //cv::inRange(dstImage, cv::Scalar(h,s,v),cv::Scalar(hh, ss,vv), imgThreshold);
             cv::imwrite( "C://Users//German//Documents//programacion//serialarduino//Threshold.jpg",imgThreshold );
             cv::imwrite( "C://Users//German//Documents//programacion//serialarduino//3.jpg",dstImage );
             res.copyTo(dstImage, imgThreshold);

             std::vector<cv::Point> points;
             cv::Mat_<uchar>::iterator it = imgThreshold.begin<uchar>();
             cv::Mat_<uchar>::iterator end = imgThreshold.end<uchar>();
             for (; it != end; ++it)
               if (*it)
                 points.push_back(it.pos());
             cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

             double angle = box.angle;
             cv::Point2f center= box.center;
             if (angle < -45.){
               angle += 90.;
               ui->Messages->appendPlainText("Angle: ");
               ui->Messages->appendPlainText(QString::number(angle));
               qDebug()<<"Angle"<<angle;
               ui->Messages->appendPlainText("Center x: ");
               ui->Messages->appendPlainText(QString::number(center.x));
               ui->Messages->appendPlainText("Center y: ");
               ui->Messages->appendPlainText(QString::number(center.y));
               qDebug()<<"Center X: "<<center.x;
               qDebug()<<"Center Y: "<<center.y;
             }
             else{
                 ui->Messages->appendPlainText("Angle: ");
                 ui->Messages->appendPlainText(QString::number(angle));
                 qDebug()<<"Angle"<<angle;
                 ui->Messages->appendPlainText("Center x: ");
                 ui->Messages->appendPlainText(QString::number(center.x));
                 ui->Messages->appendPlainText("Center y: ");
                 ui->Messages->appendPlainText(QString::number(center.y));
                 qDebug()<<"Center X: "<<center.x;
                 qDebug()<<"Center Y: "<<center.y;
             }

             cv::Point2f vertices[4];
             box.points(vertices);
             for(int i = 0; i < 4; ++i){
                 cv::line(boxy, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0), 1, CV_AA);
                 cv::line(dstImage, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 255, 0), 3, CV_AA);
                 }
             cv::imwrite( "C://Users//German//Documents//programacion//serialarduino//boxy.jpg",boxy );

             cvtColor(dstImage,dstImage, CV_HSV2BGR_FULL);
             //cvtColor(dstImage, dstImage, CV_BGR2RGB);
             if(bottomCamera)
             ui->pic->setPixmap(QPixmap::fromImage(QImage(dstImage.data, dstImage.cols, dstImage.rows, dstImage.step, QImage::Format_RGB888)));


             ///
             is_header = false;
             buffy.resize(0);
         }
         else if (is_header == false)
         {
             ui->Messages->appendPlainText(data);
             buffy.resize(0);
         }
     }
 }


void MainWindow::zoom(){
    if(mCameraViewfinder->width() != 1600)
    {
        mCameraViewfinder->setGeometry(-590,-490,1600,1440);  // zoom in
    }
    else
    {
        mCameraViewfinder->setGeometry(0,0,375,380);		// zoom out
        //mCameraViewfinder->setGeometry(375,380,0,0);		// zoom out
    }
}

void MainWindow::handleFrame(QImage imageObject)
{
    int width=640;
    int height=480;
    int r1,g1,b1,a1;
    struct blur Blur;
    struct sobelx Sobelx;
    struct sobely Sobely;
    int n,m;
    rectangle *Rect = new rectangle(package_width,package_large,camera_angle,width,height);

    QRgb value;
    QColor colors;
    QPoint position;
    QTransform matrix(-1,0,0,0,1,0,0,0,1); // m11=-1 : flipping horizontal. I do not know why
    value = qRgb(100, 0, 0); // 0xffbd9527
    imageObject=imageObject.transformed(matrix);
    imageObject=imageObject.copy();




    int y,x;
    float as;
    for ( y=0;y<height;++y) {

                 as=(float)y/(float)(height);
                 imageObject.setPixel(width/2, 480/2-height/2+y, value);
                 Rect->segmentb(as,&m,&n);
                 if(ui->BnW->isChecked())
                     value=qRgb(255,255,255);
                 imageObject.setPixel(m,n, value);
                 Rect->segmentd(as,&m,&n);
                 imageObject.setPixel(m,n, value);


           }
    for ( x=0;x<width;++x) {
                as=(float)x/(float)(width);
                imageObject.setPixel(width/2-width/2+x, height/2, value);
                Rect->segmenta(as,&m,&n);
                if(ui->BnW->isChecked())
                    value=qRgb(255,255,255);
                imageObject.setPixel(m,n, value);
                Rect->segmentc(as,&m,&n);
                imageObject.setPixel(m,n, value);

           }

   QPainter p;
     if (!p.begin(&imageObject))
     {
     }
        p.setPen(QPen(Qt::white));
        p.setFont(QFont("Arial",  12, QFont::Bold));
        QString sx= QString::number((float)X);
        QString sy= QString::number((float)Y);
        QString sz= QString::number((float)Z);
        QString sw= QString::number((float)W);
        QString s;
        s="X:"+sx+" Y:"+sy+" Z:"+sz+" W:"+sw;
        p.drawText(imageObject.rect(), Qt::AlignTop,s);

    QImage Image2=imageObject.copy();

        //Blur? Corregir el uso de la matriz en el calculo
        if(ui->BLUR->isChecked()){
          for (y=1;y<(height-1);++y )
          {
              for (x=1;x<(width-1);++x)
              {
                  /* Get pixel's RGB values */
                  position.setX(x-1); position.setY(y-1); colors=Image2.pixelColor(position); colors.getRgb(&ra,&ga,&ba,&a1);
                  position.setX(x); position.setY(y-1); colors=Image2.pixelColor(position); colors.getRgb(&rb,&gb,&bb,&a1);
                  position.setX(x+1); position.setY(y-1); colors=Image2.pixelColor(position); colors.getRgb(&rc,&gc,&bc,&a1);
                  position.setX(x-1); position.setY(y); colors=Image2.pixelColor(position); colors.getRgb(&rd,&gd,&bd,&a1);
                  position.setX(x); position.setY(y); colors=Image2.pixelColor(position); colors.getRgb(&re,&ge,&be,&a1);
                  position.setX(x+1); position.setY(y); colors=Image2.pixelColor(position); colors.getRgb(&rf,&gf,&bf,&a1);
                  position.setX(x-1); position.setY(y+1); colors=Image2.pixelColor(position); colors.getRgb(&rg,&gg,&bg,&a1);
                  position.setX(x); position.setY(y+1); colors=Image2.pixelColor(position); colors.getRgb(&rh,&gh,&bh,&a1);
                  position.setX(x+1); position.setY(y+1); colors=Image2.pixelColor(position); colors.getRgb(&ri,&gi,&bi,&a1);

                  zr=(Blur.a*ra+Blur.b*rb+Blur.c*rc+Blur.d*rd+Blur.e*re+Blur.f*rf+Blur.g*rg+Blur.h*rh+Blur.i*ri)/Blur.div;
                  zg=(Blur.a*ga+Blur.b*gb+Blur.c*gc+Blur.d*gd+Blur.e*ge+Blur.f*gf+Blur.g*gg+Blur.h*gh+Blur.i*gi)/Blur.div;
                  zb=(Blur.a*ba+Blur.b*bb+Blur.c*bc+Blur.d*bd+Blur.e*be+Blur.f*bf+Blur.g*bg+Blur.h*bh+Blur.i*bi)/Blur.div;
                  /* Invert RGB values */
                  value = qRgb((int)zr,(int)zg ,(int)zb);
                  imageObject.setPixel(x,y, value);
              }

              }
            }


        if(ui->BnW->isChecked()){

           for (y=0;y<480;++y) // setting to gray
               for(x=0;x<640;++x)
               {
                   position.setX(x);
                   position.setY(y);
                   colors=imageObject.pixelColor(position);
                   colors.getRgb(&r1,&g1,&b1,&a1);
                   value = qRgb((r1+g1+b1)/3, (r1+g1+b1)/3,(r1+g1+b1)/3);
                   imageObject.setPixel(x,y, value);

               }
        }

        QImage Image3=imageObject.copy();

        float val;
        if(ui->SOBEL->isChecked()){
          for (y=1;y<(480-1);++y )
          {
              for (x=1;x<(640-1);++x)
              {
                  /* Get pixel's RGB values */
                  position.setX(x-1); position.setY(y-1); colors=Image3.pixelColor(position); colors.getRgb(&ra,&ga,&ba,&a1);
                  position.setX(x); position.setY(y-1); colors=Image3.pixelColor(position); colors.getRgb(&rb,&gb,&bb,&a1);
                  position.setX(x+1); position.setY(y-1); colors=Image3.pixelColor(position); colors.getRgb(&rc,&gc,&bc,&a1);
                  position.setX(x-1); position.setY(y); colors=Image3.pixelColor(position); colors.getRgb(&rd,&gd,&bd,&a1);
                  position.setX(x); position.setY(y); colors=Image3.pixelColor(position); colors.getRgb(&re,&ge,&be,&a1);
                  position.setX(x+1); position.setY(y); colors=Image3.pixelColor(position); colors.getRgb(&rf,&gf,&bf,&a1);
                  position.setX(x-1); position.setY(y+1); colors=Image3.pixelColor(position); colors.getRgb(&rg,&gg,&bg,&a1);
                  position.setX(x); position.setY(y+1); colors=Image3.pixelColor(position); colors.getRgb(&rh,&gh,&bh,&a1);
                  position.setX(x+1); position.setY(y+1); colors=Image3.pixelColor(position); colors.getRgb(&ri,&gi,&bi,&a1);
                  zr=(Sobely.a*ra+Sobely.b*rb+Sobely.c*rc+
                      Sobely.d*rd+Sobely.e*re+Sobely.f*rf+
                      Sobely.g*rg+Sobely.h*rh+Sobely.i*ri);
                  zg=(Sobelx.a*ra+Sobelx.b*rb+Sobelx.c*rc+
                      Sobelx.d*rd+Sobelx.e*re+Sobelx.f*rf+
                      Sobelx.g*rg+Sobelx.h*rh+Sobelx.i*ri);
                  val=sqrt(zr*zr+zg*zg)/8;
                  val=(val<60)?255:0;
                  value = qRgb(val,val ,val);
                  imageObject.setPixel(x,y, value);
              }

              }
            }


    QPixmap pixmap = QPixmap::fromImage(imageObject);
    if(topCamera)
        ui->pic->setPixmap(pixmap);
    this->update();

}

void MainWindow::BMP_GetPixelRGB(QImage imageObject,int x,int y,int *r1,int *g1,int *b1){
    QPoint position;
    int a1;
    QColor colors;
    position.setX(x);
    position.setY(y);
    colors=imageObject.pixelColor(position);
    colors.getRgb(r1,g1,b1,&a1);
}

void MainWindow::BMP_SetPixelRGB(QImage imageObject,int x,int y,int  r1,int g1,int  b1){
    QPoint position;
    QRgb value;
    QColor colors;
    position.setX(x);
    position.setY(y);
    value = qRgb((r1+g1+b1)/3, (r1+g1+b1)/3,(r1+g1+b1)/3);
    imageObject.setPixel(x,y, value);
}


void MainWindow::command()
{
    QString s ;
    s=ui->command->text();
    s=s+"\r";
    serial->write(s.toStdString().c_str());
}

void MainWindow::Slider(int a)
{
    camera_angle=(float)a;
    W=camera_angle;
    QString s= QString::number(W);
    ui->PosW->setText(s);
    s="G1 E"+s+"\r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::SetOrigin()
{
    X=Y=Z=W=0;
    serial->write("G92 X0 Y0 Z0\r");
    qDebug()<<"Setting origin at: X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
    ui->PosX->setText("0");
    ui->PosY->setText("0");
    ui->PosZ->setText("0");
    ui->PosW->setText("0");

}

void MainWindow::Goto()
{
    QString s= QString::number(X);
    ui->PosX->setText(s);
    s="G1 X"+ui->GotoX->text()+" Y"+ui->GotoY->text()+" \r";
    X=ui->GotoX->text().toFloat();
    Y=ui->GotoY->text().toFloat();
    qDebug()<<"Going to "<<s.toStdString().c_str();
    serial->write(s.toStdString().c_str());


}

void MainWindow::Pick()
{
    QString s;
    float x,y,z;
    x=offsetX+X;
    y=offsetY+Y;
    z=offsetZ;
    QString m = QString::number(x);
    QString n = QString::number(y);
    QString p=  QString::number(z);
    s="G1 X"+m+" Y"+n+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going to pick at:"<<s.toStdString().c_str();
    s="G1 Z"+p+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going down to pick on:"<<s.toStdString().c_str();
    serial->write("M3\r");//Pump On
    x=x-offsetX;
    y=y-offsetY;
    z=Z;
    m = QString::number(x);
    n = QString::number(y);
    p = QString::number(z);
    s="G1 Z"+p+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going up.."<<s.toStdString().c_str();

    s="G1 X"+m+" Y"+n+" \r";
    qDebug()<<"Returning "<<s.toStdString().c_str();
    serial->write(s.toStdString().c_str());



}

void MainWindow::Place()
{
    QString s;
    float x,y,z;
    x=offsetX+X;
    y=offsetY+Y;
    z=offsetZ;
    QString m = QString::number(x);
    QString n = QString::number(y);
    QString p=  QString::number(z);
    s="G1 X"+m+" Y"+n+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going to pick at:"<<s.toStdString().c_str();
    s="G1 Z"+p+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going down to pick on:"<<s.toStdString().c_str();
    serial->write("M4\r");//Pump Off
    x=x-offsetX;
    y=y-offsetY;
    z=Z;
    m = QString::number(x);
    n = QString::number(y);
    p = QString::number(z);
    s="G1 Z"+p+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going up.."<<s.toStdString().c_str();

    s="G1 X"+m+" Y"+n+" \r";
    qDebug()<<"Returning "<<s.toStdString().c_str();
    serial->write(s.toStdString().c_str());
}

void MainWindow::HomeX()
{
    serial->write("G28 X0\r");
    X=0;
    ui->PosX->setText("0");
    qDebug()<<"Homing X";
}

void MainWindow::HomeY()
{
    Y=0;
    serial->write("G28 Y0\r");
    ui->PosY->setText("0");
    qDebug()<<"Homing X";
}

void MainWindow::HomeZ()
{
    Z=0;
    serial->write("G28 Z0\r");
    ui->PosZ->setText("0");
    qDebug()<<"Homing Z";
    Z=Z+11.2;
    serial->write("G1 Z10\r");
}

void MainWindow::on_PLCC28_clicked()
{
    offsetZ=4.572;
    package_width=12.573;
    package_large=12.573;
}

void MainWindow::on_TQFP44_clicked()
{
    offsetZ=1;
    package_width=10;
    package_large=10;
}

void MainWindow::on_A1_clicked()
{
    X1=X;
    Y1=Y;
}

void MainWindow::on_B_clicked()
{
    X2=X;
    Y2=Y;
    hypo=sqrt((X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1));

}

void MainWindow::on_C_clicked()
{
    X3=X;
    Y3=Y;

    catOp=sqrt((X3-X2)*(X3-X2)+(Y3-Y2)*(Y3-Y2));
    desfase=qAsin(catOp/hypo);
    //QString fase();
    ui->FASE->setText(std::to_string(desfase).c_str());
}

void MainWindow::on_Leds_ON_clicked()
{
    serial->write("Lights ON\r");

}

void MainWindow::on_Leds_OFF_clicked()
{
    serial->write("Lights OFF\r");
}

void MainWindow::on_Motors17_clicked()
{
    serial->write("M17\r");
}

void MainWindow::on_Motors18_clicked()
{
    serial->write("M18\r");
}

void MainWindow::on_CameraPort_clicked()
{
    if(bCamera->isOpen()){
        bCamera->close();
        ui->CameraPort->setText("Closed");
        ui->Messages->appendPlainText("Camera port closed\n");
    }
    else
    {
        bCamera->setPortName(ui->PortcomboBox->currentText());
        bCamera->setBaudRate(921600);
        bCamera->setDataBits(QSerialPort::Data8);
        bCamera->setParity(QSerialPort::NoParity);
        bCamera->setStopBits(QSerialPort::OneStop);
        bCamera->setFlowControl(QSerialPort::FlowControl());
        //bCamera->setReadBufferSize(4096);
        bCamera->dataTerminalReadyChanged(false);

        DCB dcbSerialParams = {0};
        dcbSerialParams.EofChar=0x1A;
        dcbSerialParams.ErrorChar=0x00;
        dcbSerialParams.EvtChar=0x00;
        dcbSerialParams.XonChar=0x11;
        dcbSerialParams.XoffChar=0x13;
        dcbSerialParams.fRtsControl=RTS_CONTROL_DISABLE;//RTS_CONTROL_HANDSHAKE;//RTS_CONTROL_DISABLE;
        //Setting the DTR to Control_Enable ensures that the Arduino is properly
        //reset upon establishing a connection
        dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;
        dcbSerialParams.XonLim=4096;
        dcbSerialParams.XoffLim=4096;
        //SetupComm(bCamera->handle(), 2*4096, 2048);

        COMMTIMEOUTS timeouts;

           timeouts.ReadIntervalTimeout = -1;
           timeouts.ReadTotalTimeoutMultiplier = -1;
           timeouts.ReadTotalTimeoutConstant = 500;
           timeouts.WriteTotalTimeoutMultiplier = 0;
           timeouts.WriteTotalTimeoutConstant = 500;
           /*if (!SetCommTimeouts(bCamera->handle(), &timeouts))
           {
               qDebug()<<" error setcommtimeout";
           }*/


        try
        {
            bCamera->open(QIODevice::ReadWrite);
            COMMTIMEOUTS timeouts;

               timeouts.ReadIntervalTimeout = -1;
               timeouts.ReadTotalTimeoutMultiplier = -1;
               timeouts.ReadTotalTimeoutConstant = 500;
               timeouts.WriteTotalTimeoutMultiplier = 0;
               timeouts.WriteTotalTimeoutConstant = 500;
               SetupComm(bCamera->handle(), 4096, 2048);
               if (!SetCommTimeouts(bCamera->handle(), &timeouts)){
                   qDebug()<<" error setcommtimeout";
               }

            ui->CameraPort->setText("Open");
            ui->Messages->appendPlainText("Camera port opened\n");
        } catch (std::exception & e) {
            QMessageBox Msgbox;
                Msgbox.setText("Could not open the port");
                Msgbox.exec();
        }{}
        bCamera->dataTerminalReadyChanged(true);


    }
    ui->bCapture->setEnabled(bCamera->isOpen());
}

void MainWindow::on_bCapture_clicked()
{
    char *tx_data = new char[2];
    //tx_data[0] = (char)0x02;//Change to 1024x768
    tx_data[0] = (char)0x01;//Change to 640x480
    tx_data[1] = (char)0x10;//Shot a picture
    if(bCamera->isOpen())
         bCamera->write(tx_data, 2);
}

void MainWindow::on_radioButton_clicked()
{
    topCamera=true;
    bottomCamera=false;
}

void MainWindow::on_radioButton_2_clicked()
{
    topCamera=false;
    bottomCamera=true;
}
