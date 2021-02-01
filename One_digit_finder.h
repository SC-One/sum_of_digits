#ifndef ONE_DIGIT_FINDER_H
#define ONE_DIGIT_FINDER_H

#include <QFile>
#include <QObject>
#include <QString>
#include <mutex>
#include <thread>
#include <vector>

class One_digit_finder : public QObject {
  Q_OBJECT
 public:
  One_digit_finder() = default;
  explicit One_digit_finder(QString filePath, QObject *parent = nullptr);
  ~One_digit_finder();

  void setFilePath(QString filePath);

 signals:
  void numberCalculated(const int value);

 public slots:
  int getNumber() const;
  int findIt_SingleThread(const int block_size = 100);
  int findIt_MultiThread(const int block_size = 9999);

 private:
  QByteArray findSmallBlock(QByteArray buffer);
  void addToByteArray(QByteArray &ba, const QByteArray &add);

 private:
  int number;
  QFile file;
  std::thread tempThread;
  std::vector<std::thread> threads;
  std::mutex baMutex;
};

#endif  // ONE_DIGIT_FINDER_H
