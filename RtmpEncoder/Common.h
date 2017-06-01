#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

#define FLV_CODECID_H264 7

//�����ͷ���ȣ�RTMP_MAX_HEADER_SIZE=18
#define RTMP_HEAD_SIZE   (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)
//�洢Nal��Ԫ���ݵ�buffer��С
#define BUFFER_SIZE 32768
//��ѰNal��Ԫʱ��һЩ��־
#define GOT_A_NAL_CROSS_BUFFER BUFFER_SIZE+1
#define GOT_A_NAL_INCLUDE_A_BUFFER BUFFER_SIZE+2
#define NO_MORE_BUFFER_TO_READ BUFFER_SIZE+3

#define NALU_TYPE_SLICE		1
#define NALU_TYPE_DPA		2
#define NALU_TYPE_DPB		3
#define NALU_TYPE_DPC		4
#define NALU_TYPE_IDR		5
#define NALU_TYPE_SEI		6
#define NALU_TYPE_SPS		7
#define NALU_TYPE_PPS		8
#define NALU_TYPE_AUD		9
#define NALU_TYPE_EOSEQ		10
#define NALU_TYPE_EOSTREAM	11
#define NALU_TYPE_FILL		12

typedef struct _NaluUnit  
{  
	int type;  
	int size;  
	unsigned char *data;  
}NaluUnit;

/**
 * _RTMPMetadata
 * �ڲ��ṹ�塣�ýṹ����Ҫ���ڴ洢�ʹ���Ԫ������Ϣ  ���ڴ����rtmpserver�ĵ�һ����
 */ 
typedef struct _RTMPMetadata  
{  
	// video, must be h264 type   
	unsigned int    nWidth;  
	unsigned int    nHeight;  
	unsigned int    nFrameRate;      
	unsigned int    nSpsLen;  
	unsigned char   *Sps;  
	unsigned int    nPpsLen;  
	unsigned char   *Pps;   
} RTMPMetadata,*LPRTMPMetadata;


//#define DebugPrint(...) printf("Filename %s, Function %s, Line %d > ", __FILE__, __FUNCTION__, __LINE__); \
//	printf( ##__VA_ARGS__); \
//	printf("\n");
#define DebugPrint(...) printf("[ Fun %s, Line %d ] :  ", __FUNCTION__, __LINE__); \
	printf( ##__VA_ARGS__); \
	printf("\n");


char * put_byte( char *output, uint8_t nVal );
char * put_be16(char *output, uint16_t nVal );
char * put_be24(char *output,uint32_t nVal );
char * put_be32(char *output, uint32_t nVal );
char *  put_be64( char *output, uint64_t nVal );
char * put_amf_string( char *c, const char *str );
char * put_amf_double( char *c, double d );

int h264_decode_sps(BYTE * buf,unsigned int nLen,int &width,int &height,int &fps);