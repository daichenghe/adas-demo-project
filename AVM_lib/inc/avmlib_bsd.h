/*******************************************************************************
* File Name          : avmlib_bsd.h
* Author             : Jerry
* Revision           : 2.1
* Date               : 15/07/2016
* Description        : Library Interface 
*******************************************************************************/

#ifndef _BSDINTERFACE_H
#define _BSDINTERFACE_H

typedef enum POS
{
	LEFT,
	RIGHT
}POS;

typedef struct CARINFORM
{
	float wheelbase ;
	float l_camerax ;
	float l_cameray;
	float fSpeed;
	float fAngle;
	float ftime;

}CarInform;

typedef struct OUTBDSRECT
{
	int x;
	int y;
	int w;
	int h;

}Outbsdrect;

typedef struct OUTBLINDRECS
{
	Outbsdrect *prect;
	int nRecNum;

}OutblindRecs;

typedef struct BSD_IMG
{
	unsigned char *pdata;
	int w;
	int h;
}bsdImg;

typedef struct  BSDINPUT
{
	bsdImg img;
	CarInform *pCarInf;
	POS pos;

}BsdInput;



/***************BSD ½Ó¿Úº¯Êý********************/

void mvBsdProcess(const BsdInput *pBsdInput); 

void mvGetBsdResult(OutblindRecs **pBlindRects,int pos);

void mvUnitBsd(int pos);




#endif

