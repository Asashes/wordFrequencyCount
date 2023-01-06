#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::CountLinkList()
{

}

//用哈希表统计词频，将结果放在tableWidget中
void Widget::CountHashMap()
{
    QFile file(ui->fileInfo->toPlainText());
    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"警告","打开文件失败!");
        return;
    }
    QHash<QString,int> hashmap;
    QTextStream text_stream(&file);
    while (!text_stream.atEnd()){
        QString str;
        text_stream>>str;
        str = str.toLower();
        //提取标点符号
        QString punctuation = str;
        punctuation.remove(QRegularExpression("[a-z0-9]"));
        if (punctuation != "") {
            for (auto it = punctuation.begin(); it!= punctuation.end(); ++it) {
                if (hashmap.find(*it) == hashmap.end()) {
                    hashmap.insert(*it,1);
                } else {
                    hashmap[*it]++;
                }
            }
        }
        //提取单词（包括连字符- 撇号'` ）、数字
        str.remove(QRegularExpression("[^a-z0-9`'-]"));
        if (str != "") {
            if (hashmap.find(str) == hashmap.end()){
                hashmap.insert(str,1);
            } else {
                hashmap[str]++;
            }
        }
    }
    QStandardItemModel* model = new QStandardItemModel(this);
    model->setColumnCount(2);
    model->setHeaderData(0,Qt::Horizontal, "单词");
    model->setHeaderData(1,Qt::Horizontal, "次数");
    int row = 0;
    for (auto it = hashmap.begin(); it != hashmap.end(); ++it){
        qDebug()<<it.key()<<it.value();
        model->setItem(row, 0, new QStandardItem(it.key()));
        model->setItem(row, 1, new QStandardItem(QString::number(it.value())));
        row++;
    }
    ui->tableView->setModel(model);
}

//用二叉树统计词频，将结果放在tableWidget中
void Widget::CountBinaryTree()
{

}

//调出文件选择界面，在fileInfo中填入文件位置信息
void Widget::on_btnFileSelect_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,QStringLiteral("选择文件"),"C:",QStringLiteral("文本文件(*txt)"));
    ui->fileInfo->clear();
    ui->fileInfo->setText(file_name);
}

//退出程序
void Widget::on_btnExit_clicked()
{
    QWidget::close();
}

//根据文件信息和选择的算法开始进行词频统计
void Widget::on_btnStartCount_clicked()
{
    if (ui->fileInfo->toPlainText() == "") {
        QMessageBox::warning(this,"警告","请选择需进行统计的文件");
        return;
    } else if (!ui->btnBinaryTree->isChecked() && !ui->btnHashMap->isChecked() && !ui->btnLinkList->isChecked()) {
        QMessageBox::warning(this,"警告","请选择一种算法");
        return;
    } else {
        if (ui->btnLinkList->isChecked()) {

        } else if (ui->btnHashMap->isChecked()) {
            CountHashMap();
        } else if (ui->btnBinaryTree->isChecked()) {

        }
    }
}

