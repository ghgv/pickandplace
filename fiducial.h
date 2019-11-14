#ifndef FIDUCIAL_H
#define FIDUCIAL_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <qtDebug>
#include <QCameraImageCapture>
#include <QCamera>

extern QCameraImageCapture  *fiduCapture;
extern QCamera *_camera;


class fiducial
{
public:
    fiducial();
};

#endif // FIDUCIAL_H
