#include "GA.hpp"
#include "src/settings.hpp"
#include <algorithm>
#include <Geodesy/Math/Ellipsoid>
#include <cassert>
#include <qapplication.h>
#include <qdebug.h>


Chromosome::Chromosome(GA &ga_) : ga(&ga_) {}
Chromosome::Chromosome(const Chromosome &other) : QVector<PZ_90>(other), fitness(other.fitness), ga(other.ga) {}


Angle angleInB(const PZ_90 &a, const PZ_90 &b, const PZ_90 &c)
{
    RotatedSpheroid sphere = RotatedSpheroid(PZ_90_02GeoPoint(b));
    GnomonicSpherePoint p0(PZ_90_02GeoPoint(a), sphere);
    GnomonicSpherePoint p1(PZ_90_02GeoPoint(b), sphere);
    GnomonicSpherePoint p2(PZ_90_02GeoPoint(c), sphere);
    GnomonicGeometry geometry(sphere);
    return geometry.angle(p1.toPoint2d(), p2.toPoint2d(), p0.toPoint2d());
}


double GA::calc_fitness(PZ_90 p1, PZ_90 p2, PZ_90 p3)
{
    if (p1 == p2) return AgentSettings::inst().MIN_ORTHO_LEN;
    if (p2 == p3) return AgentSettings::inst().MIN_ORTHO_LEN;
    if (!p1.isValid() || !p2.isValid() || !p3.isValid()) assert("Ошибка. Получены неверные
точки");
    CURVE(PZ_90) curve(p1, p2);

     if (curve.length() < AgentSettings::inst().MIN_ORTHO_LEN)
          return AgentSettings::inst().MIN_ORTHO_LEN * 2;
     if (!p3.isValid()) return curve.length();
     if (angleInB(p1, p2, p3) < Degree(AgentSettings::inst().MIN_A)) return curve.length() * 2;
     Spheroid::OrthodromicPolygon<PZ_90> poly({p1, p2, p3});
     double multiplex = 1;
     for (const auto &i : windZones)
       if (i.second.intersects(poly)) multiplex += 500 * AgentSettings::inst().Prob(i.first);
     return curve.length() * multiplex;
}
void Chromosome::calcFitness()
{
     if (!isValid()) return;
     /// Для отладки самого алгоритма, пусть критерием будет расстояние
     double len = 0;
     len += ga->calc_fitness(ga->A, first());
     for (int i = 0; i < size() - 1; i++)
       if (i < size() - 2)
          len += ga->calc_fitness(at(i), at(i + 1), at(i + 2));
       else
          len += ga->calc_fitness(at(i), at(i + 1), ga->B);
     len += ga->calc_fitness(last(), ga->B);
     fitness = len;
}
bool isValid__(const Chromosome &points)
 { for (auto &i : points)
        if (!i.isValid()) return false;
     return true;
 }

    void Chromosome::mutate()
{
    if (!isValid()) return;
    std::uniform_int_distribution<int> mutationType(Chromosome::NONE, Chromosome::MAX);
    int type = mutationType(ga->rng);
    switch ((MutationType)type) {
      case MutationSwap: {
         /// Определим случайную точку в районе
         std::uniform_int_distribution<int> pos(0, size() - 2);
         int pos_ = pos(ga->rng);
         std::swap((*this)[pos_], (*this)[pos_ + 1]);
      } break;
      case MutationRadinMove: {
         std::uniform_int_distribution<int> pos(0, size() - 1);
         int pos_ = pos(ga->rng);
         std::uniform_real_distribution<double> distrRad(-1e-5, 1e-5);
         Geodesy::Math::PZ_90 tmp = (*this)[pos_];
         tmp.setB(tmp.b() + IMath::VRadian(distrRad(ga->rng)));
         tmp.setL(tmp.l() + IMath::VRadian(distrRad(ga->rng)));
         (*this)[pos_] = tmp;
      } break;
      case MutationRandPos: {
         std::uniform_int_distribution<int> pos(0, size() - 1);
         int pos_ = pos(ga->rng);
         std::uniform_real_distribution<double> x(0, ga->initGrid.size() - 1);
         std::uniform_real_distribution<double> y(0, ga->initGrid.first().size() - 1);
         PZ_90 p = ga->initGrid[x(ga->rng)][y(ga->rng)];
         while (!p.isValid()) p = ga->initGrid[x(ga->rng)][y(ga->rng)];
         (*this)[pos_] = p;

           } break;
         case MutationOrthoMove: {
           /// Определим случайную точку в районе
           std::uniform_int_distribution<int> pos(0, size() - 2);
           int pos_ = pos(ga->rng);
           if (at(pos_) == at(pos_ + 1)) return;
           CURVE(PZ_90) curve(at(pos_), at(pos_ + 1));
           /// Определим новую длину ортодромии до следующей точки в метрах
           std::uniform_real_distribution<double> dist(50, 1000.0);
           /// Рассчитаем новое положение второй точки и положим её обратно в маршрут
           (*this)[pos_ + 1] = curve.forward(curve.length() + dist(ga->rng));
         } break;
         case NONE:
         default: return;
     }
 }
 Chromosome Chromosome::crossover(Chromosome &other)
 {
     if (!isValid()) return Chromosome();
     std::uniform_int_distribution<int> mutationType(0, 3);
     Chromosome next(*ga);
     next << mid(0, size() / 2) << other.mid(size() / 2);
     return next;
 }
 void GA::init()
 {
     for (int i = AlgoSettings::inst().minLengthPop; i <= AlgoSettings::inst().maxLengthPop; i++)
                for (int j = 0; j < AlgoSettings::inst().countOneSizePopulation; j++) {
                      std::uniform_real_distribution<double> x(0, initGrid.size() - 1);

                            std::uniform_real_distribution<double> y(0, initGrid.first().size() - 1);
          Chromosome next(*this);
          next.reserve(i);
          for (int j = 0; j < i; j++) {
               PZ_90 p = initGrid[x(rng)][y(rng)];
               while (!p.isValid()) p = initGrid[x(rng)][y(rng)];
               next.append(p);
          }
          chromosomes.append(next);
      }
}
double GA::fitness()
{
    double newFitness = 0;
    for (auto &i : chromosomes) i.calcFitness();
    /// Отсортируем от лучших к худшим
    std::sort(chromosomes.begin(), chromosomes.end(),
              [](const Chromosome &a, const Chromosome &b) { return a.fitness < b.fitness; });
    for (int i = 0; i < 10; i++) newFitness += chromosomes[i].fitness;
    double diff = lastFitness - newFitness;
    lastFitness = newFitness;
    return diff;
}
void GA::selection() { chromosomes = chromosomes.mid(0, chromosomes.size() / 2); }
void GA::crossover()
{
    QVector<Chromosome> chromosomesNext;
    chromosomesNext.reserve(chromosomes.size());
    std::uniform_int_distribution<int> pos(0, chromosomes.size() - 1);

for (int i = 0; i < chromosomes.size(); i++)
        chromosomesNext.append(chromosomes[pos(rng)].crossover(chromosomes[pos(rng)]));
        chromosomes.append(chromosomesNext);
}
void GA::mutation() {
    QVector<Chromosome> chromosomesNew;
    chromosomesNew.reserve(chromosomes.size());
    /// Выполняем мутацию
    for (int i = 0; i < 10; i++) chromosomesNew.append(chromosomes[i]);
    // for (int i = 0; i < chromosomes.size(); i++) { /// Мутрируют все
    for (int i = 10; i < chromosomes.size(); i++) {
        auto tmp = chromosomes[i];
        for (int _K_ = 0; _K_ < 20; _K_++) tmp.mutate();
        if (tmp.size() >= AlgoSettings::inst().minLengthPop && tmp.size() <= AlgoSettings::inst().maxLengthPop)
          chromosomesNew.append(tmp);
    }
    for (int i = chromosomesNew.size() - 1; i < chromosomes.size(); i++) chromosomesNew.append(chromosomes[i]);
    chromosomes = chromosomesNew;
    countGenerations++;
}
QVector<PZ_90> GA::getResult()
{
    Chromosome *res = &chromosomes.first();
    for (Chromosome &i : chromosomes)
        if (i.fitness < res->fitness) res = &i;
    return *res;
}
