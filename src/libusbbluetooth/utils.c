/* ユーティリティ関数のヘッダーファイルをインクルード */
#include <utils.h>

/**
 * エンドポイントが指定された方向を持つかチェックする。
 * 
 * @param ep エンドポイント記述子
 * @param dir チェックする方向
 * @return 指定された方向と一致すればtrue、そうでなければfalse
 */
bool _is_ep_dir(const struct libusb_endpoint_descriptor *ep, enum libusb_endpoint_direction dir)
{
    return (ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == dir;
}

/**
 * デバイス記述子がBluetoothデバイスであるかチェックする。
 * 
 * @param desc デバイス記述子
 * @return Bluetoothデバイスであればtrue、そうでなければfalse
 */
bool _device_descriptor_is_bluetooth(struct libusb_device_descriptor *desc)
{
    return (desc->bDeviceClass == LIBUSB_CLASS_WIRELESS) &&
           (desc->bDeviceSubClass == 0x01 /* SUBCLASS_RF_CONTROLLER */) &&
           (desc->bDeviceProtocol == 0x01 /* PROTOCOL_BLUETOOTH_PRIMARY_CONTROLLER */);
}

/**
 * インターフェースがBluetoothインターフェースであるかチェックする。
 * 
 * @param interface インターフェース記述子
 * @return Bluetoothインターフェースであればtrue、そうでなければfalse
 */
bool _interface_is_bluetooth(const struct libusb_interface_descriptor *interface)
{
    return (interface->bInterfaceClass == LIBUSB_CLASS_WIRELESS) &&
           (interface->bInterfaceSubClass == 0x01 /* SUBCLASS_RF_CONTROLLER */) &&
           (interface->bInterfaceProtocol == 0x01 /* PROTOCOL_BLUETOOTH_PRIMARY_CONTROLLER */);
}

/**
 * インターフェースがBluetoothの代替設定を持つかチェックする。
 * 
 * @param iface インターフェース
 * @return Bluetoothの代替設定を持つ場合はtrue、そうでなければfalse
 */
bool _interface_has_bluetooth_altsetting(const struct libusb_interface *iface)
{
    // すべての代替設定をチェック...
    for (int altsetting_idx = 0; altsetting_idx < iface->num_altsetting; altsetting_idx++)
    {
        const struct libusb_interface_descriptor *altsetting = &iface->altsetting[altsetting_idx];
        if (_interface_is_bluetooth(altsetting))
            return true;
    }
    return false;
}

/**
 * 設定がBluetoothインターフェースを持つかチェックする。
 * 
 * @param config 設定記述子
 * @return Bluetoothインターフェースを持つ場合はtrue、そうでなければfalse
 */
bool _configuration_has_bluetooth_interface(struct libusb_config_descriptor *config)
{
    for (int iface_idx = 0; iface_idx < config->bNumInterfaces; iface_idx++)
    {
        const struct libusb_interface *iface = &config->interface[iface_idx];
        if (_interface_has_bluetooth_altsetting(iface))
            return true;
    }
    return false;
}

/**
 * デバイスがBluetoothデバイスであるかチェックする。
 * 
 * @param dev チェックするデバイス
 * @param is_bt Bluetoothデバイスであるかどうかの結果を格納するフラグ
 * @return 処理結果を示すステータスコード
 */
int _is_bluetooth_device(libusb_device *dev, bool *is_bt)
{
    *is_bt = false;

    // デバイス記述子をチェック...
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < LIBUSB_SUCCESS)
        return r;
    if (_device_descriptor_is_bluetooth(&desc))
    {
        *is_bt = true;
        return LIBUSB_SUCCESS;
    }

    // 各設定について、すべてのインターフェースをチェック...
    for (int config_idx = 0; config_idx < desc.bNumConfigurations; config_idx++)
    {
        struct libusb_config_descriptor *config;
        r = libusb_get_config_descriptor(dev, config_idx, &config);
        if (r < LIBUSB_SUCCESS)
            return r;
        if (_configuration_has_bluetooth_interface(config))
        {
            *is_bt = true;
            return LIBUSB_SUCCESS;
        }
    }
    // Bluetoothデバイスではない...
    return LIBUSB_SUCCESS;
}

/**
 * デバイスのBluetoothインターフェース番号を探す。
 * 
 * @param dev デバイス
 * @param interface_number 発見したインターフェース番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
int _dev_find_bluetooth_interface(libusb_device *dev, uint8_t *interface_number)
{
    // デバイス記述子をチェック...
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < LIBUSB_SUCCESS)
        return r;

    // 各設定について、すべてのインターフェースをチェック...
    for (int config_idx = 0; config_idx < desc.bNumConfigurations; config_idx++)
    {
        struct libusb_config_descriptor *config;
        r = libusb_get_config_descriptor(dev, config_idx, &config);
        if (r < LIBUSB_SUCCESS)
            return r;

        for (int iface_idx = 0; iface_idx < config->bNumInterfaces; iface_idx++)
        {
            const struct libusb_interface *iface = &config->interface[iface_idx];
            // すべての代替設定をチェック...
            for (int altsetting_idx = 0; altsetting_idx < iface->num_altsetting; altsetting_idx++)
            {
                const struct libusb_interface_descriptor *altsetting = &iface->altsetting[altsetting_idx];
                if (_interface_is_bluetooth(altsetting))
                {
                    *interface_number = altsetting->bInterfaceNumber;
                    return LIBUSB_SUCCESS;
                }
            }
        }
    }

    return LIBUSB_ERROR_NOT_FOUND;
}

/**
 * 指定された方向と転送タイプを持つエンドポイントを探す。
 * 
 * @param dev デバイス
 * @param epnum 発見したエンドポイント番号を格納する変数
 * @param dir 探すエンドポイントの方向
 * @param transfer_type 探すエンドポイントの転送タイプ
 * @return 処理結果を示すステータスコード
 */
int _dev_find_ep(libusb_device *dev, uint8_t *epnum, enum libusb_endpoint_direction dir, enum libusb_endpoint_transfer_type transfer_type)
{
    // 現在アクティブな設定を取得...
    struct libusb_config_descriptor *config;
    int err = libusb_get_active_config_descriptor(dev, &config);
    if (err < LIBUSB_SUCCESS)
        return err;

    // Bluetoothインターフェースを探す...
    for (int iface_idx = 0; iface_idx < config->bNumInterfaces; iface_idx++)
    {
        const struct libusb_interface *iface = &config->interface[iface_idx];
        for (int altsetting_idx = 0; altsetting_idx < iface->num_altsetting; altsetting_idx++)
        {
            const struct libusb_interface_descriptor *altsetting = &iface->altsetting[altsetting_idx];
            if (_interface_is_bluetooth(altsetting))
            {
                for (int ep_idx = 0; ep_idx < altsetting->bNumEndpoints; ep_idx++)
                {
                    const struct libusb_endpoint_descriptor *ep_desc = &altsetting->endpoint[ep_idx];
                    if (_is_ep_dir(ep_desc, dir) && ep_desc->bmAttributes == transfer_type)
                    {
                        *epnum = ep_desc->bEndpointAddress;
                        return LIBUSB_SUCCESS;
                    }
                }
            }
        }
    }

    *epnum = 0;
    return LIBUSB_ERROR_NOT_FOUND;
}

/**
 * デバイスのイベントエンドポイント番号を探す。
 * イベントエンドポイントは入力方向の割り込み転送タイプである。
 * 
 * @param dev デバイス
 * @param epnum 発見したエンドポイント番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
/**
 * @brief デバイスのHCIイベントエンドポイント番号を探す
 * @details HCIイベントエンドポイントは入力方向の割り込み転送タイプのエンドポイントです。
 *          このエンドポイントを通じて、Bluetoothコントローラからのイベント通知を受信します。
 *          例えば、接続状態の変更、デバイス検出、認証結果などのイベントがこのエンドポイントを通じて通知されます。
 * 
 * @param dev デバイス
 * @param epnum 発見したエンドポイント番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
int _dev_find_evt_ep(libusb_device *dev, uint8_t *epnum)
{
    return _dev_find_ep(dev, epnum, LIBUSB_ENDPOINT_IN, LIBUSB_ENDPOINT_TRANSFER_TYPE_INTERRUPT);
}

/**
 * デバイスのACL入力エンドポイント番号を探す。
 * ACL入力エンドポイントは入力方向のバルク転送タイプである。
 * 
 * @param dev デバイス
 * @param epnum 発見したエンドポイント番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
/**
 * @brief デバイスのACL入力エンドポイント番号を探す
 * @details ACL（非同期接続レス）入力エンドポイントは、入力方向のバルク転送タイプのエンドポイントです。
 *          このエンドポイントを通じて、接続されたBluetoothデバイスからのデータを受信します。
 *          主にL2CAP（論理リンク制御とアダプテーションプロトコル）パケットの受信に使用され、
 *          オーディオストリーミング、ファイル転送などの大容量データ転送に適しています。
 * 
 * @param dev デバイス
 * @param epnum 発見したエンドポイント番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
int _dev_find_acl_in_ep(libusb_device *dev, uint8_t *epnum)
{
    return _dev_find_ep(dev, epnum, LIBUSB_ENDPOINT_IN, LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK);
}

/**
 * デバイスのACL出力エンドポイント番号を探す。
 * ACL出力エンドポイントは出力方向のバルク転送タイプである。
 * 
 * @param dev デバイス
 * @param epnum 発見したエンドポイント番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
/**
 * @brief デバイスのACL出力エンドポイント番号を探す
 * @details ACL（非同期接続レス）出力エンドポイントは、出力方向のバルク転送タイプのエンドポイントです。
 *          このエンドポイントを通じて、接続されたBluetoothデバイスへデータを送信します。
 *          主にL2CAP（論理リンク制御とアダプテーションプロトコル）パケットの送信に使用され、
 *          オーディオストリーミング、ファイル転送などの大容量データ転送に適しています。
 * 
 * @param dev デバイス
 * @param epnum 発見したエンドポイント番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
int _dev_find_acl_out_ep(libusb_device *dev, uint8_t *epnum)
{
    return _dev_find_ep(dev, epnum, LIBUSB_ENDPOINT_OUT, LIBUSB_ENDPOINT_TRANSFER_TYPE_BULK);
}
