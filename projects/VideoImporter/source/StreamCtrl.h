/**
 * streamへの入出力.
 */

#ifndef _STREAMCTRL_H
#define _STREAMCTRL_H

#include "GlobalHeader.h"
#include "VideoData.h"

namespace StreamCtrl
{
	/**
	 * Importダイアログボックスの情報を保存.
	 */
	void saveVideoData (sxsdk::shape_class& shape, const VideoData::CVideoData& data);

	/**
	 * Importダイアログボックスの情報を読み込み.
	 */
	bool loadVideoData (sxsdk::shape_class& shape, VideoData::CVideoData& data);
}

#endif
