#ifndef _H_UTILS_
#define _H_UTILS_

/* 必要なヘッダーファイルのインクルード */
#include <libusb.h>
#include <stdbool.h>

/**
 * エンドポイントが指定された方向を持つかチェックする。
 * 
 * @param ep エンドポイント記述子
 * @param dir チェックする方向
 * @return 指定された方向と一致すればtrue、そうでなければfalse
 */
bool _is_ep_dir(const struct libusb_endpoint_descriptor* ep, enum libusb_endpoint_direction dir);

/**
 * デバイス記述子がBluetoothデバイスであるかチェックする。
 * 
 * @param desc デバイス記述子
 * @return Bluetoothデバイスであればtrue、そうでなければfalse
 */
bool _device_descriptor_is_bluetooth(struct libusb_device_descriptor *desc);

/**
 * インターフェースがBluetoothインターフェースであるかチェックする。
 * 
 * @param interface インターフェース記述子
 * @return Bluetoothインターフェースであればtrue、そうでなければfalse
 */
bool _interface_is_bluetooth(const struct libusb_interface_descriptor *interface);

/**
 * インターフェースがBluetoothの代替設定を持つかチェックする。
 * 
 * @param iface インターフェース
 * @return Bluetoothの代替設定を持つ場合はtrue、そうでなければfalse
 */
bool _interface_has_bluetooth_altsetting(const struct libusb_interface *iface);

/**
 * 設定がBluetoothインターフェースを持つかチェックする。
 * 
 * @param config 設定記述子
 * @return Bluetoothインターフェースを持つ場合はtrue、そうでなければfalse
 */
bool _configuration_has_bluetooth_interface(struct libusb_config_descriptor *config);

/**
 * デバイスがBluetoothデバイスであるかチェックする。
 * 
 * @param dev チェックするデバイス
 * @param is_bt Bluetoothデバイスであるかどうかの結果を格納するフラグ
 * @return 処理結果を示すステータスコード
 */
int _is_bluetooth_device(libusb_device *dev, bool *is_bt);

/**
 * デバイスのBluetoothインターフェース番号を探す。
 * 
 * @param dev デバイス
 * @param interface_number 発見したインターフェース番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
int _dev_find_bluetooth_interface(libusb_device *dev, uint8_t *interface_number);

/**
 * デバイスのイベントエンドポイント番号を探す。
 * 
 * @param dev デバイス
 * @param epnum 発見したエンドポイント番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
int _dev_find_evt_ep(libusb_device *dev, uint8_t *epnum);

/**
 * デバイスのACL入力エンドポイント番号を探す。
 * 
 * @param dev デバイス
 * @param epnum 発見したエンドポイント番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
int _dev_find_acl_in_ep(libusb_device *dev, uint8_t *epnum);

/**
 * デバイスのACL出力エンドポイント番号を探す。
 * 
 * @param dev デバイス
 * @param epnum 発見したエンドポイント番号を格納する変数
 * @return 処理結果を示すステータスコード
 */
int _dev_find_acl_out_ep(libusb_device *dev, uint8_t *epnum);

#endif
