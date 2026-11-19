#include "settings.hpp"
#include <QSettings>
#include <QDebug>
#include <QTextStream>
#include <cstdlib>
#include <qtextcodec.h>
#include <FileSystemUtils/CSVFile>


QString AlgoSettings::Debug()
{
     return QString("Ширина региона поиска = %1\n").arg(pointsMapWidth) +
         QString("Длина региона поиска = %1\n").arg(pointsMapHegth) +
         QString("Количество популяций одного размера = %1\n").arg(countOneSizePopulation)
+
         QString("Мин длина маршрута = %1\n").arg(minLengthPop) +
         QString("Макс длина маршрута = %1\n").arg(maxLengthPop);
}
AlgoSettings &AlgoSettings::inst()
{
     static AlgoSettings settings;
     return settings;
}



AlgoSettings::AlgoSettings()
{
    QSettings settings("./MissionPlanner/algoSettings.ini", QSettings::Format::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    settings.beginGroup("Init");
    pointsMapWidth         = settings.value("pointsMapWidth", pointsMapWidth).toInt();
    pointsMapHegth         = settings.value("pointsMapHegth", pointsMapHegth).toInt();
    countOneSizePopulation = settings.value("countOneSizePopulation", countOneSizePopulation).toInt();
    minLengthPop         = settings.value("minLengthPop", minLengthPop).toInt();
    maxLengthPop           = settings.value("maxLengthPop", maxLengthPop).toInt();
    settings.endGroup();
    qDebug().noquote() << "[AlgoSettings]" << Debug();
}
AgentSettings &AgentSettings::inst()
{
    static AgentSettings agent;
    return agent;
}
AgentSettings::AgentSettings()
{
    QSettings settings("./MissionPlanner/agent.ini", QSettings::Format::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    settings.beginGroup("AgentSettings");
    V   = settings.value("V", V).toInt();
    GT = settings.value("GT", GT).toInt();
    CK = settings.value("CK", CK).toInt();
    MIN_A = settings.value("MIN_A", MIN_A).toInt();
    MIN_ORTHO_LEN = settings.value("MIN_ORTHO_LEN", MIN_ORTHO_LEN).toInt();

    settings.endGroup();
    CSVFile file("./MissionPlanner/dropProbability.csv", CSVFile::SYNC_OFF, ',');
for (auto &row : file) {
        if (row.size() < 2) continue;
        bool ok = true;
        int key = row[0].toInt(&ok);
        if (!ok) continue;
        double value = row[0].toDouble(&ok);
        if (!ok) continue;
        windSpeedToProbabilityDestroy[key] = value;
    }
    qDebug().noquote() << "[AgentSettings]" << Debug();
}
QString AgentSettings::Debug()
{
    QString map = "Функция вероятности потери агента от скорости ветра: {\n";
    QTextStream str(&map, QIODevice::WriteOnly);
    for (auto i = windSpeedToProbabilityDestroy.cbegin(); i != windSpeedToProbabilityDestroy.cend(); ++i)
        str << QString(" f(%1)=%2,\n").arg(i.key()).arg(i.value());
    str << "}\n";
    return QString("Крейсерская скорость м/с = %1\n").arg(V) + QString("Количество топлива = %1\n").arg(GT) +
         QString("Расход топлива на км при крейсерской скорости = %1\n").arg(CK) +
         QString("Минимальный угол поворота в градусах = %1\n").arg(MIN_A) + map;
}
double AgentSettings::Prob(int windSpeed)
{
    /// Если скорость равна нижнеё границы промежутка - вероятность нулевая

if (windSpeed <= 0) return 0;
    /// Если скорость за границами промежутка - вероятность потери максимальная
    return inst().windSpeedToProbabilityDestroy.value(abs(windSpeed), 1);}
double AgentSettings::CKi(double len, double windSpeed, Angle courseToWinAngle)
{
    return inst().CK * len * (inst().V + windSpeed * courseToWinAngle.cos()) / inst().V;
}
