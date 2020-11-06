/**
 * 動画ファイルをインポートする.
 */
#ifndef _IMPORT_MOVIE_H
#define _IMPORT_MOVIE_H

#include "GlobalHeader.h"
#include "VideoData.h"

#include "opencv2/videoio.hpp"
#include "opencv2/core.hpp"

#include <string>

class CImportVideoWithOpenCV
{
private:
	sxsdk::shade_interface& shade;

	std::unique_ptr<cv::VideoCapture> m_video;			// 動画読み込みクラス.
	int m_width, m_height;								// 動画の幅と高さ.
	int m_frameCount;									// 総フレーム数.
	double m_fps;										// FPS.
	int m_currentFrame;									// 読み込んだフレーム数.

	std::string m_filePath;								// ファイルのフルパス.
	std::string m_fileExtension;						// ファイル拡張子 (小文字).

	sxsdk::image_interface* m_image;					// 読み込んだ画像を保持.

	bool m_readExit;									// フレーム情報の読み込み終了.

	VideoData::CVideoData m_videoData;					// 動画情報.
	bool m_fillImageF;									// m_imageを単色で塗りつぶしている場合.

public:
	sxsdk::shape_class* pMasterImage;					// 対応するマスターイメージの参照.

private:

	/**
	 * 1フレームを読み込んでm_imageに画像を格納.
	 */
	bool m_storeImage ();

	/**
	 * m_image内を指定の色でクリア.
	 */
	void m_fillImage (const sxsdk::rgb_class& col);

public:
	CImportVideoWithOpenCV (sxsdk::shade_interface& shade);
	~CImportVideoWithOpenCV ();

	void clear ();

	/**
	 * 読み込んだファイルのフルパスを取得.
	 */
	std::string getFilePath () const { return m_filePath; }

	/**
	 * 動画の幅と高さを取得.
	 */
	int getWidth () const { return m_width; }
	int getHeight () const { return m_height; }

	/**
	 * 全フレーム数を取得.
	 */
	int getFrameCount () const { return m_frameCount; }

	/**
	 * FPSを取得.
	 */
	double getFPS () const { return m_fps; }

	/**
	 * 動画の1フレーム目を取得.
	 */
	sxsdk::image_interface* getImage () { return m_image; }

	/**
	 * 初期化処理.
	 * @param[in]  fileName   ファイル名。拡張子はmp4であること.
	 */
	bool init (const std::string& fileName, const VideoData::CVideoData& videoData, const bool readFirstFrame = true);

	/**
	 * 破棄処理.
	 */
	void term ();

	/**
	 * 1フレーム分を前から順番に読み込みイメージを返す（ランダムには読み込めない）.
	 * @param[in]  frame   レンダリング時のフレーム番号.
	 * @param[in]  fps     レンダリング時のfps.
	 */
	sxsdk::image_interface* readImage (const float frame, const float fps, bool* updateF);

	/**
	 * はじめのフレームを取得.
	 */
	sxsdk::image_interface* getFirstImage ();

};

#endif

