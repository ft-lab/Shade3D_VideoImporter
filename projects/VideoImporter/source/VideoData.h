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
		std::string fileName;			// フルパスのファイル名.


	public:
		CVideoData ();
		CVideoData (const CVideoData& v);
		~CVideoData ();

		CVideoData& operator = (const CVideoData &v) {
			this->fileName  = v.fileName;
			return (*this);
		}

		void clear ();
	};
}

#endif
