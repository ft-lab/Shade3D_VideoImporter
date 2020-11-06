/**
 * 動画の属性を表示。レンダリング時に呼ばれる.
 */

#ifndef _MOVIE_IMAGE_ATTRIBUTE_INTERFACE_H
#define _MOVIE_IMAGE_ATTRIBUTE_INTERFACE_H

#include "GlobalHeader.h"
#include "ImportVideoWithOpenCV.h"
#include "VideoData.h"

#include <memory>

class CVideoImageAttributeInterface : public sxsdk::attribute_interface
{
private:
	sxsdk::shade_interface& shade;

	// 動画の再生用.
	std::vector<CImportVideoWithOpenCV *> m_videoList;

	VideoData::CVideoData m_data;	// 動画情報.

	bool m_renderingF;			// レンダリング中の場合はtrue.
	sxsdk::rendering_context_interface* m_RC;

	int m_passTimeMS;			// idleのチェック用の経過時間保持用.

private:
	/**
	 * SDKのビルド番号を指定（これは固定で変更ナシ）。.
	 * ※ これはプラグインインターフェースごとに必ず必要。.
	 */
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }

	/**
	 * UUIDの指定（独自に定義したGUIDを指定）.
	 * ※ これはプラグインインターフェースごとに必ず必要。.
	 */
	virtual sx::uuid_class get_uuid (void * = 0) { return VIDEO_IMAGE_ATTRIBUTE_ID; }

	/**
	 * 形状の属性として呼び出し.
	 */
	virtual void accepts_shape (bool &accept, void *aux=0) { accept = true; }

	virtual bool ask_shape (sxsdk::shape_class &shape, void *aux=0);

	/**
	 * コンテキストメニューでのディム指定.
	 */
	virtual void accepts_shape_type (bool &accept, sxsdk::enums::shape_type shape_type, sxsdk::shape_class &shape);

	/**
	 * レンダリング開始前(フレームごと)に呼ばれる.
	 */
	virtual void pre_rendering (bool &b, sxsdk::rendering_context_interface *rendering_context, void *);

	/**
	 * 各フレームごとのレンダリング開始前に呼ばれる.
	 */
	virtual void create_rendering_objects (bool &b, sxsdk::shape_class &shape, sxsdk::scene_interface &scene, void *);

	/**
	 * アイドル時に定期的に呼ばれる.
	 */
	virtual void idle_task (bool &b, sxsdk::scene_interface *scene, void *aux=0);

	//--------------------------------------------------.
	//  ダイアログのイベント処理用.
	//--------------------------------------------------.
	/**
	 * ダイアログの初期化.
	 */
	virtual void initialize_dialog (sxsdk::dialog_interface &d, void * = 0);

	/** 
	 * ダイアログのイベントを受け取る.
	 */
	virtual bool respond (sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void * = 0);

	/**
	 * ダイアログのデータを設定する.
	 */
	virtual void load_dialog_data (sxsdk::dialog_interface &d, void * = 0);

private:
	/**
	 * m_videoListをクリア.
	 */
	void m_clearVideoList ();

	/**
	 * マスターイメージパート内で動画の属性を持つマスターイメージを取得し、m_videoListに格納.
	 */
	void m_storeVideoMasterImage (sxsdk::scene_interface* scene);

	/**
	 * すべての動画用のマスターイメージをはじめのフレームに戻す.
	 */
	void m_updateFirstImage ();

public:
	CVideoImageAttributeInterface (sxsdk::shade_interface& shade);
	virtual ~CVideoImageAttributeInterface ();

	/**
	 * プラグイン名をSXUL(text.sxul)より取得.
	 */
	static const char *name (sxsdk::shade_interface *shade) { return shade->gettext("movie_image_attribute_title"); }
};

#endif
