#include "One_digit_finder.h"

#include <QByteArray>
#include <QDebug>
#include <algorithm>

One_digit_finder::One_digit_finder(QString filePath, QObject* parent)
    : QObject(parent),
      number{0}

{
  setFilePath(filePath);
}

One_digit_finder::~One_digit_finder() {
  if (file.isOpen()) file.close();
  std::for_each(threads.begin(), threads.end(), [&](std::thread& t) {
    if (t.joinable()) t.join();
  });
  if (tempThread.joinable()) tempThread.join();
}

void One_digit_finder::setFilePath(QString filePath) {
  file.setFileName(filePath);
  qDebug() << "One_digit_finder::setFilePath(QString filePath)"
           << file.errorString();
}

int One_digit_finder::getNumber() const { return number; }

int One_digit_finder::findIt_SingleThread(const int block_size) {
  try {
    if (!file.open(QFile::OpenModeFlag::ReadOnly)) {
      throw(QString("could not open the file"));
    }
    QByteArray buff{file.readAll()};
    while (buff.size() > 1) {
      QByteArray temp;
      for (int i{0}; i <= buff.size() / block_size; i++) {
        temp.append(findSmallBlock(
            buff.mid(i * block_size, (i * (block_size + 1) >= buff.size()
                                          ? (i * (block_size + 1)) % block_size
                                          : block_size))));
      }
      buff = temp;
    }
    number = buff.toInt();
    return number;
  } catch (const QString e) {
    std::runtime_error(e.toStdString());
  }
  return 0;
}

int One_digit_finder::findIt_MultiThread(const int block_size) {
  try {
    if (!file.open(QFile::OpenModeFlag::ReadOnly)) {
      throw(QString("could not open the file"));
    }

    tempThread = std::thread([&, this]() {
      QByteArray buff{file.readAll()};
      while (buff.size() > 1) {
        QByteArray temp;
        for (int i{0}; i <= buff.size() / block_size; i++) {
          threads.push_back(std::thread([&, i]() {
            this->addToByteArray(
                temp,
                this->findSmallBlock(buff.mid(
                    i * block_size, (i * (block_size + 1) >= buff.size()
                                         ? (i * (block_size + 1)) % block_size
                                         : block_size))));
          }));
        }
        std::for_each(threads.begin(), threads.end(), [&](std::thread& t) {
          if (t.joinable()) t.join();
        });
        threads.clear();
        buff = temp;
      }
      number = buff.toInt();
      emit this->numberCalculated(number);
    });
  } catch (const QString e) {
    std::runtime_error(e.toStdString());
  }

  return number;
}

QByteArray One_digit_finder::findSmallBlock(QByteArray buffer) {
  while (buffer.size() > 1) {
    int sum{0};
    for (int i{0}; i < buffer.size(); i++) {
      sum += static_cast<int>(buffer.at(i) - '0');
    }
    buffer = QByteArray::number(sum);
  }
  return buffer;
}

void One_digit_finder::addToByteArray(QByteArray& ba, const QByteArray& add) {
  std::lock_guard<std::mutex> lock(baMutex);
  ba.append(add);
}
