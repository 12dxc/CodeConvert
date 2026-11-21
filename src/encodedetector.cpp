#include "encodedetector.h"

// 包含 uchardet 头文件
extern "C" {
#include <uchardet/uchardet.h>
}

EncoderDetector::EncoderDetector()
    : m_detector(nullptr)
{
    m_detector = uchardet_new();
}

EncoderDetector::~EncoderDetector()
{
    if (m_detector) {
        uchardet_delete(m_detector);
        m_detector = nullptr;
    }
}

QString EncoderDetector::detect(const QByteArray &data)
{
    if (data.isEmpty() || !m_detector) {
        return QString();
    }

    // 首先检查 BOM（返回带 BOM 区分的编码名，供 BOM 增删转换使用）
    if (hasBom(data)) {
        return "UTF-8 BOM";
    }

    if (hasUtf16LeBom(data)) {
        return "UTF-16LE";
    }

    if (hasUtf16BeBom(data)) {
        return "UTF-16BE";
    }

    // 重置检测器
    reset();

    // 使用 uchardet 检测
    int ret = uchardet_handle_data(m_detector, data.constData(), data.size());
    if (ret != 0) {
        return QString();
    }

    uchardet_data_end(m_detector);

    const char *charset = uchardet_get_charset(m_detector);
    if (!charset) {
        return QString();
    }

    return normalizeEncoding(QString::fromLatin1(charset));
}

bool EncoderDetector::hasBom(const QByteArray &data)
{
    // UTF-8 BOM: EF BB BF
    if (data.size() >= 3 &&
        static_cast<quint8>(data[0]) == 0xEF &&
        static_cast<quint8>(data[1]) == 0xBB &&
        static_cast<quint8>(data[2]) == 0xBF) {
        return true;
    }
    return false;
}

bool EncoderDetector::hasUtf16LeBom(const QByteArray &data)
{
    // UTF-16 LE BOM: FF FE
    if (data.size() >= 2 &&
        static_cast<quint8>(data[0]) == 0xFF &&
        static_cast<quint8>(data[1]) == 0xFE) {
        return true;
    }
    return false;
}

bool EncoderDetector::hasUtf16BeBom(const QByteArray &data)
{
    // UTF-16 BE BOM: FE FF
    if (data.size() >= 2 &&
        static_cast<quint8>(data[0]) == 0xFE &&
        static_cast<quint8>(data[1]) == 0xFF) {
        return true;
    }
    return false;
}

QString EncoderDetector::normalizeEncoding(const QString &encoding)
{
    QString enc = encoding.toUpper().trimmed();

    // 统一编码名称
    if (enc == "ASCII" || enc == "US-ASCII") {
        return "UTF-8";  // ASCII 是 UTF-8 的子集
    }

    if (enc == "GB2312" || enc == "GBK" || enc == "GB18030") {
        return "GB18030";  // 保留超集：用 GB18030 编解码器解码 GB2312/GBK 内容最安全
    }

    if (enc == "UTF-8" || enc == "UTF8") {
        return "UTF-8";
    }

    if (enc == "UTF-16LE" || enc == "UTF-16 LE" || enc == "UTF16LE") {
        return "UTF-16LE";
    }

    if (enc == "UTF-16BE" || enc == "UTF-16 BE" || enc == "UTF16BE") {
        return "UTF-16BE";
    }

    if (enc == "BIG5" || enc == "BIG-5") {
        return "BIG5";
    }

    if (enc == "SHIFT_JIS" || enc == "SHIFT-JIS" || enc == "SJIS") {
        return "SHIFT_JIS";
    }

    if (enc == "EUC-JP" || enc == "EUCJP") {
        return "EUC-JP";
    }

    if (enc == "EUC-KR" || enc == "EUCKR") {
        return "EUC-KR";
    }

    if (enc == "ISO-8859-1" || enc == "ISO8859-1" || enc == "LATIN1") {
        return "ISO-8859-1";
    }

    if (enc == "WINDOWS-1252" || enc == "CP1252") {
        return "WINDOWS-1252";
    }

    // 返回原始编码名称
    return encoding;
}

void EncoderDetector::reset()
{
    if (m_detector) {
        uchardet_reset(m_detector);
    }
}
