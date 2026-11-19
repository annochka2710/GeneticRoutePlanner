#ifndef ROUTEPLANER_HPP
 #define ROUTEPLANER_HPP


 #include "GA.hpp"
 #include "zones.hpp"
 #include <QWidget>
 #include <GeoItem/AllItems>
 #include <qvector.h>
 #include "settings.hpp"


 using namespace Ics::SP;
 namespace Ui
 {
     class RoutePlaner;
 }


 class RoutePlaner : public QWidget
 {
     Q_OBJECT
 public:
     explicit RoutePlaner(Zones *zones, QWidget *parent = 0);
     ~RoutePlaner();
 private slots:
     void moveGeoPoint(const PZ_90 &point);
     void setGeoPoint(const PZ_90 &point);
     void on_setStart_clicked();
     void on_setEnd_clicked();
     void on_calc_clicked();



     void updateBtns();
     void on_pushButton_2_clicked();
     void on_saveRes_clicked();
     void updateBtns();
     void on_pushButton_2_clicked();
     void on_saveRes_clicked();
private:
     void timerEvent(QTimerEvent *event) override;
     void recalcGrid();
     enum Status {
       NONE,
       SET_A,
       SET_B,
     } status = NONE;
     int gaTimerId;
     GA geneticAlgo;
     QVector<QVector<PZ90>> geoGrid;
     QIcon *startI = new QIcon;
     QIcon *endI      = new QIcon;
     QLabel *start     = new QLabel;
     QLabel *end       = new QLabel;
     QPolygon *region = new QPolygon;
     QPolygon *route = new QPolygon;
     QPolygon *grid = new QPolygon;
     Zones *zones_          = nullptr;
     QVector<QPolygon *> routes;
     Ui::RoutePlaner *ui;
};
#endif // ROUTEPLANER_HPP
