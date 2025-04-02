#pragma once

#include <qDebug>
#include <QDateTime>
#include <QString>

namespace logger {

	static void MessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
		if (msg.contains("non-JS module files deprecated.", Qt::CaseInsensitive)) {
			return;
		}
		QTextStream(stderr) << msg << Qt::endl;
	}

	static void Log(const QString& msg) {
		QString text(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]").toLocal8Bit() + " %1%2");
		qDebug().noquote() << text.arg(msg, ".");
	}

} // namespace logger