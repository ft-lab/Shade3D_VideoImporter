/**
 * 動画ファイルをインポートする.
 */
#include "ImportVideoWithOpenCV.h"
#include "StringUtil.h"

// sxsdk::image_interface* の解放処理.
// 注意点として、compointer<sxsdk::image_interface>で確保した場合は自動で解放されるため、Releaseを呼んではいけない.
#define IMAGE_INTERFACE_RELEASE(image) {if (image) { image->Release(); image = NULL; } }

CImportVideoWithOpenCV::CImportVideoWithOpenCV (sxsdk::shade_interface& shade) : shade(shade), m_image(NULL)
{
	clear();
}

CImportVideoWithOpenCV::~CImportVideoWithOpenCV ()
{
	clear();
}

void CImportVideoWithOpenCV::clear ()
{
	m_video.reset();
	IMAGE_INTERFACE_RELEASE(m_image);
	m_width = m_height = 0;
	m_frameCount = 0;
	m_fps = 30.0;
	m_currentFrame = 0;
	m_readExit = false;
	pMasterImage = NULL;
	m_loop = false;
	m_filePath = "";
	m_fileExtension = "";
}

/**
 * 初期化処理.
 * @param[in]  fileName   ファイル名。拡張子はmp4であること.
 */
bool CImportVideoWithOpenCV::init (const std::string& fileName, const bool readFirstFrame)
{
	clear();
	m_video.reset(new cv::VideoCapture());

	m_filePath = fileName;
	m_video->open(m_filePath);
	if (m_video->isOpened() == false) {
		try {
			// Shade3Dのシーンが保存されているフォルダ内に、動画ファイルがあるかチェック.
			compointer<sxsdk::scene_interface> scene(shade.get_scene_interface());
			if (scene) {
				const std::string shdFilePath(scene->get_file_path());
				if (shdFilePath == "") {
					m_video.reset();
					m_filePath = "";
					return false;
				}
				const std::string videoFileName = StringUtil::getFileName(fileName);
				const std::string shdDir = StringUtil::getFileDir(shdFilePath);
#if _WINDOWS
				m_filePath = shdDir + "\\" + videoFileName;
#else
				m_filePath = shdDir + "/" + videoFileName;
#endif
				m_video->open(m_filePath);
				if (m_video->isOpened() == false) {
					m_video.reset();
					m_filePath = "";
					return false;
				}
			}
		} catch (...) {
			return false;
		}
	}

	// ファイル拡張子.
	m_fileExtension = StringUtil::getFileExtension(fileName);

	// 動画のパラメータを取得.
	m_width      = m_video->get(cv::CAP_PROP_FRAME_WIDTH);
	m_height     = m_video->get(cv::CAP_PROP_FRAME_HEIGHT);
	m_frameCount = m_video->get(cv::CAP_PROP_FRAME_COUNT);
	m_fps        = m_video->get(cv::CAP_PROP_FPS);

	if (!readFirstFrame) return true;
	return m_storeImage();
}

/**
 * ループ再生を行うか.
 */
void CImportVideoWithOpenCV::setLoop (const bool b)
{
	m_loop = b;
}

/**
 * 破棄処理.
 */
void CImportVideoWithOpenCV::term ()
{
	// m_viderWriterを解放することでファイルが閉じられる状態になる.
	clear();
}

/**
 * 1フレームを読み込んでm_imageに画像を格納.
 */
bool CImportVideoWithOpenCV::m_storeImage ()
{
	if (!m_video) return false;
	if (m_video->isOpened() == false) return false;

	// ループ再生時は、はじめのフレームに戻る.
	const int curFramePos = m_video->get(cv::CAP_PROP_POS_FRAMES);
	if (m_loop) {
		if (curFramePos + 1 >= m_frameCount) {
			m_video->set(cv::CAP_PROP_POS_FRAMES, 0);
		}
	}

	cv::Mat cvImage;
	(*m_video) >> cvImage;
	if (cvImage.empty()) {		// 最終フレームに到達しない場合でもカラが返る場合がある.
		m_currentFrame++;
		return true;
	}
	m_currentFrame++;

	if (m_image) {
		const int wid = m_image->get_size().x;
		const int hei = m_image->get_size().y;
		if (wid != m_width || hei != m_height) IMAGE_INTERFACE_RELEASE(m_image);
	}
	if (!m_image) {
		m_image = shade.create_image_interface(sx::vec<int,2>(m_width, m_height));
	}

	std::vector<sx::rgba8_class> lines;
	lines.resize(m_width);
	int iPos = 0;

	const int type = cvImage.type();
	if (type == CV_8UC3) {
		const int widSize = m_width * 3;
		for (int y = 0; y < m_height; ++y) {
			int iPos2 = iPos;
			for (int x = 0; x < m_width; ++x) {
				lines[x].red   = *(cvImage.data + iPos2 + 2);
				lines[x].green = *(cvImage.data + iPos2 + 1);
				lines[x].blue  = *(cvImage.data + iPos2 + 0);
				lines[x].alpha = 255;
				iPos2 += 3;
			}
			m_image->set_pixels_rgba(0, y, m_width, 1, &(lines[0]));
			iPos += widSize;
		}

	} else if (type == CV_8UC4) {
		const int widSize = m_width * 4;

		for (int y = 0; y < m_height; ++y) {
			int iPos2 = iPos;
			for (int x = 0; x < m_width; ++x) {
				lines[x].red   = *(cvImage.data + iPos2 + 2);
				lines[x].green = *(cvImage.data + iPos2 + 1);
				lines[x].blue  = *(cvImage.data + iPos2 + 0);
				lines[x].alpha = *(cvImage.data + iPos2 + 3);
				iPos2 += 4;
			}
			m_image->set_pixels_rgba(0, y, m_width, 1, &(lines[0]));
			iPos += widSize;
		}
	} else {
		return false;
	}

	return true;
}

/**
 * 1フレーム分を前から順番に読み込みイメージを返す（ランダムには読み込めない）.
 * @param[in]  frame   レンダリング時のフレーム番号.
 * @param[in]  fps     レンダリング時のfps.
 */
sxsdk::image_interface* CImportVideoWithOpenCV::readImage (const float frame, const float fps, bool* updateF)
{
	if (updateF) *updateF = false;
	if (m_readExit) return m_image;

	float buffSec = (float)((float)m_currentFrame / m_fps);
	const float renderSec = (float)(frame / fps);

	while (buffSec < renderSec) {
		if (!m_storeImage()) {
			m_readExit = true;
			break;
		}
		if (updateF) *updateF = true;
		buffSec = (float)((float)m_currentFrame / m_fps);
	}

	return m_image;
}

/**
 * はじめのフレームを取得.
 */
sxsdk::image_interface* CImportVideoWithOpenCV::getFirstImage ()
{
	if (!m_video) return NULL;

	m_video->set(cv::CAP_PROP_POS_FRAMES, 0);
	m_currentFrame = 0;
	m_storeImage();

	return m_image;
}

