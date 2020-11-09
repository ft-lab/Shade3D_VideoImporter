# 他のプラグインから、Video Importerの動画ファイルを確認

Shade3DのプラグインSDKでは、「マスターイメージが動画情報を持っているか」という情報は標準のavi(Macの場合はmov)以外は対応していません。    
また、動画情報をプラグインSDKから指定することはできません。    

Video Importerプラグインは、マスターイメージに属性を持たせ動画の参照情報を埋め込んでいます。     
以下のように属性チェックを行うことで、そのマスターイメージがVideo Importerの動画情報を持つか判定できます。    

    #define VIDEO_IMPORTER_ATTRIBUTE_ID sx::uuid_class("B62E1546-5CFB-4258-AD31-1CBE1ED71499")
    
    bool checkVideoImporterImage (sxsdk::shape_class& shape) {
        // マスターイメージか判定.
        if (shape.get_type() != sxsdk::enums::master_image) return false;
        
        // 属性をチェック.
        std::string filePath = "";
        try {
            compointer<sxsdk::stream_interface> stream(shape.get_attribute_stream_interface_with_uuid(VIDEO_IMPORTER_ATTRIBUTE_ID));
            if (!stream) return false;
            
            stream->set_pointer(0);
            
            // 属性の内部バージョン.
            int iVersion;
            stream->read_int(iVersion);
            
            // 参照しているファイルパスを取得.
            char szStr[512];
            stream->read(510, szStr);
            filePath = std::string(szStr);
            
            return true;
        } catch (...) { }
        return false;
    }

このcheckVideoImporterImage関数では第一引数に入れた形状の属性をチェックし、Video Importerプラグインで指定した動画情報を持つ場合はtrueを返します。     
レンダリング時はマスターイメージのsxsdk::image_interfaceはフレームごとに自動で更新されるため、それを取得することで独自レンダラでも動画を反映することができます。     
また、このコード内の filePath に参照している動画ファイルの外部参照のフルパスが入ります。    

