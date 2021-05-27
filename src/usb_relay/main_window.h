#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool init();
    void deinit();

    void saveGeometry();
    void loadGeometry();

public slots:
    void relayAttached();
    void relayDetached();
    void relayChanged(int relayNumber);

    void _on_btnRelay_clicked(bool);
    void on_btnTurnOnAll_clicked(bool);
    void on_btnTurnOffAll_clicked(bool);

private:
    void setButtonStyleSheet(QPushButton*);

private:
    Ui::MainWindow *ui;
    QVector<QPushButton*> _buttons;
};

#endif // MAIN_WINDOW_H
