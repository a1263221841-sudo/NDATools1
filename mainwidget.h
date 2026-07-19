#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QAction>
#include <QCloseEvent>
#include <QEvent>
#include <QMenu>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget() override;

private slots:
    void on_pushButton_ThemeToggle_clicked();

protected:
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    Ui::MainWidget *ui;
    QSystemTrayIcon *trayIcon = nullptr;
    QMenu *trayMenu = nullptr;
    QAction *showAction = nullptr;
    QAction *hideAction = nullptr;
    QAction *darkThemeAction = nullptr;
    QAction *lightThemeAction = nullptr;
    QAction *autoStartAction = nullptr;
    QAction *quitAction = nullptr;
    bool quitRequested = false;

    void initApplicationShell();
    void setupMainLayout();
    void createTrayIcon();
    void createTrayMenu();
    void loadSettings();
    void saveTheme(const QString &themeName);
    void applyTheme(const QString &themeName);
    QString currentTheme() const;
    void updateThemeUi(const QString &themeName);
    void showFromTray();
    void hideToTray();
    bool isAutoStartEnabled() const;
    void setAutoStartEnabled(bool enabled);
};
#endif // MAINWIDGET_H
