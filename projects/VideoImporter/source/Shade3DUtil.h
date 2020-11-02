/**
 * Shade3Dのシーン走査のため便利機能など.
 */

#ifndef _SHADE3DUTIL_H
#define _SHADE3DUTIL_H

#include "GlobalHeader.h"

#include <vector>

namespace Shade3DUtil
{
	/**
	 * マスターイメージパートを取得.
	 * @param[in] scene  シーンクラス.
	 */
	sxsdk::shape_class* findMasteImagePart (sxsdk::scene_interface* scene);

	/**
	 * 指定のイメージに対応するマスターイメージを取得.
	 * @param[in] scene  シーンクラス.
	 * @param[in] image  対象のイメージ.
	 * @return マスターイメージが存在する場合はそのポインタ.
	 */
	sxsdk::master_image_class* getMasterImageFromImage (sxsdk::scene_interface* scene, sxsdk::image_interface* image);
}

#endif
