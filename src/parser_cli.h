#pragma once

#include <QCommandLineParser>

namespace parser_cli {

    namespace option_names {

        static constexpr const char* SRC_FILE = "input";
        static constexpr const char* DST_FILE = "output";
        static constexpr const char* PAGES_PER_CYCLE = "pages-per-cycle";
        static constexpr const char* LIMIT_CONNECTION_TIMED = "limit-connection-timed";
        static constexpr const char* TIMING_FOR_LOGIN_ATTEMPT = "timing-login-attempt";

    } // namespace parser_cli::option_names

    void SetCliParser(QCommandLineParser& parser) {
        using namespace option_names;
        parser.setApplicationDescription("TP-Link Checker CLI tool");
        parser.addHelpOption();
        parser.addVersionOption();

        QCommandLineOption input_file(QStringList() << "i" << SRC_FILE,
            "Path to file with IPs",
            "Text Document", "data/input.txt");

        QCommandLineOption output_file(QStringList() << "o" << DST_FILE,
            "Path to file for saving valid IPs",
            "Text Document", "output/output_%1.txt");

        QCommandLineOption pages_per_cycle(QStringList() << "p" << PAGES_PER_CYCLE,
            "Num of processing pages per cycle",
            "numbers", "10");

        QCommandLineOption limit_connection_timed(QStringList() << "l" << LIMIT_CONNECTION_TIMED,
            "Limit connection timed for attempt",
            "seconds", "30");

        QCommandLineOption timing_login_attempt(QStringList() << "t" << TIMING_FOR_LOGIN_ATTEMPT,
            "Timings for login attempt",
            "seconds", "10");

        parser.addOption(input_file);
        parser.addOption(output_file);
        parser.addOption(pages_per_cycle);
        parser.addOption(limit_connection_timed);
        parser.addOption(timing_login_attempt);
    }

} // namespace parser_cli