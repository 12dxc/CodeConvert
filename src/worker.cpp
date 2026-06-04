#include "worker.h"
#include "encodedetector.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>

// Qt6 中 QTextCodec 被移到 qt5compat 模块
#include <QTextCodec>

Worker::Worker(QObject *parent)
    : QObject(parent)
    , m_isFolderMode(false)
{
}

Worker::~Worker()
{
}

void Worker::setParameters(const QString &path, const QStringList &suffixes,
                          const QString &targetEncoding, bool isFolderMode)
{
    m_path = path;
    m_suffixes = suffixes;
    m_targetEncoding = targetEncoding;
    m_isFolderMode = isFolderMode;
}

void Worker::process()
{
    emit logMessage("开始处理...");

    if (m_isFolderMode) {
        exploreFolder(m_path);
    } else {
        convertFile(m_path);
    }

    emit logMessage("处理完成!");
    emit finished();
}

void Worker::convertFile(const QString &filePath)
{
    emit logMessage(QString("正在处理: %1").arg(filePath));

    // 读取文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit logMessage(QString("无法打开文件: %1").arg(filePath));
        emit progressUpdate(QFileInfo(filePath).fileName(), "", "", "打开失败");
        return;
    }

    QByteArray content = file.readAll();
    file.close();

    if (content.isEmpty()) {
        emit logMessage(QString("文件为空: %1").arg(filePath));
        emit progressUpdate(QFileInfo(filePath).fileName(), "空文件", "", "跳过");
        return;
    }

    // 检测源编码
    EncoderDetector detector;
    QString sourceEncoding = detector.detect(content);

    if (sourceEncoding.isEmpty()) {
        emit logMessage(QString("无法识别编码: %1").arg(filePath));
        emit progressUpdate(QFileInfo(filePath).fileName(), "无法识别", "", "识别失败");
        return;
    }

    // 检查是否需要转换
    QString targetCodecName = getTargetCodecName();
    if (sourceEncoding.toUpper() == targetCodecName.toUpper() ||
        (sourceEncoding.toUpper() == "UTF-8" && m_targetEncoding.toUpper() == "UTF-8")) {
        emit logMessage(QString("此文件格式无需转换: %1").arg(filePath));
        return;
    }

    // 获取编解码器
    QTextCodec *sourceCodec = QTextCodec::codecForName(sourceEncoding.toLatin1());
    QTextCodec *targetCodec = QTextCodec::codecForName(targetCodecName.toLatin1());

    if (!sourceCodec) {
        emit logMessage(QString("不支持的源编码: %1 (%2)").arg(sourceEncoding, filePath));
        emit progressUpdate(QFileInfo(filePath).fileName(), sourceEncoding, "", "转换失败");
        return;
    }

    if (!targetCodec) {
        emit logMessage(QString("不支持的目标编码: %1").arg(targetCodecName));
        emit progressUpdate(QFileInfo(filePath).fileName(), sourceEncoding, targetCodecName, "转换失败");
        return;
    }

    // 解码源文件
    QTextDecoder decoder(sourceCodec);
    QString unicodeText = decoder.toUnicode(content);

    // 检查解码是否成功
    if (decoder.hasFailure()) {
        emit logMessage(QString("解码失败: %1 (编码: %2)").arg(filePath, sourceEncoding));
        emit progressUpdate(QFileInfo(filePath).fileName(), sourceEncoding, targetCodecName, "解码失败");
        return;
    }

    // 编码为目标格式
    QTextEncoder encoder(targetCodec);
    QByteArray targetContent = encoder.fromUnicode(unicodeText);

    // 添加 BOM (如果是 UTF-8 BOM)
    targetContent = addBom(targetContent, m_targetEncoding);

    // 写入文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        emit logMessage(QString("无法写入文件: %1").arg(filePath));
        emit progressUpdate(QFileInfo(filePath).fileName(), sourceEncoding, targetCodecName, "写入失败");
        return;
    }

    file.write(targetContent);
    file.close();

    emit logMessage(QString("转换成功: %1 (%2 -> %3)").arg(filePath, sourceEncoding, targetCodecName));
    emit progressUpdate(QFileInfo(filePath).fileName(), sourceEncoding, targetCodecName, "成功");
}

void Worker::exploreFolder(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) {
        emit logMessage(QString("文件夹不存在: %1").arg(dirPath));
        return;
    }

    // 设置过滤器
    QDirIterator::IteratorFlags flags = QDirIterator::Subdirectories;
    QDir::Filters filters = QDir::Files | QDir::NoDotAndDotDot;

    // 遍历文件夹
    QDirIterator it(dirPath, filters, flags);
    while (it.hasNext()) {
        it.next();

        QString filePath = it.filePath();
        QString suffix = it.fileInfo().suffix().toLower();

        // 检查文件后缀
        if (shouldProcessSuffix(suffix)) {
            convertFile(filePath);
        }
    }
}

bool Worker::shouldProcessSuffix(const QString &suffix) const
{
    if (suffix.isEmpty()) {
        return false;
    }

    QString suffixWithDot = "." + suffix.toLower();

    for (const QString &s : m_suffixes) {
        if (s.toLower() == suffixWithDot) {
            return true;
        }
    }

    return false;
}

QString Worker::getTargetCodecName() const
{
    // 将目标编码转换为 Qt 支持的编码名称
    QString enc = m_targetEncoding.toUpper();

    if (enc == "UTF-8 BOM" || enc == "UTF-8-SIG" || enc == "UTF8BOM") {
        return "UTF-8";
    }

    if (enc == "UTF-8" || enc == "UTF8") {
        return "UTF-8";
    }

    if (enc == "GB2312" || enc == "GBK") {
        return "GB2312";
    }

    // 返回原始编码名称
    return m_targetEncoding;
}

QByteArray Worker::addBom(const QByteArray &data, const QString &encoding) const
{
    QString enc = encoding.toUpper();

    // UTF-8 BOM: EF BB BF
    if (enc == "UTF-8 BOM" || enc == "UTF-8-SIG" || enc == "UTF8BOM") {
        QByteArray bomData;
        bomData.append("\xEF\xBB\xBF");
        bomData.append(data);
        return bomData;
    }

    // UTF-16 LE BOM: FF FE
    if (enc == "UTF-16LE" || enc == "UTF-16 LE") {
        QByteArray bomData;
        bomData.append("\xFF\xFE");
        bomData.append(data);
        return bomData;
    }

    // UTF-16 BE BOM: FE FF
    if (enc == "UTF-16BE" || enc == "UTF-16 BE") {
        QByteArray bomData;
        bomData.append("\xFE\xFF");
        bomData.append(data);
        return bomData;
    }

    return data;
}
