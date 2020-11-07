/**
 *  @file   main.cpp
 *  @brief  mp4/webm動画をインポートするプラグイン.
 *          Shade3DのプラグインSDKでは動画の読み込み機能はないため、
 *          静止画として1フレーム目を読み込み、レンダラプラグインの前処理で画像を差し替えるとする.
 */

#include "GlobalHeader.h"
#include "VideoImageImporterInterface.h"
#include "VideoImageAttributeInterface.h"

//**************************************************//
//	グローバル関数									//
//**************************************************//
/**
 * プラグインインターフェースの生成.
 */
extern "C" SXSDKEXPORT void STDCALL create_interface (const IID &iid, int i, void **p, sxsdk::shade_interface *shade, void *) {
	unknown_interface *u = NULL;
	
	if (iid == image_importer_iid) {
		if (i == 0) {
			u = new CVideoImageImporterInterface(*shade);
		}
	}
	if (iid == attribute_iid) {
		if (i == 0) {
			u = new CVideoImageAttributeInterface(*shade);
		}
	}

	if (u) {
		u->AddRef();
		*p = (void *)u;
	}
}

/**
 * インターフェースの数を返す.
 */
extern "C" SXSDKEXPORT int STDCALL has_interface (const IID &iid, sxsdk::shade_interface *shade) {

	if (iid == image_importer_iid) return 1;
	if (iid == attribute_iid) return 1;

	return 0;
}

/**
 * インターフェース名を返す.
 */
extern "C" SXSDKEXPORT const char * STDCALL get_name (const IID &iid, int i, sxsdk::shade_interface *shade, void *) {
	// SXULより、プラグイン名を取得して渡す.
	if (iid == image_importer_iid) {
		if (i == 0) {
			return CVideoImageImporterInterface::name(shade);
		}
	}
	if (iid == attribute_iid) {
		if (i == 0) {
			return CVideoImageAttributeInterface::name(shade);
		}
	}

	return 0;
}

/**
 * プラグインのUUIDを返す.
 */
extern "C" SXSDKEXPORT sx::uuid_class STDCALL get_uuid (const IID &iid, int i, void *) {
	if (iid == image_importer_iid) {
		if (i == 0) {
			return VIDEO_IMAGE_IMPORTER_ID;
		}
	}
	if (iid == attribute_iid) {
		if (i == 0) {
			return VIDEO_IMAGE_ATTRIBUTE_ID;
		}
	}

	return sx::uuid_class(0, 0, 0, 0);
}

/**
 * バージョン情報.
 */
extern "C" SXSDKEXPORT void STDCALL get_info (sxsdk::shade_plugin_info &info, sxsdk::shade_interface *shade, void *) {
	info.sdk_version = SHADE_BUILD_NUMBER;
	info.recommended_shade_version = 410000;
	info.major_version = 1;
	info.minor_version = 0;
	info.micro_version = 0;
	info.build_number =  0;
}

/**
 * 常駐プラグイン.
 */
extern "C" SXSDKEXPORT bool STDCALL is_resident (const IID &iid, int i, void *) {
	return true;
}

