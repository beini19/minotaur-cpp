#include "serialbox.h"
#include "ui_serialbox.h"

#include "../utility/utility.h"
#include "../utility/font.h"

#include <QMessageBox>
#include <QSerialPort>
#include <QSerialPortInfo>

SerialBox::SerialBox(
    const std::shared_ptr<Solenoid> &solenoid,
    QWidget *parent
) :
    QDialog(parent),
    ui(new Ui::SerialBox),
    m_solenoid(solenoid),
    m_status(SerialStatus::DISCONNECTED) {
    ui->setupUi(this);
    ui->baud_combo_box->setEditable(false);

    // Set console font of serial display
    ui->serial_display->setFont(qfont::courier(8));
    ui->serial_display->setReadOnly(true);

    // Populate combo boxes
    auto baud_rates = {
        QSerialPort::Baud1200,
        QSerialPort::Baud2400,
        QSerialPort::Baud4800,
        QSerialPort::Baud9600,
        QSerialPort::Baud19200,
        QSerialPort::Baud38400,
        QSerialPort::Baud57600,
        QSerialPort::Baud115200
    };
    for (auto baud_rate : baud_rates) {
        ui->baud_combo_box->addItem(QString::number(baud_rate), QVariant(baud_rate));
    }
    int index = ui->baud_combo_box->findData(QVariant(QSerialPort::Baud9600));
    ui->baud_combo_box->setCurrentIndex(index);

    // Serial ports
    auto serial_ports = QSerialPortInfo::availablePorts();
    for (auto &serial_port : serial_ports) {
        ui->serial_combo_box->addItem(serial_port.portName());
    }

    // Set status
    if (m_solenoid->serial_port().isOpen()) {
        m_status = SerialStatus::CONNECTED;
    }
    update_status(m_status);

    // Setup sliders
    setup_power_slider(*(ui->up_slider));
    setup_power_slider(*(ui->down_slider));
    setup_power_slider(*(ui->left_slider));
    setup_power_slider(*(ui->right_slider));
    setup_slider(*(ui->delay_slider), Delay::DELAY_DEFAULT, Delay::DELAY_MIN, Delay::DELAY_MAX, Delay::DELAY_INTERVAL);

    connect(m_solenoid.get(), &Solenoid::serial_status, this, &SerialBox::update_status);
    connect(ui->disconnect_button, &QPushButton::clicked, m_solenoid.get(), &Solenoid::attempt_disconnect);
    connect(ui->connect_button, &QPushButton::clicked, this, &SerialBox::attempt_connection);
    connect(ui->up_slider, &QSlider::valueChanged, m_solenoid.get(), &Solenoid::change_up_power);
    connect(ui->down_slider, &QSlider::valueChanged, m_solenoid.get(), &Solenoid::change_down_power);
    connect(ui->left_slider, &QSlider::valueChanged, m_solenoid.get(), &Solenoid::change_left_power);
    connect(ui->right_slider, &QSlider::valueChanged, m_solenoid.get(), &Solenoid::change_right_power);
    connect(ui->delay_slider, &QSlider::valueChanged, m_solenoid.get(), &Solenoid::change_delay);

    // Close SerialBox when Cancel is clicked
    connect(ui->button_box->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &QDialog::close);
}

SerialBox::~SerialBox() {
    delete ui;
}

void SerialBox::attempt_connection() {
    QString serial_port = ui->serial_combo_box->currentText();
    if (serial_port.isEmpty()) { return; }
    auto baud_rate = static_cast<QSerialPort::BaudRate>(
        ui->baud_combo_box->currentData().toInt()
    );
    m_solenoid->attempt_connection(serial_port, baud_rate);
}

void SerialBox::update_status(SerialStatus new_status) {
    m_status = new_status;
    QPalette palette = ui->serial_status->palette();
    Qt::GlobalColor color;
    switch (m_status) {
        case SerialStatus::CONNECTED:
            ui->serial_status->setText("CONNECTED");
            color = Qt::blue;
            break;
        case SerialStatus::DISCONNECTED:
            ui->serial_status->setText("DISCONNECTED");
            color = Qt::red;
            break;
        default:
            ui->serial_status->setText("CONNECTING");
            color = Qt::yellow;
            break;
    }
    palette.setColor(ui->serial_status->foregroundRole(), color);
    ui->serial_status->setPalette(palette);
}

void SerialBox::append_text(const std::string &text) {
    auto display = ui->serial_display;
    display->moveCursor(QTextCursor::End);
    display->insertPlainText(QString::fromStdString(text));
    display->moveCursor(QTextCursor::End);
}

void SerialBox::setup_power_slider(QSlider &slider) {
    setup_slider(slider, 255, Power::POWER_MIN, Power::POWER_MAX, Power::POWER_INTERVAL);
}

void SerialBox::setup_slider(QSlider &slider, int value, int min, int max, int interval) {
    slider.setTickInterval(interval);
    slider.setTickPosition(QSlider::TicksBothSides);
    slider.setMaximum(max);
    slider.setMinimum(min);
    slider.setTracking(false);
    slider.setValue(value);
}