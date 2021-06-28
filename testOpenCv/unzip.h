#ifndef UNZIP_H
#define UNZIP_H

#include <QObject>
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

using namespace cv;
namespace fs = std::filesystem;


static bool GetAllFormatFilePath(QString& findPath, QList<QString>& receiveFilePath, QString format )
{
    QDir* dir = new QDir(findPath);
    if (!dir->exists())
    return false;

    QStringList filter;
    filter << format;

    QList<QFileInfo>* dirInfoList = new QList<QFileInfo>(dir->entryInfoList(QDir::Dirs));
    QList<QFileInfo>* fomatInfoList = new QList<QFileInfo>(dir->entryInfoList(filter));

    for (int i = 0; i < dirInfoList->count(); i++) {
    if (dirInfoList->at(i).fileName() == "." || dirInfoList->at(i).fileName() == "..")
    continue;
    QString dirTmp = dirInfoList->at(i).filePath();
    GetAllFormatFilePath(dirTmp, receiveFilePath, format);
    }
    for (int i = 0; i < fomatInfoList->count(); i++)
    {
        receiveFilePath << fomatInfoList->at(i).filePath();
    }
    delete dirInfoList;
    delete dir;
    return true;
}







class unzip : public QObject
{
    Q_OBJECT
public:
    explicit unzip(QObject *parent = nullptr);

    ~unzip();
signals:
    void sendMessage(QString const&);

public slots:
    void start_run(std::string const&, QString);

private:
    QString outPutDir;

    void recursive_extraction(std::string const &output_directory);

    bool get_panel_num(std::wstring const &dir_name, int &panel_num, bool &top);

    QStringList getAllFolderName(QString path);

    bool create_composed(bool const &top, std::string const &panel_folder,
                                 int const &panel,
                                 int const &num_of_grabbers,
                                 int const &num_of_buffers,
                                 std::string const &output_filepath);

    void copyImage(QString& path , std::string panelPath, QString& out, bool top, int num_buffers, int num_grabbers, int panel);

    void outputCSV(QString const& job, QString const& path, int const& panel, bool top);

    QStringList getFileNames(const QString &path);

};

#endif // UNZIP_H
