#ifndef _H_USBBLUETOOTH_IO_
#define _H_USBBLUETOOTH_IO_

/* 必要なヘッダーファイルのインクルード */
#include <usbbluetooth_api.h>
#include <usbbluetooth_device.h>
#include <stdint.h>

/**
 * @brief USB Bluetoothデバイスをオープン
 * @details 指定されたUSB Bluetoothデバイスをオープンし、通信可能な状態にします。
 *          以下の処理が行われます：
 *          - USBデバイスのオープン
 *          - インターフェースの設定
 *          - エンドポイントの初期化
 * 
 * @param[in] dev オープンするデバイス
 * @return 処理結果を示すステータスコード（成功時はUBT_SUCCESS）
 */
usbbluetooth_status_t USBBLUETOOTH_CALL usbbluetooth_open(usbbluetooth_device_t *dev);

/**
 * @brief USB Bluetoothデバイスをクローズ
 * @details 指定されたデバイスを安全にクローズし、他のプロセスが
 *          使用可能な状態にします。以下の処理が行われます：
 *          - 進行中の転送の中止
 *          - インターフェースの解放
 *          - USBデバイスのクローズ
 * 
 * @param[in] dev クローズするデバイス
 */
void USBBLUETOOTH_CALL usbbluetooth_close(usbbluetooth_device_t *dev);

/**
 * @brief デバイスにHCIコマンドを送信
 * @details 指定されたデバイスにHCI（Host Controller Interface）コマンドを
 *          送信します。この関数は非同期で実行され、データの送信が
 *          完了するまでブロックします。
 * 
 * @param[in] dev 書き込み先のデバイス
 * @param[in] data 送信するHCIコマンドデータ
 * @param[in] size データサイズ（バイト単位）
 * @return 処理結果を示すステータスコード（成功時はUBT_SUCCESS）
 */
usbbluetooth_status_t USBBLUETOOTH_CALL usbbluetooth_write(usbbluetooth_device_t *dev, uint8_t *data, uint16_t size);

/**
 * @brief デバイスからHCIデータを受信
 * @details 指定されたデバイスからHCI（Host Controller Interface）データを
 *          受信します。この関数は非同期で実行され、データの受信が
 *          完了するまで、またはタイムアウトが発生するまでブロックします。
 * 
 * @param[in] dev 読み取り元のデバイス
 * @param[out] data 受信したデータを格納するバッファ
 * @param[in,out] size 入力時はバッファサイズ、出力時は実際に受信したデータサイズ（バイト単位）
 * @return 処理結果を示すステータスコード（成功時はUBT_SUCCESS）
 */
usbbluetooth_status_t USBBLUETOOTH_CALL usbbluetooth_read(usbbluetooth_device_t *dev, uint8_t *data, uint16_t *size);

#endif
