/* エラーコード定義のヘッダーファイルをインクルード */
#include <usbbluetooth_err.h>

/* libusbとログ機能のヘッダーファイルをインクルード */
#include <libusb.h>
#include <usbbluetooth_log.h>

/**
 * USB Bluetoothライブラリの初期化を行う関数
 *
 * @return 初期化の結果を示すステータスコード
 */
usbbluetooth_status_t USBBLUETOOTH_CALL usbbluetooth_init()
{
    /* ログレベルを設定（デフォルトではログ出力なし） */
    usbbluetooth_log_set_level(USBBLUETOOTH_LOG_NONE);
    
    /* libusbの初期化 */
    int r = libusb_init(NULL);
    
    /* USBDKオプションを設定 */
    libusb_set_option(NULL, LIBUSB_OPTION_USE_USBDK);
    
    /* 初期化結果に基づいてステータスを返す */
    return (r == LIBUSB_SUCCESS) ? USBBLUETOOTH_STATUS_OK : USBBLUETOOTH_STATUS_ERR_UNK;
}

/**
 * USB Bluetoothライブラリの終了処理を行う関数
 */
void USBBLUETOOTH_CALL usbbluetooth_exit()
{
    /* libusbの終了処理 */
    libusb_exit(NULL);
}
