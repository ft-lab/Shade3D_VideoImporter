# Video Importer for Shade3D

Shade3Dに動画ファイル（mp4/webm/mov）を読み込み、動画テクスチャとしてレンダリングに反映するプラグインです。    
本プラグインは動画出力でOpenCV 4.5.0 ( https://opencv.org/ )を使用しています。    

## 機能

以下の機能があります。    

* 動画ファイル(mp4/webm、Windows環境では加えてmov)をシーンに外部参照としてインポートし、マスターイメージにします。    
* 読み込んだ動画は、レンダリング時に動画テクスチャとして反映できます。     
* レンダリング時に、動画の開始フレーム位置/終了フレーム位置の指定ができます。
* レンダリング時に、動画のループ再生の指定ができます。
* レンダリング時に、動画の開始フレーム/終了フレーム外の場合に、指定の色で塗りつぶすことができます。    

## 動作環境

* Windows 7/8/10以降のOS
* macOS 10.11以降
* Shade3D ver.14以降で、Standard/Professional版（Basic版では動作しません）
* Shade3Dの64bit版のみで使用できます。32bit版のShade3Dには対応していません。

## 起動方法

### プラグインダウンロード

以下からプラグインの最新版をダウンロードしてください。     

https://github.com/ft-lab/Shade3D_VideoImporter/releases

Video Importer for Shade3Dプラグインは、OpenCVの動的ライブラリを参照します。    
起動方法はWindows/Macで異なります。    

### Windows環境の場合

[起動方法 (Windows)](./setup_win.md) をご参照くださいませ。    


### Mac環境の場合

[起動方法 (Mac)](./setup_mac.md) をご参照くださいませ。    

## 使い方

ツールボックスの「作成」-「その他」-「イメージ」でイメージをシーンに読み込む際に、mp4/webm、Windows環境では加えてmovを選択できます。    
<img src="./images/video_importer_01.png"/>     
シーンに「外部参照として」動画ファイルを読み込みます。    
正しく読み込まれた場合は、マスターイメージが生成されます。     
このとき、対象のマスターイメージのラベルに [Video] が付いています。    
<img src="./images/video_importer_02.png"/>     
また、このときの形状情報ウィンドウでは、はじめのフレームが静止画として反映されます。    
このときの「動画情報」は使用されません。    

### 動画情報の確認

ブラウザで [Video] のラベルが付いたマスターイメージを選択し、    
「情報」で「動画情報」を選択します。    
<img src="./images/video_importer_03.png"/>     

動画情報ダイアログボックスが表示されます。    
<img src="./images/video_importer_04.png"/>     


## 制限事項

* オーディオ情報は反映されません。    

## ビルド方法 (開発者向け)

プラグインのビルド方法です。    

* [ビルド方法 (Windows)](./build_win.md)
* [ビルド方法 (Mac)](./build_mac.md)

## ライセンス

This software is released under the MIT License, see [LICENSE](./LICENSE).    

### OpenCVのライセンス

OpenCVは「3-clause BSD License」になります。    
https://opencv.org/license/    

### H.264のライセンス

「Video Importer for Shade3D」では、OpenCVの機能を使用してmp4(CODECはH.264)形式の読み込みを行っています。    
本プラグインではH.264にはタッチしていません。    
OpenCVでは、Windows版のmp4(H.264)インポート/エクスポートで「Microsoft Media Foundation (MSMF)」を使用しているようです。    
この場合は特にライセンスはOpenCV側では問われないようです。    

H.264を使用している場合の出力動画の使用については、使い方によってはエンドユーザーとしてのライセンスが必要になる場合があります。    
"AVC Patent Portfolio License" ( https://www.mpegla.com/programs/avc-h-264/license-agreement/ )も参照するようにしてください。    

## 更新履歴

### [2020/11/07] ver.1.0.0.0

* 初回バージョン

