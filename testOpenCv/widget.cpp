#include "widget.h"
#include "ui_widget.h"
#include <QTextCodec>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    filename = "";
    outPutDir = ui->outPutDirLineEdit->text();
    unzip *dowork = new unzip();
    dowork->moveToThread(&dothread);

    connect(this, &Widget::startThread, dowork, &unzip::start_run);
    connect(dowork, &unzip::sendMessage, this, &Widget::getMessage);

    dothread.start();

}

Widget::~Widget()
{
    delete ui;
    dothread.quit();
    dothread.wait();

}


bool Widget::create_composed(bool const &top, std::string const &panel_folder,
                             int const &num_of_grabbers,
                             int const &num_of_buffers,
                             std::string const &output_filepath)
{

    try {
      std::string filn;
      cv::Mat composed;
      cv::Rect roi_r;
      for (int strip_idx = 1; strip_idx <= num_of_grabbers; strip_idx++) {
        for (int image = 1; image <= num_of_buffers; image++) {
          if (top)
            filn = fmt::format("{}/{}/imScale{}.jpeg", panel_folder, strip_idx,
                               image);
          else
            filn = fmt::format("{}/{}/imScale{}.jpeg", panel_folder,
                               num_of_grabbers - strip_idx + 1, image);

          cv::Mat newmembuf_strip = cv::imread(filn, 0);
          if (strip_idx == 1 && image == 1) // init the buffer
          {
            composed =
                cv::Mat::zeros(cv::Size(newmembuf_strip.rows * num_of_grabbers,
                                        newmembuf_strip.cols * num_of_buffers),
                               CV_8UC1);
            roi_r = cv::Rect(0, 0, newmembuf_strip.rows, newmembuf_strip.cols);
          }

          cv::Rect roi = cv::Rect((strip_idx - 1) * newmembuf_strip.rows,
                                  (image - 1) * newmembuf_strip.cols, roi_r.width,
                                  roi_r.height);
          cv::Mat dstroi = composed(roi);

          cv::transpose(newmembuf_strip, newmembuf_strip);
          // cv::flip(newmembuf_strip, newmembuf_strip, 1);

          if (!top)
            cv::flip(newmembuf_strip, newmembuf_strip, 1);
          if (newmembuf_strip.dims > 0) {
            newmembuf_strip(roi_r).copyTo(dstroi);
          }
        }
      }

      std::vector<int> compression_params;
      compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
      compression_params.push_back(30);
      compression_params.push_back(cv::IMWRITE_JPEG_PROGRESSIVE);
      compression_params.push_back(1);
      compression_params.push_back(cv::IMWRITE_JPEG_OPTIMIZE);
      compression_params.push_back(1);

      bool ok = cv::imwrite(output_filepath, composed, compression_params);
      return ok;
    } catch (std::exception &e) {
      ui->textEdit->append(QString("Error while creating composed serv image -- %1").arg(e.what()));
      return false;
    }
}


void Widget::unpackZip()
{
    QDir dir;
    boost::locale::generator gen;
    auto loc = gen.generate("zh-CN.UTF-8");
    std::wstring wfilename = boost::locale::conv::to_utf<wchar_t>(filename, loc);

    if (!fs::exists(wfilename))
    {
        std::cout << "Does not exist"<<std::endl;
        ui->textEdit->append("Does not exist");
        return;
    }
    std::string output_directory = "D:\\test\\temp";
    if (fs::exists(output_directory))
    {
        fs::remove_all(output_directory);
    }

    fs::create_directories(output_directory);
    bool ok = fs::copy_file(wfilename, fs::path(output_directory) /
                                         fs::path(wfilename).filename());
    recursive_extraction(output_directory);

    // auto panel_num = get_panel_num(filename);

    for (auto &p : fs::recursive_directory_iterator(output_directory))
    {
    if (p.is_directory())
    {
      int panel_num;
      bool top;
      if (get_panel_num(p.path().filename().wstring(), panel_num, top))
      {
        auto content_path = p.path() / std::to_string(panel_num);
        if (!fs::exists(content_path))
          continue;
        int num_buffers = 0;
        int num_grabbers = 0;
        QStringList list = getAllFolderName(QString::fromStdWString(((LPCTSTR)content_path.c_str())));
        foreach(auto str, list)
        {
            if(str.toInt() !=0)
                num_grabbers += 1;
        }

        fs::directory_iterator end_iter;
        for (fs::directory_iterator iter(content_path/"1"); iter!=end_iter; ++iter)
        {
            if (fs::is_regular_file(iter->status()))
            {
                if(iter->path().extension().string() == ".jpeg")
                    num_buffers += 1;
            }
        }
        QStringList res;
        QString path = QString::fromStdWString(((LPCTSTR)content_path.c_str()));
        GetAllFormatFilePath(path, res, "bmScaleMask.gif");
        QString outPath="";
        QString job="";
        QStringList splitList = res[0].split("/");
        if(splitList.count()>7)
           job = splitList[splitList.count()-3]+"/"+splitList[splitList.count()-2];
        outPath = outPutDir + QString("/%1/").arg(panel_num) + job;
        outPath.replace("\\", "/");
        if(dir.mkpath(outPath))
            ui->textEdit->append(QString("create dir %1 success").arg(outPath));
        else
            ui->textEdit->append(QString("create dir %1 fail").arg(outPath));
        QFile::copy(res[0],outPath+"/"+"bmScaleMask.gif");
        outputCSV(job, path, panel_num, top);
        path += QString("/Composed_serv_%1.jpeg").arg(panel_num);
//        fs::remove(content_path/fmt::format("Composed_serv_{}.jpeg",panel_num));
        outPath += QString("/Composed_serv_%1.jpeg").arg(panel_num);
        copyImage(path, content_path.string(), outPath, top, num_buffers, num_grabbers);


      }
    }
    }
    return;
  }



void Widget::recursive_extraction(std::string const &output_directory)
{
  int const max_passes = 10;
  std::set<fs::path> completed_decs = {};
  int pass = 0;

  while (++pass <= max_passes) {
    for (auto &p : fs::recursive_directory_iterator(output_directory)) {
      auto contents = p.path();
      if (p.is_regular_file()) {
        auto ext = contents.extension();
        if (ext == ".zip" || ext == ".7z" || ext == ".rar") {
          // if already done, then skip
          if (completed_decs.find(contents) != completed_decs.end())
            continue;

          fs::path output = contents.parent_path() /
                            contents.filename().replace_extension("");
          std::wstring const command = L"7z x -y -o\"" + output.wstring() +
                                       L"\" \"" + contents.wstring() + L"\"";
          auto ret = _wsystem(command.c_str());
          fmt::print("Returned {}", ret);
          completed_decs.insert(contents);
        }
      }
    }
  }
}

bool Widget::get_panel_num(std::wstring const &dir_name, int &panel_num, bool &top)
{
    panel_num = -1;
    std::wregex pattern(LR"(.*?([0-9]{1,3})_(Top|Bot).*)");
    std::wsmatch sm;
    if (std::regex_search(dir_name, sm, pattern)) {
    panel_num = std::stoi(sm[1].str());
    if (sm[2].str() == L"Top")
      top = true;
    else if (sm[2].str() == L"Bot")
      top = false;
    else
      return false;
    return true;
    }
    return false;
}



QStringList Widget::getAllFolderName(QString path)
{
    QDir dir(path);
    QStringList mFloderNames;
    if (!dir.exists()) mFloderNames = QStringList("");
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    mFloderNames = dir.entryList();
//    for (int i = 0; i < mImgNames.size(); ++i)
//    {
//        qDebug() << "entryList: " << i << "-" << mImgNames[i];
//    }
    return mFloderNames;
}


void Widget::copyImage(QString& path , std::string panelPath, QString& out, bool top, int num_buffers, int num_grabbers)
{


    //判断image是否存在
    if(QFile::exists(path))
    {
        QFile::copy(path, out);
    }
    else
    {
        create_composed(top, panelPath, num_grabbers, num_buffers, out.toStdString());
    }

}



void Widget::outputCSV(QString const& job, QString const& path, int const& panel, bool top)
{

    QFile data("D:/job_panel.csv");
    if(!data.exists())
    {
        //第一次打开
        if(data.open(QFile::WriteOnly))        // 打开文件
        {
            QTextStream out(&data);    // 输入流
            out <<"job"<<","<<"panel"<<","<<"panel path"<<","<<"top";
            out <<"\n";
        }
        data.close();
    }
    data.setFileName("D:/job_panel.csv");
    if(data.open(QFile::WriteOnly | QFile::Append))
    {
        QTextStream out(&data);    // 输入流
        out<<job<<","<<panel<<","<<path<<","<<(top?"top":"bot");
        out <<"\n";
    }
    data.close();
}



QStringList Widget::getFileNames(const QString &path)
{
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.zip" << "*.rar"<<"*.7z";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable);
    return files;
}


void Widget::on_pushButton_clicked()
{
    if(ui->outPutDirLineEdit->text().isEmpty())
    {
        ui->textEdit->append("Please input and output file directory");
        return;
    }
    filename = ui->lineEdit->text().toStdString();
    outPutDir = ui->outPutDirLineEdit->text();
    emit startThread(filename, outPutDir);
//    unpackZip();
}

void Widget::on_pushButton_2_clicked()
{
    if(ui->outPutDirLineEdit->text().isEmpty())
    {
        ui->textEdit->append("Please input and output file directory");
        return;
    }
    QStringList fileNames = getFileNames(ui->lineEdit_2->text());
    outPutDir = ui->outPutDirLineEdit->text();
    foreach(auto name, fileNames)
    {
        filename = ui->lineEdit_2->text().toStdString() + "/" + name.toStdString();
        emit startThread(filename, outPutDir);
//        unpackZip();
    }
}

void Widget::on_pushButton_3_clicked()
{
//    std::vector<Mat> Img;
//    Mat result, result1, result2, result3;
//    filename = ui->lineEdit->text().toStdString();
//    boost::locale::generator gen;
//    auto loc = gen.generate("zh-CN.UTF-8");


//    for(int y=1;y<=3;++y)
//    {
//        for(int x=1;x<=25;++x)
//        {
//            std::string temp = fmt::format("{}/{}/imScale{}.jpeg",filename,y,x);
//            std::wstring imgPath = boost::locale::conv::to_utf<wchar_t>(temp, loc);
//            if(!fs::exists(fs::path(imgPath).string()))
//                qDebug()<<"not exits";
//            auto content_path = fs::path(temp).filename();
//            Mat img = imread(fs::path(imgPath).string());
//            Img.push_back(img);
//        }

//        if(y==1)
//            hconcat(Img, result1);
//        else if(y==2)
//            hconcat(Img, result2);
//        else
//            hconcat(Img, result3);
//        Img.clear();
//    }

//    Img.push_back(result1);
//    Img.push_back(result2);
//    Img.push_back(result3);


//    vconcat(Img, result); //垂直方向拼接
//    imwrite("D:/result.jpeg", result);
    QString filename = QFileDialog::getOpenFileName(this, "choose zip file", "D:/", "file(*.*);;zip file(*.zip);; rar file(*.rar);; 7z file(*.7z))");
    ui->lineEdit->setText(filename);
}

void Widget::on_pushButton_4_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this, "choose dir", "D:/");
    ui->lineEdit_2->setText(dirName);
}

void Widget::on_pushButton_5_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this, "choose dir", "D:/");
    ui->outPutDirLineEdit->setText(dirName);
}


void Widget::getMessage(QString const& msg)
{
    ui->textEdit->append(msg);
}
