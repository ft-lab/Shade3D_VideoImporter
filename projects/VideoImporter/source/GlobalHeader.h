/**
 *  @file   GlobalHeader.h
 *  @brief  共通して使用する変数など.
 */

#ifndef _GLOBALHEADER_H
#define _GLOBALHEADER_H

#include "sxsdk.cxx"

/**
 * プラグインインターフェイス派生クラスのプラグインID.
 */
#define VIDEO_IMAGE_IMPORTER_ID sx::uuid_class("4F6EA34E-FC8B-4E0A-847C-0A48435F9D48")
#define VIDEO_IMAGE_ATTRIBUTE_ID sx::uuid_class("B62E1546-5CFB-4258-AD31-1CBE1ED71499")

// streamのバージョン.
#define SHAPE_VIDEO_DATA_STREAM_VERSION 0x100

#endif
