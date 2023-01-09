#ifndef WIDGET_H
#define WIDGET_H
#define KeepLiveBegin(ignoreevent) {IgnoreEvent* kl_ie =NULL; if(ignoreevent)kl_ie = new IgnoreEvent(); std::condition_variable kl_cv;std::mutex kl_mtx;std::unique_lock <std::mutex> kl_lck(kl_mtx);\
        std::thread kl_thread([&](){kl_mtx.lock(); kl_mtx.unlock();

#define KeepLiveEnd	kl_cv.notify_all();});kl_thread.detach(); \
        while (kl_cv.wait_for(kl_lck, std::chrono::milliseconds(100)) == std::cv_status::timeout){qApp->processEvents();}if(kl_ie!=NULL) delete kl_ie;}

#include <thread>
#include <mutex>
#include <condition_variable>
#include <QApplication>

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QHash>
#include <QStandardItemModel>
#include <list>
#include <QMap>
#include <QSize>
#include <QDir>
#include <QDebug>
#include <sys/time.h>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    std::list<std::pair<QString,int>>::iterator FindInList(std::list<std::pair<QString,int>>& list, QString str);
private slots:
    void on_btnFileSelect_clicked();

    void on_btnExit_clicked();

    void on_btnStartCount_clicked();

    void on_btnExport_clicked();

    void on_btnTest_clicked();

    void set_progress(int value);

signals:
    void progress_changed(int value);

private:
    Ui::Widget *ui;
    int progress;
    void CountLinkList();
    void CountHashMap();
    void CountBinaryTree();
    void SetTestFile();
    void WriteResultFile(QString algorithm = "", QString time = "");
};
#endif // WIDGET_H

class IgnoreEvent :public QObject
{
public:
    IgnoreEvent(QObject* obj=qApp)
    {
        m_obj = obj;
        m_obj->installEventFilter(this);
    }
    ~IgnoreEvent()
    {
        m_obj->removeEventFilter(this);
    }
    bool eventFilter(QObject *obj, QEvent *event)
    {
        if (event->type() == QEvent::KeyPress || event->type() == QEvent::MouseButtonPress)
        {
            event->ignore();
            return true;
        }
        return QObject::eventFilter(obj, event);
    }
private:
    QObject* m_obj;
};
