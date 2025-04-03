#include "urls_dispenser.h"

UrlsDispenser::UrlsDispenser(QStringList urls, UrlsCount count,
	std::unique_ptr<url_handler::UrlHandler> handler,
	QObject* parent)
	: QObject(parent)
	, urls_(std::move(urls))
	, count_(count)
	, handler_(std::move(handler)) {
	connect(handler_.get(), &url_handler::UrlHandler::processingUrlsFinished, this, &UrlsDispenser::onFreeForProcessing);
}

url_handler::UrlHandler* UrlsDispenser::GetUrlHander() {
	return handler_.get();
}

void UrlsDispenser::onFreeForProcessing(const QStringList& failed_to_load_urls) {
	OnFreeForProcessing(urls_);
	if (urls_.empty()) {
		if (!failed_urls_handle_mode_) {
			failed_to_load_urls_ = failed_to_load_urls;
			handler_->SetUrlFormat(common::DEFAULT_URL_FORMAT);
			failed_urls_handle_mode_ = true;
			logger::Log("Switching to failed URLs handling mode");
		}
		OnFreeForProcessing(failed_to_load_urls_);
		if (failed_to_load_urls_.empty()) {
			disconnect(handler_.get(), &url_handler::UrlHandler::processingUrlsFinished, this, &UrlsDispenser::onFreeForProcessing);
			QTimer::singleShot(handler_->GetLimitConnectionTimed(), this, [this]() {
				logger::Log("All URLs has been processed");
				emit urlsProcessed();
				qDebug() << "Press Ctrl+C for exit";
				});
		}
	}
}

void UrlsDispenser::OnFreeForProcessing(QStringList& urls) {
	int count = count_;
	if (count > urls.size()) {
		count = urls.size();
	}
	for (; count != 0; --count) {
		(*handler_)(urls.back());
		urls.pop_back();
	}
}