#ifndef ENCODEDETECTOR_H
#define ENCODEDETECTOR_H

#include <QString>
#include <QByteArray>

// 前向声明 uchardet 类型
typedef struct uchardet * uchardet_t;

/**
 * @brief 编码检测器类
 *
 * 封装 uchardet 库，提供统一的编码检测接口
 * 用于自动检测文件的字符编码格式
 */
class EncoderDetector
{
public:
    EncoderDetector();
    ~EncoderDetector();

    /**
     * @brief 检测数据的编码格式
     * @param data 待检测的数据
     * @return 检测到的编码名称（如 "UTF-8", "GB2312" 等），检测失败返回空字符串
     */
    QString detect(const QByteArray &data);

    /**
     * @brief 检测数据是否为 UTF-8 编码
     * @param data 待检测的数据
     * @return 如果是 UTF-8 编码返回 true
     */
    bool isUtf8(const QByteArray &data);

    /**
     * @brief 检测数据是否包含 UTF-8 BOM
     * @param data 待检测的数据
     * @return 如果包含 BOM 返回 true
     */
    bool hasBom(const QByteArray &data);

    /**
     * @brief 检测数据是否包含 UTF-16 LE BOM
     * @param data 待检测的数据
     * @return 如果包含 UTF-16 LE BOM 返回 true
     */
    bool hasUtf16LeBom(const QByteArray &data);

    /**
     * @brief 检测数据是否包含 UTF-16 BE BOM
     * @param data 待检测的数据
     * @return 如果包含 UTF-16 BE BOM 返回 true
     */
    bool hasUtf16BeBom(const QByteArray &data);

    /**
     * @brief 标准化编码名称
     * @param encoding 原始编码名称
     * @return 标准化后的编码名称
     */
    static QString normalizeEncoding(const QString &encoding);

private:
    uchardet_t m_detector;  ///< uchardet 检测器实例

    /**
     * @brief 重置检测器状态
     */
    void reset();
};

#endif // ENCODEDETECTOR_H
