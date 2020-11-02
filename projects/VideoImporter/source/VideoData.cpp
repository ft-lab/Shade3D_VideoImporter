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
	this->fileName  = v.fileName;
}

CVideoData::~CVideoData ()
{
}

void CVideoData::clear ()
{
	fileName = "";
}

