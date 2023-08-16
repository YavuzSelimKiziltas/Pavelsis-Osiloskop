#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSerialPort>
#include "QString"
#include "QMouseEvent"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    Ui::Widget *ui;
    ~Widget();

private:
    int frequencyValue;
    int amplitudeValue;
    int amplitude, frequency;
    QString dataFromSerialPort;
    QVector<double> xData, y1Data, y2Data;

private slots:

    void read_data();

    void update_graph_with_new_data(double xValue, double y1Value, double y2Value);

    void makePlot1(int graphIndex);
    void makePlot2(int graphIndex);

    void write_to_serial_data();

    void on_horizontalEnableRadio_toggled(bool checked);

    void on_verticalEnableRadio_toggled(bool checked);

    void on_cursor2HorizontalSlider_valueChanged(int value);

    void on_cursor1HorizontalSlider_valueChanged(int value);

    void on_cursor1VerticalSlider_valueChanged(int value);

    void on_cursor2VerticalSlider_valueChanged(int value);

    void draw_vertical_cursor_1(int value);

    void draw_vertical_cursor_2(int value);

    void draw_horizontal_cursor_1(int value);

    void draw_horizontal_cursor_2(int value);

    void on_stopButton_pressed();

    //void on_stopRadio_toggled(bool checked);

    void on_verticalCursorButton_pressed();

    void on_horizontalCursorButton_pressed();

    void on_connectButton_pressed();

    void mousePressEvent(QMouseEvent* event);

    void on_amplitudeDial_sliderReleased();

    void on_frequencyDial_sliderReleased();

    void on_amplitudeDial_valueChanged(int value);

    void on_frequencyDial_valueChanged(int value);

    void on_channel1Button_pressed();

    void on_channel2Button_pressed();

private:

    QSerialPort *COMPORT;
};
#endif // WIDGET_H
