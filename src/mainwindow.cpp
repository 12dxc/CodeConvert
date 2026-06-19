#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QUrl>
#include <QFileInfo>

// 内置文件后缀
const QStringList MainWindow::BUILTIN_SUFFIXES = {".c", ".h", ".cpp", ".hpp", ".txt", ".bat", ".py", ".java"};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , workerThread(nullptr)
    , worker(nullptr)
    , isFolderMode(false)
    , m_processedCount(0)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    initForm();
    connectSlots();

    // 初始化文件后缀列表
    fileSuffixes = {".c", ".h", ".cpp", ".hpp"};  // 默认选中的后缀
}

MainWindow::~MainWindow()
{
    // 等待工作线程结束
    if (workerThread && workerThread->isRunning()) {
        workerThread->quit();
        workerThread->wait();
    }

    delete ui;
}

void MainWindow::initForm()
{
    // 设置表格列
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"文件名", "识别类型", "解码方法", "结果"});

    // 设置表格列宽
    ui->tableWidget->setColumnWidth(0, 165);
    ui->tableWidget->setColumnWidth(1, 70);
    ui->tableWidget->setColumnWidth(2, 70);
    ui->tableWidget->setColumnWidth(3, 70);

    // 表格排序
    ui->tableWidget->setSortingEnabled(true);

    // 进度条
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    ui->progressBar->setFormat("%v / %m");

    // 筛选下拉框
    ui->comboBoxFilter->addItem("全部");
    ui->comboBoxFilter->addItem("仅成功");
    ui->comboBoxFilter->addItem("仅失败");

    // QSplitter：上下分割表格区域和日志区域
    ui->splitter->setOrientation(Qt::Vertical);
    ui->splitter->setStretchFactor(0, 3);  // 上部表格区域占更多空间
    ui->splitter->setStretchFactor(1, 1);  // 下部日志区域占较少空间

    // 设置默认选中的文件类型
    ui->cbCFile->setChecked(true);
    ui->cbHFile->setChecked(true);
    ui->cbCppFile->setChecked(true);
    ui->cbHppFile->setChecked(true);

    // 设置默认编码类型
    ui->comboBox->setCurrentIndex(0);  // UTF-8 BOM

    // 设置默认标签页
    ui->tabWidget->setCurrentIndex(1);  // 文件夹标签页
}

void MainWindow::connectSlots()
{
    // 连接按钮信号槽
    connect(ui->btnChooseFile, &QPushButton::clicked, this, &MainWindow::onOpenFileClicked);
    connect(ui->btnChooseFolder, &QPushButton::clicked, this, &MainWindow::onOpenFolderClicked);
    connect(ui->btnClear, &QPushButton::clicked, this, &MainWindow::onBtnClearClicked);
    connect(ui->btnCustomCheck, &QPushButton::clicked, this, &MainWindow::onCustomEncodeCheck);
    connect(ui->btnTransmit, &QPushButton::clicked, this, &MainWindow::onTransmitClicked);

    // 连接下拉框信号槽
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onCbEncodeIndexChanged);

    // 连接文件类型复选框信号槽
    QList<QCheckBox*> checkBoxes = ui->groupBox->findChildren<QCheckBox*>();
    for (QCheckBox *checkBox : checkBoxes) {
        connect(checkBox, &QCheckBox::stateChanged, this, &MainWindow::onFileTypeChanged);
    }

    // 连接筛选下拉框
    connect(ui->comboBoxFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onFilterChanged);
}

void MainWindow::onOpenFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择文件", ".");
    if (fileName.isEmpty()) {
        return;
    }

    currentPath = fileName;
    isFolderMode = false;
    ui->labelPath->setText(fileName);
}

void MainWindow::onOpenFolderClicked()
{
    QString folderName = QFileDialog::getExistingDirectory(this, "选择文件夹", ".");
    if (folderName.isEmpty()) {
        return;
    }

    currentPath = folderName;
    isFolderMode = true;
    ui->labelPath->setText(folderName);
}

void MainWindow::onBtnClearClicked()
{
    ui->textBrowser->clear();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->progressBar->setValue(0);
}

void MainWindow::onCbEncodeIndexChanged(int index)
{
    // 编码类型改变时的处理
    QStringList encodeTypes = {"UTF-8 BOM", "UTF-8", "GB2312"};
    if (index >= 0 && index < encodeTypes.size()) {
        onLogMessage(QString("设置编码类型: %1").arg(encodeTypes[index]));
    }
}

void MainWindow::onCustomEncodeCheck()
{
    QString customStr = ui->leditCustomEncode->text().trimmed();
    if (customStr.isEmpty()) {
        return;
    }

    QStringList customArr = customStr.split(' ', Qt::SkipEmptyParts);

    for (const QString &item : customArr) {
        // 验证自定义后缀格式
        if (item.length() < 2) {
            QMessageBox::critical(this, "错误", "自定义后缀无效:长度至少为2!");
            return;
        }

        if (!item.startsWith('.')) {
            QMessageBox::critical(this, "错误", "自定义后缀无效:必须以 '.' 打头!");
            return;
        }

        if (item.count('.') > 1) {
            QMessageBox::critical(this, "错误", "自定义后缀无效:一种格式中不能出现多个 '.'!");
            return;
        }

        // 移除后缀重复的元素
        if (!fileSuffixes.contains(item) && !customFileSuffixes.contains(item)) {
            customFileSuffixes.append(item);
            onLogMessage(QString("添加自定义后缀: %1").arg(item));
        }
    }

    ui->leditCustomEncode->clear();
}

void MainWindow::onFileTypeChanged(int state)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox*>(sender());
    if (!checkBox) {
        return;
    }

    QString itemText = checkBox->text();

    if (state == Qt::Unchecked) {
        if (fileSuffixes.contains(itemText)) {
            fileSuffixes.removeOne(itemText);
        }
    } else {
        if (!fileSuffixes.contains(itemText)) {
            fileSuffixes.append(itemText);
        }
        if (customFileSuffixes.contains(itemText)) {
            customFileSuffixes.removeOne(itemText);
        }
    }
}

void MainWindow::onTransmitClicked()
{
    if (currentPath.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择'文件'或'文件夹'路径!");
        return;
    }

    // 清空结果表格
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    // 获取文件后缀列表
    QStringList suffixes = getFileSuffixes();

    // 检查文件夹模式下是否有后缀
    if (isFolderMode && suffixes.isEmpty()) {
        QMessageBox::critical(this, "错误", "请设置需要处理的文件后缀格式!");
        return;
    }

    // 禁用控件
    setWidgetsEnabled(false);

    // 重置进度条
    ui->progressBar->setValue(0);
    m_processedCount = 0;

    // 创建工作线程
    workerThread = new QThread(this);
    worker = new Worker();
    worker->setParameters(currentPath, suffixes, getTargetEncoding(), isFolderMode);
    worker->moveToThread(workerThread);

    // 连接信号槽
    connect(workerThread, &QThread::started, worker, &Worker::process);
    connect(worker, &Worker::finished, this, &MainWindow::onWorkerFinished);
    connect(worker, &Worker::progressUpdate, this, &MainWindow::onProgressUpdate);
    connect(worker, &Worker::logMessage, this, &MainWindow::onLogMessage);
    connect(worker, &Worker::totalCount, this, &MainWindow::onTotalCount);
    connect(worker, &Worker::progressChanged, this, &MainWindow::onProgressChanged);
    connect(worker, &Worker::finished, workerThread, &QThread::quit);
    connect(workerThread, &QThread::finished, worker, &Worker::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);

    // 启动线程
    workerThread->start();
}

void MainWindow::onProgressUpdate(const QString &filename, const QString &sourceEncoding,
                                 const QString &decoding, const QString &result)
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(filename));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(sourceEncoding));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(decoding));
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(result));

    // 设置居中对齐
    for (int col = 0; col < 4; ++col) {
        if (ui->tableWidget->item(row, col)) {
            ui->tableWidget->item(row, col)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void MainWindow::onLogMessage(const QString &message)
{
    ui->textBrowser->append(message);
}

void MainWindow::onWorkerFinished()
{
    setWidgetsEnabled(true);
    ui->progressBar->setValue(ui->progressBar->maximum());
    onLogMessage("处理完成!");
}

void MainWindow::setWidgetsEnabled(bool enabled)
{
    ui->groupBoxPath->setEnabled(enabled);
    ui->groupBoxEncode->setEnabled(enabled);
    ui->btnTransmit->setEnabled(enabled);
    ui->btnClear->setEnabled(enabled);
}

QStringList MainWindow::getFileSuffixes() const
{
    QStringList suffixes;
    suffixes.append(fileSuffixes);
    suffixes.append(customFileSuffixes);
    return suffixes;
}

QString MainWindow::getTargetEncoding() const
{
    QStringList encodeTypes = {"UTF-8 BOM", "UTF-8", "GB2312"};
    int index = ui->comboBox->currentIndex();

    if (index >= 0 && index < encodeTypes.size()) {
        return encodeTypes[index];
    }

    return "UTF-8 BOM";  // 默认返回 UTF-8 BOM
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (!mimeData->hasUrls()) {
        return;
    }

    QList<QUrl> urls = mimeData->urls();
    if (urls.isEmpty()) {
        return;
    }

    QString path = urls.first().toLocalFile();
    QFileInfo fi(path);

    if (fi.isDir()) {
        currentPath = path;
        isFolderMode = true;
        ui->labelPath->setText(path);
    } else if (fi.isFile()) {
        currentPath = path;
        isFolderMode = false;
        ui->labelPath->setText(path);
    }
}

void MainWindow::onTotalCount(int total)
{
    ui->progressBar->setRange(0, total);
    ui->progressBar->setValue(0);
}

void MainWindow::onProgressChanged(int current)
{
    ui->progressBar->setValue(current);
}

void MainWindow::onFilterChanged(int index)
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QTableWidgetItem *resultItem = ui->tableWidget->item(row, 3);
        if (!resultItem) {
            continue;
        }

        QString result = resultItem->text();
        bool show = false;

        switch (index) {
        case 0:  // 全部
            show = true;
            break;
        case 1:  // 仅成功
            show = result.contains("成功");
            break;
        case 2:  // 仅失败
            show = result.contains("失败") || result.contains("跳过");
            break;
        }

        ui->tableWidget->setRowHidden(row, !show);
    }
}
