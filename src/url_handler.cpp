#include "url_handler.h"

namespace url_handler {

    UrlHandler::UrlHandler(QStringList& valid_urls, QObject* parent)
        : QObject(parent)
        , valid_urls_(valid_urls) {
    }

    void UrlHandler::operator()(const QString& url) {
        WebPage* page = new WebPage();
        connect(page, &WebPage::loadFinished, this, [this, page, url](bool ok) {
            onLoadFinished(page, url, ok);
            });

        page->load(QUrl(url_format_.arg(url)));
        pages_.append(page);
    }

    void UrlHandler::SetUrlFormat(const QString& url_format) {
        url_format_ = url_format;
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
            page->runJavaScript(script, [this, page, url](const QVariant& result) {
                if (result.toBool() == true) {
                    CheckLoginAttempt(page, url);
                }
                else {
                    logger::Log("Unknown version html: " + page->url().toString());
                    RemovePage(page);

                }
                });
        }
        else {
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

    void UrlHandler::CheckLoginAttempt(WebPage* page, const QString url) {
        QTimer::singleShot(10000, this, [this, page, url]() {
            page->toHtml([this, page, url](const QString& html) {
                if (html.contains("Logout")) {
                    valid_urls_.append(url);
                    logger::Log("Log In Success: " + url);
                }
                else {
                    logger::Log("Wrong password: " + url);
                }
                RemovePage(page);
                });
            });
    }

} // namespace url_handler