//20170914

//client/"destination": the program has a configuration of several remote file/directory paths mapped to local file/directory paths
//the program can request a list (hash+size) of remote file/directories paths per mapping to a serer/"source" program,
//this list it's tailored on the server/"source" program and client/"destination" will always recieve the same no matter the client side paths configurations,
//except if the server configuration is changed but that doesn't depende on the client
//client/"destination" also has a tcp server to recieve requests telling if anything has changed from the server/"source" program
//so it can request the list again to check for changes.

//right now it's command-line-always-running-process

#include "mirrorConfig.hpp"

#include "signalso/signal.hpp"

#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "essentialQtso/essentialQt.hpp"

#include <QCoreApplication>
#include <QTimer>

#include <thread>

#ifdef DEBUGJOUVEN
#include <iostream>
#endif


namespace
{

}

int main(int argc, char *argv[])
{
    MACRO_signalHandler

//    //"idle-waiting" thread to tell qt event loop to quit when a signal happens
//    eines::signal::launchThread_f([]()
//    {
//        while (eines::signal::isRunning_f())
//        {
//            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//#ifdef DEBUGJOUVEN
//            //std::cout << "qthreads counter " << QThreadCount_f() << std::endl;
//#endif
//        }
//        //quit qt the last thing, this includes this thread and "thread for the qt stuff"
//        while (eines::signal::threadCount_f() > 2 or QThreadCount_f() > 0)
//        {
//            std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        }
//        QCoreApplication::exit();
//    });

    //thread for the main program qt stuff
    eines::signal::launchThread_f([&]()
    {
        QCoreApplication qtapp(argc, argv);
        qtAppRef_ext = &qtapp;

        QTimer::singleShot(0, &qtapp, std::bind(&mirrorConfig_c::initialSetup_f, &mirrorConfig_ext));
        qtCycleRef_ext = new QTimer(&qtapp);
        QObject::connect(qtCycleRef_ext, &QTimer::timeout, qtCycleRef_ext, std::bind(&mirrorConfig_c::mainLoop_f, &mirrorConfig_ext));

        returnValue_ext = qtapp.exec();
    });

    while (not eines::signal::isTheEnd_f())
    {}

    return returnValue_ext;
}
