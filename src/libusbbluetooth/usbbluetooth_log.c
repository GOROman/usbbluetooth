/* ログ機能のヘッダーファイルをインクルード */
#include <usbbluetooth_log.h>

/* 必要なヘッダーファイルをインクルード */
#include <libusb.h>
#include <stdarg.h>
#include <stdio.h>

/* グローバルなログレベル設定（デフォルトはログ出力なし） */
static usbbluetooth_log_level_t _log_level = USBBLUETOOTH_LOG_NONE;

/**
 * ログレベルから表示用の文字タグを取得する。
 * 
 * @param l ログレベル
 * @return ログレベルに対応する文字タグ
 */
static char _log_tag(usbbluetooth_log_level_t l)
{
    switch (l)
    {
    case USBBLUETOOTH_LOG_NONE:
        return ' ';  /* ログなしの場合は空白 */
    case USBBLUETOOTH_LOG_ERROR:
        return 'E';  /* エラーの場合はE */
    case USBBLUETOOTH_LOG_WARN:
        return 'W';  /* 警告の場合はW */
    case USBBLUETOOTH_LOG_INFO:
        return 'I';  /* 情報の場合はI */
    case USBBLUETOOTH_LOG_DEBUG:
        return 'D';  /* デバッグの場合はD */
    }
    return ' ';  /* デフォルトは空白 */
}

/**
 * USB Bluetoothのログレベルをlibusbのログレベルに変換する。
 * 
 * @param l USB Bluetoothのログレベル
 * @return libusbのログレベル
 */
static enum libusb_log_level _log_level_to_libusb(usbbluetooth_log_level_t l)
{
    return l;  /* 現在は同じ値を使用 */
}

/**
 * ログメッセージをフォーマットして出力する。
 * 
 * @param l ログレベル
 * @param fmt 書式指定文字列
 * @param args 可変引数リスト
 */
static void _log_format(usbbluetooth_log_level_t l, const char *fmt, va_list args)
{
    /* ログレベルのタグを表示 */
    printf("[%c]", _log_tag(l));
    
    /* メッセージ本体を書式指定に従って出力 */
    vprintf(fmt, args);
    
    /* 改行を出力 */
    printf("\n");
}

/**
 * ログ出力レベルを設定する。
 * 
 * @param level 設定するログレベル
 */
void USBBLUETOOTH_CALL usbbluetooth_log_set_level(usbbluetooth_log_level_t level)
{
    /* ローカルのログレベルを設定 */
    _log_level = level;
    
    /* libusbのログレベルも設定 */
    libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, _log_level_to_libusb(level));
    
    /* デバッグログを出力 */
    usbbluetooth_log_debug("usbbluetooth_log_set_level[l=%d]", level);
}

/**
 * 指定されたレベルでログを出力する。
 * 
 * @param level ログレベル
 * @param fmt 書式指定文字列
 * @param ... 可変引数
 */
void usbbluetooth_log(usbbluetooth_log_level_t level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    /* 現在のログレベルが指定されたレベル以上の場合のみ出力 */
    if (level <= _log_level)
        _log_format(level, fmt, args);
    
    va_end(args);
}
