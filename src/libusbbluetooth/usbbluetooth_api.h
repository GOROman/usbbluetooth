/**
 * @file usbbluetooth_api.h
 * @brief USB BluetoothライブラリのAPI定義ヘッダーファイル
 * @details このファイルでは、プラットフォーム間の互換性を確保するための
 *          関数呼び出し規約などの基本的なAPI定義を行います。
 */

#ifndef _H_USBBLUETOOTH_API_
#define _H_USBBLUETOOTH_API_

/* Windowsプラットフォーム用のヘッダーファイルをインクルード */
#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#endif

/**
 * 関数の呼び出し規約を定義するマクロ
 * 
 * このマクロは戻り値の型と関数名の間に配置する必要があります。
 * 関数の定義と宣言の両方でこのマクロを使用する必要があります。
 * これにより、異なるプラットフォーム間での互換性を確保します。
 */
/* プラットフォームに応じた呼び出し規約の定義 */
#if defined(_WIN32) || defined(__CYGWIN__)
#define USBBLUETOOTH_CALL WINAPI  /* Windows環境ではWINAPI呼び出し規約を使用 */
#else
#define USBBLUETOOTH_CALL         /* 他の環境ではデフォルトの呼び出し規約を使用 */
#endif

#endif