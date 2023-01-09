#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(this,&Widget::progress_changed,this,&Widget::set_progress);
    setWindowIcon(QIcon("://quin.ico"));
    setWindowTitle("词频统计程序");
}

Widget::~Widget()
{
    delete ui;
}

//连接到进度条的槽函数，用于更新进度条的值
void Widget::set_progress(int value)    //由于在子线程中无法直接操作ui，本想在子线程中发出信号调用该槽函数更新进度条，但最终没有用子线程，也就意义不大了
{
    ui->progressBar->setValue(value);
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
    ui->progressBar->setRange(0,file.size());
    ui->progressBar->setValue(0);
    double time_Start = (double)clock();
    uchar* const pfile = file.map(0,file.size());   //指向被映射到内存的文件内容
    uchar* p = pfile;           //遍历文件
     //KeepLiveBegin(true)
    IgnoreEvent ignore;
    QElapsedTimer et;
    et.start();
    while (p - pfile < file.size()) {
        if (et.elapsed() > kprocess_events_time) {      //每隔 100ms调用 QCoreApplication::processEvents()防止 ui假死
            QCoreApplication::processEvents();
            et.restart();
        }
        emit progress_changed(p - pfile);
        //ui->progressBar->setValue(p - pfile);
        QString str;        //提取每个单词
        while (char(*p) == ' ' || char(*p) == '\n' || char(*p) == '\r' || char(*p) == '\t') p++;
        while (p - pfile < file.size() && char(*p) != ' ' && char(*p) != '\n' && char(*p) != '\r' && char(*p) != '\t') {
            if (char(*p) >= 'a' && char(*p) <= 'z') {
                str += char(*p);
                p++;
            } else if (char(*p) >= 'A' && char(*p) <= 'Z') {
                str += char(*p + 32);
                p++;
            } else if (char(*p) >= '0' && char(*p) <= '9') {
                str += char(*p);
                p++;
            } else {
                auto it = FindInList(list,QString(char(*p)));
                if (it == list.end()) {
                    list.push_back(std::make_pair(QString(char(*p)),1));
                } else {
                    it->second++;
                }
                p++;
            }
        }
        auto it = FindInList(list,str);
        if (it == list.end()) {
            list.push_back(std::make_pair(str,1));
        } else {
            it->second++;
        }
    }
    //KeepLiveEnd
    double time_End = (double)clock();
    qDebug() << QString::number((time_End - time_Start)/1000.0) + "s";
    //ui->progressBar->setValue(file.size());
    emit progress_changed(file.size());
    file.unmap(pfile);
    file.close();
    //设置tableWidget
    ui->tableWidget->clear();
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setRowCount(list.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "单词" << "次数");
    int row = 0;
    for (auto it = list.begin(); it != list.end(); ++it){
        //qDebug() << it.key() << it.value();
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
    QHash<QString,int> hashmap;             //存储单词和数字
    QHash<char,int> hashmap_punctuation;    //存储标点
    ui->progressBar->setRange(0,file.size());
    ui->progressBar->setValue(0);
    double time_Start = (double)clock();
    uchar* const pfile = file.map(0,file.size());   //指向被映射到内存的文件内容
    uchar* p = pfile;   //遍历文件
    //KeepLiveBegin(true)
    IgnoreEvent ignore;
    QElapsedTimer et;
    et.start();
    while (p - pfile < file.size()) {
        if (et.elapsed() > kprocess_events_time) {       //每隔 100ms调用 QCoreApplication::processEvents()防止 ui假死
            QCoreApplication::processEvents();
            et.restart();
        }
        emit progress_changed(p - pfile);
        //ui->progressBar->setValue(p - pfile);
        QString str;        //提取每个单词
        while (char(*p) == ' ' || char(*p) == '\n' || char(*p) == '\r' || char(*p) == '\t') p++;    //跳过单词间的空白符号
        while (p - pfile < file.size() && char(*p) != ' ' && char(*p) != '\n' && char(*p) != '\r' && char(*p) != '\t') {
            if (char(*p) >= 'a' && char(*p) <= 'z') {       //提取字母和数字
                str += char(*p);
                p++;
            } else if (char(*p) >= 'A' && char(*p) <= 'Z') {
                str += char(*p + 32);
                p++;
            } else if (char(*p) >= '0' && char(*p) <= '9') {
                str += char(*p);
                p++;
            } else {    //非字母和数字的符号放进 hashmap_punctuation
                if (hashmap_punctuation.find(char(*p)) == hashmap_punctuation.end()) {
                    hashmap_punctuation.insert(char(*p),1);
                } else {
                    hashmap_punctuation[char(*p)]++;
                }
                p++;
            }
        }
        if (hashmap.find(str) == hashmap.end()){
            hashmap.insert(str,1);
        } else {
            hashmap[str]++;
        }
    }
    //KeepLiveEnd
    double time_End = (double)clock();
    qDebug() << QString::number((time_End - time_Start)/1000.0) + "s";
    //ui->progressBar->setValue(file.size());
    emit progress_changed(file.size());
    file.unmap(pfile);
    file.close();
    //设置tableWidget
    ui->tableWidget->clear();
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setRowCount(hashmap.size() + hashmap_punctuation.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "单词" << "次数");
    int row = 0;
    for (auto it = hashmap.begin(); it != hashmap.end(); ++it){
        //qDebug() << it.key() << it.value();
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(it.key()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
        row++;
    }
    for (auto it = hashmap_punctuation.begin(); it != hashmap_punctuation.end(); ++it){
        //qDebug() << it.key() << it.value();
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString(it.key())));
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
    QMap<QString,int> rb_tree;              //存储单词和数字
    QMap<char,int> rb_tree_punctuation;     //存储标点
    ui->progressBar->setRange(0,file.size());
    ui->progressBar->setValue(0);
    double time_Start = (double)clock();
    uchar* const pfile = file.map(0,file.size());   //指向被映射到内存的文件内容
    uchar* p = pfile;       //遍历文件
    //KeepLiveBegin(true)
    IgnoreEvent ignore;
    QElapsedTimer et;
    et.start();
    while (p - pfile < file.size()) {
        if (et.elapsed() > kprocess_events_time) {      //每隔 100ms调用 QCoreApplication::processEvents()防止 ui假死
            QCoreApplication::processEvents();
            et.restart();
        }
        emit progress_changed(p - pfile);
        //ui->progressBar->setValue(p - pfile);
        QString str;        //提取每个单词
        while (char(*p) == ' ' || char(*p) == '\n' || char(*p) == '\r' || char(*p) == '\t') p++;    //跳过单词间的空白符号
        while (p - pfile < file.size() && char(*p) != ' ' && char(*p) != '\n' && char(*p) != '\r' && char(*p) != '\t') {
            if (char(*p) >= 'a' && char(*p) <= 'z') {           //提取字母和数字
                str += char(*p);
                p++;
            } else if (char(*p) >= 'A' && char(*p) <= 'Z') {
                str += char(*p + 32);
                p++;
            } else if (char(*p) >= '0' && char(*p) <= '9') {
                str += char(*p);
                p++;
            } else {
                if (rb_tree_punctuation.find(char(*p)) == rb_tree_punctuation.end()) {
                    rb_tree_punctuation.insert(char(*p),1);
                } else {
                    rb_tree_punctuation[char(*p)]++;
                }
                p++;
            }
        }
        if (rb_tree.find(str) == rb_tree.end()){
            rb_tree.insert(str,1);
        } else {
            rb_tree[str]++;
        }
    }
    //KeepLiveEnd
    double time_End = (double)clock();
    qDebug() << QString::number((time_End - time_Start)/1000.0) + "s";
    //ui->progressBar->setValue(file.size());
    emit progress_changed(file.size());
    file.unmap(pfile);
    file.close();
    //设置tableWidget
    ui->tableWidget->clear();
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setRowCount(rb_tree.size() + rb_tree_punctuation.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "单词" << "次数");
    int row = 0;
    for (auto it = rb_tree.begin(); it != rb_tree.end(); ++it){
        //qDebug() << it.key() << it.value();
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(it.key()));
        //ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setData(Qt::EditRole , it.value());
        ui->tableWidget->setItem(row , 1 , item);
        row++;
    }
    for (auto it = rb_tree_punctuation.begin(); it != rb_tree_punctuation.end(); ++it){
        //qDebug() << it.key() << it.value();
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString(it.key())));
        //ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setData(Qt::EditRole , it.value());
        ui->tableWidget->setItem(row , 1 , item);
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
/*void Widget::SetTestFile()
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
}*/

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
        QMessageBox::information(this,"提示","统计完成，结果文件 result.txt 的路径：" + QDir::currentPath());
    }
}
/*
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
    QMessageBox::information(this,"提示","测试完成，结果文件 result.txt 的路径：" + QDir::currentPath());
}*/
