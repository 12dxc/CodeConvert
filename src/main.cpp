#include <QApplication>
#include "mainwindow.h"
#include "version.h"

/**
 * @brief 程序入口函数
 *
 * 初始化 Qt 应用程序并显示主窗口
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息（版本号由 CMake 生成的 version.h 提供）
    app.setApplicationName("CodeConvert");
    app.setApplicationVersion(CODECONVERT_VERSION);
    app.setOrganizationName("CodeConvert");

    // 创建并显示主窗口
    MainWindow window;
    window.show();

    return app.exec();
}
