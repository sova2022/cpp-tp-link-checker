#pragma once

#include <QCommandLineParser>

namespace parser_cli {

    namespace option_names {

        static constexpr const char* SRC_FILE = "input";
        static constexpr const char* DST_FILE = "output";
        static constexpr const char* SAVE_HTML = "save_html";
        static constexpr const char* PAGES_PER_CYCLE = "pages_per_cycle";

    } // namespace parser_cli::option_names

    void SetCliParser(QCommandLineParser& parser) {
        using namespace option_names;
        parser.setApplicationDescription("TP-Link Checker CLI tool");
        parser.addHelpOption();
        parser.addVersionOption();

        QCommandLineOption input_file(QStringList() << "i" << SRC_FILE,
            "Path to file with IP addresses",
            "Text Document", "data/input.txt");

        QCommandLineOption output_file(QStringList() << "o" << DST_FILE,
            "Path to file for saving valid IP addresses",
            "Text Document", "output/output_%1.txt");

        QCommandLineOption pages_per_cycle(QStringList() << "p" << PAGES_PER_CYCLE,
            "Count of processing pages per one cycle",
            "number", "10");

        parser.addOption(input_file);
        parser.addOption(output_file);
        parser.addOption(pages_per_cycle);
    }

} // namespace parser_cli