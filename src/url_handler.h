#pragma once

#include <QUrl>

#include "common.h"
#include "logger.h"
#include "web_page.h"

namespace url_handler {
    using namespace common;

    class UrlHandler : public QObject {
        Q_OBJECT
    public:
        UrlHandler(QStringList& valid_urls, Timings timings, QObject* parent = nullptr);

        ~UrlHandler() = default;

        void operator()(const QString& url);

        int GetLimitConnectionTimed();
        void SetUrlFormat(const QString& url_format);
        void StopHandlingUrls();

    signals:
        void processingUrlsFinished(const QStringList& failed_to_load_urls);

    private slots:
        void onLoadFinished(WebPage* page, const QString& url, bool ok);

    private:
        QString url_format_;
        QStringList& valid_urls_;
        QVector<WebPage*> pages_;
        QStringList failed_to_load_urls_;
        Timings timings_;

        void AnalyzeHtml(WebPage* page, const QString& url, const QString& html);
        void RemovePage(WebPage* page);
        void CheckLoginAttempt(WebPage* page, const QString& url, const QString& script);        
    };

} // namespace url_handler