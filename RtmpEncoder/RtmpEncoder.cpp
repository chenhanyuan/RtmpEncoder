// RtmpEncoder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "CRtmpStream.h"
#include "UsbCamera.h"
#include "CEncoder.h"

//#define SAVEH264FILE

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned char *rgbData = (unsigned char *)malloc(1280*720*3);
	unsigned int rgbSize = 0;
	/*usb Camera*/
	UsbCamera usbCamera;
	usbCamera.Init();
	usbCamera.SetUsbCamera();

	/*Rtmp Protocol*/
	CRtmpStream::InitSocket();
	CRtmpStream rtmpStream;
	rtmpStream.InitRtmp();
	rtmpStream.ConnectToServer("rtmp://192.168.16.144/live/demo");

	/*Encode*/
	CEncoder Encode;
	CEncoder::avInit();
	Encode.AV_InitCodec(CODEC_ID_H264, 1280, 720);

	int outbuf_size = 900000;  
	uint8_t * outbuf= (uint8_t*)malloc(outbuf_size); 
	uint8_t *yuv_buff = (uint8_t *) malloc((1280 * 720 * 3) / 2);

#ifdef SAVEH264FILE
	FILE *f=NULL;   
	char * filename = "myData.h264";  
	f = fopen(filename, "wb");  
	if (!f)  
	{  
		//TRACE( "could not open %s\n", filename);  
		getchar();  
		exit(1);  
	}  
#endif

	//for (int i=0; i<500; i++)
	enum AVPixelFormat srcFormat, dstFormat;

	srcFormat = AV_PIX_FMT_BGR24;//camera获取的是BGR24的数据

	dstFormat = AV_PIX_FMT_YUV420P;

	while(1)
	{
		AVPacket avpkt = {0};
		usbCamera.ReadCameraData(rgbData, rgbSize);

		avpkt.data = outbuf;  
		avpkt.size = outbuf_size; 
		int rec = Encode.Sws_ScaleHandle(&avpkt, rgbData, srcFormat, yuv_buff, dstFormat);

		if (avpkt.size != 0)
		{
			rtmpStream.putH264BufferToRtmpStream(avpkt.data, avpkt.size);
		}

#ifdef SAVEH264FILE
		if (rec == 0)  
		{  
			fwrite(avpkt.data, 1, avpkt.size, f);  
			//printf("i : %d\n", i);
		}
#endif
		DebugPrint("Encode size : %d", avpkt.size);
	}
/*
	CRtmpStream::InitSocket();
	CRtmpStream rtmpStream;
	rtmpStream.InitRtmp();
	rtmpStream.ConnectToServer("rtmp://192.168.16.82/live/demo");


	rtmpStream.SendH264File("stream.h264");

	DebugPrint("Connect Success");*/

	return 0;
}
