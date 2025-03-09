/* エラー処理関連のヘッダーファイルをインクルード */
#include <usbbluetooth_err.h>

/**
 * ステータスコードを文字列に変換する関数
 * 
 * @param status 変換するステータスコード
 * @return ステータスコードに対応する文字列
 */
const char * USBBLUETOOTH_CALL usbbluetooth_status_name(usbbluetooth_status_t status)
{
	switch (status)
	{
	case USBBLUETOOTH_STATUS_OK:
		return "USBBLUETOOTH_STATUS_OK"; /* 正常終了 */
	case USBBLUETOOTH_STATUS_ERR_UNK:
		return "USBBLUETOOTH_STATUS_ERR_UNK"; /* 不明なエラー */
	case USBBLUETOOTH_STATUS_ERR_NOMEM:
		return "USBBLUETOOTH_STATUS_ERR_NOMEM"; /* メモリ不足エラー */
	default:
		return "**UNKNOWN**"; /* 未定義のエラーコード */
	}
}
