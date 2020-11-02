/**
 * streamへの入出力.
 */
#include "StreamCtrl.h"

#include <string>
#include <algorithm>
#include <iostream>

/**
 * マスターイメージパートに対して、rendering_objectsの属性を割り当て.
 */
void StreamCtrl::setRenderingObjectsAttr (sxsdk::shape_class& masterImagePart)
{
	try {
		// マスターイメージはrendering_objects で呼ばれるようにしている.
		compointer<sxsdk::stream_interface> stream(masterImagePart.create_attribute_stream_interface_with_uuid(VIDEO_IMAGE_ATTRIBUTE_ID, 
			sxsdk::uuid_from_pluginid(0), sxsdk::uuid_from_pluginid(0), VIDEO_IMAGE_ATTRIBUTE_ID));
		if (!stream) return;
		stream->set_pointer(0);
		stream->set_size(0);
	} catch (...) { }
}

/**
 * Importダイアログボックスの情報を保存.
 */
void StreamCtrl::saveVideoData (sxsdk::shape_class& shape, const VideoData::CVideoData& data)
{
	try {
		// マスターイメージはrendering_objects で呼ばれるようにしている.
		compointer<sxsdk::stream_interface> stream(shape.create_attribute_stream_interface_with_uuid(VIDEO_IMAGE_ATTRIBUTE_ID));
		if (!stream) return;
		stream->set_pointer(0);
		stream->set_size(0);

		int iDat;
		int iVersion = SHAPE_VIDEO_DATA_STREAM_VERSION;
		stream->write_int(iVersion);

		char szStr[512];
		memset(szStr, 0, 510);
		sprintf(szStr, "%s", data.fileName.c_str());
		stream->write(510, szStr);

		iDat = data.playLoop ? 1 : 0;
		stream->write_int(iDat);

		stream->set_label("[Video]");
	} catch (...) { }
}

/**
 * Importダイアログボックスの情報を読み込み.
 */
bool StreamCtrl::loadVideoData (sxsdk::shape_class& shape, VideoData::CVideoData& data)
{
	data.clear();

	try {
		compointer<sxsdk::stream_interface> stream(shape.get_attribute_stream_interface_with_uuid(VIDEO_IMAGE_ATTRIBUTE_ID));
		if (!stream) return false;
		stream->set_pointer(0);

		int iDat;
		int iVersion;
		stream->read_int(iVersion);

		char szStr[512];
		stream->read(510, szStr);
		data.fileName = std::string(szStr);

		stream->read_int(iDat);
		data.playLoop = iDat ? true : false;

		return true;

	} catch (...) { }

	return false;
}

