#ifndef WIDGET_H
#define WIDGET_H
#define UNICODE

#include <QWidget>
#include <dialog.h>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <opencv2/imgproc/types_c.h>
#include "fmt/format.h"
#include "boost/locale.hpp"
#include "boost/tokenizer.hpp"
#include "filesystem"
#include "regex"
#include <fstream>
#include <QDir>
#include "Windows.h"
#include <QFileDialog>
#include <unzip.h>
#include <QThread>

#pragma execution_character_set("utf-8")

using namespace cv;
namespace fs = std::filesystem;



//找到指定的文件名
//static bool GetAllFormatFilePath(QString& findPath, QList<QString>& receiveFilePath, QString format )
//{
//    QDir* dir = new QDir(findPath);
//    if (!dir->exists())
//    return false;

//    QStringList filter;
//    filter << format;

//    QList<QFileInfo>* dirInfoList = new QList<QFileInfo>(dir->entryInfoList(QDir::Dirs));
//    QList<QFileInfo>* fomatInfoList = new QList<QFileInfo>(dir->entryInfoList(filter));

//    for (int i = 0; i < dirInfoList->count(); i++) {
//    if (dirInfoList->at(i).fileName() == "." || dirInfoList->at(i).fileName() == "..")
//    continue;
//    QString dirTmp = dirInfoList->at(i).filePath();
//    GetAllFormatFilePath(dirTmp, receiveFilePath, format);
//    }
//    for (int i = 0; i < fomatInfoList->count(); i++)
//    {
//        receiveFilePath << fomatInfoList->at(i).filePath();
//    }
//    delete dirInfoList;
//    delete dir;
//    return true;
//}


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    bool create_composed(bool const &top, std::string const &panel_folder,
                         int const &num_of_grabbers,
                         int const &num_of_buffers,
                         std::string const &output_filepath);
    void unpackZip();

    void recursive_extraction(std::string const &output_directory);

    bool get_panel_num(std::wstring const &dir_name, int &panel_num, bool &top);

    QStringList getAllFolderName(QString path);


    QStringList getFileNames(const QString &path);

    void copyImage(QString&, std::string, QString&, bool, int, int);

    void outputCSV(QString const&, QString const& panlePath, int const& panel, bool top);

private slots:


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void getMessage(QString const&);

signals:
    void startThread(std::string const&, QString);

private:
    Ui::Widget *ui;
    std::string  filename;
    QString outPutDir;
    QThread dothread;


};
#endif // WIDGET_H
