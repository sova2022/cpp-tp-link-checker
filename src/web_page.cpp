#include "web_page.h"

WebPage::WebPage(QObject* parent)
    : QWebEnginePage(parent) {
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
    load_timer_.start(30000);
}

void WebPage::onTimeout() {
    emit loadFinished(false);
}