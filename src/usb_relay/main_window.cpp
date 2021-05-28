#include "main_window.h"
#include "ui_main_window.h"

#include "usb_relay.h"

#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/config/appl_conf.h"
#include "shared/qt/quuidex.h"
#include "shared/qt/logger_operators.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->btnRelay1->setProperty("RelayIndex", 1);
    ui->btnRelay2->setProperty("RelayIndex", 2);
    ui->btnRelay3->setProperty("RelayIndex", 3);
    ui->btnRelay4->setProperty("RelayIndex", 4);
    ui->btnRelay5->setProperty("RelayIndex", 5);
    ui->btnRelay6->setProperty("RelayIndex", 6);
    ui->btnRelay7->setProperty("RelayIndex", 7);
    ui->btnRelay8->setProperty("RelayIndex", 8);

    _buttons.append(ui->btnRelay1);
    _buttons.append(ui->btnRelay2);
    _buttons.append(ui->btnRelay3);
    _buttons.append(ui->btnRelay4);
    _buttons.append(ui->btnRelay5);
    _buttons.append(ui->btnRelay6);
    _buttons.append(ui->btnRelay7);
    _buttons.append(ui->btnRelay8);

    relayDetached();

    for (int i = 0; i < _buttons.count(); ++i)
    {
        chk_connect_a(_buttons[i], &QAbstractButton::clicked,
                      this, &MainWindow::_on_btnRelay_clicked)
    }
    relayAttached();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::init()
{
    return true;
}

void MainWindow::deinit()
{
}

void MainWindow::saveGeometry()
{
    QPoint p = pos();
    QVector<int> v {p.x(), p.y(), width(), height()};
    config::state().setValue("windows.main_window.geometry", v);
}

void MainWindow::loadGeometry()
{
    QVector<int> v {0, 0, 800, 600};
    config::state().getValue("windows.main_window.geometry", v);
    move(v[0], v[1]);
    resize(v[2], v[3]);
}

void MainWindow::relayAttached()
{
    if (!usb::relay().isAttached())
        return;

    ui->labelProdName->setText(usb::relay().product());
    ui->labelSerialStr->setText(usb::relay().serial());
    ui->labelRelayCount->setText(QString::number(usb::relay().count()));

    for (int i = 0; i < _buttons.count(); ++i)
        _buttons[i]->setEnabled(i < usb::relay().count());

    QVector<int> states = usb::relay().states();
    for (int i = 0; i < states.count(); ++i)
    {
        _buttons[i]->setChecked(states[i]);
        setButtonStyleSheet(_buttons[i]);
    }

    ui->btnTurnOnAll->setEnabled(true);
    ui->btnTurnOffAll->setEnabled(true);
}

void MainWindow::relayDetached()
{
    ui->labelProdName->clear();
    ui->labelSerialStr->clear();
    ui->labelRelayCount->clear();

    for (int i = 0; i < _buttons.count(); ++i)
    {
        _buttons[i]->setEnabled(false);
        _buttons[i]->setChecked(false);
        setButtonStyleSheet(_buttons[i]);
    }

    ui->btnTurnOnAll->setEnabled(false);
    ui->btnTurnOffAll->setEnabled(false);
}

void MainWindow::relayChanged(int /*relayNumber*/)
{
    QVector<int> states = usb::relay().states();
    for (int i = 0; i < states.count(); ++i)
    {
        _buttons[i]->setChecked(states[i]);
        setButtonStyleSheet(_buttons[i]);
    }
}

void MainWindow::relayFailChange(int relayNumber, const QString& errorMessage)
{
    QMessageBox::critical(this, "Error", errorMessage);
}

void MainWindow::_on_btnRelay_clicked(bool checked)
{
    int index = sender()->property("RelayIndex").toInt();
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    if (!usb::relay().toggle(index, checked))
        button->setChecked(!checked);
}

void MainWindow::on_btnTurnOnAll_clicked(bool)
{
    usb::relay().toggle(0, true);
}

void MainWindow::on_btnTurnOffAll_clicked(bool)
{
    usb::relay().toggle(0, false);
}

void MainWindow::setButtonStyleSheet(QPushButton* btn)
{
    if (btn->isChecked())
        btn->setStyleSheet("background-color: rgb(183, 226, 178)");
    else
        btn->setStyleSheet("");
}
