#include "register.h"
#include <exception>
#include <QHeaderView>

Register::Register(QString name, int address, int width)
    : QObject(nullptr),
      name(name),
      address(address),
      width(width)
{
    value = 0;
}

void Register::assignUI(QCheckBox *cb, int bitpos, bool inverted)
{
    connect(this, &Register::valueChanged, [=](unsigned int newval) {
        bool bit = newval & (1UL << bitpos);
        if(inverted) {
            bit = !bit;
        }
        cb->setChecked(bit);
    });
    connect(cb, &QCheckBox::toggled, [=](bool checked){
        if(inverted) {
            checked = !checked;
        }
        setValue(checked, bitpos, 1);
    });
}

void Register::assignUI(QComboBox *cb, int pos, int width)
{
    connect(this, &Register::valueChanged, [=]() {
        auto value = getValue(pos, width);
        if(cb->count() > static_cast<int>(value)) {
            cb->setCurrentIndex(value);
        }
    });
    connect(cb, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index){
        setValue(index, pos, width);
    });
}

void Register::assignUI(QSpinBox *sb, int pos, int width)
{
    connect(this, &Register::valueChanged, [=]() {
        auto value = getValue(pos, width);
        sb->setValue(value);
    });
    connect(sb, qOverload<int>(&QSpinBox::valueChanged), [=](int index){
        setValue(index, pos, width);
    });
}

QString Register::hexString()
{
    return "0x" + QString("%1").arg(value, (width-1)/4 + 1, 16, QChar('0'));
}

bool Register::setFromString(QString s)
{
    bool okay;
    auto num = s.toULong(&okay, 0);
    if(okay) {
        setValue(num);
    }
    return okay;
}

unsigned long Register::getValue()
{
    return value;
}

unsigned long Register::getValue(int pos, int width)
{
    unsigned long mask = 0;
    for(int i=0;i<width;i++) {
        mask |= (1UL << i);
    }
    mask <<= pos;
    auto masked = value & mask;
    masked >>= pos;
    return masked;
}

void Register::setValue(unsigned long newval)
{
    setValue(newval, 0, width);
}

void Register::setValue(unsigned long newval, int pos, int width)
{
    unsigned long mask = 0;
    for(int i=0;i<width;i++) {
        mask |= (1UL << i);
    }
    newval &= mask;
    newval <<= pos;
    mask <<= pos;
    auto oldval = value;
    value &= ~mask;
    value |= newval;
    if(newval != oldval) {
        emit valueChanged(value);
    }
}

int Register::getAddress() const
{
    return address;
}

QString Register::getName() const
{
    return name;
}

void Register::fillTableWidget(QTableWidget *l, std::vector<Register *> regs)
{
    l->clear();
    l->setRowCount(regs.size());
    l->setColumnCount(3);
    l->setHorizontalHeaderLabels({"Name", "Address", "Value"});
    l->verticalHeader()->setVisible(false);
    l->horizontalHeader()->setStretchLastSection(true);
//    l->setSortingEnabled(true);
    for(unsigned int i=0;i<regs.size();i++) {
        l->setItem(i, 0, new QTableWidgetItem(regs[i]->getName()));
        l->item(i, 0)->setFlags(l->item(i, 0)->flags() &= ~Qt::ItemIsEditable);
        auto address = "0x" + QString::number(regs[i]->address, 16);
        l->setItem(i, 1, new QTableWidgetItem(address));
        l->item(i, 1)->setFlags(l->item(i, 1)->flags() &= ~Qt::ItemIsEditable);
        auto valueItem = new QTableWidgetItem;
        valueItem->setFlags(valueItem->flags() | Qt::ItemIsEditable);
        valueItem->setData(Qt::DisplayRole, regs[i]->hexString());
        l->setItem(i, 2, valueItem);
        connect(regs[i], &Register::valueChanged, [=]() {
            l->item(i, 2)->setData(Qt::DisplayRole, regs[i]->hexString());
        });
    }
    connect(l, &QTableWidget::cellChanged, [=](int row, int column){
        if(column == 2) {
            auto reg = regs[row];
            QString input = l->item(row, column)->data(Qt::DisplayRole).toString();
            reg->setFromString(input);
            l->item(row, column)->setData(Qt::DisplayRole, reg->hexString());
        }
    });
}

