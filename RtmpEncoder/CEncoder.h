#ifndef _CENCODER_H_
#define _CENCODER_H_

extern "C"{
#include <libavcodec/avcodec.h>  
#include <libavformat/avformat.h>  
#include <libswscale/swscale.h> 
#include <libavutil/mem.h>
};

class CEncoder
{
public:
	CEncoder();
	~CEncoder();

	static void avInit();
private:
	AVFrame *m_pRGBFrame;  //RGB֡����    
	AVFrame *m_pYUVFrame;  //YUV֡����  

	AVCodecContext *avCodecContext;  
	AVCodecContext *in_c;  
	AVCodec *pCodecH264; //������ 

public:
	void AV_InitCodec(AVCodecID CodeId, int nWidth, int nHeight);
	SwsContext *scxt;

	int Sws_ScaleHandle(AVPacket *avpkt, uint8_t *pBits, enum AVPixelFormat srcFormat, uint8_t *yuv_buff, enum AVPixelFormat dstFormat);
};


#endif