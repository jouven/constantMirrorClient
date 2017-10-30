//program initial config reading structs/classes, the config is read from a json configuration file
#ifndef CMC_MIRRORCONFIG_HPP
#define CMC_MIRRORCONFIG_HPP

#include "baseClassQtso/baseClassQt.hpp"
#include "fileHashQtso/fileHashQt.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QHostAddress>
//#include <QHash>
#include <QSet>

#include <chrono>
#include <vector>
#include <unordered_map>
//#include <unordered_set>

struct fileStatusArrayPlusHostInfo_s : public fileStatusArray_s
{
    QString hostStr_pub;
    quint16 port_pub = 0;
public:
    fileStatusArrayPlusHostInfo_s
    (
            const std::unordered_map<std::string, fileStatus_s>& fileStatusUMAP_par_con
            , const QString& hostStr_par_con
            , const quint16 port_par_con
    );

    void read_f(const QJsonObject &json);
    void write_f(QJsonObject &json) const;
};

struct downloadInfo_s
{
    QString source_pub;
    QString destination_pub;
    //This is not used yet, TODO decide if will be used
    uint_fast64_t size_pub = 0;
    downloadInfo_s(
            const QString& source_par_con
            , const QString& destination_par_con
            , const uint_fast64_t size_par_con
    ) : source_pub(source_par_con)
      , destination_pub(destination_par_con)
      , size_pub(size_par_con)
    {}
};

class mirrorConfigSourceDestinationMapping_c : public eines::baseClassQt_c
{
    int_fast64_t id_pri = 0;

    //serialized/deserialized fields BEGIN

    //the remote "local path"
    //when using single file source/destination, the sourcePath must match exactly an item of the source list server (absolute file path)
    QString originalSourcePath_pri;
    QString sourcePath_pri;
    //the ip/dns to which to connect string
    QString sourceAddressStr_pri;
    //the ip/dns to which to connect
    QHostAddress sourceAddress_pri;

    //the request server port to connect
    quint16 sourceRequestPort_pri = 0;
    //the download server port to connect
    quint16 sourceDownloadPort_pri = 0;
    //the local path
    QString originalDestinationPath_pri;
    QString destinationPath_pri;
    //get filtered files, ignore those which do not match
    QStringList filenameFilters_pri;
    bool includeSubdirectories_pri = true;
    //for directory paths, source wise only grab files from those with a named file
    QString includeDirectoriesWithFileX_pri;
    //for directory paths, copy every source, be it on the root or on a subdirectory, to the destination root path, not creating any subdirectory on the destination
    bool noSubdirectoriesInDestination_pri = false;

    //"source"/"destination" how often to check local files for changes
    qint64 localCheckIntervalMilliseconds_pri = 1000;
    //"destination" only how often to request the list from the "source" server and check for file changes
    //although the server will try to notify the changes to the clients that connected previously, if they disconnected or the server goes down...
    //just check the server again, the timeout of this period will be refreshed if the server notifies the client first
    //mandatory
    qint64 remoteCheckIntervalMilliseconds_pri = 5000;

    //TODO implementar
    bool syncDeletions_pri = true;
    //otherwise rename first, copies and if successful then deletes the renamed
    bool deleteThenCopy_pri = false;


    int_fast64_t localLastCheckedIntervalMilliseconds_pri = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    int_fast64_t remoteLastCheckedIntervalMilliseconds_pri = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

    //serialized/deserialized fields END

    qint64 gcdWaitMilliseconds_pri = 0;

    //when dealing with a single file mapping
    //local
    uint_fast64_t localHash_pri = 0;
    uint_fast64_t localLastModificationTime_pri = 0;

    //when dealing with directory mapping
    //key = localPath
    std::unordered_map<std::string, fileStatus_s> localFileStatusUMAP_pri;
    //key = "sourcePath"
    std::unordered_map<std::string, fileStatus_s> remoteFileStatusUMAP_pri;

    //saves wich remote directories hold file X when includeDirectoriesWithFileX_pri is not empty
    QSet<QString> directoryContainsFileX_pri;

    //this might fail i hope QString can be used here
    std::vector<downloadInfo_s> filesToDownload_pri;
    int_fast32_t currentDownloadCount_pri = 0;
    int_fast32_t maxDownloadCount_pri = 5;

    bool remoteScanThreadExists_pri = false;

    //control variables
    bool initialLocalScanSet_pri = false;

    bool initialRemoteScanSet_pri= false;

    bool remoteHasUpdated_pri = false;

    bool compareLocalAndRemoteThreadExists_pri = false;

    bool compareRequired_pri = false;
    //bool downloadRequired_pri = false;

    //temp variable to read the json, since I'm not sure it will be read all at once
    QByteArray destinationJSONByteArray_pri;

    //result of isValid_f
    bool isValid_pri = false;

    //variable to track is the paths consist of just a single file or a folder (the files in the folder)
    bool isSingleFile_pri = false;
    bool isSingleFileSet_pri = false;

    bool remoteTriedOnce_pri = false;

public:
    mirrorConfigSourceDestinationMapping_c();

    void read_f(const QJsonObject &json);
    void write_f(QJsonObject &json) const;
    void setRemoteHasUpdated_f();
    QString sourcePath_f() const;
    QString destinationPath_f() const;
    quint16 sourceRequestPort_f() const;
    quint16 sourceDownloadPort_f() const;
    QString getIncludeDirectoriesWithFileX_f() const;
    qint64 localCheckIntervalMilliseconds_f() const;
    qint64 remoteCheckIntervalMilliseconds_f() const;
    int_fast32_t currentDownloadCount_f() const;
    bool isValid_f() const;

    void checkValid_f();

    qint64 gcdWaitMilliseconds_f() const;

    void localScan_f();
    void checkRemoteFiles_f();
    void compareLocalAndRemote_f();
    void compareLocalAndRemoteThread_f();
    void download_f();

    QHostAddress sourceAddress_f() const;
    bool remoteTriedOnce_f() const;
    void printRemoteFileList_f() const;
};

class mirrorConfig_c : public eines::baseClassQt_c
{
    //serialized/deserialized fields BEGIN

    //TODO, AFTER IT WORKS, make them optional and just use the defaults, printing, after, what the defaults are after the server/s is up
    //ip-dns interface to use (always mandatory)
    QString selfServerAddressStr_pri;

    //update server port (mandatory for destinations)
    quint16 updateServerPort_pri = 0;

    std::vector<mirrorConfigSourceDestinationMapping_c> sourceDestinationMappings_pri;

    bool tryPrintRemoteFileListOnceAndQuit_pri = false;

    //serialized/deserialized fields END

    QHostAddress selfServerAddress_pri;

    bool localScanThreadExists_pri = false;

    qint64 gcdWaitMillisecondsAll_pri = 0;

    int_fast32_t maxDownloadCountGlobal_pri = 10;

    //holds the result of checkValid_f
    bool valid_pri = false;

    bool allRemotesTriedOnce_f() const;
    void printAllRemoteFileLists_f() const;
public:
    void read_f(const QJsonObject &json);
    void write_f(QJsonObject &json) const;
    quint16 updateServerPort_f() const;
    uint_fast32_t maxDownloadCountGlobal_f() const;
    //std::vector<mirrorConfigSourceDestinationMapping_c> sourceDestinationMappings_f() const;
    void checkValid_f();

    void initialSetup_f();

    void localScan_f();
    void checkRemoteFiles_f();
    void compareLocalAndRemote_f();
    void mainLoop_f();
    void downloadFiles_f();
    uint_fast32_t maxCurrentDownloadCount_f() const;
    void setRemoteHasUpdated_f(const QHostAddress& address_par_con, const quint16 port_par_con);
};

extern mirrorConfig_c mirrorConfig_ext;

#endif // MIRRORCONFIG_HPP
