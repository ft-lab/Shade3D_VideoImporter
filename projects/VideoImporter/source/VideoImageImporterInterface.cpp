/**
 * MP4/webm形式の動画ファイルをインポート.
 */
#include "VideoImageImporterInterface.h"
#include "ImportVideoWithOpenCV.h"
#include "VideoData.h"
#include "Shade3DUtil.h"
#include "StreamCtrl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CVideoImageImporterInterface::CVideoImageImporterInterface (sxsdk::shade_interface& shade) : shade(shade), m_importImage(shade)
{
	m_filePath = "";
	m_image = NULL;
}

CVideoImageImporterInterface::~CVideoImageImporterInterface ()
{
}

const char *CVideoImageImporterInterface::get_file_extension (int index, void*)
{
	if (index == 0) { return "mp4"; }
	if (index == 1) { return "webm"; }
#if _WINDOWS
	if (index == 2) { return "mov"; }
#endif

	return NULL;
}

const char *CVideoImageImporterInterface::get_file_description (int index, void*)
{
	if (index == 0) { return "mp4"; }
	if (index == 1) { return "webm"; }
#if _WINDOWS
	if (index == 2) { return "mov"; }
#endif

	return NULL;
}

bool CVideoImageImporterInterface::do_pre_import (int index, void*)
{
	return true;
}

sxsdk::image_interface *CVideoImageImporterInterface::do_import (int index, sxsdk::stream_interface *stream, void*)
{
	if (!stream) return NULL;

	// ファイルパスを取得.
	m_filePath = stream->get_file_path();

	// 動画を読み込み.
	// マスターイメージとしての属性割り当ては、do_importから抜けた後に遅延で行っている (idle_task内).
	m_image = NULL;
	VideoData::CVideoData videoD;
	if (m_importImage.init(m_filePath, videoD)) {
		m_image = m_importImage.getImage();
		compointer<sxsdk::image_interface> dImage(m_image->duplicate_image());
		dImage->AddRef();
		return dImage;

	} else { }

	return NULL;
}

/**
 * アイドル時に定期的に呼ばれる.
 */
void CVideoImageImporterInterface::idle_task (bool &b, sxsdk::scene_interface *scene, void *)
{
	// イメージが読み込まれた場合、マスターイメージに属性を割り当て.
	if (m_image) {
		sxsdk::master_image_class* masterImageC = Shade3DUtil::getMasterImageFromImage(scene, m_image);
		if (masterImageC) {
			VideoData::CVideoData videoD;
			videoD.fileName = m_filePath;
			StreamCtrl::saveVideoData(*masterImageC, videoD);

			// 読み込んだOpenCVのクラスを解放.
			m_importImage.term();

			m_image = NULL;
		}
	}
}

