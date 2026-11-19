#pragma once
 #include "zones.hpp"
 #include <qvector.h>
 #include <random>
 #include <sys/types.h>
 #include "settings.hpp"
 #include <Geodesy/Math/GnomonicPoint>
 #include <Geodesy/Math/Spheroid>


 using namespace Geodesy::Math;
 class GA;


 struct Chromosome : public QVector<PZ_90> {
   enum MutationType {
        NONE = 0,
        // MutationDel, MutationAdd,
        MutationSwap = 1,
        // MutationRnad,
        MutationRandPos = 2,
        MutationOrthoMove = 3,
        MutationOrtoRotate = 4,
        MutationRadinMove = 5,
        MAX = MutationRadinMove
   };
   Chromosome();
   bool isValid();
   Chromosome(GA &ga_);
   Chromosome(const Chromosome &other);

 void mutate();
      Chromosome crossover(Chromosome &other);
      void calcFitness();
      double fitness = 0;
 private:
      GA *ga = nullptr;
 };


 class GA
 {
      std::random_device dev;
      std::mt19937 rng;
      friend Chromosome;
      double calc_fitness(PZ_90 p1, PZ_90 p2, PZ_90 p3 = {});
 public:
      GA() : rng(dev()) {}
      QVector<QVector<PZ_90>> initGrid;
      // QList<QPair<int, QPolygon *>> windZones;
      QList<QPair<int, Spheroid::OrthodromicPolygon<PZ_90>>> windZones;
      PZ_90 A, B;
      QVector<Chromosome> chromosomes;
      double lastFitness = 0;
      int countGenerations = 0;
      void init();
      double fitness(); /// Возвращает разницу текущего с предыдущим
      void selection();
      void crossover();
      void mutation();
      QVector<PZ_90> getResult();
};
