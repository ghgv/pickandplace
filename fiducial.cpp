#include "fiducial.h"

fiducial::fiducial()
{

    _camera->setCaptureMode(QCamera::CaptureStillImage);
    _camera->searchAndLock();
    //on shutter button pressed
    fiduCapture->capture();
    //on shutter button released
    _camera->unlock();

    cv::Mat  result;
    cv::Mat img = cv::imread("C://Users//German//Documents//pickandplace//letter.png", 1);

    cv::Mat templateImg = cv::imread("C://Users//German//Documents//pickandplace//simbol.png", 1);
    /// Reduce the size of the image to display it on my screen
    cv::resize(img, img, cv::Size(), 0.5, 0.5);
    /// Reduce the size of the template image
    /// (first to fit the size used to create the image test, second to fit the size of the reduced image)
    cv::resize(templateImg, templateImg, cv::Size(), 0.25, 0.25);

    cv::Mat img_display;
    img.copyTo(img_display);

    // Create the result matrix
    int result_cols =  img.cols - templateImg.cols + 1;
    int result_rows = img.rows - templateImg.rows + 1;
    result.create(result_rows, result_cols, CV_32FC1);

     /// Do the Matching and Normalize
    cv::matchTemplate(img, templateImg, result, CV_TM_CCORR_NORMED);
    cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchLoc;

    for(;;) {
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
        matchLoc = maxLoc;
        std::cout << "Max correlation=" << maxVal << std::endl;
        if(maxVal < 0.8) {
          break;
        }

    /// Show me what you got
    cv::rectangle(img_display, matchLoc, cv::Point(matchLoc.x + templateImg.cols , matchLoc.y + templateImg.rows),
    cv::Scalar::all(0), 2, 8, 0);
    cv::rectangle(result, cv::Point(matchLoc.x - templateImg.cols/2 , matchLoc.y - templateImg.rows/2),
    cv::Point(matchLoc.x + templateImg.cols/2 , matchLoc.y + templateImg.rows/2 ), cv::Scalar::all(0), 2, 8, 0);
    /// Fill the detected location with a rectangle of zero
    cv::rectangle(result, cv::Point( matchLoc.x - templateImg.cols/2 , matchLoc.y - templateImg.rows/2),
    cv::Point(matchLoc.x + templateImg.cols/2 , matchLoc.y + templateImg.rows/2 ), cv::Scalar::all(0), -1);
    qDebug()<<matchLoc.x<<" "<<matchLoc.y;
    } while (maxVal > 0.9);


    cv::imshow("result", result);
    cv::imshow("img_display", img_display);
    cv::waitKey(0);
}
