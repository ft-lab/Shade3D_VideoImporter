/**
 * 動画の属性を表示。レンダリング時に呼ばれる.
 */
#include "VideoImageAttributeInterface.h"
#include "ImportVideoWithOpenCV.h"
#include "StreamCtrl.h"
#include "Shade3DUtil.h"
#include "StringUtil.h"

enum {
	dlg_info_image_name = 101,						// イメージ名.
	dlg_info_file_path = 102,						// ファイルパス.
	dlg_info_size = 103,							// サイズ.
	dlg_info_frame_count = 104,						// フレーム総数.
	dlg_info_frame_rate = 105,						// フレームレート.

	dlg_loop = 201,									// ループ再生.
	dlg_useColor = 202,								// 動画を再生しないフレームで色を指定.
	dlg_color = 203,								// 動画を再生しないときの色.
	dlg_start_frame = 204,							// 開始フレーム.
	dlg_use_end_frame = 205,						// 終了フレームを使用.
	dlg_end_frame = 206,							// 終了フレーム.

	dlg_changeFile = 302,							// 変更ボタン.

};

CVideoImageAttributeInterface::CVideoImageAttributeInterface (sxsdk::shade_interface& shade) : shade(shade)
{
	m_renderingF = false;
	m_RC = NULL;
	m_passTimeMS = 0;
	m_pCurrentShape = NULL;
}

CVideoImageAttributeInterface::~CVideoImageAttributeInterface ()
{
	m_clearVideoList();
}

bool CVideoImageAttributeInterface::ask_shape (sxsdk::shape_class &shape, void *)
{
	// マスターイメージかどうか.
	if (shape.get_type() != sxsdk::enums::master_image) {
		shade.message(shade.gettext("msg_not_master_image"));
		return false;
	}

	// マスターイメージに割り当てられた属性を取得.
	if (!StreamCtrl::loadVideoData(shape, m_data)) {
		shade.message(shade.gettext("msg_not_video_attribute"));
		return false;
	}

	m_pCurrentShape = &shape;

	compointer<sxsdk::dialog_interface> dlg(shade.create_dialog_interface_with_uuid(VIDEO_IMAGE_ATTRIBUTE_ID));
	dlg->set_resource_name("video_info_dlg");
	dlg->set_responder(this);
	this->AddRef();

	m_data.name = std::string(shape.get_name());

	// 動画情報をOpenCV経由で読み込み.
	CImportVideoWithOpenCV importVideo(shade);
	if (importVideo.init(m_data.fileName, m_data, false)) {
		m_data.fileName   = importVideo.getFilePath();
		m_data.width      = importVideo.getWidth();
		m_data.height     = importVideo.getHeight();
		m_data.frameCount = importVideo.getFrameCount();
		m_data.frameRate  = (float)importVideo.getFPS();
		
		// すでに指定されているマスターイメージとサイズが異なる場合は、古いイメージを消して入れ替え.
		sxsdk::master_image_class& masterImage = shape.get_master_image();
		sxsdk::image_interface* sImage = masterImage.get_image();
		if (sImage && (sImage->get_size().x != m_data.width || sImage->get_size().y != m_data.height)) {
			sImage->Release();
			sImage = NULL;
		}
		if (!sImage) {
			sxsdk::image_interface* firstImage = importVideo.getFirstImage();
			compointer<sxsdk::image_interface> dImage(firstImage->duplicate_image());
			dImage->AddRef();
			masterImage.set_image(dImage);

			const std::string name = StringUtil::getFileName(m_data.fileName);
			masterImage.set_name(name.c_str());

			StreamCtrl::saveVideoData(shape, m_data);
		}

	} else {
		std::string msgStr = std::string("[ ") + m_data.fileName + std::string(" ]\n");
		msgStr += shade.gettext("msg_not_exist_file");
		shade.show_message_box(msgStr.c_str(), false);
		return false;
	}

	if (dlg->ask()) {
		StreamCtrl::saveVideoData(shape, m_data);
	}

	return true;
}

/**
 * コンテキストメニューでのディム指定.
 */
void CVideoImageAttributeInterface::accepts_shape_type (bool &accept, sxsdk::enums::shape_type shape_type, sxsdk::shape_class &shape)
{
	accept = false;
	if (shape_type == sxsdk::enums::master_image) {
		VideoData::CVideoData videoD;
		if (StreamCtrl::loadVideoData(shape, videoD)) accept = true;
	}
}

/**
 * レンダリング開始前(フレームごと)に呼ばれる.
 */
void CVideoImageAttributeInterface::pre_rendering (bool &b, sxsdk::rendering_context_interface *rendering_context, void *)
{
	m_RC = rendering_context;

	const float curFrame = rendering_context->get_current_frame();
	compointer<sxsdk::scene_interface> scene(rendering_context->get_scene_interface());
	const float curFPS   = (float)(scene->get_frame_rate());

	if (!m_renderingF) {
		m_passTimeMS = scene->get_current_time();

		// マスターイメージから動画属性を持つ場合の情報をリストに格納.
		m_storeVideoMasterImage(scene);

		m_renderingF = true;
	}

	// 画像を更新.
	bool updateF = false;
	for (size_t i = 0; i < m_videoList.size(); ++i) {
		CImportVideoWithOpenCV* importVideo = m_videoList[i];
		sxsdk::shape_class* pShape = importVideo->pMasterImage;
		if (!importVideo || !pShape) continue;

		// 動画のフレームを更新.
		sxsdk::image_interface* sImage = importVideo->readImage(curFrame, curFPS, &updateF);
		if (!sImage) continue;

		const int width  = importVideo->getWidth();
		const int height = importVideo->getHeight();
		sxsdk::master_image_class& masterImage = pShape->get_master_image();
		sxsdk::image_interface* image = masterImage.get_image();
		if (!image) continue;

		const int wid2 = image->get_size().x;
		const int hei2 = image->get_size().y;
		if (width != wid2 || height != hei2) continue;

		// sImageの画像をimageにコピー.
		std::vector<sx::rgba8_class> cols;
		cols.resize(width);
		for (int y = 0; y < height; ++y) {
			sImage->get_pixels_rgba(0, y, width, 1, &cols[0]);
			image->set_pixels_rgba(0, y, width, 1, &cols[0]);
		}
		image->update();
	}
}

/**
 * m_videoListをクリア.
 */
void CVideoImageAttributeInterface::m_clearVideoList ()
{
	for (size_t i = 0; i < m_videoList.size(); ++i) {
		if (m_videoList[i]) delete m_videoList[i];
	}
	m_videoList.clear();
}

/**
 * マスターイメージパート内で動画の属性を持つマスターイメージを取得し、m_videoListに格納.
 * TODO : sceneが異なる場合、シーンごとに複数を持たせる必要がある（同時に複数シーンをレンダリングする可能性）.
 */
void CVideoImageAttributeInterface::m_storeVideoMasterImage (sxsdk::scene_interface* scene)
{
	m_clearVideoList();

	sxsdk::shape_class* masterImagePart = Shade3DUtil::findMasteImagePart(scene);
	if (!masterImagePart) return;

	if (masterImagePart->has_son()) {
		VideoData::CVideoData videoD;
		sxsdk::shape_class* pS = masterImagePart->get_son();
		while (pS->has_bro()) {
			pS = pS->get_bro();
			if (!pS) break;

			if (!StreamCtrl::loadVideoData(*pS, videoD)) continue;
			m_videoList.push_back(new CImportVideoWithOpenCV(shade));
			CImportVideoWithOpenCV* importVideo = m_videoList.back();
			if (!importVideo->init(videoD.fileName, videoD, false)) {
				delete m_videoList[m_videoList.size() - 1];
				m_videoList.pop_back();
				{
					std::string msgStr = std::string("[") + std::string(pS->get_name()) + std::string("]");
					shade.message(msgStr.c_str());
					msgStr = std::string("  ") + videoD.fileName;
					shade.message(msgStr.c_str());
					msgStr = std::string("  ") + shade.gettext("msg_not_exist_file");
					shade.message(msgStr.c_str());
				}

			} else {
				importVideo->pMasterImage = pS;

				const int vWidth  = importVideo->getWidth();
				const int vHeight = importVideo->getHeight();

				// すでに指定されているマスターイメージとサイズが異なる場合は更新をスキップ.
				sxsdk::master_image_class& masterImage = pS->get_master_image();
				sxsdk::image_interface* sImage = masterImage.get_image();
				if (sImage && (sImage->get_size().x != vWidth || sImage->get_size().y != vHeight)) {
					delete m_videoList[m_videoList.size() - 1];
					m_videoList.pop_back();

					{
						std::string msgStr = std::string("[") + std::string(pS->get_name()) + std::string("]");
						shade.message(msgStr.c_str());
						msgStr = std::string("  ") + videoD.fileName;
						shade.message(msgStr.c_str());
						msgStr = std::string("  ") + shade.gettext("msg_different_resolution");
						shade.message(msgStr.c_str());
					}
				}
			}
		}
	}
}

/**
 * アイドル時に定期的に呼ばれる.
 */
void CVideoImageAttributeInterface::idle_task (bool &b, sxsdk::scene_interface *scene, void *)
{
	// チェックは1000ms間隔になるようにする.
	if (m_renderingF && m_RC) {
		const int curTime = scene->get_current_time();
		if (m_passTimeMS > 0 && m_passTimeMS + 1000 > curTime) return;
		m_passTimeMS = curTime;
	}

	// レンダリングが終了したときに動画情報をクリアする.
	try {
		if (m_renderingF && m_RC) {
			// sceneを使うのではなく、rendering_contextからのシーンを使う.
			compointer<sxsdk::scene_interface> scene2(m_RC->get_scene_interface());
			compointer<sxsdk::rendering_interface> ri(scene2->get_rendering_interface());
			if (!(ri->is_still_rendering())) {		// レンダリング中でない場合.
				// マスターイメージをはじめのフレームに戻す.
				m_updateFirstImage();

				m_clearVideoList();
				m_renderingF = false;
				m_RC = NULL;
				m_passTimeMS = 0;
			}
		}

	} catch (...) { }
}

/**
 * すべての動画用のマスターイメージをはじめのフレームに戻す.
 */
void CVideoImageAttributeInterface::m_updateFirstImage ()
{
	for (size_t i = 0; i < m_videoList.size(); ++i) {
		CImportVideoWithOpenCV* importVideo = m_videoList[i];
		sxsdk::shape_class* pShape = importVideo->pMasterImage;
		if (!importVideo || !pShape) continue;

		sxsdk::image_interface* sImage = importVideo->getFirstImage();
		if (!sImage) continue;

		const int width  = importVideo->getWidth();
		const int height = importVideo->getHeight();
		sxsdk::master_image_class& masterImage = pShape->get_master_image();
		sxsdk::image_interface* image = masterImage.get_image();
		if (!image) continue;

		const int wid2 = image->get_size().x;
		const int hei2 = image->get_size().y;
		if (width != wid2 || height != hei2) continue;

		// sImageの画像をimageにコピー.
		std::vector<sx::rgba8_class> cols;
		cols.resize(width);
		for (int y = 0; y < height; ++y) {
			sImage->get_pixels_rgba(0, y, width, 1, &cols[0]);
			image->set_pixels_rgba(0, y, width, 1, &cols[0]);
		}
		image->update();
	}
}

/**
 * カレント形状の参照先を変更.
 */
void CVideoImageAttributeInterface::m_changeVideoFile (sxsdk::dialog_interface &d)
{
	if (!m_pCurrentShape) return;

	try {
		// 動画ファイルを読み込む.
		compointer<sxsdk::dialog_interface> dialog(shade.create_dialog_interface());
#if _WINDOWS
		const std::string extStr = "mov/mp4/webm (*.mov *.mp4 *.webm)|mov;mp4;webm|mov (*.mov)|mov|mp4 (*.mp4)|mp4|webm (*.webm)|webm";
#else
		const std::string extStr = "mp4/webm (*.mp4 *.webm)|mp4;webm|mp4 (*.mp4)|mp4|webm (*.webm)|webm";
#endif

		std::string fPath(dialog->ask_path(true, extStr.c_str()));
		if (fPath != "") {
			CImportVideoWithOpenCV importVideo(shade);
			if (importVideo.init(fPath, m_data, false)) {
				m_data.fileName   = importVideo.getFilePath();
				m_data.width      = importVideo.getWidth();
				m_data.height     = importVideo.getHeight();
				m_data.frameCount = importVideo.getFrameCount();
				m_data.frameRate  = (float)importVideo.getFPS();

				// マスターイメージ内のイメージを入れ替え.
				sxsdk::master_image_class& masterImage = m_pCurrentShape->get_master_image();
				sxsdk::image_interface* sImage = masterImage.get_image();
				if (sImage) {
					sImage->Release();
					sImage = NULL;
				}
				if (!sImage) {
					sxsdk::image_interface* firstImage = importVideo.getFirstImage();
					compointer<sxsdk::image_interface> dImage(firstImage->duplicate_image());
					dImage->AddRef();
					masterImage.set_image(dImage);

					const std::string name = StringUtil::getFileName(m_data.fileName);
					masterImage.set_name(name.c_str());

					StreamCtrl::saveVideoData(*m_pCurrentShape, m_data);
				}

				// ダイアログの更新.
				load_dialog_data(d);

			} else {
				shade.show_message_box(shade.gettext("msg_failed_load_video_file"), false);
			}
		}
	} catch (...) { }
}

/**
 * ダイアログの初期化.
 */
void CVideoImageAttributeInterface::initialize_dialog (sxsdk::dialog_interface &d, void *)
{

}

/** 
 * ダイアログのイベントを受け取る.
 */
bool CVideoImageAttributeInterface::respond (sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *)
{
	const int id = item.get_id();		// アクションがあったダイアログアイテムのID.

	if (id == dlg_loop) {
		m_data.playLoop = item.get_bool();
		return true;
	}
	if (id == dlg_useColor) {
		m_data.useColor = item.get_bool();
		load_dialog_data(d);
		return true;
	}

	if (id == dlg_color) {
		m_data.color = item.get_rgb();
		return true;
	}
	if (id == dlg_start_frame) {
		m_data.startFrame = item.get_float();
		return true;
	}
	if (id == dlg_end_frame) {
		m_data.endFrame = item.get_float();
		return true;
	}
	if (id == dlg_use_end_frame) {
		m_data.useEndFrame = item.get_bool();
		load_dialog_data(d);
		return true;
	}

	if (id == dlg_changeFile) {
		m_changeVideoFile(d);
		return true;
	}

	return false;
}

/**
 * ダイアログのデータを設定する.
 */
void CVideoImageAttributeInterface::load_dialog_data (sxsdk::dialog_interface &d, void *)
{
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_loop));
		item->set_bool(m_data.playLoop);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_useColor));
		item->set_bool(m_data.useColor);
	}

	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_color));
		item->set_rgb(m_data.color);
		item->set_enabled(m_data.useColor);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_start_frame));
		item->set_float(m_data.startFrame);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_end_frame));
		item->set_float(m_data.endFrame);
		item->set_enabled(m_data.useEndFrame);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_use_end_frame));
		item->set_bool(m_data.useEndFrame);
	}

	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_info_image_name));
		item->set_text(m_data.name.c_str());
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_info_file_path));

		// 折り返して表示.
		const std::string filePathWrap = StringUtil::convWrapString(m_data.fileName, 40);
		item->set_text(filePathWrap.c_str());
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_info_size));
		char szStr[256];
		sprintf(szStr, "%d x %d", m_data.width, m_data.height);
		item->set_text(szStr);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_info_frame_count));
		char szStr[256];
		sprintf(szStr, "%d", m_data.frameCount);
		item->set_text(szStr);
	}
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_info_frame_rate));
		char szStr[256];
		sprintf(szStr, "%.3f", m_data.frameRate);
		item->set_text(szStr);
	}
}

