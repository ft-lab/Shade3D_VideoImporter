/**
 *  動画ファイルの情報.
 */

#ifndef _VIDEODATA_H
#define _VIDEODATA_H

#include "GlobalHeader.h"

namespace VideoData {

	/**
	 * 動画情報.
	 */
	class CVideoData
	{
	public:
		std::string name;				// マスターイメージ名.
		std::string fileName;			// フルパスのファイル名.
		int width, height;				// 画像サイズ.
		int frameCount;					// フレーム総数.
		float frameRate;				// フレームレート.

		bool playLoop;					// ループ再生.

	public:
		CVideoData ();
		CVideoData (const CVideoData& v);
		~CVideoData ();

		CVideoData& operator = (const CVideoData &v) {
			this->name       = v.name;
			this->fileName   = v.fileName;
			this->width      = v.width;
			this->height     = v.height;
			this->frameCount = v.frameCount;
			this->frameRate  = v.frameRate;
			this->playLoop   = v.playLoop;

			return (*this);
		}

		void clear ();
	};
}

#endif
