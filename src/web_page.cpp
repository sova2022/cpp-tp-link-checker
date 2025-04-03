#include "web_page.h"

WebPage::WebPage(int limit_connection_timed, QObject* parent)
    : QWebEnginePage(parent)
    , limit_connection_timed_(limit_connection_timed) {
    connect(this, &QWebEnginePage::certificateError, this, &WebPage::onCertificateError);
    connect(this, &QWebEnginePage::loadStarted, this, &WebPage::onLoadStarted);
    connect(&load_timer_, &QTimer::timeout, this, &WebPage::onTimeout);
    profile()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
}

WebPage::~WebPage() {
    if (!profile()->isOffTheRecord()) {
        profile()->deleteLater();
    }
    this->deleteLater();
}

void WebPage::StopTimeOutTimer() {
    load_timer_.stop();
    disconnect();
}

void  WebPage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
    const QString& message,
    int lineNumber,
    const QString& sourceID) {
    return;
}

void WebPage::onCertificateError(const QWebEngineCertificateError& error) {
    const_cast<QWebEngineCertificateError&>(error).acceptCertificate();
}

void WebPage::onLoadStarted() {
    load_timer_.start(limit_connection_timed_);
}

void WebPage::onTimeout() {
    emit loadFinished(false);
}