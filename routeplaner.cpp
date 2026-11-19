#include "src/routeplaner.hpp"
#include "src/GA.hpp"
#include "src/settings.hpp"
#include "ui_routeplaner.h"
#include <algorithm>
#include <qalgorithms.h>
#include <qdebug.h>
#include <QRandomGenerator>
#include <qglobal.h>
#include <qpair.h>
#include <qpixmap.h>
#include <GeoGUI/GeoCoordView>
#include <vector>


RoutePlaner::RoutePlaner(Zones *zones, QWidget *parent)
    : QWidget(parent), zones_(zones), ui(new Ui::RoutePlaner)
{
    ui->setupUi(this);
    geoGrid.resize(AlgoSettings::inst().pointsMapHegth);
    for (auto &i : geoGrid) i.resize(AlgoSettings::inst().pointsMapWidth);
    start->setFollowObject(startI);
    end->setFollowObject(endI);
    start->setStyle(":/ppmFormStyle.ini");
    end->setStyle(":/ppmFormStyle.ini");
    for (int i = 0; i < 10; i++) {
      QPolygon *tmp = new QPolygon;
      tmp->setStyle(":/routeStyle.ini");
      tmp->borderPen.setColor(QColor::fromRgb(QRandomGenerator::global()->generate()));
      routes.append(tmp); }

    route->points.resize(2);
    route->setStyle(":/routeStyle.ini");
    grid->setStyle(":/regionStyle.ini");
    region->setStyle(":/regionStyle.ini");
    region->trueOrthodromes = true;
    startI->image       = QPixmap(":/startA.png").scaled({64, 64}, Qt::KeepAspectRatio);
    endI->image          = QPixmap(":/endB.png").scaled({64, 64}, Qt::KeepAspectRatio);
    startI->isVisible   = false;
    endI->isVisible     = false;
    start->isVisible    = false;
    end->isVisible      = false;
    start->zValue       = (end->zValue = (endI->zValue = (startI->zValue = 10)));
}
RoutePlaner::~RoutePlaner() { delete ui; }
void RoutePlaner::on_setStart_clicked()
{
    status = SET_A;
    updateBtns();
}
void RoutePlaner::on_setEnd_clicked()
{
    status = SET_B;
    updateBtns();
}
void RoutePlaner::moveGeoPoint(const Geodesy::Math::PZ_903d &point)
{
    if (status == NONE) return;
    if (status == SET_RECT) {
      if (region->points.size() == 0) return;

      region->points.resize(4);
      PZ_90_3 p0         = region->points.first();
      region->points[1] = PZ_90_3(p0.b(), point.l(), 0);
      region->points[2] = point;
      region->points[3] = PZ_90_3(point.b(), p0.l(), 0);
      region->updateGeo();
      recalcGrid();
  }
if (status == SET_A) {
      startI->isVisible = true;
      start->isVisible = true;
      startI->geoPos = point;
      start->id       = "Начало маршрута";
      start->texts     = GeoGUI::toString(point.to2d()).split(", ");
      start->texts.push_front(" ");
      startI->updateGeo();
      startI->updateView();
      start->updateView();
      route->points.first() = point;
  }
  if (status == SET_B) {
      endI->isVisible = true;
      end->isVisible = true;
      endI->geoPos = point;
      end->id         = "Конец маршрута";
      end->texts       = GeoGUI::toString(point.to2d()).split(", ");
      end->texts.push_front(" ");
      endI->updateGeo();
      endI->updateView();

        end->updateView();
        route->points.last() = point;
    }
    route->isVisible = end->isVisible && start->isVisible;
    route->updateGeo();
    route->updateView();
}
void RoutePlaner::setGeoPoint(const Geodesy::Math::PZ_903d &p)
{
    if (status == NONE) return;
    if (status == SET_RECT) {
        if (region->points.size() == 0) {
            region->points.append(p);
            region->updateGeo();
            return;
        }
    }
    status = NONE;
    updateBtns();
}
void RoutePlaner::updateBtns()
{
    ui->setEnd->setEnabled(status == NONE);
    ui->setStart->setEnabled(status == NONE);
    ui->calc->setEnabled(status == NONE);
    ui->saveRes->setEnabled(status == NONE);
    ui->pushButton_2->setEnabled(status == NONE);
}



        void RoutePlaner::recalcGrid()
{
    if (region->points.size() != 4) return;
    PZ_90_3 p0 = region->points[0], p1 = region->points[2];
    PZ_90 nortWest(qMax(p0.b(), p1.b()), qMin(p0.l(), p1.l()));
    PZ_90 nortEast(qMax(p0.b(), p1.b()), qMax(p0.l(), p1.l()));
    PZ_90 southWest(qMin(p0.b(), p1.b()), qMin(p0.l(), p1.l()));
    PZ_90 southEast(qMin(p0.b(), p1.b()), qMax(p0.l(), p1.l()));
    CURVE(PZ_90) East(nortWest, southWest);
    CURVE(PZ_90) West(nortEast, southEast);
    auto EastPoints = East.getPoints((size_t)AlgoSettings::inst().pointsMapHegth);
    auto WestPoints = West.getPoints((size_t)AlgoSettings::inst().pointsMapHegth);
    for (int i = 0; i < geoGrid.size(); i++) {
        CURVE(PZ_90) horisontal(WestPoints[i], EastPoints[i]);
        geoGrid[i] = QVector<PZ_90>::fromStdVector(horisontal.getPoints((size_t)AlgoSettings::inst().pointsMapWidth));
    }
    for (int j = 0; j < geoGrid.size(); j++) grid->points.clear();
    grid->points.resize(0);
    grid->points.reserve(AlgoSettings::inst().pointsMapHegth * AlgoSettings::inst().pointsMapWidth);
    for (int i = 0; i < geoGrid.size(); i++)
        if (i % 2 == 0)
          for (int j = 0; j < geoGrid[i].size(); j++) grid->points.append(geoGrid[i][j].to3d());
        else
          for (int j = geoGrid[i].size() - 1; j >= 0; j--) grid->points.append(geoGrid[i][j].to3d());


    for (int i = AlgoSettings::inst().pointsMapWidth - 1; i >= 0; i--)
        if (i % 2 == 0)

                for (int j = 0; j < AlgoSettings::inst().pointsMapHegth; j++) grid->points.append(geoGrid[j][i].to3d());
        else
          for (int j = AlgoSettings::inst().pointsMapHegth - 1; j >= 0; j--)
               grid->points.append(geoGrid[j][i].to3d());
    grid->updateGeo();
}
void RoutePlaner::on_pushButton_2_clicked()
{
    region->points.resize(0);
    region->updateGeo();
    status = SET_RECT;
    updateBtns();
}
void RoutePlaner::on_calc_clicked()
{
    geneticAlgo.initGrid = geoGrid;
    geneticAlgo.windZones.clear();
    geneticAlgo.windZones.reserve(zones_->getRestrictedAreas().size());
    for (auto &i : zones_->getRestrictedAreas()) {
        std::vector<PZ_90> points;
        points.reserve(i.second->points.size());
        for (auto p : i.second->points) points.push_back(p.to2d());
        Spheroid::OrthodromicPolygon<PZ_90> newZone(points);
        geneticAlgo.windZones.append(qMakePair(i.first, points));
    }
    geneticAlgo.A = startI->geoPos.to2d();
    geneticAlgo.B = endI->geoPos.to2d();
    geneticAlgo.init();

    route->isVisible = false;
    route->updateView();
    grid->isVisible = false;
    grid->updateView();
    gaTimerId = startTimer(10);
}
inst().pointsMapWidth - 1; i >= 0; i--)
      if (i % 2 == 0)
#define EPS 0.1
void RoutePlaner::timerEvent(QTimerEvent *)
{
    static quint64 count = 0;
    count++;
    static double maxFitnessPer10 = 0;
    double currentFitness       = geneticAlgo.fitness();
    if ((geneticAlgo.countGenerations + 1) % 10 == 0) {
      qDebug() << "fitness" << maxFitnessPer10 << "EPS=" << EPS;
      if (maxFitnessPer10 < EPS) {
        killTimer(gaTimerId);
        ui->populatesCount->setText(QString("Маршрут построен за %1 итераций").arg(geneticAlgo.countGenerations));
        for (int i = 0; i < std::min(geneticAlgo.chromosomes.size(), 10); i++) {
           QPolygon *tmp = routes[i];
           tmp->points.resize(0);
           tmp->points.reserve(geneticAlgo.chromosomes[i].size() + 2);
           tmp->points.append(startI->geoPos);
           for (auto j : geneticAlgo.chromosomes[i]) tmp->points.append(j.to3d());
           tmp->points.append(endI->geoPos);
           tmp->setStyle(":/routeStyle.ini");

    tmp->isVisible       = (i == 0);
             tmp->needShowPoints = true;
             tmp->isEditable = true;
             tmp->zValue = 100;
             } return;
        }
        maxFitnessPer10 = 0;
    } else
        maxFitnessPer10 = std::max(maxFitnessPer10, currentFitness);
    geneticAlgo.selection();
    geneticAlgo.crossover();
    geneticAlgo.mutation();
    ui->populatesCount->setText(QString("Количество поколений %1").arg(geneticAlgo.countGenerations));
    if (count % 10 != 0) return;
    /// Сделано для одного шага алгоритма
    for (int i = 0; i < std::min(geneticAlgo.chromosomes.size(), 10); i++) {
        QPolygon *tmp = routes[i];
        tmp->points.resize(0);
        tmp->points.reserve(geneticAlgo.chromosomes[i].size() + 2);
        tmp->points.append(startI->geoPos);
        for (auto j : geneticAlgo.chromosomes[i]) tmp->points.append(j.to3d());
        tmp->points.append(endI->geoPos);
        tmp->updateGeo();
    }
}
void RoutePlaner::on_saveRes_clicked() {}
