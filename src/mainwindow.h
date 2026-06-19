#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QStringList>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Worker;

/**
 * @brief 主窗口类
 *
 * 管理 UI 交互，处理用户输入，启动工作线程，接收信号更新界面
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    /**
     * @brief 拖拽进入事件
     */
    void dragEnterEvent(QDragEnterEvent *event) override;

    /**
     * @brief 拖放事件
     */
    void dropEvent(QDropEvent *event) override;

private slots:
    /**
     * @brief 选择文件按钮点击事件
     */
    void onOpenFileClicked();

    /**
     * @brief 选择文件夹按钮点击事件
     */
    void onOpenFolderClicked();

    /**
     * @brief 清屏按钮点击事件
     */
    void onBtnClearClicked();

    /**
     * @brief 编码类型下拉框改变事件
     * @param index 选中的索引
     */
    void onCbEncodeIndexChanged(int index);

    /**
     * @brief 自定义编码检查按钮点击事件
     */
    void onCustomEncodeCheck();

    /**
     * @brief 文件类型复选框状态改变事件
     * @param state 复选框状态
     */
    void onFileTypeChanged(int state);

    /**
     * @brief 转换按钮点击事件
     */
    void onTransmitClicked();

    /**
     * @brief 进度更新槽函数
     * @param filename 文件名
     * @param sourceEncoding 源编码
     * @param decoding 解码方法
     * @param result 处理结果
     */
    void onProgressUpdate(const QString &filename, const QString &sourceEncoding,
                         const QString &decoding, const QString &result);

    /**
     * @brief 日志消息槽函数
     * @param message 日志消息
     */
    void onLogMessage(const QString &message);

    /**
     * @brief 工作线程完成槽函数
     */
    void onWorkerFinished();

    /**
     * @brief 接收文件总数
     * @param total 待处理文件总数
     */
    void onTotalCount(int total);

    /**
     * @brief 接收当前进度
     * @param current 已处理文件数
     */
    void onProgressChanged(int current);

    /**
     * @brief 筛选下拉框改变事件
     * @param index 选中的索引
     */
    void onFilterChanged(int index);

private:
    /**
     * @brief 初始化界面
     */
    void initForm();

    /**
     * @brief 连接信号槽
     */
    void connectSlots();

    /**
     * @brief 设置控件启用状态
     * @param enabled 是否启用
     */
    void setWidgetsEnabled(bool enabled);

    /**
     * @brief 获取当前选择的文件后缀列表
     * @return 文件后缀列表
     */
    QStringList getFileSuffixes() const;

    /**
     * @brief 获取目标编码名称
     * @return 目标编码名称
     */
    QString getTargetEncoding() const;

    Ui::MainWindow *ui;          ///< UI 界面
    QThread *workerThread;       ///< 工作线程
    Worker *worker;              ///< 工作对象
    QString currentPath;         ///< 当前选择的路径
    QStringList fileSuffixes;    ///< 内置文件后缀列表
    QStringList customFileSuffixes;  ///< 自定义文件后缀列表
    bool isFolderMode;           ///< 是否为文件夹模式
    int m_processedCount;        ///< 已处理文件数（进度条用）

    // 内置文件后缀
    static const QStringList BUILTIN_SUFFIXES;
};

#endif // MAINWINDOW_H
