#ifndef _H_USBBLUETOOTH_CORE_
#define _H_USBBLUETOOTH_CORE_

/* 必要なヘッダーファイルのインクルード */
#include <usbbluetooth_api.h>
#include <usbbluetooth_err.h>

/**
 * @brief USB Bluetoothライブラリの初期化
 * @details ライブラリの内部状態を初期化し、使用可能な状態にします。
 *          主に以下の処理を行います：
 *          - libusbライブラリの初期化
 *          - 内部データ構造の初期化
 *          - ログシステムの初期化
 *
 * @note この関数は、他のライブラリ関数を呼び出す前に必ず呼び出されなければなりません。
 *       複数回呼び出しても安全ですが、通常はプログラム開始時に1回だけ呼び出します。
 *
 * @return 初期化の結果を示すステータスコード
 */
usbbluetooth_status_t USBBLUETOOTH_CALL usbbluetooth_init();

/**
 * @brief ライブラリの終了処理とリソースの解放
 * @details ライブラリが使用している全てのリソースを解放します。
 *          以下の処理が行われます：
 *          - 開いている全てのデバイスのクローズ
 *          - libusbコンテキストの解放
 *          - 内部データ構造の解放
 *          - ログシステムの終了処理
 *
 * @note この関数は、プログラムの終了時に必ず呼び出してください。
 *       呼び出し忘れるとメモリリークやリソースの未解放が発生する可能性があります。
 */
void USBBLUETOOTH_CALL usbbluetooth_exit();

#endif
