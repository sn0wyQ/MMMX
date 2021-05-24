#include "message_handler.h"

MessageHandler::MessageHandler(bool enable_ignore_level,
                               QtMsgType ignore_level,
                               const QString& log_filename) :
    enable_ignore_level_{enable_ignore_level},
    ignore_level_{ignore_level},
    log_file_(log_filename) {
  if (!log_file_.isOpen()) {
    log_file_.open(QIODevice::WriteOnly | QIODevice::Append);
  }
}

void MessageHandler::Handle(QtMsgType type,
                            const QMessageLogContext& context,
                            const QString& message) {
  QString txt;
  if (enable_ignore_level_) {
    if (LevelByMessageType(type) > LevelByMessageType(ignore_level_)) {
      switch (type) {
        case QtDebugMsg:
          txt = QString("Debug: %1").arg(message);
          break;

        case QtInfoMsg:
          txt = message;
          break;

        case QtWarningMsg:
          txt = QString("Warning: %1").arg(message);
          break;

        case QtCriticalMsg:
          txt = QString("Critical: %1").arg(message);
          break;

        case QtFatalMsg:
          txt = QString("Fatal: %1").arg(message);
          break;
      }
    }
  } else {
    txt = message;
  }
  if (txt.isEmpty()) {
    return;
  }

  if (LevelByMessageType(type) <= LevelByMessageType(QtInfoMsg)) {
    QTextStream(stdout) << txt << Qt::endl;
  } else {
    QTextStream(stderr) << txt << QString(". At %2:%3")
        .arg(context.file)
        .arg(context.line) << Qt::endl;
  }
}

int MessageHandler::LevelByMessageType(QtMsgType msg_type) {
  switch (msg_type) {
    case QtDebugMsg: return 0;
    case QtInfoMsg: return 1;
    case QtWarningMsg: return 2;
    case QtCriticalMsg: return 3;
    case QtFatalMsg: return 4;
    default: return 0;
  }
}
