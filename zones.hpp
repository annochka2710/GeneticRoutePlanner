#ifndef ZONES_HPP
#define ZONES_HPP
#include <QWidget>
#include <SPGeoItem/Polygon>
#include <QListWidgetItem>
#include <qcolor.h>
#include <qpair.h>


using namespace Ics::SP;
namespace Ui
{
    class Zones;
}


class Zones : public QWidget
{
    Q_OBJECT


public:
    static QColor Interpolate(qreal val, const QRgb &minC = QColor(Qt::green).rgb(),
                   const QRgb &maxC = QColor(Qt::red).rgb(), qreal minV = 0, qreal maxV = 23);

explicit Zones(QWidget *parent = 0);
    QList<QPair<int, QPolygon *>> &getRestrictedAreas() { return zones; }
    void rebuildList();
    ~Zones();


public slots:
    void addZone(QPolygon *zone, int size);


private slots:
    void on_deleteZone_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_itemSelectionChanged();
private:
    QList<QPair<int, QPolygon *>> zones;
    Ui::Zones *ui; }; #endif // ZONES_HPP
