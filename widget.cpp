#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("://quin.ico"));
    setWindowTitle("词频统计程序");
}

Widget::~Widget()
{
    delete ui;
}

//在list中查找对应的str，找到返回对应节点的迭代器，否则返回list末尾的迭代器
std::list<std::pair<QString,int>>::iterator Widget::FindInList(std::list<std::pair<QString,int>>& list, QString str)
{
    for (auto it = list.begin(); it != list.end(); ++it) {
        if (it->first == str) {
            return it;
        }
    }
    return list.end();
}

//用双向链表统计词频，将结果放在tableWidget中
void Widget::CountLinkList()
{
    QFile file(ui->fileInfo->toPlainText());
    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"警告","打开文件失败!");
        return;
    }
    std::list<std::pair<QString,int>> list;
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
                auto list_it = FindInList(list,*it);
                if (list_it == list.end()) {
                    list.push_back(std::make_pair(*it,1));
                } else {
                    list_it->second++;
                }
            }
        }
        //提取单词（包括连字符- 撇号'` ）、数字
        str.remove(QRegularExpression("[^a-z0-9`'-]"));
        if (str != "") {
            auto list_it = FindInList(list,str);
            if (list_it == list.end()){
                list.push_back(std::make_pair(str,1));
            } else {
                list_it->second++;
            }
        }
    }
    file.close();
    //设置tableWidget
    ui->tableWidget->clear();
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setRowCount(list.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "单词" << "次数");
    int row = 0;
    for (auto it = list.begin(); it != list.end(); ++it){
        //qDebug() << it->first << it->second;
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(it->first));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(it->second)));
        row++;
    }
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
    file.close();
    //设置tableWidget
    ui->tableWidget->clear();
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setRowCount(hashmap.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "单词" << "次数");
    int row = 0;
    for (auto it = hashmap.begin(); it != hashmap.end(); ++it){
        //qDebug() << it.key() << it.value();
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(it.key()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
        row++;
    }
}

//用二叉树统计词频，将结果放在tableWidget中
void Widget::CountBinaryTree()
{
    QFile file(ui->fileInfo->toPlainText());
    if (!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"警告","打开文件失败!");
        return;
    }
    QMap<QString,int> rb_tree;
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
                if (rb_tree.find(*it) == rb_tree.end()) {
                    rb_tree.insert(*it,1);
                } else {
                    rb_tree[*it]++;
                }
            }
        }
        //提取单词（包括连字符- 撇号'` ）、数字
        str.remove(QRegularExpression("[^a-z0-9`'-]"));
        if (str != "") {
            if (rb_tree.find(str) == rb_tree.end()){
                rb_tree.insert(str,1);
            } else {
                rb_tree[str]++;
            }
        }
    }
    file.close();
    //设置tableWidget
    ui->tableWidget->clear();
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setRowCount(rb_tree.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "单词" << "次数");
    int row = 0;
    for (auto it = rb_tree.begin(); it != rb_tree.end(); ++it){
        //qDebug() << it.key() << it.value();
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(it.key()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
        row++;
    }
    ui->tableWidget->sortByColumn(1,Qt::DescendingOrder);
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
            CountLinkList();
        } else if (ui->btnHashMap->isChecked()) {
            CountHashMap();
        } else if (ui->btnBinaryTree->isChecked()) {
            CountBinaryTree();
        }
    }
}

//输出结果到文件
void Widget::on_btnExport_clicked()
{
    WriteResultFile();
}

//新建测试文件和结果文件
void Widget::SetTestFile()
{
    QFile file("test_text.txt");
    if (!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this,"警告","创建测试文件失败!");
        return;
    }
    file.write(QString("It seems to me a very difficult thing to put into words the beliefs we hold and what they make us do in our life.\n").toUtf8());
    file.write(QString("I think I was fortunate because I grew up in a family where there was a very deep religious feeling.\n").toUtf8());
    file.write(QString("I don’t think it was spoken of a great deal. It was more or less taken for granted that everybody held certain beliefs").toUtf8());
    file.write(QString(" and needed certain reinforcements of their strength and that came through your belief in God and your knowledge of prayer.").toUtf8());
    ui->fileInfo->clear();
    ui->fileInfo->setText(QDir::currentPath()+"/test_text.txt");
    QFile file_r("result.txt");
    if (!file_r.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        QMessageBox::warning(this,"警告","创建结果文件失败!");
        return;
    }
}

//将结果写入文件，自动化测试调用该函数时要传入所用的算法和时间
void Widget::WriteResultFile(QString algorithm, QString time)
{
    if (ui->tableWidget->rowCount() == 0) {
        QMessageBox::warning(this,"警告","请先进行词频统计");
        return;
    }
    QFile file("result.txt");
    if (algorithm != "" && time != "") {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QMessageBox::warning(this,"警告","打开文件失败!");
            return;
        }
        file.write(("使用算法：" + algorithm + ' ' + "所用时间:" + time + '\n').toUtf8());
        qDebug() << "使用算法：" + algorithm + ' ' + "所用时间:" + time;
    } else {
        if (!file.open(QIODevice::WriteOnly)){
            QMessageBox::warning(this,"警告","打开文件失败!");
            return;
        }
    }
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        QString word, count;
        word = ui->tableWidget->item(i,0)->text();
        count = ui->tableWidget->item(i,1)->text();
        file.write((word + ' ' + count + '\n').toUtf8());
    }
    file.close();
    if(algorithm == "" && time == ""){
        QMessageBox::information(this,"提示","结果文件result.txt的路径：" + QDir::currentPath());
    }
}

void Widget::on_btnTest_clicked()
{
    SetTestFile();
    double time_Start = (double)clock();
    CountLinkList();
    double time_End = (double)clock();
    QString time = QString::number((time_End - time_Start)/1000.0) + "s";
    WriteResultFile("双链表",time);
    time_Start = (double)clock();
    CountHashMap();
    time_End = (double)clock();
    time = QString::number((time_End - time_Start)/1000.0) + "s";
    WriteResultFile("哈希表",time);
    time_Start = (double)clock();
    CountBinaryTree();
    time_End = (double)clock();
    time = QString::number((time_End - time_Start)/1000.0) + "s";
    WriteResultFile("二叉树",time);
    ui->fileInfo->clear();
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);
    QMessageBox::information(this,"提示","测试完成，结果文件result.txt的路径：" + QDir::currentPath());
}
