#define UNICODE
#include "boost/locale.hpp"
#include "boost/tokenizer.hpp"
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ostream.h"
#include "regex"
#include <fstream>
#include "filesystem"
#include <io.h>
#include <QFileInfo>
#include <QDir>
#include "Windows.h"
#include <QDebug>
#include <QTextCodec>


#pragma execution_character_set("utf-8")

namespace fs = std::filesystem;

std::string  filename = "";
QString oldFileName = "";

QString oldStr = "";
int panel = 0;
QList<std::string> unZipList;
std::vector<std::string> topPath;
std::vector<std::string> botPath;

boost::locale::generator gen;
auto loc = gen.generate("zh-CN.UTF-8");


std::vector<std::string> readDestPath(QString path)
{
    QFile file(path);
    std::vector<std::string> strList;
    if(file.open(QIODevice::ReadOnly))
    {
        while(!file.atEnd())
        {
             QString str = file.readLine();
             QStringList list = str.split(QRegExp("Source--"));
             if(list.count()<=1) continue;
//             std::cout<<list[1].toStdString();
             strList.push_back(list[1].replace("\r\n","").toStdString());
        }
    }
    file.close();
    return strList;
}



int get_filenames(fs::path path, std::string top, int I)
{
    if (!fs::exists(path)) {
    std::cout << "Does not exist"<<std::endl;
    return -1;
    }
    std::string dest;
    if(top == "top")
    {
        int i = std::stoi(path.filename().string());
        dest = topPath[i];
    }
    else
    {

        int i = std::stoi(path.filename().string());
        dest = botPath[i];

    }
//    if (!fs::exists(dest))
//        fs::create_directories(dest);

    fs::directory_iterator end_iter;
    for (fs::directory_iterator iter(path); iter!=end_iter; ++iter)
    {
        if (fs::is_regular_file(iter->status()))
        {
            if(iter->path().extension().string() == ".dat")
            {
                QString temp = QString::fromStdString(iter->path().filename().string());
                int n = temp.replace("imFile", "").replace(".dat", "").trimmed().toInt();
                std::wstring name = boost::locale::conv::to_utf<wchar_t>(dest + "\\imfile" + std::to_string(50*I+n) +".dat", loc);
                bool ok = fs::copy_file(iter->path().wstring(), name, fs::copy_options::overwrite_existing);
                if(!ok)
                {
                    std::wcout<<iter->path().wstring()<<"->"<<name<<"fail"<<std::endl;
                }
            }
            else
            {
                QString temp = QString::fromStdString(iter->path().filename().string());
                int n = temp.replace("imScale", "").replace(".jpeg", "").trimmed().toInt();
                std::wstring name = boost::locale::conv::to_utf<wchar_t>(dest + "\\imScale" + std::to_string(50*I+n) +".jpeg", loc);
                bool ok = fs::copy_file(iter->path().wstring(), name, fs::copy_options::overwrite_existing);
                if(!ok)
                {
                    std::wcout<<iter->path().wstring()<<"->"<<name<<"fail"<<std::endl;
                }
            }


        }
//        if (fs::is_directory(iter->status()))
//        {
//            get_filenames(iter->path(), top, I);
//        }
    }
    return 0;

}



// check if filename is panel
bool get_panel_num(std::wstring const &dir_name, int &panel_num, bool &top) {
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



void recursive_extraction(std::string const &output_directory) {
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


void getAllFiles(std::string path, std::vector<std::string>& files) {
    //文件句柄
    long hFile = 0;
     //文件信息
     struct _finddata_t fileinfo;  //很少用的文件信息读取结构
     std::string p;  //string类很有意思的一个赋值函数:assign()，有很多重载版本
     if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) != -1) {
         do {
              if ((fileinfo.attrib & _A_SUBDIR)) {  //比较文件类型是否是文件夹
                if (strcmp(fileinfo.name,".") != 0 && strcmp(fileinfo.name,"..") != 0) {
                     files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                     getAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
                 }
             } else {
                 files.push_back(p.assign(path).append("\\").append(fileinfo.name));
             }
         } while (_findnext(hFile, &fileinfo) == 0);  //寻找下一个，成功返回0，否则-1
         _findclose(hFile);
     }
 }

void create_new_table(QString path)
{
    QList<std::string> listStr;
    std::string str;
    //打开test.csv文件,并过滤内容
    QFile file(path);
    if(file.open(QIODevice::ReadOnly))
    {
        int n = -1;
        while (!file.atEnd())
        {
            str = file.readLine().toStdString();
            str.pop_back();
            QStringList temp = QString::fromStdString(str).trimmed().split(QRegExp(","));

            if(temp.count()<7)return;
//            qDebug()<<temp[3].toInt()<<temp[4].toInt()<<temp[5].toDouble()<<temp[6].toInt();
            if(temp[3].toInt() == 15834 && temp[4].toInt() == 3 && temp[5].toDouble() == 0.6 && temp[6].toInt() == 8)
            {
                if(temp[0] == oldStr && temp[1].toInt() == panel)
                {

                }
                else if(temp[0] != oldStr)
                {
                    int pos = str.find(',', 0);
                    std::string	newStr1 = str.substr(0, pos);
                    unZipList.append(newStr1);
                    n += 1;
                }

                else
                {
                    n +=1;
                }
                str = str + "," + std::to_string(n);
                listStr.append(str);
                oldStr = temp[0];
                panel = temp[1].toInt();
            }
        }

    }

    file.close();
    //生成一个新的csv文件
    file.setFileName("D:/new.csv");
    if(file.exists()) file.remove();
    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        foreach(auto str, listStr)
        {

            file.write(str.data(), str.length());
            file.write("\n");

        }
    }
    file.close();
}


//获取指定文件夹下的所有文件夹
QStringList getAllFolderName(QString path)
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




void find_files(const fs::path& dir,const std::string& filename,std::vector<fs::path>& v)
{

    if (!fs::exists(dir) || !fs::is_directory(dir)) return;

    fs::recursive_directory_iterator end;

    for (fs::recursive_directory_iterator pos(dir);pos!=end;++pos)
    {
        if (!fs::is_directory(*pos)/* && pos->path().filename().string() == filename*/)
        {
            v.push_back(pos->path());
        }
    }

}

//size_t copy_files(const fs::path& from_dir,const fs::path& to_dir,const std::string& filename = "*")
//{

//    std::cout<<from_dir<<to_dir;
//    if (!fs::is_directory(from_dir))
//    {
//        std::cout <<"args is not a dir." << std::endl;
//        return 0;
//    }

//    std::cout << "prepare for copy,please wait..."<< std::endl;

//    std::vector<fs::path> v;

//    find_files(from_dir,filename,v);

//    if (v.empty())
//    {
//        std::cout << "0 file copied." << std::endl;

//        return 0;
//    }

//    std::cout << "now begin copy files ..." << std::endl;

//    fs::path tmp;

//    foreach(auto &p,v)
//    {
//        tmp = to_dir / p.string().substr(from_dir.string().length());

//        if (!fs::exists(tmp.parent_path()))
//        {
//            fs::create_directories(tmp.parent_path());
//        }

//        fs::copy_file(p,tmp);

//    }

//    std::cout << v.size() << "file copied." << std::endl;

//    return v.size();
//}



//拷贝文件夹：
bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if(!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
        if(!targetDir.mkdir(targetDir.absolutePath()))
            return false;
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir()){    /**< 当为目录时，递归的进行copy */
            if(!copyDirectoryFiles(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()),
                coverFileIfExist))
                return false;
        }
        else{            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
            if(coverFileIfExist && targetDir.exists(fileInfo.fileName())){
                targetDir.remove(fileInfo.fileName());
            }

            /// 进行文件copy
            if(!QFile::copy(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()))){
                    return false;
            }
        }
    }
    return true;

}



void outputCSV(std::string path, int panel, bool top, int ccdconfig, int ccdnumber, double cad_res, double img_res)
{

    QFile data("D:/test.csv");
    if(!data.exists())
    {
        //第一次打开
        if(data.open(QFile::WriteOnly | QFile::Append))        // 打开文件
        {
            QTextStream out(&data);    // 输入流
            out <<"FileNamae"<<","<<"Number"<<","<<"Slide"<<","<<"CCDConfig"<<","<<"CCDNumber"<<","<<"Cad Resolution"<<","<<"Img Resolution";
            out << "\n";
        }
        data.close();
    }
    QFileInfo info(QString::fromStdString(path));
    QString name = info.fileName();
//    QStringList list = name.split(QRegExp("\\."+info.suffix()));
//    if(list.count()<=0)
//    {
//        std::cout<<"获取文件名字失败";
//        return;
//    }
    if(data.open(QFile::WriteOnly | QFile::Append))        // 打开文件
    {
        QTextStream out(&data);    // 输入流
        out <<name<<","<<panel<<","<<(top ? "top" : "bot")<<","<<ccdconfig<<","<<ccdnumber<<","<<cad_res<<","<<img_res;
        out << "\n";
    }
    data.close();

}


int m_run()
{
    boost::locale::generator gen;
    auto loc = gen.generate("zh-CN.UTF-8");
    std::wstring wfilename = boost::locale::conv::to_utf<wchar_t>(filename, loc);

    if (!fs::exists(wfilename)) {
    std::cout << "Does not exist"<<std::endl;
    return -1;
    }
    std::string output_directory = (fs::temp_directory_path() / "Test").string();
    if (fs::exists(output_directory))
    fs::remove_all(output_directory);

    fs::create_directories(output_directory);
    bool ok = fs::copy_file(wfilename, fs::path(output_directory) /
                                         fs::path(wfilename).filename());

    recursive_extraction(output_directory);

    // auto panel_num = get_panel_num(filename);

    for (auto &p : fs::recursive_directory_iterator(output_directory)) {
    if (p.is_directory()) {
      int panel_num;
      bool top;
      if (get_panel_num(p.path().filename().wstring(), panel_num, top)) {
        auto content_path = p.path() / std::to_string(panel_num);
        if (!fs::exists(content_path))
          continue;

        /// Capture info
        int ccdconfig;
        auto const ccdconfigfile = fs::path(content_path) / "ccdconfig.txt";

        int ccdnumber;
        auto const ccdnumberfile = fs::path(content_path) / "ccdnumber.txt";

        if (fs::exists(ccdconfigfile)) {
          std::ifstream ifs(ccdconfigfile);
          assert(ifs.good());
          ifs >> ccdconfig;
        }

        if (fs::exists(ccdnumberfile)) {
          std::ifstream ifs(ccdnumberfile);
          assert(ifs.good());
          ifs >> ccdnumber;
        }

        double cad_res;
        double img_res;
        auto const machinesettingsfile =
            fs::path(content_path) / "MachineSettings.txt";
        if (fs::exists(machinesettingsfile)) {
          std::ifstream ifs(machinesettingsfile);
          assert(ifs.good());
          std::string str;
          int line_num = 0;
          while (std::getline(ifs, str)) {
            if (line_num > 1)
              break;
            boost::char_separator<char> sep(", ");
            boost::tokenizer<boost::char_separator<char>> toks(str, sep);
            std::vector<std::string> tokens(toks.begin(), toks.end());

            if (line_num == 0)
              cad_res = std::stod(tokens[2]);
            if (line_num == 1)
              img_res = std::stod(tokens[2]);
            ++line_num;
          }
        }
        std::cout << "\n";
        std::cout << "------------------------------------\n";
        fmt::print(L"Filename: {}", L"zzz");
        fmt::print("Panel number : {}, Side : {}\n", panel_num,
                   top ? "top" : "bot");
        fmt::print("CCDConfig : {}\nCCDNumber: {}\nCad Resolution:{}\nImg "
                   "Resolution:{}\n",
                   ccdconfig, ccdnumber, cad_res, img_res);
        std::cout << "------------------------------------\n";
        outputCSV(filename, panel_num, top, ccdconfig, ccdnumber, cad_res, img_res);

      }
    }
    }
    return 0;
}



bool AllisNum(std::string str)
{
    for (int i = 0; i < str.size(); i++)
    {
        int tmp = (int)str[i];
        if (tmp >= 48 && tmp <= 57)
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}


std::string GbkToUtf8(const char *src_str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    std::string strTemp = str;
    if (wstr) delete[] wstr;
    if (str) delete[] str;
    return strTemp;
}


int main(int, char **)
{

    std::cout<<"Please enter zip or rar or 7z file path"<<std::endl;
    while(true)
    {
        char m[200];
        std::cin.getline(m, 200);
        filename = GbkToUtf8(m);
        if(filename == "q" || filename == "Q")
            break;
        m_run();
    }


    return 0;


    topPath = readDestPath("C:/services/Slave_SourcesDestinations_top.txt");
    botPath = readDestPath("C:/services/Slave_sourcesDestinations_bot.txt");

    try {

    create_new_table("D:/test.csv");

    std::string output_directory = "D:\\test\\temp";
    int I = -1;

    foreach(auto str, unZipList)
    {
        filename = "D:\\WYDownload\\" + str;
        std::wstring wfilename = boost::locale::conv::to_utf<wchar_t>(filename, loc);
        if (!fs::exists(wfilename)) {
        std::cout << "zip file Does not exist"<<std::endl;
        continue;
        }
        std::cout<<filename<<std::endl;
        int oldpanel = 0;

        if (fs::exists(output_directory))
        fs::remove_all(output_directory);

        fs::create_directories(output_directory);

        bool ok = fs::copy_file(wfilename, fs::path(output_directory) /
                                       fs::path(wfilename).filename());
        if(!ok)
        {
            std::cout<<"copy file fail";
            continue;
        }
        recursive_extraction(output_directory);

        for (auto &p : fs::recursive_directory_iterator(output_directory)) {
        if (p.is_directory()) {
          int panel_num;
          bool top;
          if (get_panel_num(p.path().filename().wstring(), panel_num, top)) {
            auto content_path = p.path() / std::to_string(panel_num);
            if (!fs::exists(content_path))
              continue;

            if(panel_num != oldpanel)
                I += 1;
            oldpanel = panel_num;
            std::string temp = std::to_string(panel_num) + "_" + (!top ? "top":"bot");
            //找到单面板，并复制文件
            if(!fs::exists(p.path().parent_path()/temp/std::to_string(panel_num)))
            {
//                std::cout<<p.path().parent_path()/temp/std::to_string(panel_num);
                QStringList list = getAllFolderName(QString::fromStdWString(((LPCTSTR)content_path.c_str())));
                foreach(auto str, list)
                {
                    if(str.toInt() != 0)
                    {
                        std::string t = (!top ? "top":"bot");
                        get_filenames(fs::path(content_path.wstring() + L'\\' + str.toStdWString()), t, I);
                    }
                }
            }
            QStringList list1 = getAllFolderName(QString::fromStdWString(((LPCTSTR)content_path.c_str())));
            foreach(auto str, list1)
            {
                if(str.toInt() != 0)
                {
                    std::string t = (top ? "top":"bot");
                    get_filenames(fs::path(content_path.wstring() + L'\\' + str.toStdWString()), t, I);
                }
                else
                {
                    copyDirectoryFiles(QString::fromStdWString(((LPCTSTR)content_path.c_str()))+"/" + str, QString("D:/jobs/") + str, true);

                }
            }
//            for (auto &subp : fs::recursive_directory_iterator(content_path))
//            {
//                if(subp.is_directory())
//                {
//                    if(AllisNum(subp.path().filename().string()))
//                    {
//                        std::string t = top ? "top":"bot";
//                        get_filenames(subp.path(), t, I);

//                    }
//                }
//            }

          }
        }
      }
   }
    }
    catch(std::exception e)
    {
     std::cout<<e.what();
    }
}

