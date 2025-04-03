#include "url_handler.h"

namespace url_handler {

    UrlHandler::UrlHandler(QStringList& valid_urls, Timings timings, QObject* parent)
        : QObject(parent)
        , valid_urls_(valid_urls)
        , timings_(timings) {
    }

    void UrlHandler::operator()(const QString& url) {
        WebPage* page = new WebPage(GetLimitConnectionTimed());
        connect(page, &WebPage::loadFinished, this, [this, page, url](bool ok) {
            onLoadFinished(page, url, ok);
            });

        page->load(QUrl(url_format_.arg(url)));
        pages_.append(page);
    }

    int UrlHandler::GetLimitConnectionTimed() {
        return timings_.limit_connection_timed;
    }

    void UrlHandler::SetUrlFormat(const QString& url_format) {
        url_format_ = url_format;
    }

    void UrlHandler::StopHandlingUrls() {
        disconnect();
        for (auto page : pages_) {
            page->deleteLater();
        }
        pages_.clear();
    }

    void UrlHandler::onLoadFinished(WebPage* page, const QString& url, bool ok) {
        page->StopTimeOutTimer();
        if (!ok) {
            logger::Log("Failed to load: " + url);
            RemovePage(page);

            failed_to_load_urls_ << url;
            return;
        }

        page->toHtml([this, page, url](const QString& html) {
            AnalyzeHtml(page, url, html);
            });
    }

    void UrlHandler::AnalyzeHtml(WebPage* page, const QString& url, const QString& html) {
        if (html.contains("<title>")) {
            QString script;
            if (html.contains("pcPassword")) {
                script = LOGIN_VARS.at(Key::LOGIN).arg(PASSWORD);
            }
            else {
                script = LOGIN_VARS.at(Key::LOGIN_ALT).arg(PASSWORD);
            }
            CheckLoginAttempt(page, url, script);
        }
        else {
            logger::Log("Unknown version html: " + page->url().toString());
            RemovePage(page);

        }
    }

    void UrlHandler::RemovePage(WebPage* page) {
        pages_.removeOne(page);
        if (pages_.empty()) {
            emit processingUrlsFinished(failed_to_load_urls_);
        }
        page->deleteLater();
    }

    void UrlHandler::CheckLoginAttempt(WebPage* page, const QString& url, const QString& script) {
        
        QTimer* timer = new QTimer(this);
        timer->setSingleShot(true);

        connect(timer, &QTimer::timeout, this, [this, page, url, timer]() {
            timer->deleteLater();
            page->runJavaScript(JS_CODE_CHECK_USER_CONFLICT_SITUATION, [this, page, url](const QVariant& result) {
                QString errorMsg = result.toString();
                if (errorMsg.isEmpty()) {
                    logger::Log("Wrong password: " + url);
                }
                else {
                    logger::Log("Log In failure: " + url + " " + result.toString());
                    valid_urls_.append(url + " " + result.toString());
                }
                RemovePage(page);
                });            
            
            });

        timer->start(timings_.timing_login_attempt);

        connect(page, &QWebEnginePage::urlChanged, this, [this, page, url, timer](const QUrl& new_url) {
            timer->stop();
            timer->deleteLater();
            if (new_url.toString().contains("index")) {
                valid_urls_.append(url);
                logger::Log("Log In Success: " + url);
            }
            RemovePage(page);
            });

        page->runJavaScript(script);
    }

} // namespace url_handler