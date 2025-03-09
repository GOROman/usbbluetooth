/**
 * @file usbbluetooth.h
 * @brief USB Bluetoothライブラリのメインヘッダーファイル
 * @details このファイルは、USB Bluetoothライブラリのすべてのヘッダーファイルを
 *          インクルードし、ライブラリの全機能にアクセスできるようにします。
 */

#ifndef _H_USBBLUETOOTH_
#define _H_USBBLUETOOTH_

/* 各コンポーネントのヘッダーファイルをインクルード */
#include <usbbluetooth_err.h>   /* エラー処理関連 */
#include <usbbluetooth_core.h>  /* コア機能関連 */
#include <usbbluetooth_device.h> /* デバイス操作関連 */
#include <usbbluetooth_io.h>    /* 入出力操作関連 */
#include <usbbluetooth_log.h>   /* ログ出力関連 */

#endif
