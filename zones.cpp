#include "zones.hpp"
#include "ui_zones.h"
#include <QFile>
#include <qnamespace.h>
#include <qpair.h>


QList<QPair<int, QPolygon *>> loadZones(QString path = "./Zones.bin")
{
    QList<QPair<int, QPolygon *>> res;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return res;
    QDataStream in(&file);

int size = 0;
    in >> size;
    for (int i = 0; i < size; i++) {
        qDebug() << "[loadZones] Добавлена зона №" << (i + 1);
        QPolygon *poly = new QPolygon;
        int windSpeed;
        in >> windSpeed >> (*poly);
        poly->setStyle(":/zoneStyle.ini");
        poly->needShowPoints = false;
        poly->backgroundColor = Zones::Interpolate(windSpeed);
        res.append(qMakePair(windSpeed, poly));
    }
    return res;
}
private:
    QList<QPair<int, QPolygon *>> zones;
    Ui::Zones *ui; }; #endif // ZONES_HPP
void saveZones(QList<QPair<int, QPolygon *>> zones, QString path = "./Zones.bin")
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return;
    QDataStream out(&file);
    out << zones.size();
    for (int i = 0; i < zones.size(); i++) out << (zones[i].first) << (*zones[i].second);
}
Zones::Zones(QWidget *parent) : QWidget(parent), ui(new Ui::Zones)
{
    ui->setupUi(this);
    zones = loadZones();

rebuildList();
    for (auto i : zones) addGeoItem(i.second);
}
Zones::~Zones() { delete ui; }
void Zones::rebuildList()
{
    ui->listWidget->clear();
    for (int i = 0; i < zones.size(); i++) {
        QListWidgetItem *item =
          new QListWidgetItem(QString("Зона №%1, Скорость ветра %2 м/с").arg(i +
1).arg(zones[i].first));
        ui->listWidget->addItem(item);
    }
}
void Zones::on_deleteZone_clicked()
{
    int row = ui->listWidget->currentRow();
    if (row == -1) return;
    delete zones[row].second;
    zones.removeAt(row);
    rebuildList();
    saveZones(zones);
}
void Zones::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    changeCenterAnimated(zones[ui->listWidget->row(item)].second->getCenter());
}




void Zones::addZone(QPolygon *zone, int size)
{
    QPolygon *newp = zone->clone();
    newp->needShowPoints = false;
    addGeoItem(newp);
    zones.append(qMakePair(size, newp));
    rebuildList();
    saveZones(zones);
}
void Zones::on_listWidget_itemSelectionChanged()
{
    for (auto i : zones) {
        i.second->needShowPoints = false;
        i.second->backgroundColor = Interpolate(i.first);
    }
    for (auto i : ui->listWidget->selectedItems()) zones[ui->listWidget->row(i)].second->borderPen.setColor(Qt::black);
    for (auto i : zones) i.second->updateView();
}
QColor Zones::Interpolate(qreal val, const QRgb &minC, const QRgb &maxC, qreal minV, qreal
maxV)
{
    qreal w1 = (val - minV) / (maxV - minV); // вес maхC
    QColor res = qRgb(qRed(minC) * (1 - w1) + qRed(maxC) * w1, qGreen(minC) * (1 - w1) +
qGreen(maxC) * w1,
               qBlue(minC) * (1 - w1) + qBlue(maxC) * w1);
    res.setAlpha(80);
    return res;
}
