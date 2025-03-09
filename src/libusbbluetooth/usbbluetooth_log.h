#ifndef _H_USBBLUETOOTH_LOG_
#define _H_USBBLUETOOTH_LOG_

/* API定義のヘッダーファイルをインクルード */
#include <usbbluetooth_api.h>

/**
 * @brief ログ出力の重要度を示す列挙型
 * @details ログメッセージの重要度や種類を指定するための列挙型です。
 *          値が大きいほど詳細なログが出力されます。ログレベルは
 *          上位のレベルを設定すると、それ以下のレベルのログも出力されます。
 */
typedef enum
{
    USBBLUETOOTH_LOG_NONE = 0,  /* ログ出力なし（全てのログを無効化） */
    USBBLUETOOTH_LOG_ERROR = 1, /* エラーログ（致命的な問題やエラーの通知） */
    USBBLUETOOTH_LOG_WARN = 2,  /* 警告ログ（潜在的な問題や注意点の通知） */
    USBBLUETOOTH_LOG_INFO = 3,  /* 情報ログ（通常の動作状況や重要なイベントの通知） */
    USBBLUETOOTH_LOG_DEBUG = 4  /* デバッグログ（開発者向けの詳細な動作情報） */
} usbbluetooth_log_level_t;

/**
 * @brief ログ出力のレベルを設定
 * @details ログ出力のレベルを指定します。設定されたレベル以下の
 *          重要度のログが出力されます。例えば、INFOレベルを設定すると、
 *          INFO、WARN、ERRORのログが出力されます。
 * 
 * @param[in] level 設定するログレベル（NONEからDEBUGまで）
 */
void USBBLUETOOTH_CALL usbbluetooth_log_set_level(usbbluetooth_log_level_t level);

/**
 * @brief 指定されたレベルでログを出力
 * @details 指定されたレベルでログメッセージを出力します。メッセージは
 *          printf形式の書式指定文字列と可変引数を使用して指定します。
 *          ログにはレベル、タイムスタンプ、メッセージが含まれます。
 * 
 * @param[in] level ログレベル（ERRORからDEBUGまで）
 * @param[in] fmt printf形式の書式指定文字列
 * @param[in] ... 書式指定文字列に対応する可変引数
 */
void usbbluetooth_log(usbbluetooth_log_level_t level, const char *fmt, ...);

/* 各ログレベルに対応するマクロ定義（使いやすさのためのショートカット） */
#define usbbluetooth_log_error(...) usbbluetooth_log(USBBLUETOOTH_LOG_ERROR, __VA_ARGS__)
#define usbbluetooth_log_warn(...) usbbluetooth_log(USBBLUETOOTH_LOG_WARN, __VA_ARGS__)
#define usbbluetooth_log_info(...) usbbluetooth_log(USBBLUETOOTH_LOG_INFO, __VA_ARGS__)
#define usbbluetooth_log_debug(...) usbbluetooth_log(USBBLUETOOTH_LOG_DEBUG, __VA_ARGS__)

#endif
