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

	this->playLoop    = v.playLoop;
	this->useColor    = v.useColor;
	this->color       = v.color;
	this->startFrame  = v.startFrame;
	this->endFrame    = v.endFrame;
	this->useEndFrame = v.useEndFrame;
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
	useColor = true;
	color = sxsdk::rgb_class(0, 0, 0);
	startFrame  = 0.0f;
	endFrame    = 300.0f;
	useEndFrame = false;
}
