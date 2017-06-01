#include "StdAfx.h"
#include "CRtmpStream.h"

CRtmpStream::CRtmpStream()
{
	rtmp = NULL;
	memset(&metaData, 0, sizeof(metaData));
	metaData.Sps = NULL;
	metaData.Pps = NULL;
}

CRtmpStream::~CRtmpStream()
{
	

}


void CRtmpStream::InitSocket()
{
	WORD version;
	WSADATA wsaData;
	version = MAKEWORD(1, 1);
	WSAStartup(version, &wsaData);
}

void CRtmpStream::CleanupSockets()
{
	WSACleanup();
}


int CRtmpStream::InitRtmp()
{
	rtmp = RTMP_Alloc();
	RTMP_Init(rtmp);

	return 0;
}

bool CRtmpStream::ConnectToServer(char *url)
{

	if (RTMP_SetupURL(rtmp, url) < 0)
	{
		return FALSE;
	}
	RTMP_EnableWrite(rtmp);
	if (RTMP_Connect(rtmp, NULL) < 0)
	{
		return FALSE;
	}
	if (RTMP_ConnectStream(rtmp, 0) < 0)
	{
		return FALSE;
	}

	return TRUE;
}

void CRtmpStream::DisconnectToServer()
{
	//RTMPMetadata 

}

bool CRtmpStream::SendAMFPacket(LPRTMPMetadata lpMetaData) //Ӧ����SendAMFPacket 
{
	if(lpMetaData == NULL)  
	{  
		return false;  
	}  
	char body[1024] = {0};;  

	char * p = (char *)body; 
	//p = put_byte(p, AMF_STRING );
	//p = put_amf_string(p , "@setDataFrame" );

	p = put_byte( p, AMF_STRING );
	p = put_amf_string( p, "onMetaData" ); 

	p = put_byte(p, AMF_OBJECT );
	p = put_amf_string( p, "copyright" );
	p = put_byte(p, AMF_STRING );
	p = put_amf_string( p, "firehood" ); 

	p =put_amf_string( p, "width");
	p =put_amf_double( p, lpMetaData->nWidth);

	p =put_amf_string( p, "height");
	p =put_amf_double( p, lpMetaData->nHeight);

	p =put_amf_string( p, "framerate" );
	p =put_amf_double( p, lpMetaData->nFrameRate); 

	p =put_amf_string( p, "videocodecid" );
	p =put_amf_double( p, FLV_CODECID_H264 );

	p =put_amf_string( p, "" );
	p =put_byte( p, AMF_OBJECT_END  );

	int index = p-body;

	return SendPacket(RTMP_PACKET_TYPE_INFO,(unsigned char*)body,p-body,0);

}

bool CRtmpStream::SendPacket(unsigned int nTagType,unsigned char *data,unsigned int size,unsigned int nTimestamp)
{
	RTMPPacket* packet;
	/*������ڴ�ͳ�ʼ��,lenΪ���峤��*/
	packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+size);
	memset(packet,0,RTMP_HEAD_SIZE);
	/*�����ڴ�*/
	packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
	packet->m_nBodySize = size;
	memcpy(packet->m_body,data,size);
	packet->m_hasAbsTimestamp = 0;
	packet->m_packetType = nTagType; /*�˴�Ϊ����������һ������Ƶ,һ������Ƶ*/
	packet->m_nInfoField2 = rtmp->m_stream_id;
	packet->m_nChannel = 0x04;

	packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
	if (RTMP_PACKET_TYPE_AUDIO ==nTagType && size !=4)
	{
		packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	}
	packet->m_nTimeStamp = nTimestamp;
	/*����*/
	int nRet =0;
	if (RTMP_IsConnected(rtmp))
	{
		nRet = RTMP_SendPacket(rtmp, packet, TRUE); /*TRUEΪ�Ž����Ͷ���,FALSE�ǲ��Ž����Ͷ���,ֱ�ӷ���*/
	}
	/*�ͷ��ڴ�*/
	free(packet);
	return nRet; 
}

bool CRtmpStream::SendVideoSpsPpsPacket(unsigned char *pps,int pps_len,unsigned char * sps,int sps_len)
{
	RTMPPacket * packet=NULL;//rtmp���ṹ
	unsigned char * body=NULL;
	int i;
	packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+1024);
	//RTMPPacket_Reset(packet);//����packet״̬
	memset(packet,0,RTMP_HEAD_SIZE+1024);
	packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
	body = (unsigned char *)packet->m_body;
	i = 0;
	body[i++] = 0x17;
	body[i++] = 0x00;

	body[i++] = 0x00;
	body[i++] = 0x00;
	body[i++] = 0x00;

	/*AVCDecoderConfigurationRecord*/
	body[i++] = 0x01;
	body[i++] = sps[1];
	body[i++] = sps[2];
	body[i++] = sps[3];
	body[i++] = 0xff;

	/*sps*/
	body[i++]   = 0xe1;
	body[i++] = (sps_len >> 8) & 0xff;
	body[i++] = sps_len & 0xff;
	memcpy(&body[i],sps,sps_len);
	i +=  sps_len;

	/*pps*/
	body[i++]   = 0x01;
	body[i++] = (pps_len >> 8) & 0xff;
	body[i++] = (pps_len) & 0xff;
	memcpy(&body[i],pps,pps_len);
	i +=  pps_len;

	packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
	packet->m_nBodySize = i;
	packet->m_nChannel = 0x04;
	packet->m_nTimeStamp = 0;
	packet->m_hasAbsTimestamp = 0;
	packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
	packet->m_nInfoField2 = rtmp->m_stream_id;

	/*���÷��ͽӿ�*/
	int nRet = RTMP_SendPacket(rtmp,packet,TRUE);
	free(packet);    //�ͷ��ڴ�
	return nRet;
}

bool CRtmpStream::SendH264Packet(unsigned char *data, unsigned int size, int bIsKeyFrame, unsigned int nTimeStamp) 
{
	if(data == NULL && size<11){  
		return false;  
	}  

	unsigned char *body = (unsigned char*)malloc(size+9);  
	memset(body,0,size+9);

	int i = 0; 
	if(bIsKeyFrame)
	{  
		body[i++] = 0x17;// 1:Iframe  7:AVC   
		body[i++] = 0x01;// AVC NALU   
		body[i++] = 0x00;  
		body[i++] = 0x00;  
		body[i++] = 0x00;  


		// NALU size   
		body[i++] = size>>24 &0xff;  
		body[i++] = size>>16 &0xff;  
		body[i++] = size>>8 &0xff;  
		body[i++] = size&0xff;
		// NALU data   
		memcpy(&body[i],data,size);  
		SendVideoSpsPpsPacket(metaData.Pps,metaData.nPpsLen,metaData.Sps,metaData.nSpsLen);
	}
	else
	{  
		body[i++] = 0x27;// 2:Pframe  7:AVC   
		body[i++] = 0x01;// AVC NALU   
		body[i++] = 0x00;  
		body[i++] = 0x00;  
		body[i++] = 0x00;  


		// NALU size   
		body[i++] = size>>24 &0xff;  
		body[i++] = size>>16 &0xff;  
		body[i++] = size>>8 &0xff;  
		body[i++] = size&0xff;
		// NALU data   
		memcpy(&body[i],data,size);  
	}  


	int bRet = SendPacket(RTMP_PACKET_TYPE_VIDEO,body,i+size,nTimeStamp);  

	free(body);  

	return bRet;  
}

bool CRtmpStream::SendH264File(char *filename)
{
	int hLength=0;
	FILE *fp = NULL;
	if (filename == NULL)
	{
		DebugPrint("%s is not error", filename);
		return false;
	}
	fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		DebugPrint("%s is not exist", filename);
		return false;
	}
	fseek(fp, 0, SEEK_END);
	h264FileLength = ftell(fp);
	h264FileBuffer = (unsigned char *)malloc(h264FileLength);
	fseek(fp, 0, SEEK_SET);
	fread(h264FileBuffer, 1, h264FileLength, fp);
	fclose(fp);

	Geth264FileSpsPpsData(h264FileBuffer, h264FileLength, &metaData);

	//parseSpsData();//����sps��ȡ����߼�������Ϣ
	h264_decode_sps(metaData.Sps, metaData.nSpsLen, (int &)metaData.nWidth, (int &)metaData.nHeight, (int &)metaData.nFrameRate);
	DebugPrint("Width : %d; Heigth %d; FrameRate %d", metaData.nWidth, metaData.nHeight, metaData.nFrameRate);
	if (metaData.nFrameRate==0)
	{
		metaData.nFrameRate = 25;
	}

	unsigned int tick = 0;  
	unsigned int tick_gap = 1000/metaData.nFrameRate; 
	int m_CurPos = 0;
	//unsigned char *p = h264FileBuffer;
	NaluUnit nalu;
	int readSize = 0;
	while (m_CurPos<h264FileLength)
	{
		unsigned char *p = h264FileBuffer+m_CurPos;
		readSize = ReadOneNaluFromBuf(nalu, p, h264FileLength-m_CurPos); 
		m_CurPos += readSize;
		if (nalu.type == 0x07||nalu.type == 0x08)
		{
			continue;
		}
		int bKeyframe  = (nalu.type == 0x05) ? TRUE : FALSE;
		SendH264Packet(nalu.data, nalu.size, bKeyframe, tick);
		tick += tick_gap;
		if (nalu.data != NULL)
		{
			free(nalu.data);
		}
		DebugPrint("m_Cur Pos : %d, tick : %d", m_CurPos, tick);
		Sleep(tick_gap);
	}

}

bool CRtmpStream::Geth264FileSpsPpsData(unsigned char *fileData, int fileLength, LPRTMPMetadata pMetaData)
{
	if (fileData == NULL)
	{
		DebugPrint("data is null");
		return false;
	}
	int readNaluType=0;//���û�ȡ��־
	int m_CurPos=0;
	int i = 0;
	unsigned char *p = fileData;
	while (i<fileLength)
	{
		int j = i;
		if (p[j++]==0x00&&p[j++]==0x00)
		{
			if ((p[j]==0x01) || (p[j++]==0x00 && p[j]==0x01))
			{
				j++;
				if ((p[j]&0x1f)==NALU_TYPE_SPS||readNaluType==NALU_TYPE_SPS)//sps
				{
					if (readNaluType ==0 )
					{
						readNaluType = NALU_TYPE_SPS;
						m_CurPos = j;//��ȡsps��λ��
						i = j;
						continue;
					}
					else if (readNaluType == NALU_TYPE_SPS)
					{
						//��������
						pMetaData->nSpsLen = i-m_CurPos;
						pMetaData->Sps = (unsigned char *)malloc(pMetaData->nSpsLen);
						memcpy(pMetaData->Sps, &p[m_CurPos], pMetaData->nSpsLen);
						m_CurPos = 0;
						readNaluType = 0;
						i--;
					}
				}
				else if ((p[j]&0x1f)==NALU_TYPE_PPS||readNaluType==NALU_TYPE_PPS)//pps
				{
					if (readNaluType == 0 )
					{
						readNaluType = NALU_TYPE_PPS;
						m_CurPos = j;//��ȡsps��λ��
						i = j;
						continue;
					}
					else if (readNaluType == NALU_TYPE_PPS)
					{
						//��������
						pMetaData->nPpsLen = i-m_CurPos;
						pMetaData->Pps = (unsigned char *)malloc(pMetaData->nPpsLen);
						memcpy(pMetaData->Pps, &p[m_CurPos], pMetaData->nPpsLen);
						m_CurPos = 0;
						readNaluType = 0;
						return true;
					}
				}
			}
		}
		i++;
	}

	return true;
}

int CRtmpStream::ReadOneNaluFromBuf(NaluUnit &nalu, unsigned char *data, int dataLength)  //h264���ݻ�ȡÿһ��Nalu
{  
	int i = 0;
	while(i<dataLength)  
	{  
		int j = i;
		if(data[j++] == 0x00 &&  data[j++] == 0x00) 
		{  
			if ((data[j] == 0x01) || (data[j++] == 0x00 &&  data[j] == 0x01))
			{
				j++;
				int m_CurPos = j;
				nalu.type = data[j]&0x1f;
				while (j<dataLength)
				{
					int k = j;
					if (data[k++] == 0x00 &&  data[k++] == 0x00)
					{
						if ((data[k] == 0x01)||(data[k++] == 0x00 &&  data[k] == 0x01))
						{
							nalu.size = j - m_CurPos;
							nalu.data = (unsigned char *)malloc(nalu.size);
							memcpy(nalu.data, data+m_CurPos, nalu.size);
							return j;
						}
					}
					j++;
				}
				nalu.size = j - m_CurPos;
				nalu.data = (unsigned char *)malloc(nalu.size);
				memcpy(nalu.data, data+m_CurPos, nalu.size);
				return j;//���һ��Nalu�Ĵ���
			} 
		} 
		i++;
	}  
	return 0;  
}

bool CRtmpStream::putH264BufferToRtmpStream(unsigned char *h264Buffer, unsigned int h264size)
{
	int m_CurPos=0;
	NaluUnit nalu;
	unsigned int tick = 0;  
	unsigned int tick_gap = 0;
	while (m_CurPos < h264size)
	{
		unsigned char *p = h264Buffer+m_CurPos;
		int readSize = ReadOneNaluFromBuf(nalu, p, h264size-m_CurPos);
		m_CurPos += readSize;

		if (nalu.type == 0x07||nalu.type == 0x08)
		{
			if (nalu.type == 0x07 && metaData.nSpsLen == 0)
			{
				metaData.nSpsLen = nalu.size;
				metaData.Sps = (unsigned char *)malloc(metaData.nSpsLen);
				memcpy(metaData.Sps, nalu.data, metaData.nSpsLen);
			}
			if (nalu.type == 0x08 && metaData.nPpsLen == 0)
			{
				metaData.nPpsLen = nalu.size;
				metaData.Pps = (unsigned char *)malloc(metaData.nPpsLen);
				memcpy(metaData.Pps, nalu.data, metaData.nPpsLen);
			}
			if (metaData.nWidth == 0 && metaData.nHeight == 0 && metaData.nSpsLen != 0)
			{
				h264_decode_sps(metaData.Sps, metaData.nSpsLen, (int &)metaData.nWidth, (int &)metaData.nHeight, (int &)metaData.nFrameRate);
				DebugPrint("Width : %d; Heigth %d; FrameRate %d", metaData.nWidth, metaData.nHeight, metaData.nFrameRate);
				if (metaData.nFrameRate==0)
				{
					metaData.nFrameRate = 25;
				}
				tick_gap = 1000/metaData.nFrameRate;
			}
			continue;
		}

		int bKeyframe  = (nalu.type == 0x05) ? TRUE : FALSE;
		SendH264Packet(nalu.data, nalu.size, bKeyframe, tick);
		tick += tick_gap;
		Sleep(tick_gap);
		if (nalu.data != NULL)
		{
			free(nalu.data);
		}
	}

	return true;
}
