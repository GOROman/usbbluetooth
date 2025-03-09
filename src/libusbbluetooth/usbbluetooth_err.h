#ifndef _H_USBBLUETOOTH_ERR_
#define _H_USBBLUETOOTH_ERR_

/* API定義のヘッダーファイルをインクルード */
#include <usbbluetooth_api.h>

/**
 * @brief エラーコードの列挙型
 * @details ライブラリ内の各関数の実行結果を示すステータスコードです。
 *          全ての関数は、処理の成功・失敗を示すためにこの型の値を返します。
 *          正の値は成功を、負の値は各種エラーを示します。
 */
typedef enum
{
    USBBLUETOOTH_STATUS_OK = 0,               /* 正常終了（処理が正常に完了） */
    USBBLUETOOTH_STATUS_ERR_UNK = -1,         /* 不明なエラー（原因特定できない一般的なエラー） */
    USBBLUETOOTH_STATUS_ERR_NOMEM = -2,       /* メモリ不足エラー（メモリの確保に失敗） */
    USBBLUETOOTH_STATUS_ERR_DEVICE_CLOSED = -3, /* デバイスクローズエラー（既に閉じられたデバイスにアクセス） */
} usbbluetooth_status_t;

/**
 * @brief エラー番号から説明用文字列を取得
 * @details 指定されたステータスコードに対応する説明文字列を返します。
 *          この関数は主にデバッグやエラーログの出力時に使用され、
 *          人間が読みやすい形式でエラーの内容を表示します。
 * 
 * @param[in] status 説明文字列を取得したいステータスコード
 * @return ステータスコードに対応する説明文字列（NULLで終わる文字列）
 */
const char * USBBLUETOOTH_CALL usbbluetooth_status_name(usbbluetooth_status_t status);

#endif