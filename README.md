# KN MemoPad 機能追加版

![](knmemopad.png)

Knatech 氏制作の SHARP Brain 用テキストエディタ **KN MemoPad** に、**文字色と背景色の変更機能**、 **UTF-8・UTF-16 LE・CP1252 文字コード及び CRLF・LF・CR 改行コードのサポート**、**行・列数表示機能**、**ツールバー**などを追加したフォークです。

オリジナル版は[ここ](https://web.archive.org/web/20210405101135if_/https://bbuseruploads.s3.amazonaws.com/knatech/brain-tools/downloads/knmemopad-0.12-arm.zip?Signature=M8M5J6%2BYIpwBRsuX9Kr5co2%2FkCQ%3D&Expires=1617619295&AWSAccessKeyId=AKIA6KOSE3BNJRRFUUX6&versionId=null&response-content-disposition=attachment%3B%20filename%3D%22knmemopad-0.12-arm.zip%22)から入手できます。詳しい使い方は[こちら](https://web.archive.org/web/20210405101157if_/https://bbuseruploads.s3.amazonaws.com/knatech/brain-tools/downloads/brain_apps_knmemopad.pdf?Signature=Ws5WiFXqyhIS1pF%2Fzzm2tbNdcic%3D&Expires=1617619316&AWSAccessKeyId=AKIA6KOSE3BNJRRFUUX6&versionId=null&response-content-disposition=attachment%3B%20filename%3D%22brain_apps_knmemopad.pdf%22)を参照してください。

## 追加機能の仕様

### 文字コード

BOM の情報だけを基に文字コードの自動判別を行います。BOM 無しファイル（Shift_JIS/BOM 無し UTF-8）の場合は、判別できませんので予め **Tools -> Charset...** で選択してから開いてください。先に開いてから選択し、Reload でも構いません。この機能は **Tools -> Charset... -> Auto Detect by BOM** から無効化も可能です。Reload 及び Save は、その時点で選択されている文字コードで行われます。予め開きたい or 保存したい文字コードに設定してからそれらの処理を行ってください。間違った文字コードで処理してしまっても、正しく設定してからやり直せば大丈夫です。

自動判別は以下の仕様です。

1. 最初が `EF BB BF` なら UTF-8、`FF FE` なら UTF-16 LE として読み込む
1. 違ったら BOM 無しと判断し、現在ユーザが設定している文字コードを見る
1. UTF-16 LE 以外ならそのまま、UTF-16 LE なら Shift_JIS に設定する

### 改行コード

[wstring 中の改行コードを変換する - わびさびサンプルソース](https://www.wabiapp.com/WabiSampleSource/windows/convert_crlf_w.html)で提供されているコードを基に、カスタマイズして使用しています。どの改行コードでも指定不要で読み込める他、バグなどで改行コードが混在したファイルでも、正常に読み込むことができます。読み込んだとき、最も多かった改行コード種をそのファイルの改行コードと判断し、**Tools -> Newline Code...** に設定します。保存前に別の改行コードに設定すれば、その改行コードで保存します。

### 行・列表示

wordwrap が無効であれば正しく表示されますが、有効になっていると折り返しも 1 行とカウントされます。行・列表示を参考にする場合は、 wordwrap を無効化することをお勧めします。Windows 標準のメモ帳でも、Windows 10 で改良される前まで wordwrap 有効時はステータスバーが無効になっていました。

## 動作要件

これは **Windows CE** 用ソフトウェアです。**[SHARP Brain PW-SH1](https://jp.sharp/support/dictionary/product/pw-sh1.html) (Windows Embedded CE 6.0, Armv5TEJ)** で動作確認を行っています。一部 SHARP Brain 専用の機能があります。

## 利用上の注意

**製作者は、このプログラムの利用によって生じた、いかなる損害についても責任を負いません。**

オリジナル版の規定に従い **[GNU General Public License](LICENSE)** の下で配布します。

## ダウンロード

**[Releases](../../releases)** からダウンロードできます。SHARP Brain では Armv4I 向けのものをダウンロードし、[こちら](https://brain.fandom.com/ja/wiki/%E3%82%A2%E3%83%97%E3%83%AA%E3%81%AE%E8%B5%B7%E5%8B%95%E6%96%B9%E6%B3%95)の手順に従って導入してください。

## ビルド方法

```sh
git clone --recursive https://github.com/watamario15/kn-memopad.git
```

によって submodule 込みで clone したのち、eMbedded Visual C++ 4.0 から .vcw ワークスペースを開き、左上のメニューで「KNMemoPad」を選択した状態でビルドします。
