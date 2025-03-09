/**
 * @file usbbluetooth_io.c
 * @brief USB Bluetoothデバイスの入出力操作を実装するファイル
 */

#include <usbbluetooth_io.h>
#include <utils.h>
#include <usbbluetooth_log.h>

/* デバイスとの通信タイムアウト時間（ミリ秒） */
#define TIMEOUT 1000

/**
 * @brief HCIヘッダータイプの定義
 * @details Bluetoothプロトコルで使用される各種パケットタイプを定義します
 */
typedef enum
{
    HCI_HDR_TYPE_ACK = 0,          /**< 確認応答パケット */
    HCI_HDR_TYPE_CMD = 1,          /**< コマンドパケット */
    HCI_HDR_TYPE_ACL = 2,          /**< ACLデータパケット */
    HCI_HDR_TYPE_SYN = 3,          /**< 同期パケット */
    HCI_HDR_TYPE_EVT = 4,          /**< イベントパケット */
    HCI_HDR_TYPE_VENDOR = 14,      /**< ベンダー固有パケット */
    HCI_HDR_TYPE_LINK_CONTROL = 15, /**< リンク制御パケット */
} hci_hdr_type_t;

/**
 * @brief USB Bluetoothデバイスをオープンし、通信可能な状態にする
 * @param[in,out] dev デバイス構造体へのポインタ
 * @return 処理結果を示すステータスコード
 * @details 以下の処理を実行します：
 *   - デバイスのオープン
 *   - Bluetoothインターフェースの検索
 *   - カーネルドライバの自動デタッチ設定
 *   - インターフェースのクレーム
 *   - 各種エンドポイントの検索
 */
usbbluetooth_status_t USBBLUETOOTH_CALL usbbluetooth_open(usbbluetooth_device_t *dev)
{
    // Open the device and get a handle...
    int err = libusb_open(dev->device.usb, &dev->handle);
    if (err < LIBUSB_SUCCESS)
        return USBBLUETOOTH_STATUS_ERR_UNK;

    err = _dev_find_bluetooth_interface(dev->device.usb, &dev->interface_num);
    if (err < LIBUSB_SUCCESS)
        return USBBLUETOOTH_STATUS_ERR_UNK;

    err = libusb_set_auto_detach_kernel_driver(dev->handle, 1);
    if (err < LIBUSB_SUCCESS && err != LIBUSB_ERROR_NOT_SUPPORTED)
        return USBBLUETOOTH_STATUS_ERR_UNK;

    err = libusb_claim_interface(dev->handle, dev->interface_num);
    if (err < LIBUSB_SUCCESS)
        return USBBLUETOOTH_STATUS_ERR_UNK;

    err = _dev_find_evt_ep(dev->device.usb, &dev->epnum_evt);
    if (err < LIBUSB_SUCCESS)
        return USBBLUETOOTH_STATUS_ERR_UNK;

    err = _dev_find_acl_in_ep(dev->device.usb, &dev->epnum_acl_in);
    if (err < LIBUSB_SUCCESS)
        return USBBLUETOOTH_STATUS_ERR_UNK;

    err = _dev_find_acl_out_ep(dev->device.usb, &dev->epnum_acl_out);
    if (err < LIBUSB_SUCCESS)
        return USBBLUETOOTH_STATUS_ERR_UNK;

    return USBBLUETOOTH_STATUS_OK;
}

/**
 * @brief USB Bluetoothデバイスをクローズする
 * @param[in,out] dev デバイス構造体へのポインタ
 * @details デバイスハンドルが有効な場合、以下の処理を実行します：
 *   - インターフェースの解放
 *   - デバイスのクローズ
 *   - デバイスハンドルの無効化
 */
void USBBLUETOOTH_CALL usbbluetooth_close(usbbluetooth_device_t *dev)
{
    if (dev->handle == NULL)
        return;
    libusb_release_interface(dev->handle, dev->interface_num);
    libusb_close(dev->handle);
    dev->handle = NULL;
}

/**
 * @brief USB Bluetoothデバイスにデータを書き込む
 * @param[in] dev デバイス構造体へのポインタ
 * @param[in] data 書き込むデータバッファ
 * @param[in] size データサイズ
 * @return 処理結果を示すステータスコード
 * @details パケットタイプに応じて適切な転送方法を選択します：
 *   - HCI_HDR_TYPE_CMD: コントロール転送
 *   - HCI_HDR_TYPE_ACL: バルク転送
 */
usbbluetooth_status_t USBBLUETOOTH_CALL usbbluetooth_write(usbbluetooth_device_t *dev, uint8_t *data, uint16_t size)
{
    if (dev->handle == NULL)
        return USBBLUETOOTH_STATUS_ERR_DEVICE_CLOSED;

    uint8_t type = data[0];
    size--;
    int err = LIBUSB_SUCCESS;
    switch (type)
    {
    case HCI_HDR_TYPE_CMD:
        err = libusb_control_transfer(dev->handle,
                                      LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                                      0x00, 0x0000, 0x0000, &data[1], size, TIMEOUT);
        return (err < LIBUSB_SUCCESS) ? USBBLUETOOTH_STATUS_ERR_UNK : USBBLUETOOTH_STATUS_OK;
    case HCI_HDR_TYPE_ACL:
        err = libusb_bulk_transfer(dev->handle, dev->epnum_acl_out, &data[1], size, NULL, TIMEOUT);
        return (err < LIBUSB_SUCCESS) ? USBBLUETOOTH_STATUS_ERR_UNK : USBBLUETOOTH_STATUS_OK;
    default:
        return USBBLUETOOTH_STATUS_ERR_UNK;
    }
}

/**
 * @brief ACLデータを読み込む内部関数
 * @param[in] dev デバイス構造体へのポインタ
 * @param[out] data 読み込みバッファ
 * @param[in,out] size バッファサイズ（入力）と実際に読み込んだサイズ（出力）
 * @return libusb処理結果コード
 * @details バルク転送を使用してACLデータを読み込みます
 */
int _read_data(usbbluetooth_device_t *dev, uint8_t *data, uint16_t *size)
{
    int recevd = 0;
    int err = libusb_bulk_transfer(dev->handle, dev->epnum_acl_in, &data[1], (*size) - 1, &recevd, TIMEOUT);
    if (err < LIBUSB_SUCCESS)
        return err;
    data[0] = HCI_HDR_TYPE_ACL;
    *size = recevd + 1;
    return LIBUSB_SUCCESS;
}

/**
 * @brief HCIイベントを読み込む内部関数
 * @param[in] dev デバイス構造体へのポインタ
 * @param[out] data 読み込みバッファ
 * @param[in,out] size バッファサイズ（入力）と実際に読み込んだサイズ（出力）
 * @return libusb処理結果コード
 * @details 割り込み転送を使用してHCIイベントを読み込みます
 */
int _read_evts(usbbluetooth_device_t *dev, uint8_t *data, uint16_t *size)
{
    int recevd = 0;
    int err = libusb_interrupt_transfer(dev->handle, dev->epnum_evt, &data[1], (*size) - 1, &recevd, TIMEOUT);
    if (err < LIBUSB_SUCCESS)
        return err;
    data[0] = HCI_HDR_TYPE_EVT;
    *size = recevd + 1;
    return LIBUSB_SUCCESS;
}

/**
 * @brief USB Bluetoothデバイスからデータを読み込む
 * @param[in] dev デバイス構造体へのポインタ
 * @param[out] data 読み込みバッファ
 * @param[in,out] size バッファサイズ（入力）と実際に読み込んだサイズ（出力）
 * @return 処理結果を示すステータスコード
 * @details 以下の順序でデータの読み込みを試みます：
 *   1. HCIイベントの読み込み
 *   2. ACLデータの読み込み
 *   3. データが無い場合はサイズを0として正常終了
 */
usbbluetooth_status_t USBBLUETOOTH_CALL usbbluetooth_read(usbbluetooth_device_t *dev, uint8_t *data, uint16_t *size)
{
    usbbluetooth_log_debug("usbbluetooth_read");

    if (dev->handle == NULL)
        return USBBLUETOOTH_STATUS_ERR_DEVICE_CLOSED;

    int err = _read_evts(dev, data, size);
    usbbluetooth_log_debug("_read_evts[err=%d, size=%d]", err, *size);
    if (err != LIBUSB_ERROR_TIMEOUT)
        return (err == LIBUSB_SUCCESS) ? USBBLUETOOTH_STATUS_OK : USBBLUETOOTH_STATUS_ERR_UNK;

    err = _read_data(dev, data, size);
    usbbluetooth_log_debug("_read_data[err=%d, size=%d]", err, *size);
    if (err != LIBUSB_ERROR_TIMEOUT)
        return (err == LIBUSB_SUCCESS) ? USBBLUETOOTH_STATUS_OK : USBBLUETOOTH_STATUS_ERR_UNK;

    // No data to retrieve...
    *size = 0;
    return USBBLUETOOTH_STATUS_OK;
}
