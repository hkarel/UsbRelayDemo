#include "main_window.h"

#include "shared/container_ptr.h"
#include "shared/utils.h"
#include "shared/steady_timer.h"
#include "shared/logger/logger.h"
#include "shared/logger/config.h"
#include "shared/logger/format.h"
#include "shared/config/appl_conf.h"
#include "shared/config/logger_conf.h"
#include "shared/qt/logger_operators.h"
#include "shared/qt/version_number.h"

#include "usb_relay.h"

#include <QApplication>
#include <stdlib.h>
#include <unistd.h>

#define APPLICATION_NAME "UsbRelay Demo"

using namespace std;

void stopLog()
{
    alog::logger().flush();
    alog::logger().waitingFlush();
    alog::logger().stop();
}

void stopProgram()
{
    //db::firebird::pool().close();

    usb::relay().stop();

    log_info << log_format("'%?' is stopped", APPLICATION_NAME);
    stopLog();
}


int main(int argc, char *argv[])
{
    // Устанавливаем в качестве разделителя целой и дробной части символ '.',
    // если этого не сделать - функции преобразования строк в числа (std::atof)
    // буду неправильно работать.
    qputenv("LC_NUMERIC", "C");

    int ret = 0;
    try
    {
        alog::logger().start();

#ifdef NDEBUG
        alog::logger().addSaverStdOut(alog::Level::Info, true);
#else
        alog::logger().addSaverStdOut(alog::Level::Debug2);
#endif

        // Путь к основному конфиг-файлу
        QString configFile = config::qdir() + "/usb_relay_demo.conf";
        if (QFile::exists(configFile))
        {
            if (!config::base().readFile(configFile.toStdString()))
            {
                stopLog();
                return 1;
            }
        }
        else
        {
            QFile file;
            QByteArray conf;

            // Создаем демонстрационные конфиг-файлы
            file.setFileName("://usb_relay_demo.conf");
            file.open(QIODevice::ReadOnly);
            conf = file.readAll();

            if (!config::base().readString(conf.toStdString()))
            {
                stopLog();
                return 1;
            }
        }
        config::base().setReadOnly(true);
        config::base().setSaveDisabled(true);

        QString configFileS;
#ifdef MINGW
        config::base().getValue("state.file_win", configFileS);
#else
        config::base().getValue("state.file", configFileS);
#endif
        config::dirExpansion(configFileS);
        config::state().readFile(configFileS.toStdString());

        // Создаем дефолтный сэйвер для логгера
        if (!alog::configDefaultSaver())
        {
            stopLog();
            return 1;
        }

        log_info << log_format(
            "'%?' is running (version: %?; gitrev: %?)",
            APPLICATION_NAME, productVersion().toString(), GIT_REVISION);
        alog::logger().flush();

        //alog::logger().removeSaverStdOut();
        //alog::logger().removeSaverStdErr();

        alog::printSaversInfo();

        QApplication appl {argc, argv};

        appl.setApplicationDisplayName(APPLICATION_NAME);
        appl.setApplicationVersion(VERSION_PROJECT);

        if (!usb::relay().init(/*{0, 1}*/))
        {
            stopProgram();
            return 1;
        }
        usb::relay().start();

        MainWindow mw;

        chk_connect_q(&usb::relay(), &usb::Relay::attached,
                      &mw, &MainWindow::relayAttached)
        chk_connect_q(&usb::relay(), &usb::Relay::detached,
                      &mw, &MainWindow::relayDetached)
        chk_connect_q(&usb::relay(), &usb::Relay::changed,
                      &mw, &MainWindow::relayChanged)
        chk_connect_q(&usb::relay(), &usb::Relay::failChange,
                      &mw, &MainWindow::relayFailChange)
        mw.init();
        mw.loadGeometry();
        mw.show();

        ret = appl.exec();

        mw.saveGeometry();
        mw.deinit();

        config::state().save();
    }
    catch (YAML::ParserException& e)
    {
        log_error << "YAML error. Detail: " << e.what();
    }
    catch (std::exception& e)
    {
        log_error << "Failed initialization. Detail: " << e.what();
        ret = 1;
    }
    catch (...)
    {
        log_error << "Failed initialization. Unknown error";
        ret = 1;
    }

    stopProgram();
    return ret;
}
