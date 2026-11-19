#pragma once
#include <QString>
#include <QHash>
#include <Geodesy/Math/Ellipsoid>
#include <Geodesy/Math/PZ_90>


using PZ_90 = Geodesy::Math::PZ_902d;
using PZ_90_3 = Geodesy::Math::PZ_903;
#define CURVE(T) Geodesy::Math::Ellipsoid::OrthodromicCurveEx<T>
#define ORTHO_DIST(p1, p2) CURVE(decltype(p1))::length(p1, p2);
#define DIRECT(p1, angle, dist) CURVE(decltype(p1))::directCalculate(p1, angle, dist);
struct AgentSettings {
     int V       = 60; //!< Крейсерская скорость м/с
     int GT       = 1500; //!< Количество топлива
     int CK      = 15; //!< Расход топлива на км при крейсерской скорости
     int MIN_A      = 20; //!< Минимальный угол поворота в градусах
     int MIN_ORTHO_LEN = 500; //!< Минимальный размер ортодромы
     QString Debug();
     static AgentSettings &inst();
     static double Prob(int windSpeed);
     /// Затрачиваемое топливо на участок длины len со скоростью ветра windSpeed
     static double CKi(double len, double windSpeed, Angle courseToWinAngle = Angle::pi);


private:
     QHash<int, double> windSpeedToProbabilityDestroy;
     AgentSettings();
};
struct AlgoSettings {
     int pointsMapWidth        = 100; //!< Ширина региона поиска маршрута



int pointsMapHegth           = 100; //!< Длина региона поиска маршрута
     int countOneSizePopulation = 10; //!< Количество популяций одного размера
     int minLengthPop         = 3; //!< Минимальная длина маршрута
     int maxLengthPop         = 50; //!< Максимальная длина маршрута
     QString Debug();
     static AlgoSettings &inst();
private:
     AlgoSettings();
};
