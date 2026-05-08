# KN MemoPad Enhanced Edition

**English** | [日本語](README-ja.md)

![](knmemopad.png)

This is a fork of **KN MemoPad**, a text editor for the SHARP Brain created by Knatech, with added features such as **customizable text and background colors**, **support for UTF-8, UTF-16 LE, and CP1252 encodings as well as CRLF, LF, and CR newline formats**, **line and column display**, and a **toolbar**.

The original version can be downloaded [here](https://web.archive.org/web/20210405101135if_/https://bbuseruploads.s3.amazonaws.com/knatech/brain-tools/downloads/knmemopad-0.12-arm.zip?Signature=M8M5J6%2BYIpwBRsuX9Kr5co2%2FkCQ%3D&Expires=1617619295&AWSAccessKeyId=AKIA6KOSE3BNJRRFUUX6&versionId=null&response-content-disposition=attachment%3B%20filename%3D%22knmemopad-0.12-arm.zip%22). For detailed usage instructions, please refer to [this document](https://web.archive.org/web/20210405101157if_/https://bbuseruploads.s3.amazonaws.com/knatech/brain-tools/downloads/brain_apps_knmemopad.pdf?Signature=Ws5WiFXqyhIS1pF%2Fzzm2tbNdcic%3D&Expires=1617619316&AWSAccessKeyId=AKIA6KOSE3BNJRRFUUX6&versionId=null&response-content-disposition=attachment%3B%20filename%3D%22brain_apps_knmemopad.pdf%22).

## Added Feature Specifications

### Character Encodings

Automatic encoding detection is performed based solely on BOM information. Files without a BOM (such as Shift_JIS or UTF-8 without BOM) cannot be detected automatically, so please select the encoding beforehand via **Tools -> Charset...** before opening the file. Alternatively, you may open the file first, then change the encoding and use Reload. This feature can also be disabled via **Tools -> Charset... -> Auto Detect by BOM**. Reload and Save operations use the encoding currently selected at the time. Make sure to set the desired encoding before performing these operations. Even if you accidentally load or save with the wrong encoding, you can just re-select the correct encoding and retry.

Automatic detection works as follows:

1. If the file begins with `EF BB BF`, it is loaded as UTF-8; if it begins with `FF FE`, it is loaded as UTF-16 LE.
2. Otherwise, the file is treated as having no BOM, and the user-selected encoding is checked.
3. If the selected encoding is anything other than UTF-16 LE, it is used as-is. If it is UTF-16 LE, the encoding is switched to Shift_JIS instead.

### Newline Formats

The implementation is based on code provided in [wstring newline conversion - Wabisabi Sample Source](https://www.wabiapp.com/WabiSampleSource/windows/convert_crlf_w.html), with custom modifications. Files using any newline format can be loaded without additional configuration, and even files containing mixed newline formats due to bugs or other issues can still be read correctly. When a file is loaded, the most frequently used newline type is detected and automatically selected in **Tools -> Newline Code...**. If you choose a different newline format before saving, the file will be saved using that format.

### Line and Column Display

Line and column numbers are displayed correctly when word wrap is disabled. However, when word wrap is enabled, wrapped lines makes another line. If you rely on the line and column display, disabling word wrap is recommended. Even the standard Windows Notepad disabled its status bar while word wrap was enabled prior to improvements introduced in Windows 10.

## System Requirements

This software is designed for **Windows CE**. It has been tested on the **[SHARP Brain PW-SH1](https://jp.sharp/support/dictionary/product/pw-sh1.html)** (Windows Embedded CE 6.0, Armv5TEJ). Some features are specific to SHARP Brain devices.

## License

In accordance with the original version, this software is distributed under the **[GNU General Public License Version 3](COPYING)**.

## Download

Downloads are available from **[Releases](../../releases)**.

For SHARP Brain devices, download the Armv4I build and install it according to the instructions provided [here](https://brain.fandom.com/ja/wiki/%E3%82%A2%E3%83%97%E3%83%AA%E3%81%AE%E8%B5%B7%E5%8B%95%E6%96%B9%E6%B3%95).

## Building

After cloning the repository with submodules using:

```sh
git clone --recursive https://github.com/watamario15/kn-memopad.git
```

open the .vcw workspace in eMbedded Visual C++ 4.0, select “KNMemoPad” from the menu in the upper-left corner, and build the project.
