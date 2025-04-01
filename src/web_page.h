#pragma once

#include <QApplication>
#include <QTimer>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineCertificateError>

class WebPage : public QWebEnginePage {
    Q_OBJECT
public:
    explicit WebPage(QObject* parent = nullptr);

    ~WebPage() override;

    void StopTimeOutTimer();

protected:
    void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
        const QString& message,
        int lineNumber,
        const QString& sourceID) override;

private slots:
    void onCertificateError(const QWebEngineCertificateError& error);
    void onLoadStarted();
    void onTimeout();

private:
    QTimer load_timer_ = QTimer(this);
};