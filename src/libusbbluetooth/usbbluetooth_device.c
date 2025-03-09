/* デバイス関連のヘッダーファイルをインクルード */
#include <usbbluetooth_device.h>

/* 必要なヘッダーファイルをインクルード */
#include <stdlib.h>
#include <utils.h>
#include <usbbluetooth_log.h>

/* 内部関数のプロトタイプ宣言 */
static int _count_bluetooth_devices(libusb_device **list, int *num);
static usbbluetooth_device_t *_dev_from_libusb(libusb_device *dev);

/**
 * マシンに接続されているUSB Bluetoothデバイスの一覧を取得する。
 * 
 * @param list_ptr デバイスリストを格納するポインタのポインタ
 * @return 処理結果を示すステータスコード
 */
usbbluetooth_status_t USBBLUETOOTH_CALL usbbluetooth_get_device_list(usbbluetooth_device_t ***list_ptr)
{
    usbbluetooth_log_debug("usbbluetooth_get_device_list");

    // リストパラメータをチェック
    if (list_ptr == NULL)
        return USBBLUETOOTH_STATUS_ERR_UNK;

    // すべてのデバイスのリストを取得...
    libusb_device **devs_internal;
    int r = libusb_get_device_list(NULL, &devs_internal);
    usbbluetooth_log_debug("libusb_get_device_list[r=%d]", r);
    if (r < LIBUSB_SUCCESS)
        return USBBLUETOOTH_STATUS_ERR_UNK;

    // Bluetoothデバイスの数をカウント...
    int num_devs = 0;
    r = _count_bluetooth_devices(devs_internal, &num_devs);
    usbbluetooth_log_debug("_count_bluetooth_devices[r=%d, n=%d]", r, num_devs);
    if (r < LIBUSB_SUCCESS)
        return USBBLUETOOTH_STATUS_ERR_UNK;

    // 新しいリストを作成!
    *list_ptr = calloc((size_t)num_devs + 1, sizeof(struct usbbluetooth_device_t *));
    if (*list_ptr == NULL)
        return USBBLUETOOTH_STATUS_ERR_NOMEM;

    // リストを終端...
    (*list_ptr)[num_devs] = NULL;

    // リストを取得
    usbbluetooth_device_t **list = *list_ptr;

    // 再度イテレート...
    libusb_device *dev;
    for (int i = 0, pos = 0; (dev = devs_internal[i]) != NULL; i++)
    {
        bool is_bt = false;
        if (_is_bluetooth_device(dev, &is_bt) == LIBUSB_SUCCESS && is_bt)
        {
            list[pos++] = usbbluetooth_reference_device(_dev_from_libusb(dev));
        }
    }

    // クリーンアップ
    libusb_free_device_list(devs_internal, 1);

    return USBBLUETOOTH_STATUS_OK;
}

/**
 * Bluetoothデバイスの数をカウントする内部関数。
 * 
 * @param list libusbデバイスのリスト
 * @param num カウントした数を格納する変数へのポインタ
 * @return 処理結果を示すステータスコード
 */
static int _count_bluetooth_devices(libusb_device **list, int *num)
{
    // カウンタをリセット...
    *num = 0;

    // すべてのデバイスをイテレート...
    libusb_device *dev;
    for (int i = 0; (dev = list[i]) != NULL; i++)
    {
        // デバイスがBluetoothインターフェースを持つか確認...
        bool is_bt = false;
        int r = _is_bluetooth_device(dev, &is_bt);
        if (r == LIBUSB_ERROR_NOT_FOUND)
            is_bt = false;
        else if (r < LIBUSB_SUCCESS)
            return r;
        if (is_bt)
            (*num)++;
    }
    return LIBUSB_SUCCESS;
}

/**
 * libusbデバイスからUSB Bluetoothデバイス構造体を作成する内部関数。
 * 
 * @param dev libusbデバイス
 * @return 新しく作成されたUSB Bluetoothデバイス構造体へのポインタ
 */
static usbbluetooth_device_t *_dev_from_libusb(libusb_device *dev)
{
    usbbluetooth_device_t *btdev = calloc(1, sizeof(usbbluetooth_device_t));
    btdev->ref_count = 0;
    btdev->type = USBBLUETOOTH_DEVICE_TYPE_USB;
    btdev->device.usb = libusb_ref_device(dev);
    struct libusb_device_descriptor desc;
    libusb_get_device_descriptor(dev, &desc);
    btdev->vendor_id = desc.idVendor;
    btdev->product_id = desc.idProduct;
    btdev->handle = NULL;
    btdev->interface_num = 0;
    btdev->epnum_evt = 0;
    btdev->epnum_acl_in = 0;
    btdev->epnum_acl_out = 0;
    return btdev;
}

/**
 * USB Bluetoothデバイスのリストを解放する。
 * 
 * @param list 解放するデバイスリストのポインタのポインタ
 */
void USBBLUETOOTH_CALL usbbluetooth_free_device_list(usbbluetooth_device_t ***list)
{
    // 空のリストはクリーンアップできない...
    if (list == NULL || *list == NULL)
        return;

    // 各デバイスをイテレート...
    usbbluetooth_device_t *dev;
    for (int i = 0; (dev = (*list)[i]) != NULL; i++)
        usbbluetooth_unreference_device(&dev);

    // リストを解放
    free(*list);

    // ポインタをnullに設定...
    *list = NULL;
}

/**
 * Bluetoothデバイスへの参照を取得する。
 * 参照カウンタを増やし、デバイスの使用を宣言する。
 * 
 * @param dev 参照するデバイス
 * @return 参照されたデバイスのポインタ
 */
usbbluetooth_device_t * USBBLUETOOTH_CALL usbbluetooth_reference_device(usbbluetooth_device_t *dev)
{
    dev->ref_count++;
    return dev;
}

/**
 * デバイスの参照を解除し、メモリを解放する。
 * 参照カウンタが0になると、デバイスのメモリが解放される。
 * 
 * @param dev_ptr 参照を解除するデバイスのポインタのポインタ
 */
void USBBLUETOOTH_CALL usbbluetooth_unreference_device(usbbluetooth_device_t **dev_ptr)
{
    if (dev_ptr == NULL || *dev_ptr == NULL)
		return;

    usbbluetooth_device_t *dev = *dev_ptr;
    dev->ref_count--;
    
    if (dev->ref_count == 0) {
        libusb_unref_device(dev->device.usb);
        free(dev);
        *dev_ptr = NULL;
    }
}
