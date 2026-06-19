#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>

/**
 * @brief 工作线程类
 *
 * 在后台线程中执行文件遍历和编码转换任务
 * 通过信号槽向主线程发送进度信息
 */
class Worker : public QObject
{
    Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

    /**
     * @brief 设置处理参数
     * @param path 文件或文件夹路径
     * @param suffixes 需要处理的文件后缀列表
     * @param targetEncoding 目标编码格式
     * @param isFolderMode 是否为文件夹模式
     */
    void setParameters(const QString &path, const QStringList &suffixes,
                      const QString &targetEncoding, bool isFolderMode);

public slots:
    /**
     * @brief 开始处理任务
     *
     * 在工作线程中执行，遍历文件并进行编码转换
     */
    void process();

signals:
    /**
     * @brief 进度更新信号
     * @param filename 文件名
     * @param sourceEncoding 源编码
     * @param decoding 解码方法
     * @param result 处理结果
     */
    void progressUpdate(const QString &filename, const QString &sourceEncoding,
                       const QString &decoding, const QString &result);

    /**
     * @brief 日志消息信号
     * @param message 日志消息
     */
    void logMessage(const QString &message);

    /**
     * @brief 处理完成信号
     */
    void finished();

    /**
     * @brief 文件总数信号（进度条用）
     * @param total 待处理文件总数
     */
    void totalCount(int total);

    /**
     * @brief 当前已处理数量信号
     * @param current 已处理文件数
     */
    void progressChanged(int current);

private:
    /**
     * @brief 转换单个文件的编码
     * @param filePath 文件路径
     */
    void convertFile(const QString &filePath);

    /**
     * @brief 递归遍历文件夹
     * @param dirPath 文件夹路径
     */
    void exploreFolder(const QString &dirPath);

    /**
     * @brief 检查文件后缀是否在处理列表中
     * @param suffix 文件后缀
     * @return 如果需要处理返回 true
     */
    bool shouldProcessSuffix(const QString &suffix) const;

    /**
     * @brief 获取目标编码的 Qt 编码名称
     * @return Qt 编码名称
     */
    QString getTargetCodecName() const;

    /**
     * @brief 添加 BOM 到数据
     * @param data 原始数据
     * @param encoding 编码类型
     * @return 添加 BOM 后的数据
     */
    QByteArray addBom(const QByteArray &data, const QString &encoding) const;

    QString m_path;              ///< 处理路径
    QStringList m_suffixes;      ///< 文件后缀列表
    QString m_targetEncoding;    ///< 目标编码
    bool m_isFolderMode;         ///< 是否为文件夹模式
};

#endif // WORKER_H
