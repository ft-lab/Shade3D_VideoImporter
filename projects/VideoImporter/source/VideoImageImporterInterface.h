/**
 * MP4/webm形式の動画ファイルをインポート.
 */

#ifndef _MOVIE_IMAGE_IMPORTER_INTERFACE_H
#define _MOVIE_IMAGE_IMPORTER_INTERFACE_H

#include "GlobalHeader.h"
#include "ImportVideoWithOpenCV.h"

#include <memory>

class CVideoImageImporterInterface : public sxsdk::image_importer_interface
{
private:
	sxsdk::shade_interface& shade;

	CImportVideoWithOpenCV m_importImage;
	std::string m_filePath;				// 動画ファイルのパス.
	sxsdk::image_interface* m_image;

	/**
	 * SDKのビルド番号を指定（これは固定で変更ナシ）。.
	 * ※ これはプラグインインターフェースごとに必ず必要。.
	 */
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }

	/**
	 * UUIDの指定（独自に定義したGUIDを指定）.
	 * ※ これはプラグインインターフェースごとに必ず必要。.
	 */
	virtual sx::uuid_class get_uuid (void * = 0) { return VIDEO_IMAGE_IMPORTER_ID; }

	virtual const char *get_file_extension (int index, void*);
	virtual const char *get_file_description (int index, void*);
	virtual bool do_pre_import (int index, void*);
	virtual sxsdk::image_interface *do_import (int index, sxsdk::stream_interface *stream, void*);

	/**
	 * アイドル時に定期的に呼ばれる.
	 */
	virtual void idle_task (bool &b, sxsdk::scene_interface *scene, void *aux=0);

public:
	CVideoImageImporterInterface (sxsdk::shade_interface& shade);
	virtual ~CVideoImageImporterInterface ();

	/**
	 * プラグイン名をSXUL(text.sxul)より取得.
	 */
	static const char *name (sxsdk::shade_interface *shade) { return shade->gettext("movie_image_importer_title"); }
};

#endif
