#include "mainwindow.h"

#include <QDebug>
#include <QLabel>
#include <QObject>
#include <mutex>
#include <thread>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)

{
  ui->setupUi(this);
  //  connect(ui->pushButton, &QPushButton::clicked, this, [&]() {
  //    finder.setFilePath(ui->lineEdit->text());
  //    ui->label->setText(QString::number(finder.findIt_SingleThread()));
  //  });

  connect(ui->pushButton, &QPushButton::clicked, this, [&]() {
    finder.setFilePath(ui->lineEdit->text());
    ui->label->setText(QString::number(finder.findIt_MultiThread()));
  });
  connect(&finder, &One_digit_finder::numberCalculated, ui->label,
          [this](const int number) { ui->label->setNum(number); });
}

MainWindow::~MainWindow() { delete ui; }
