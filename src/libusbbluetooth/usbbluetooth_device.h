#ifndef _H_USBBLUETOOTH_DEVICE_
#define _H_USBBLUETOOTH_DEVICE_

/* 必要なヘッダーファイルのインクルード */
#include <usbbluetooth_api.h>
#include <usbbluetooth_err.h>
#include <stdint.h>
#include <libusb.h>

/**
 * @brief デバイスタイプの列挙型
 * @details USB BluetoothデバイスまたはシリアルBluetoothデバイスを
 *          識別するための列挙型です。将来的な拡張性を考慮して
 *          デバイスタイプを区別します。
 */
typedef enum
{
    USBBLUETOOTH_DEVICE_TYPE_USB = 0,    /* USBタイプのデバイス */
    USBBLUETOOTH_DEVICE_TYPE_SERIAL = 1  /* シリアルタイプのデバイス */
} usbbluetooth_device_type_t;

/**
 * @brief USB Bluetoothデバイスの構造体
 * @details デバイスの状態管理に必要な全ての情報を保持します。
 *          参照カウンタによるメモリ管理、デバイスの識別情報、
 *          通信に必要なエンドポイント情報などが含まれます。
 */
typedef struct
{
    uint8_t ref_count;                   /* 参照カウンタ（デバイスの使用状態を追跡） */
    usbbluetooth_device_type_t type;     /* デバイスタイプ（USB/シリアル識別用） */
    union
    {
        libusb_device *usb;              /* USBデバイスハンドル */
    } device;
    uint16_t vendor_id;                  /* ベンダーID（製造元企業の識別子） */
    uint16_t product_id;                 /* 製品ID（製品の識別子） */
    libusb_device_handle *handle;        /* libusbデバイスハンドル（デバイスとの通信用） */
    uint8_t interface_num;               /* インターフェース番号（使用するUSBインターフェース） */
    uint8_t epnum_evt;                   /* イベント用エンドポイント番号（HCIイベント受信用） */
    uint8_t epnum_acl_in;                /* ACL入力用エンドポイント番号（ACLデータ受信用） */
    uint8_t epnum_acl_out;               /* ACL出力用エンドポイント番号（ACLデータ送信用） */
} usbbluetooth_device_t;

/**
 * @brief マシンに接続されているUSB Bluetoothデバイスの一覧を取得
 * @details システムに接続されている全てのUSB Bluetoothデバイスを検出し、
 *          リストとして返します。この関数は内部でlibusbを使用して
 *          デバイスの列挙を行います。
 * 
 * @param[out] list デバイスリストを格納するポインタのポインタ
 * @return 処理結果を示すステータスコード（成功時はUBT_SUCCESS）
 */
usbbluetooth_status_t USBBLUETOOTH_CALL usbbluetooth_get_device_list(usbbluetooth_device_t ***list);

/**
 * @brief USB Bluetoothデバイスのリストを解放
 * @details usbbluetooth_get_device_listで取得したデバイスリストの
 *          メモリを解放します。この関数は必ずデバイスリストの使用後に
 *          呼び出す必要があります。
 * 
 * @param[in,out] list 解放するデバイスリストのポインタのポインタ
 */
void USBBLUETOOTH_CALL usbbluetooth_free_device_list(usbbluetooth_device_t ***list);

/**
 * @brief Bluetoothデバイスへの参照を取得
 * @details デバイスの参照カウンタをインクリメントし、デバイスの
 *          使用を宣言します。これにより、デバイスが使用中の間は
 *          メモリが解放されないことが保証されます。
 * 
 * @param[in] dev 参照するデバイス
 * @return 参照されたデバイスのポインタ
 */
usbbluetooth_device_t *USBBLUETOOTH_CALL usbbluetooth_reference_device(usbbluetooth_device_t *dev);

/**
 * @brief デバイスの参照を解除しメモリを解放
 * @details デバイスの参照カウンタをデクリメントし、カウンタが0に
 *          なった場合はデバイスのメモリを解放します。デバイスの
 *          使用が完了したら必ずこの関数を呼び出してください。
 * 
 * @param[in,out] dev 参照を解除するデバイスのポインタのポインタ
 */
void USBBLUETOOTH_CALL usbbluetooth_unreference_device(usbbluetooth_device_t **dev);

#endif
