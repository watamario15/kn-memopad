# KN MemoPad 機能追加版

Knatech 氏制作の SHARP Brain 用テキストエディタ **KN MemoPad** に、**文字色と背景色の変更機能**、 **UTF-8・UTF-16 LE・MS1252 文字コード及び CRLF・LF・CR 改行コードのサポート**、**行・列数表示機能**を追加したフォークです。既存機能はそのままですので、何かミスがなければ完全な上位互換として動作するはずです。

オリジナル版との干渉が起こることを防ぐため、設定の保存ファイルは別にしています。

オリジナル版は[ここ](https://web.archive.org/web/20210405101135if_/https://bbuseruploads.s3.amazonaws.com/knatech/brain-tools/downloads/knmemopad-0.12-arm.zip?Signature=M8M5J6%2BYIpwBRsuX9Kr5co2%2FkCQ%3D&Expires=1617619295&AWSAccessKeyId=AKIA6KOSE3BNJRRFUUX6&versionId=null&response-content-disposition=attachment%3B%20filename%3D%22knmemopad-0.12-arm.zip%22)から入手できます。詳しい使い方は[こちら](https://web.archive.org/web/20210405101157if_/https://bbuseruploads.s3.amazonaws.com/knatech/brain-tools/downloads/brain_apps_knmemopad.pdf?Signature=Ws5WiFXqyhIS1pF%2Fzzm2tbNdcic%3D&Expires=1617619316&AWSAccessKeyId=AKIA6KOSE3BNJRRFUUX6&versionId=null&response-content-disposition=attachment%3B%20filename%3D%22brain_apps_knmemopad.pdf%22)を参照してください。

## 動作要件

これは **Windows CE (Armv4I)** 用ソフトウェアです。一部 SHARP Brain 専用の機能があります。

**SHARP Brain PW-SH1 (Windows Embedded CE 6.0, Armv5TEJ)** で動作確認を行っています。

## 文字コード切り替えについて

BOM の情報だけを基に文字コードの自動判別を行います。BOM 無しファイル（Shift_JIS/BOM 無し UTF-8）の場合は、判別できませんので予め **Tools -> Charset...** で選択してから開いてください。先に開いてから選択し、Reload でも構いません。この機能は **Tools -> Charset... -> Auto Detect by BOM** から無効化も可能です。

Reload 及び Save は、その時点で選択されている文字コードで行われます。予め開きたい or 保存したい文字コードに設定してからそれらの処理を行ってください。間違った文字コードで処理してしまっても、正しく設定してからやり直せば大丈夫です。

### 自動判別の仕様

1. 最初が「EF BB BF」なら UTF-8、「FF FE」なら UTF-16 LE として読み込む。
1. 違ったら BOM 無しと判断し、現在ユーザが設定している文字コードを見る。UTF-16 LE 以外ならそのまま、UTF-16 LE なら Shift_JIS に設定する。

## 改行コード処理について

[wstring 中の改行コードを変換する - わびさびサンプルソース](https://www.wabiapp.com/WabiSampleSource/windows/convert_crlf_w.html)で提供されているコードを基に、カスタマイズして使用しています。

どの改行コードでも指定不要で読み込める他、バグなどで改行コードが混在したファイルでも、正常に読み込むことができます。

読み込んだとき、最も多かった改行コード種をそのファイルの改行コードと判断し、**Tools -> Newline Code...** に設定します。保存前に別の改行コードに設定すれば、その改行コードで保存します。

## 行・列数表示について

wordwrap が無効であれば正しく表示されますが、有効になっていると折り返しも 1 行とカウントされてしまいます。行・列数表示を参考にする場合は、 wordwrap を無効化することをお勧めします。Windows 標準のメモ帳でも、Windows 10 で改良される前まで wordwrap 有効時はステータスバーが無効になっていました。

## ソースコードについて

**eMbedded Visual C++ 4.0** から .vcw ワークスペースを開いてください。

## 著作権情報など

**製作者は、このプログラムの利用によって生じた、いかなる損害についても責任を負いません。**

オリジナル版の規定に従い、同じく **[GNU General Public License](LICENSE)** で配布します。

## 変更履歴

### v0.12 rev7 (2020/11/1)

改行コード CRLF で保存しようとすると、本文が空のデータが保存される問題を修正。

### v0.12 rev6 (2020/10/3)

ステータスバーの表示・非表示を切り替えられるようにした。

Reload 時は文字コード自動判別を行わないように変更した。

ステータスバーの行・列表示欄の幅が狭く、不足する場合があったのを修正。

ステータスバーがテキスト編集部分の上に被さってしまう問題を修正。

ウィンドウサイズが変化したとき、ステータスバーが適切に移動しない問題を修正。

その他、細かい修正や変更を行った。

### v0.12 rev5 (2020/9/30)

MS1252（英語圏 Windows の ANSI）に対応した（海外のソフトの readme でたまに使われているので）。

現在指定されている改行コードと文字コードもステータスバーに表示するようにした。

Add BOM メニュー項目がうまく機能しなくなっていたのを修正。

### v0.12 rev4 (2020/9/30)

各改行コードに対応した。

現在キャレットがある行・列の番号を、ステータスバーに表示するようにした。

### v0.12 rev3 (2020/9/28)

BOM だけで判別する、簡単な文字コード自動判別を実装した。

### v0.12 rev2 (2020/9/23)

UTF-8（BOM 有り無し両方）と UTF-16 LE に対応した。それに伴い、ファイル読み込み・保存の部分を大きく書き換えた。これによりオリジナル版の、ファイル末尾に勝手に改行が追加される仕様はなくなった。

色選択ダイアログ左下の、作成した色リストを保存するようにした。

バージョン情報を追加し、ソフトウェア内からも確認できるようにした。

コマンドラインや関連付けで指定されたファイルも開けるようにした。

### v0.12 rev1 (2020/9/20)

文字色・背景色を変更できる機能を追加した。
