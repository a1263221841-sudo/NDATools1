#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStyle>
#include <QVBoxLayout>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    setupMainLayout();
    initApplicationShell();
}

MainWidget::~MainWidget()
{
    if (trayIcon) {
        trayIcon->hide();
    }
    delete ui;
}

void MainWidget::initApplicationShell()
{
    setWindowTitle(QStringLiteral("NDA Tools 网络调试助手"));
    setMinimumSize(1180, 780);
    resize(1260, 820);
    createTrayIcon();
    loadSettings();
}

void MainWidget::setupMainLayout()
{
    QHBoxLayout *topBarLayout = new QHBoxLayout();
    topBarLayout->setContentsMargins(0, 0, 0, 0);
    topBarLayout->addStretch();
    topBarLayout->addWidget(ui->label_ThemeTitle);
    topBarLayout->addWidget(ui->label_CurrentTheme);
    topBarLayout->addWidget(ui->pushButton_ThemeToggle);

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(10, 8, 10, 10);
    rootLayout->setSpacing(6);
    rootLayout->addLayout(topBarLayout);
    rootLayout->addWidget(ui->tab_Widget, 1);

    ui->pushButton_ThemeToggle->setMinimumHeight(34);
    ui->pushButton_ThemeToggle->setMinimumWidth(150);
    ui->tab_Widget->setMinimumSize(1120, 700);
}

void MainWidget::createTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    trayIcon = new QSystemTrayIcon(this);
    QIcon icon = windowIcon();
    if (icon.isNull()) {
        icon = style()->standardIcon(QStyle::SP_ComputerIcon);
        setWindowIcon(icon);
    }
    trayIcon->setIcon(icon);
    trayIcon->setToolTip(QStringLiteral("NDA Tools 网络调试助手"));

    createTrayMenu();
    trayIcon->setContextMenu(trayMenu);

    connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
            if (isHidden() || isMinimized()) {
                showFromTray();
            } else {
                hideToTray();
            }
        }
    });

    trayIcon->show();
}

void MainWidget::createTrayMenu()
{
    trayMenu = new QMenu(this);

    showAction = trayMenu->addAction(QStringLiteral("显示主窗口"));
    hideAction = trayMenu->addAction(QStringLiteral("隐藏到托盘"));
    trayMenu->addSeparator();

    darkThemeAction = trayMenu->addAction(QStringLiteral("深色模式"));
    darkThemeAction->setCheckable(true);
    lightThemeAction = trayMenu->addAction(QStringLiteral("浅色模式"));
    lightThemeAction->setCheckable(true);
    trayMenu->addSeparator();

    autoStartAction = trayMenu->addAction(QStringLiteral("开机自启"));
    autoStartAction->setCheckable(true);
    trayMenu->addSeparator();

    quitAction = trayMenu->addAction(QStringLiteral("退出"));

    connect(showAction, &QAction::triggered, this, &MainWidget::showFromTray);
    connect(hideAction, &QAction::triggered, this, &MainWidget::hideToTray);
    connect(darkThemeAction, &QAction::triggered, this, [this]() {
        applyTheme(QStringLiteral("dark"));
        saveTheme(QStringLiteral("dark"));
    });
    connect(lightThemeAction, &QAction::triggered, this, [this]() {
        applyTheme(QStringLiteral("light"));
        saveTheme(QStringLiteral("light"));
    });
    connect(autoStartAction, &QAction::toggled, this, &MainWidget::setAutoStartEnabled);
    connect(quitAction, &QAction::triggered, this, [this]() {
        quitRequested = true;
        if (trayIcon) {
            trayIcon->hide();
        }
        close();
    });
}

void MainWidget::loadSettings()
{
    QSettings settings;
    const QString themeName = settings.value(QStringLiteral("App/theme"), QStringLiteral("light")).toString();
    applyTheme(themeName);
    updateThemeUi(themeName);

    if (autoStartAction) {
        autoStartAction->blockSignals(true);
        autoStartAction->setChecked(isAutoStartEnabled());
        autoStartAction->blockSignals(false);
    }
}

void MainWidget::saveTheme(const QString &themeName)
{
    QSettings settings;
    settings.setValue(QStringLiteral("App/theme"), themeName);
}

void MainWidget::applyTheme(const QString &themeName)
{
    const bool dark = (themeName == QLatin1String("dark"));

    if (darkThemeAction) {
        darkThemeAction->setChecked(dark);
    }
    if (lightThemeAction) {
        lightThemeAction->setChecked(!dark);
    }

    updateThemeUi(themeName);

    if (dark) {
        qApp->setStyleSheet(
            "QWidget { background-color: #1f2937; color: #f9fafb; }"
            "QTabWidget::pane { border: 1px solid #374151; }"
            "QTabBar::tab { background: #374151; color: #f9fafb; padding: 6px 12px; }"
            "QTabBar::tab:selected { background: #2563eb; }"
            "QPushButton { background-color: #2563eb; color: #ffffff; border: none; border-radius: 4px; padding: 4px 10px; }"
            "QPushButton:hover { background-color: #1d4ed8; }"
            "QLineEdit, QTextEdit, QPlainTextEdit, QListWidget, QComboBox, QSpinBox {"
            " background-color: #111827; color: #f9fafb; border: 1px solid #4b5563; }"
            "QGroupBox { border: 1px solid #4b5563; margin-top: 8px; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }");
        return;
    }

    qApp->setStyleSheet(
        "QWidget { background-color: #f3f4f6; color: #111827; }"
        "QTabWidget::pane { border: 1px solid #d1d5db; }"
        "QTabBar::tab { background: #e5e7eb; color: #111827; padding: 6px 12px; }"
        "QTabBar::tab:selected { background: #2563eb; color: #ffffff; }"
        "QPushButton { background-color: #2563eb; color: #ffffff; border: none; border-radius: 4px; padding: 4px 10px; }"
        "QPushButton:hover { background-color: #1d4ed8; }"
        "QLineEdit, QTextEdit, QPlainTextEdit, QListWidget, QComboBox, QSpinBox {"
        " background-color: #ffffff; color: #111827; border: 1px solid #d1d5db; }"
        "QGroupBox { border: 1px solid #d1d5db; margin-top: 8px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }");
}

QString MainWidget::currentTheme() const
{
    QSettings settings;
    return settings.value(QStringLiteral("App/theme"), QStringLiteral("light")).toString();
}

void MainWidget::updateThemeUi(const QString &themeName)
{
    if (!ui) {
        return;
    }

    const bool dark = (themeName == QLatin1String("dark"));
    if (ui->label_CurrentTheme) {
        ui->label_CurrentTheme->setText(dark ? QStringLiteral("深色模式") : QStringLiteral("浅色模式"));
    }
    if (ui->pushButton_ThemeToggle) {
        ui->pushButton_ThemeToggle->setText(dark ? QStringLiteral("切换到浅色模式")
                                                 : QStringLiteral("切换到深色模式"));
    }
}

void MainWidget::on_pushButton_ThemeToggle_clicked()
{
    const QString nextTheme = currentTheme() == QLatin1String("dark")
        ? QStringLiteral("light")
        : QStringLiteral("dark");
    applyTheme(nextTheme);
    saveTheme(nextTheme);
}

void MainWidget::showFromTray()
{
    showNormal();
    raise();
    activateWindow();
}

void MainWidget::hideToTray()
{
    hide();
    if (trayIcon) {
        trayIcon->showMessage(QStringLiteral("NDA Tools"),
                              QStringLiteral("应用已最小化到系统托盘"),
                              QSystemTrayIcon::Information,
                              1500);
    }
}

bool MainWidget::isAutoStartEnabled() const
{
#ifdef Q_OS_WIN
    QSettings reg(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                  QSettings::NativeFormat);
    return reg.contains(QCoreApplication::applicationName());
#else
    return false;
#endif
}

void MainWidget::setAutoStartEnabled(bool enabled)
{
#ifdef Q_OS_WIN
    QSettings reg(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                  QSettings::NativeFormat);
    const QString appName = QCoreApplication::applicationName();
    const QString appPath = QDir::toNativeSeparators(QFileInfo(QCoreApplication::applicationFilePath()).absoluteFilePath());

    if (enabled) {
        reg.setValue(appName, QStringLiteral("\"%1\"").arg(appPath));
    } else {
        reg.remove(appName);
    }
#else
    Q_UNUSED(enabled)
    QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("当前仅实现 Windows 开机自启。"));
#endif
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    if (quitRequested || !trayIcon || !trayIcon->isVisible()) {
        QWidget::closeEvent(event);
        return;
    }

    hideToTray();
    event->ignore();
}

void MainWidget::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);

    if (event->type() == QEvent::WindowStateChange && isMinimized() && trayIcon && trayIcon->isVisible()) {
        hideToTray();
    }
}
