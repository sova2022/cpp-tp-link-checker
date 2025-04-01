#pragma once

#include <QObject>

#include <memory>

#include "url_handler.h"

class UrlsDispenser : public QObject {
	Q_OBJECT
public:
	using UrlsCount = int;

	explicit UrlsDispenser(QStringList urls, UrlsCount count,
		std::unique_ptr<url_handler::UrlHandler> handler,
		QObject* parent = nullptr);

	~UrlsDispenser() = default;

signals:
	void urlsProcessed();

public slots:
	void onFreeForProcessing(const QStringList& failed_to_load_urls);

private:
	bool failed_urls_handle_mode_ = false;
	QStringList urls_;
	QStringList failed_to_load_urls_;
	UrlsCount count_;
	std::unique_ptr<url_handler::UrlHandler> handler_;

	void OnFreeForProcessing(QStringList& urls);
};