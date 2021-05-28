#pragma once

#include "shared/defmac.h"
#include "shared/safe_singleton.h"
#include "shared/qt/qthreadex.h"

#include <QtCore>
#include <usb.h>
#include <atomic>

namespace usb {

class Relay : public QThreadEx
{
public:
    bool init(const QVector<int>& states = {});

    // Наименование продукта
    QString product() const;

    // Серийный строковый идентификатор продукта
    QString serial() const;

    // Вектор текущих состояний реле
    QVector<int> states() const;

    // Возвращает количество реле в подключенном устройстве
    int count() const;

    // Возвращает TRUE если устройство подключено
    bool isAttached() const {return _deviceInitialized;}

signals:
    void attached();
    void detached();
    void changed(int relayNumber);

    // Сигнал эмитируется если не удалось изменить состояние реле
    void failChange(int relayNumber, const QString& errorMessage);

public slots:
    //bool toggle(const QVector<int> states);
    bool toggle(int relayNumber, bool value);

private:
    Q_OBJECT
    Relay() = default;
    DISABLE_DEFAULT_COPY(Relay)

    void run() override;
    void threadStopEstablished() override;

    bool captureDevice();
    void releaseDevice();

    int readStates(char* buff, int buffSize);

    QVector<int> statesInternal() const;
    bool toggleInternal(int relayNumber, bool value);

private:
    int _usbBusNumber = {0};
    int _usbDeviceNumber = {0};

    QVector<int> _initStates;

    usb_dev_handle*  _deviceHandle = {nullptr};
    std::atomic_bool _deviceInitialized = {false};
    std::atomic_int  _usbContinuousErrors = {0};
    std::atomic_int  _usbLastErrorCode = {0};

    QString _product;
    QString _serial;
    quint8  _states = {0};
    qint32  _count = {0};

    mutable QMutex _threadLock;
    mutable QWaitCondition _threadCond;

    template<typename T, int> friend T& ::safe_singleton();
};

Relay& relay();

} // namespace usb
