#ifndef _USBCAMERA_H_
#define _USBCAMERA_H_

#include <cv.h>  
#include <cxcore.h>  
#include <highgui.h>  
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

class UsbCamera
{
public:
	UsbCamera();
	~UsbCamera();
public:

	cv::VideoCapture cap;
	//cv::VideoCapture cap("bike.avi"); ��ֱ�Ӷ�ȡ�ļ�
	cv::Mat frame;
	bool Init();
	bool SetUsbCamera();//���÷ֱ��ʵ���Ϣ
	bool ReadCameraData(unsigned char *rgbData, unsigned int &rgbSize);

	bool saveBmpImage(IplImage &pFrame);//usb camera ��ȡ������������rgb 24λ 
};


#endif