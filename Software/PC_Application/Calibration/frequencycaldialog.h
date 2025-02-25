#ifndef FREQUENCYCALDIALOG_H
#define FREQUENCYCALDIALOG_H

#include "Device/device.h"
#include "modehandler.h"
#include "mode.h"

#include <QDialog>

namespace Ui {
class FrequencyCalDialog;
}

class FrequencyCalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FrequencyCalDialog(Device *dev, ModeHandler *handler, QWidget *parent = nullptr);
    ~FrequencyCalDialog();

private:
    Ui::FrequencyCalDialog *ui;
    Device *dev;
};

#endif // FREQUENCYCALDIALOG_H
