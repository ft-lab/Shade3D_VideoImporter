/**
 * 動画の属性を表示。レンダリング時に呼ばれる.
 */
#include "VideoImageAttributeInterface.h"
#include "ImportVideoWithOpenCV.h"
#include "StreamCtrl.h"
#include "VideoData.h"
#include "Shade3DUtil.h"

CVideoImageAttributeInterface::CVideoImageAttributeInterface (sxsdk::shade_interface& shade) : shade(shade)
{
	m_renderingF = false;
	m_RC = NULL;
}

CVideoImageAttributeInterface::~CVideoImageAttributeInterface ()
{
	m_clearVideoList();
}

bool CVideoImageAttributeInterface::ask_shape (sxsdk::shape_class &shape, void *)
{
	m_outputVideoInfo(shape);
	return true;
}

/**
 * 動画情報をメッセージウィンドウに表示.
 */
void CVideoImageAttributeInterface::m_outputVideoInfo (sxsdk::shape_class& shape)
{
	if (shape.get_type() != sxsdk::enums::master_image) {
		shade.message(shade.gettext("msg_not_master_image"));
		return;
	}

	// 動画情報を取得.
	VideoData::CVideoData videoD;
	if (!StreamCtrl::loadVideoData(shape, videoD)) {
		shade.message(shade.gettext("msg_not_video_attribute"));
		return;
	}

	// 動画情報をOpenCV経由で読み込み.
	CImportVideoWithOpenCV importVideo(shade);
	if (importVideo.init(videoD.fileName, false)) {
		const int width      = importVideo.getWidth();
		const int height     = importVideo.getHeight();
		const int frameCount = importVideo.getFrameCount();
		const float fps      = (float)importVideo.getFPS();

		char szStr[512];
		sprintf(szStr, "[%s]", shape.get_name());
		shade.message(szStr);

		sprintf(szStr, "  %s", videoD.fileName.c_str());
		shade.message(szStr);

		sprintf(szStr, "  %d x %d", width, height);
		shade.message(szStr);

		sprintf(szStr, "  %d frames", frameCount);
		shade.message(szStr);

		sprintf(szStr, "  %.4f fps", fps);
		shade.message(szStr);

		shade.message("");
	}
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

	char szStr[256];
	sprintf(szStr, "pre_rendering %f updateF = %d", curFrame, updateF);
	shade.message(szStr);
}

/**
 * 各フレームごとのレンダリング開始前に呼ばれる.
 * ここではマスターイメージパートが呼ばれる.
 * ※ このコールバックは、はじめのpre_renderingの後に呼ばれる.
 */
void CVideoImageAttributeInterface::create_rendering_objects (bool &b, sxsdk::shape_class &shape, sxsdk::scene_interface &scene, void *)
{
	if (!m_renderingF) {
		// マスターイメージから動画属性を持つ場合の情報をリストに格納.
		m_storeVideoMasterImage(&scene);

		char szStr[256];
		sprintf(szStr, "videos %d", (int)m_videoList.size());
		shade.message(szStr);

		m_renderingF = true;
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
			if (!importVideo->init(videoD.fileName)) {
				delete m_videoList[m_videoList.size() - 1];
				m_videoList[m_videoList.size() - 1] = NULL;
			}
			importVideo->pMasterImage = pS;
		}
	}
}

/**
 * アイドル時に定期的に呼ばれる.
 */
void CVideoImageAttributeInterface::idle_task (bool &b, sxsdk::scene_interface *scene, void *)
{
	// レンダリングが終了したときに動画情報をクリアする.
	try {
		if (m_renderingF && m_RC) {
			compointer<sxsdk::scene_interface> scene2(m_RC->get_scene_interface());
			compointer<sxsdk::rendering_interface> ri(scene2->get_rendering_interface());
			if (!(ri->is_still_rendering())) {		// レンダリング中でない場合.
				// マスターイメージをはじめのフレームに戻す.
				m_updateFirstImage();

				m_clearVideoList();
				m_renderingF = false;
				m_RC = NULL;
				shade.message("exit rendering.");
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
