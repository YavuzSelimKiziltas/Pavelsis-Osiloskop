#include "widget.h"
#include "ui_widget.h"
#include <QSerialPortInfo>                      // Otomatik olarak müsait seri portları listelemekte kullanılacak

/*
 *  Qt Version 5.15.2
 *  Qt Creator 11.0.0
 *  Kit MSVC2015 64 bit
 *  QCustomPlot Version 2.1.0
*/

const int AMPLITUDE_MIN = 1;                    // min genlik belirtilir
const int AMPLITUDE_MAX = 10;                   // max genlik belirtilir
const int FREQUENCY_MIN = 1;                    // min frekans belirtilir
const int FREQUENCY_MAX = 100;                  // max frekans belirtilir

#define SLIDER_MIN 0                            // min slider değeri (Horizontal ve Vertical Slider için)
#define SLIDER_MAX 100                          // max slider değeri (Horizontal ve Vertical Slider için)

// min ve max x - y degerleri hesaplanir. Başlangıç değerleri daha sonra güncelleniyor
double minX = 0.0f;
double maxX = 5.0f;
double minY = 0.0f;
double maxY = 5.0f;

// Default Positions of Vertical and Horizontal Cursors
#define HORIZONTAL_CURSOR_X 40
#define HORIZONTAL_CURSOR_Y1 40
#define HORIZONTAL_CURSOR_Y2 500

#define VERTICAL_CURSOR_Y 40
#define VERTICAL_CURSOR_X1 40
#define VERTICAL_CURSOR_X2 660

// QCustomPlot için Boy ve genişlik değeri
#define HEIGHT 460
#define WIDTH 620

/* customPlot'ta birden fazla grafik eklemek için index numarası gerekiyor */
static int graph1Index = 0;
static int graph2Index = 0;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    /* Amplitude degerini ayarlayan widgeti slota bagliyoruz ve default ayarlarini yüklüyoruz
    sliderReleased yerine sliderValueChanged kullanilirsa hizli veri transferinden dolayi
    buffer overflow oluyor. Bu durum frequencyDial icinde gecerli
    */
    connect(ui->amplitudeDial, &QDial::sliderReleased, this, &Widget::on_amplitudeDial_sliderReleased);
    // min ve max genlik degeri set edilir
    ui->amplitudeDial->setMinimum(AMPLITUDE_MIN);
    ui->amplitudeDial->setMaximum(AMPLITUDE_MAX);
    // baslangıc genlik ve frekans degerleri set edilir        // INTEGER
    amplitude = AMPLITUDE_MIN; // Baslangıcta amplitude degeri minimum degerle baslasin
    ui->amplitudeDial->setValue(static_cast<int>(amplitude));

    connect(ui->frequencyDial, &QDial::sliderReleased, this, &Widget::on_frequencyDial_sliderReleased);
    // min ve max genlik degeri set edilir
    ui->frequencyDial->setMinimum(FREQUENCY_MIN);
    ui->frequencyDial->setMaximum(FREQUENCY_MAX);
    // baslangıc genlik ve frekans degerleri set edilir        // INTEGER
    frequency = FREQUENCY_MIN; // Baslangıcta amplitude degeri minimum degerle baslasin
    ui->frequencyDial->setValue(static_cast<int>(frequency));

    // Set min and max slider values
    ui->cursor1HorizontalSlider->setMinimum(SLIDER_MIN);
    ui->cursor1HorizontalSlider->setMaximum(SLIDER_MAX);
    ui->cursor2HorizontalSlider->setMinimum(SLIDER_MIN);
    ui->cursor2HorizontalSlider->setMaximum(SLIDER_MAX);

    ui->cursor1VerticalSlider->setMinimum(SLIDER_MIN);
    ui->cursor1VerticalSlider->setMaximum(SLIDER_MAX);
    ui->cursor2VerticalSlider->setMinimum(SLIDER_MIN);
    ui->cursor2VerticalSlider->setMaximum(SLIDER_MAX);

    /* Ilk basta cursor ile ilgili slider label ve diger baglantili elemanlari görünmez
    yapıyoruz Vertical Cursor veya Horizontal Cursor acildigi zaman ilgili elemanlar da
    tekrardan görünür duruma  geliyor.
    */
    ui->cursor1HorizontalSlider->setVisible(0);
    ui->cursor2HorizontalSlider->setVisible(0);
    ui->cursor1VerticalSlider->setVisible(0);
    ui->cursor2VerticalSlider->setVisible(0);

    ui->verticalLine1->setVisible(0);
    ui->verticalLine2->setVisible(0);
    ui->horizontalLine1->setVisible(0);
    ui->horizontalLine2->setVisible(0);

    ui->horizontalLabel1->setVisible(0);
    ui->horizontalLabel2->setVisible(0);
    ui->verticalLabel1->setVisible(0);
    ui->verticalLabel2->setVisible(0);

    // Red label sadece kırmızı bir nokta çiziyor ekrana
    ui->redLabel->setVisible(0);
    ui->pointLabel->setVisible(0);


    // Cursorların rengini ayarlıyoruz
    ui->verticalLine1->setStyleSheet("background-color: rgb(255, 175, 5);");
    ui->verticalLine2->setStyleSheet("background-color: rgb(255, 175, 5);");
    ui->horizontalLine1->setStyleSheet("background-color: rgb(14, 171, 231);");
    ui->horizontalLine2->setStyleSheet("background-color: rgb(14, 171, 231);");

    // Slider hareket ettigi zaman cursor'da onunla birlikte hareket ediyor
    connect(ui->cursor1HorizontalSlider, &QSlider::sliderMoved, this, &Widget::draw_vertical_cursor_1);
    connect(ui->cursor2HorizontalSlider, &QSlider::sliderMoved, this, &Widget::draw_vertical_cursor_2);
    connect(ui->cursor1VerticalSlider, &QSlider::sliderMoved, this, &Widget::draw_horizontal_cursor_1);
    connect(ui->cursor2VerticalSlider, &QSlider::sliderMoved, this, &Widget::draw_horizontal_cursor_2);


    // Baslangicta müsait olan portları bularak comboBox'a ekliyoruz basta QSerialPortInfo kütüphanesini eklemeyi unutma
    QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : availablePorts) {
        ui->serialPortComboBox->addItem(portInfo.portName());
    }

    /* Grafik Cizim Ayarlari
    iRangeDrag ile ekranı saga sola sürüklenebilir hale getiriyoruz
    iRangeZoom ile zoom yapma özelligini aktif hale getiriyoruz
    iSelectItems ise daha sonradan bir datapoint'e tiklayabilmemizi sagliyor
    */
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems);
    // Sadece x ekseninde drag ve zoom yapabilmemizi sağlıyor (Durumsal olarak gerekliydi)
    ui->customPlot->axisRects().at(0)->setRangeDrag(Qt::Horizontal);
    ui->customPlot->axisRects().at(0)->setRangeZoom(Qt::Horizontal);

    // x eksenindeki gridi acar ve gri renkte gösterir
    ui->customPlot->xAxis->grid()->setSubGridVisible(true);
    ui->customPlot->xAxis->grid()->setSubGridPen(QPen(Qt::gray, 1, Qt::DotLine));

    // y eksenindeki gridi acar ve gri renkte gösterir
    ui->customPlot->yAxis->grid()->setSubGridVisible(true);
    ui->customPlot->yAxis->grid()->setSubGridPen(QPen(Qt::gray, 1, Qt::DotLine));

    // 40 pixel kenar margini ekler (güzel görünmesi icin)
    ui->customPlot->plotLayout()->setMargins(QMargins(40, 40, 40, 40));
    ui->customPlot->axisRect()->setAutoMargins(QCP::MarginSide::msNone);
    ui->customPlot->axisRect()->setMargins(QMargins(0, 0, 0, 0));
    ui->customPlot->axisRect()->setupFullAxesBox();
    ui->customPlot->xAxis->setLabel("TIME");
    ui->customPlot->yAxis->setLabel("VOLTAGE");

}

// Calles the deconstructor to delete the ui when window is closed
Widget::~Widget()
{
    delete ui;
}


void Widget::on_channel1Button_pressed()
{
    static bool isClicked = 1;
    // toggles the channel1Radio button. Later checked in update_graph_with_new_data
    ui->channel1Radio->setChecked(!(ui->channel1Radio->isChecked()));

    if(isClicked) {
        graph1Index = ui->customPlot->graphCount();  // gets the graph current count
        ui->customPlot->addGraph();
        qDebug() << "Open Channel 1, Index = " << graph1Index;
        makePlot1(graph1Index);
        ui->channel1Button->setStyleSheet("background-color: rgb(255, 107, 243);");
    }

    else {
        qDebug() << "Destroy Channel 1";

        /* Check the documentation. Basically shifts other graph's index
        so that there is no out of index error
        */
        if(graph1Index == 0 && ui->customPlot->graphCount() == 2) {
            graph2Index = 0;
        }

        qDebug() << "Removing Index = " << graph1Index;
        ui->customPlot->removeGraph(graph1Index);
        ui->customPlot->replot();
        ui->channel1Button->setStyleSheet("background-color: white;");
    }
    // Whenever button is pressed isClicked is toggled so that we can check On/Off condition
    isClicked = !isClicked;
}


void Widget::on_channel2Button_pressed()
{
    static bool isClicked2 = 1;
    // toggles the channel1Radio button. Later checked in update_graph_with_new_data
    ui->channel2Radio->setChecked(!(ui->channel2Radio->isChecked()));

    if(isClicked2) {
        graph2Index = ui->customPlot->graphCount();   // gets the number of graph
        ui->customPlot->addGraph();
        qDebug() << "Open Channel 2, Index = " << graph2Index;
        makePlot2(graph2Index);
        ui->channel2Button->setStyleSheet("background-color: rgb(0, 107, 243)");
    }

    else {
        qDebug() << "Destroy Channel 2";

        /* Check the documentation. Basically shifts other graph's index
        so that there is no out of index error
        */
        if(graph2Index == 0 && ui->customPlot->graphCount() == 2) {
            graph1Index = 0;
        }

        qDebug() << "Removing Index = " << graph2Index;
        ui->customPlot->removeGraph(graph2Index);
        ui->customPlot->replot();
        ui->channel2Button->setStyleSheet("background-color: white;");
    }

    // Whenever button is pressed isClicked is toggled so that we can check On/Off condition
    isClicked2 = !isClicked2;
}

/* moves the cursor with slider value is the pixel value */
void Widget::draw_horizontal_cursor_1(int value)
{
    ui->horizontalLine1->move(HORIZONTAL_CURSOR_X, HORIZONTAL_CURSOR_Y1 + value);
    ui->horizontalLabel1->move(50, 20 + value);     // Original position of the label is (50,20)
}

/* moves the cursor with slider value is the pixel value */
void Widget::draw_horizontal_cursor_2(int value)
{
    // value is pixel value. Subtract because it is going down
    ui->horizontalLine2->move(HORIZONTAL_CURSOR_X, HORIZONTAL_CURSOR_Y2 - value);
    ui->horizontalLabel2->move(600, 480 - value);     // Original position of the label is (600,480)
}

/* moves the cursor with slider value is the pixel value */
void Widget::draw_vertical_cursor_1(int value)
{
    // value is pixel value
    ui->verticalLine1->move(VERTICAL_CURSOR_X1 + value, VERTICAL_CURSOR_Y);
    ui->verticalLabel1->move(50 + value, 480);     // Original position of the label is (50,480)
}

/* moves the cursor with slider value is the pixel value */
void Widget::draw_vertical_cursor_2(int value)
{
    // value is pixel value. Subtract because it is going left
    ui->verticalLine2->move(VERTICAL_CURSOR_X2 - value, VERTICAL_CURSOR_Y);
    ui->verticalLabel2->move(600 - value, 50);     // Original position of the label is (600,480)
}

// Reads the values from connected COMPORT
// Read data is in x,y1,y2 format
// y1 is for plot 1 and y2 is for plot 2 x is the same for all of them
void Widget::read_data()
{
    // x ve y alınacak
    if (COMPORT->isOpen())                              // bu slot sadece seri port aciksa aktiflesir
    {
        while (COMPORT->bytesAvailable())               // COMPORT'tan veri alindiysa burasi 1 olarak donecek
        {
            dataFromSerialPort += COMPORT->readAll();  // porttan alinan tüm veri Data_FromSerialPort degiskenine eklenir

            if (dataFromSerialPort.endsWith('\n'))     // Data_FromSerialPort degiskenindeki veiler \n ile bitiyorsa bu blok calisir
            {                                           // yani; sürekli gelen mesaj paketleri \n ler ile birbirinden ayirt edilir.

                QStringList dataList = dataFromSerialPort.split(',');  // mesaj paketinde gelen x ve y verileri , ile birbirinden ayrilir.
                if (dataList.size() >= 3)                               // En az 2 veri gelmiş mi diye kontrol edilir
                {
                    bool xConversionOk, y1ConversionOk, y2ConversionOk;       // gelen x ve y verilerinin dogrulugunu kontrol icin kullanilir

                    // gelen dataları double degiskene parslama islemi yapilir
                    // datalar dataList adindaki arraye aktarilir

                    double xValue  = dataList[0].toDouble(&xConversionOk);    // QcustomPlot uses double type
                    double y1Value = dataList[1].toDouble(&y1ConversionOk);   // that's why we convert to double
                    double y2Value = dataList[2].toDouble(&y2ConversionOk);

                    if (xConversionOk && y1ConversionOk && y2ConversionOk)
                    {
                        update_graph_with_new_data(xValue, y1Value, y2Value);     // double olarak alinan x ve y degerleri plotlanmak uzere diger slota aktarilir

                        //qDebug() << "(DOUBLE) x value: " << xValue << ", y1 value: " << y1Value << ", y2 value: " << y2Value;
                        // x ve y degerleri ayri ayri ekrana yazdirilir
                    }
                    else
                    {
                        //qDebug() << "Conversion is not properly done!";
                    }
                }
                else
                {
                    //qDebug() << "Geçersiz veri formatı!";
                }

                //qDebug() << "Data from Serial Port: " << dataFromSerialPort;
                ui->receivedDataText->setPlainText(dataFromSerialPort);

                // bu mesaj paketiyle isimiz bitti, \n'den sonraki, yani diger pakete gecmeden once datalari tuttugumuz degisken temizlenir
                dataFromSerialPort.clear();
                break;
            }
        }
    }
}

void Widget::update_graph_with_new_data(double xValue, double y1Value, double y2Value)
{
    // yeni veri grafige eklenir
    xData.push_back(xValue);
    y1Data.push_back(y1Value);
    y2Data.push_back(y2Value);

    // this is for debug purposes
    ui->receivedDataText->setPlainText(QString("(%1 , %2, %3)").arg(xValue).arg(y1Value).arg(y2Value));

    // vektör boyutlari kontrol edilir ve eger cok fazlaysa ilk veriler silinir
    if (xData.size() > 1000)
    {
        xData.remove(0);
        y1Data.remove(0);
        y2Data.remove(0);
    }

    // Her gelen data ile birlikte acik olan kanaldaki grafik güncellenir!!!
    if(!(ui->stopRadio->isChecked())) {

        // if both channel is open plot both
        if(ui->channel1Radio->isChecked() && ui->channel2Radio->isChecked()) {
            makePlot1(graph1Index);
            makePlot2(graph2Index);

            // compares the min and max values of both plot and determines the yRange
            double y1Min = *std::min_element(y1Data.constBegin(), y1Data.constEnd());
            double y1Max = *std::max_element(y1Data.constBegin(), y1Data.constEnd());
            double y2Min = *std::min_element(y2Data.constBegin(), y2Data.constEnd());
            double y2Max = *std::max_element(y2Data.constBegin(), y2Data.constEnd());

            double minY = (y1Min < y2Min) ? y1Min : y2Min;
            double maxY = (y1Max > y2Max) ? y1Max : y2Max;
            ui->customPlot->yAxis->setRange(minY - 1, maxY + 1);    // 1 is forbetter visualization
        }

        // if only channel 1 is open then plot channel 1
        else if(ui->channel1Radio->isChecked() && !(ui->channel2Radio->isChecked())) {
            makePlot1(graph1Index);
            minY = *std::min_element(y1Data.constBegin(), y1Data.constEnd());
            maxY = *std::max_element(y1Data.constBegin(), y1Data.constEnd());
            ui->customPlot->yAxis->setRange(minY - 1, maxY + 1);    // 1 is forbetter visualization
        }

        // if only channel 2 is open then plot channel 1
        else if(ui->channel2Radio->isChecked() && !(ui->channel1Radio->isChecked())) {
            makePlot2(graph2Index);
            minY = *std::min_element(y2Data.constBegin(), y2Data.constEnd());
            maxY = *std::max_element(y2Data.constBegin(), y2Data.constEnd());
            ui->customPlot->yAxis->setRange(minY - 1, maxY + 1);    // 1 is forbetter visualization
        }

    }

}

// Plots the channel 1
void Widget::makePlot1(int graphIndex)
{
    ui->customPlot->graph(graphIndex)->setData(xData, y1Data);    // Set x and y data for the graph

    // Changes the graph line color
    QPen graphPen;
    graphPen.setColor(QColor(255, 107, 243));
    ui->customPlot->graph(graphIndex)->setPen(graphPen);

    // shifts the graph display to the left so we get more realistic look
    int numDataPoints = xData.size();
    if (numDataPoints >= 20)
    {
        double xMax = xData.last(); // Since xData is always increasing last data will have the largest value
        double xMin = xData.at(numDataPoints - 20); // Son indeksten 20 onceki verinin zaman degeri (20 opsiyonel)
        ui->customPlot->xAxis->setRange(xMin, xMax);
    }
    // if there is no data xData will be empty so as a default we assign 0-10 range
    else if(xData.isEmpty()) {
        ui->customPlot->xAxis->setRange(0, 10);
    }
    // When numDataPoints is not empty but also smalled than 20
    else
    {
        ui->customPlot->xAxis->setRange(xData.first(), xData.last());
    }

    // shows the x and y value of clicked point
    connect(ui->customPlot, &QCustomPlot::mousePress, this, &Widget::mousePressEvent);

    // replot is necessary to save changes
    ui->customPlot->replot();
}

// Plots the channel 2
void Widget::makePlot2(int graphIndex)
{
    ui->customPlot->graph(graphIndex)->setData(xData, y2Data);    // Set x and y data for the graph

    // Changes the graph line color
    QPen graphPen;
    graphPen.setColor(QColor(0, 107, 243));
    ui->customPlot->graph(graphIndex)->setPen(graphPen);

    // shifts the graph display to the left so we get more realistic look
    int numDataPoints = xData.size();
    if (numDataPoints >= 20)
    {
        double xMax = xData.last(); // Since xData is always increasing last data will have the largest value
        double xMin = xData.at(numDataPoints - 20); // Son indeksten 20 onceki verinin zaman degeri (20 opsiyonel)
        ui->customPlot->xAxis->setRange(xMin, xMax);
    }
    // if there is no data xData will be empty so as a default we assign 0-10 range
    else if(xData.isEmpty()) {
        ui->customPlot->xAxis->setRange(0, 10);
    }
    // When numDataPoints is not empty but also smalled than 20
    else
    {
        ui->customPlot->xAxis->setRange(xData.first(), xData.last());
    }

    // shows the x and y value of clicked point
    connect(ui->customPlot, &QCustomPlot::mousePress, this, &Widget::mousePressEvent);

    // replot is necessary to save changes
    ui->customPlot->replot();
}


// Mouse Event for displaying the clicked data point
void Widget::mousePressEvent(QMouseEvent* event)
{
    // count is for keeping track of clickes
    static int count = 0;

    // get the value of clicked point
    double x = ui->customPlot->xAxis->pixelToCoord(event->pos().x());
    double y = ui->customPlot->yAxis->pixelToCoord(event->pos().y());

    // Tolerance level(%5) in case user clicks on empty space
    double xTolerance = (maxX - minX) / 20;
    double yTolerance = (maxY - minY) / 20;

    // İf we click on the info label it will disappear
    if(count == 1 && ui->pointLabel->geometry().contains(event->pos())) {
        count = 0;
        ui->pointLabel->setVisible(0);
        ui->redLabel->setVisible(0);
        ui->pointLabel->move(0,0);
        ui->redLabel->move(0,0);
    }

    for (int i = 0; i < xData.size(); i++) {
        if(abs(x - xData.at(i)) < xTolerance && (abs(y - y1Data.at(i)) < yTolerance || abs(y - y2Data.at(i)) < yTolerance)) {

            // Koordinatları ekranda gostermek icin bir metin olustur
            QString pointInfo = QString("X: %1\nY: %2").arg(x, 0, 'f', 2).arg(y, 0, 'f', 2);

            // Kırmızı yuvarlak bir labelı tıklanılan noktaya taşır
            ui->redLabel->setVisible(1);
            ui->redLabel->move(event->pos().x(), event->pos().y());

            ui->pointLabel->setVisible(1);
            ui->pointLabel->move(event->pos().x(), event->pos().y()-40);
            ui->pointLabel->setText(pointInfo);

            count = 1;
        }
    }
}


// Sends the frequency and amplitude data to the device
void Widget::write_to_serial_data()
{
    // First check if the COMPORT is open
    // Data Format is frequency,amplitude\n
    if(COMPORT->isOpen()) {
        // Gets the frequency value to send over serial port
        int freqValue = ui->frequencyDial->value();
        QString dataToSend = QString::number(freqValue) + ",";
        COMPORT->write(dataToSend.toLatin1());
        //qDebug() << "FREKANS:" << dataToSend;

        // Gets the frequency value to send over serial port
        int amplitudeValue = ui->amplitudeDial->value();
        dataToSend = QString::number(amplitudeValue) + "\n";
        COMPORT->write(dataToSend.toLatin1());
        //qDebug() << "GENLIK : " << dataToSend;
    }

    else {
        qDebug() << "Connect Your Device";
    }
}


// Makes the cursors, labels and sliders visible to use
void Widget::on_horizontalEnableRadio_toggled(bool checked)
{
    ui->cursor1VerticalSlider->setVisible(checked);
    ui->cursor2VerticalSlider->setVisible(checked);
    ui->horizontalLine1->setVisible(checked);
    ui->horizontalLine2->setVisible(checked);
    ui->horizontalLabel1->setVisible(checked);
    ui->horizontalLabel2->setVisible(checked);

    if(checked) {
        // Prints the distance between two cursors on the label below customPlot object
        double yDifference = ui->customPlot->yAxis->pixelToCoord(ui->horizontalLine2->y()) - ui->customPlot->yAxis->pixelToCoord(ui->horizontalLine1->y());
        ui->dyLabel->setText(QString("Dy = %1").arg(abs(yDifference), 0, 'f', 2));

        // Prints the position of the cursor right next to it
        double yPosition;
        yPosition = ui->customPlot->yAxis->pixelToCoord(ui->horizontalLine1->y());
        ui->horizontalLabel1->setText(QString("y = %1").arg(yPosition, 0, 'f', 2));
    }

    else
        ui->dyLabel->setText("");
}

// Makes the cursors, labels and sliders visible to use
void Widget::on_verticalEnableRadio_toggled(bool checked)
{
    ui->cursor1HorizontalSlider->setVisible(checked);
    ui->cursor2HorizontalSlider->setVisible(checked);
    ui->verticalLine1->setVisible(checked);
    ui->verticalLine2->setVisible(checked);
    ui->verticalLabel1->setVisible(checked);
    ui->verticalLabel2->setVisible(checked);


    if(checked) {
        // Prints the distance between two cursors on the label below customPlot object
        double xDifference = ui->customPlot->xAxis->pixelToCoord(ui->verticalLine1->x()) - ui->customPlot->xAxis->pixelToCoord(ui->verticalLine2->x());
        ui->dxLabel->setText(QString("Dx = %1").arg(abs(xDifference), 0, 'f', 2));

        // Prints the position of the cursor right next to it
        double xPosition;
        xPosition = ui->customPlot->xAxis->pixelToCoord(ui->verticalLine1->x());
        ui->verticalLabel1->setText(QString("x = %1").arg(xPosition, 0, 'f', 2));
    }
    else
        ui->dxLabel->setText("");
}

/* on_cursor1HorizontalSlider_valueChanged, on_cursor2HorizontalSlider_valueChanged
   on_cursor1VerticalSlider_valueChanged, on_cursor1VerticalSlider_valueChanged
   They all do the same thing which is moving the cursor
*/
void Widget::on_cursor1HorizontalSlider_valueChanged(int value)
{
    ui->receivedDataText->setPlainText(QString::number(value));
    // 6.2 is because width is 620px and we map it to 100 slider value
    draw_vertical_cursor_1(value * 6.2);

    // When cursor is moved recalculates the difference and position value
    double xDifference = ui->customPlot->xAxis->pixelToCoord(ui->verticalLine1->x()) - ui->customPlot->xAxis->pixelToCoord(ui->verticalLine2->x());
    ui->dxLabel->setText(QString("Dx = %1").arg(abs(xDifference), 0, 'f', 2));

    double xPosition;
    xPosition = ui->customPlot->xAxis->pixelToCoord(ui->verticalLine1->x());
    ui->verticalLabel1->setText(QString("x = %1").arg(xPosition, 0, 'f', 2));
}

void Widget::on_cursor2HorizontalSlider_valueChanged(int value)
{
    ui->receivedDataText->setPlainText(QString::number(value));
    // 6.2 is because width is 620px and we map it to 100 slider value
    draw_vertical_cursor_2(value * 6.2);

    double xDifference = ui->customPlot->xAxis->pixelToCoord(ui->verticalLine1->x()) - ui->customPlot->xAxis->pixelToCoord(ui->verticalLine2->x());
    ui->dxLabel->setText(QString("Dx = %1").arg(abs(xDifference), 0, 'f', 2));

    double xPosition;
    xPosition = ui->customPlot->xAxis->pixelToCoord(ui->verticalLine2->x());
    ui->verticalLabel2->setText(QString("x = %1").arg(xPosition, 0, 'f', 2));
}

void Widget::on_cursor1VerticalSlider_valueChanged(int value)
{
    ui->receivedDataText->setPlainText(QString::number(value));
    // 4.6 is because height is 460px and we map it to 100 slider value
    draw_horizontal_cursor_1(value * 4.6);

    double yDifference = ui->customPlot->yAxis->pixelToCoord(ui->horizontalLine2->y()) - ui->customPlot->yAxis->pixelToCoord(ui->horizontalLine1->y());
    ui->dyLabel->setText(QString("Dy = %1").arg(abs(yDifference), 0, 'f', 2));

    double yPosition;
    yPosition = ui->customPlot->yAxis->pixelToCoord(ui->horizontalLine1->y());
    ui->horizontalLabel1->setText(QString("y = %1").arg(yPosition, 0, 'f', 2));
}


void Widget::on_cursor2VerticalSlider_valueChanged(int value)
{
    ui->receivedDataText->setPlainText(QString::number(value));
    // 4.6 is because height is 460px and we map it to 100 slider value
    draw_horizontal_cursor_2(value * 4.6);

    double yDifference = ui->customPlot->yAxis->pixelToCoord(ui->horizontalLine2->y()) - ui->customPlot->yAxis->pixelToCoord(ui->horizontalLine1->y());
    ui->dyLabel->setText(QString("Dy = %1").arg(abs(yDifference), 0, 'f', 2));


    double yPosition;
    yPosition = ui->customPlot->yAxis->pixelToCoord(ui->horizontalLine2->y());
    ui->horizontalLabel2->setText(QString("y = %1").arg(yPosition, 0, 'f', 2));
}

// Toggles the radio button. Later checked inupdate_graph_with_new_data()
// Its purpose is to stop plotting so that we can examine the graph more detailed
void Widget::on_stopButton_pressed()
{
    ui->stopRadio->setChecked(!(ui->stopRadio->isChecked()));
}

// To make it easy to touch on touchscreen
void Widget::on_verticalCursorButton_pressed()
{
    ui->verticalEnableRadio->setChecked(!(ui->verticalEnableRadio->isChecked()));
}

// To make it easy to touch on touchscreen
void Widget::on_horizontalCursorButton_pressed()
{
    ui->horizontalEnableRadio->setChecked(!(ui->horizontalEnableRadio->isChecked()));
}

// If we use on_valueChanged rx buffer overflows at the devi
void Widget::on_amplitudeDial_sliderReleased()
{
    amplitudeValue = ui->amplitudeDial->value();
}

// If we use on_valueChanged rx buffer overflows at the device
void Widget::on_frequencyDial_sliderReleased()
{
    frequencyValue = ui->frequencyDial->value();
}

// Just updates the label to have better visualization
void Widget::on_amplitudeDial_valueChanged(int value)
{
    ui->amplitudeLabel->setText(QString("Amplitude: %1").arg(value));
}

// Just updates the label to have better visualization
void Widget::on_frequencyDial_valueChanged(int value)
{
    ui->frequencyLabel->setText(QString("Frequency: %1 Hz").arg(value));
}

// Reads the port name and connects to the device
void Widget::on_connectButton_pressed()
{
    // A flag to keep track of connection
    static bool isConnected = false;
    // For debug purposes this one is used to keep track of errors
    static int numberOfErrors = 0;

    // if we are reconnecting delete the previous COMPORT
    if (isConnected && COMPORT) {
        COMPORT->close();
        delete COMPORT;
        COMPORT = nullptr;
    }

    // Assign the COMPORT
    COMPORT = new QSerialPort(this);
    QString comPortName;

    // Reads the comboBox current Text and saves it in comPortName
    comPortName = ui->serialPortComboBox->currentText();

    // If we are already connected, disconnect
    if (isConnected) {
        COMPORT->close();
        isConnected = false;
        ui->connectButton->setText("Disconnected"); // update button label
        ui->connectButton->setStyleSheet("background-color: rgb(255, 0, 0);"); // When it is disconnected change background to red
        ui->receivedDataText->setPlainText(QString("Serial port %1 closed!").arg(comPortName));
        return; // Disconnected, terminate
    }

    // If port is open then closes it
    if (COMPORT->isOpen()) {
        ui->receivedDataText->setPlainText(QString("Closing the Port!"));
        COMPORT->close();
    }

    // Set the retrieved COM port name
    COMPORT->setPortName(comPortName);
    COMPORT->setBaudRate(QSerialPort::Baud38400);
    COMPORT->setParity(QSerialPort::NoParity);
    COMPORT->setDataBits(QSerialPort::Data8);
    COMPORT->setStopBits(QSerialPort::OneStop);
    COMPORT->setFlowControl(QSerialPort::NoFlowControl);

    // Attempt to open the serial port
    if (COMPORT->open(QIODevice::ReadWrite)) {
        connect(COMPORT, SIGNAL(readyRead()), this, SLOT(read_data()));
        connect(ui->amplitudeDial, &QDial::sliderReleased, this, &Widget::write_to_serial_data);
        connect(ui->frequencyDial, &QDial::sliderReleased, this, &Widget::write_to_serial_data);
        ui->receivedDataText->setPlainText(QString("Serial port %1 opened!").arg(comPortName));
        numberOfErrors = 0;
        isConnected = true; // Port is connected succcessfully
        ui->connectButton->setText("Connected"); // Updates the button text
        ui->connectButton->setStyleSheet("background-color: rgb(0, 255, 0);"); // When it is connected change background to green
    } else {
        ui->receivedDataText->setPlainText(QString("Cannot open %1\nNumber of trials = %2").arg(comPortName).arg(numberOfErrors));
        numberOfErrors++;
        ui->connectButton->setStyleSheet("background-color: rgb(255, 255, 255);");
    }
}
