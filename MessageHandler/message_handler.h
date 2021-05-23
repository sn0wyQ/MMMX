#ifndef MESSAGEHANDLER_MESSAGE_HANDLER_H_
#define MESSAGEHANDLER_MESSAGE_HANDLER_H_

#include <QDebug>
#include <QFile>
#include <QTime>

class MessageHandler {
 public:
  MessageHandler(bool enable_ignore_level,
                 QtMsgType ignore_level,
                 const QString& log_filename);

  void Handle(QtMsgType type,
                  const QMessageLogContext& context,
                  const QString& message);

 private:
  bool enable_ignore_level_;
  QtMsgType ignore_level_;
  QFile log_file_;
  static int LevelByMessageType(QtMsgType msg_type);
};

#endif  // MESSAGEHANDLER_MESSAGE_HANDLER_H_
