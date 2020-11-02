/**
 *  動画ファイルのオプション情報.
 */

#include "VideoData.h"

using namespace VideoData;

CVideoData::CVideoData ()
{
	clear();
}

CVideoData::CVideoData (const CVideoData& v)
{
	this->name       = v.name;
	this->fileName   = v.fileName;
	this->width      = v.width;
	this->height     = v.height;
	this->frameCount = v.frameCount;
	this->frameRate  = v.frameRate;
	this->playLoop   = v.playLoop;
}

CVideoData::~CVideoData ()
{
}

void CVideoData::clear ()
{
	name = "";
	fileName = "";
	width = height = 0;
	frameCount = 0;
	frameRate = 0.0f;
	playLoop = false;
}
