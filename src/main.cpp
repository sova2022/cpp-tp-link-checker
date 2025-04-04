#include <QFile>

#include <atomic>
#include <iostream>
#include <csignal>
#include <windows.h>

#include "parser_cli.h"
#include "urls_dispenser.h"

using namespace common;
using namespace parser_cli;

static std::atomic<UrlsDispenser*> global_dispenser = nullptr;

static QStringList ReadUrlsFromFile(const QString& file_path) {
    QStringList url_list;
    QFile file(file_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logger::Log("Error file can't open: " + file_path);
        return url_list;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            url_list.append(line);
        }
    }
    file.close();
    return url_list;
}

static void SaveUrlsToFile(const QStringList& valid_urls, const QString& file_path) {
    if (valid_urls.empty()) return;

    QFile file(file_path.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh.mm.ss").toLocal8Bit()));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString& url : valid_urls) {
            out << url << Qt::endl;
        }
        file.close();
    }
    else {
        logger::Log("Failed to open file for writing: " + file.errorString());
    }
}

int main(int argc, char* argv[]) {
    qInstallMessageHandler(logger::MessageHandler);
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-logging --log-level=3");

    auto app = std::make_unique<QApplication>(argc, argv);
    app->setApplicationName(PROGRAM_NAME);
    app->setApplicationVersion(VERSION);    

    QCommandLineParser parser;
    SetCliParser(parser);
    parser.process(*app);    
    std::cout << "src file path: " << parser.value(option_names::SRC_FILE).toStdString() << std::endl;
    std::cout << "dst file path: " << (parser.value(option_names::DST_FILE)).arg(
                 "yyyy-mm-dd hh.mm.ss").toStdString() << std::endl;

    Timings timings(
        parser.value(option_names::LIMIT_CONNECTION_TIMED).toInt() * 1000,  // convert to milliseconds
        parser.value(option_names::TIMING_FOR_LOGIN_ATTEMPT).toInt() * 1000 // convert to milliseconds
    );
    QStringList valid_urls;
    auto handler = std::make_unique<url_handler::UrlHandler>(valid_urls, timings);
    handler->SetUrlFormat(ALT_URL_FORMAT);

    logger::Log("Started reading from input file");

    QStringList urls = ReadUrlsFromFile(parser.value(option_names::SRC_FILE));

    auto dispenser = std::make_unique<UrlsDispenser>(std::move(urls),
        parser.value(option_names::PAGES_PER_CYCLE).toInt(), std::move(handler));
    global_dispenser.store(dispenser.get());

    QObject::connect(dispenser.get(), &UrlsDispenser::urlsProcessed, [&valid_urls, &parser]() {
            SaveUrlsToFile(valid_urls, parser.value(option_names::DST_FILE));
        });

    logger::Log("Processing urls has been started");

    // let's go
    dispenser->onFreeForProcessing(QStringList());
    
    static auto console_handler = [](DWORD signal) -> BOOL {
        if (signal == CTRL_C_EVENT) {
            std::cout << CTRL_C_DETECTED_INFO << std::endl;
            if (auto* d = global_dispenser.load()) {
                d->GetUrlHander()->StopHandlingUrls();
                emit d->urlsProcessed();
            }
            QApplication::quit();
            return TRUE;
        }
        return FALSE;
        };
    SetConsoleCtrlHandler(console_handler, TRUE);
    return app->exec();
}