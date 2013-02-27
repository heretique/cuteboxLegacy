#include <QMap>

#include "genfileiconprovider.h"
#include "genresourcereader.h"

static QMap<QString, QString> initTypesAndIcons()
{
    QMap<QString, QString> map;

    map.insert("folder", "folder");
    map.insert("mp3", "music");
    map.insert("mp4", "music");
    map.insert("ogg", "music");
    map.insert("aac", "music");
    map.insert("wma", "music");
    map.insert("flac", "music");
    map.insert("jpg", "page_white_picture");
    map.insert("jpeg", "page_white_picture");
    map.insert("gif", "page_white_picture");
    map.insert("png", "page_white_picture");
    map.insert("tiff", "page_white_picture");
    map.insert("tga", "page_white_picture");
    map.insert("svg", "page_white_vector");
    map.insert("ai", "page_white_vector");
    map.insert("bmp", "page_white_picture");
    map.insert("pdf", "page_white_acrobat");
    map.insert("zip", "package");
    map.insert("rar", "package");
    map.insert("7z", "package");
    map.insert("gz", "package");
    map.insert("tar", "package");
    map.insert("bz", "package");
    map.insert("arj", "package");
    map.insert("c", "page_white_c");
    map.insert("cc", "page_white_c");
    map.insert("cpp", "page_white_cplusplus");
    map.insert("c++", "page_white_cplusplus");
    map.insert("cs", "page_white_csharp");
    map.insert("java", "page_white_code");
    map.insert("jar", "page_white_code");
    map.insert("h", "page_white_h");
    map.insert("txt", "page_white_text");
    map.insert("log", "page_white_text");
    map.insert("swf", "page_white_flash");
    map.insert("php", "page_white_php");
    map.insert("xls", "page_white_excel");
    map.insert("ppt", "page_white_powerpoint");
    map.insert("doc", "page_white_word");
    map.insert("docx", "page_white_word");

    return map;
}

static const QMap<QString, QString> TypesAndIcons = initTypesAndIcons();

GenFileIconProvider::GenFileIconProvider()
{

}

GenFileIconProvider::~GenFileIconProvider()
{

}

QIcon GenFileIconProvider::icon(const QFileInfo &info) const
{
    if (info.isDir() || info.isRoot())
        return GenResourceReader::iconFromResource("folder");

    return GenResourceReader::iconFromResource(iconNameFromExtension(info.suffix()));
}

QString GenFileIconProvider::iconNameFromExtension(QString extension) const
{
    QString iconName =  TypesAndIcons.value(extension.toLower(), QString(""));
    if (iconName.isEmpty())
        iconName = "page_white";
    return iconName;
}
