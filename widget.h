#ifndef WIDGET_H
#define WIDGET_H

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

private:
    Ui::Widget *ui;
    void CountLinkList();
    void CountHashMap();
    void CountBinaryTree();

};
#endif // WIDGET_H

