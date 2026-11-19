#pragma once
#include "unavailable.hpp"
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <vector>

namespace Geodesy { namespace Math {
// Angles and coordinates use radians; no datum conversion is implemented.
class Angle {
public:
    static constexpr double pi = 3.14159265358979323846;
    Angle(double radians = 0) : value_(radians) {}
    double radians() const { return value_; }
    double cos() const { return std::cos(value_); }
    friend Angle operator+(Angle a, Angle b) { return a.value_ + b.value_; }
    friend bool operator<(Angle a, Angle b) { return a.value_ < b.value_; }
    friend bool operator==(Angle a, Angle b) { return a.value_ == b.value_; }
private:
    double value_;
};
inline Angle Degree(double degrees) { return degrees * Angle::pi / 180.0; }
class PZ_903d;
class PZ_902d {
public:
    PZ_902d() : latitude_(nan()), longitude_(nan()) {}
    PZ_902d(Angle latitude, Angle longitude) : latitude_(latitude), longitude_(longitude) {}
    Angle b() const { return latitude_; }
    Angle l() const { return longitude_; }
    void setB(Angle latitude) { latitude_ = latitude; }
    void setL(Angle longitude) { longitude_ = longitude; }
    bool isValid() const {
        return std::isfinite(b().radians()) && std::isfinite(l().radians()) &&
            std::abs(b().radians()) <= Angle::pi / 2 && std::abs(l().radians()) <= Angle::pi;
    }
    PZ_903d to3d() const;
    friend bool operator==(const PZ_902d &a, const PZ_902d &b) {
        return a.latitude_ == b.latitude_ && a.longitude_ == b.longitude_;
    }
private:
    static double nan() { return std::numeric_limits<double>::quiet_NaN(); }
    Angle latitude_, longitude_;
};
class PZ_903d {
public:
    PZ_903d() = default;
    PZ_903d(Angle latitude, Angle longitude, double height = 0)
        : point_(latitude, longitude), height_(height) {}
    Angle b() const { return point_.b(); }
    Angle l() const { return point_.l(); }
    double h() const { return height_; }
    bool isValid() const { return point_.isValid() && std::isfinite(height_); }
    PZ_902d to2d() const { return point_; }
private:
    PZ_902d point_;
    double height_ = 0;
};
inline PZ_903d PZ_902d::to3d() const { return {b(), l(), 0}; }
using PZ_903 = PZ_903d;
using PZ_90 = PZ_902d;
struct GeoPoint { PZ_902d coordinate; };
inline GeoPoint PZ_90_02GeoPoint(const PZ_902d &point) { return {point}; }
class RotatedSpheroid {
public:
    explicit RotatedSpheroid(const GeoPoint &) {}
};
struct Point2d { double x = 0; double y = 0; };
class GnomonicSpherePoint {
public:
    GnomonicSpherePoint(const GeoPoint &, const RotatedSpheroid &) {}
    Point2d toPoint2d() const {
        demo_stubs::unavailable("Geodesy::GnomonicSpherePoint::toPoint2d");
    }
};
class GnomonicGeometry {
public:
    explicit GnomonicGeometry(const RotatedSpheroid &) {}
    Angle angle(const Point2d &, const Point2d &, const Point2d &) const {
        demo_stubs::unavailable("Geodesy::GnomonicGeometry::angle");
    }
};
namespace Ellipsoid {
template <class Point> class OrthodromicCurveEx {
public:
    OrthodromicCurveEx(const Point &, const Point &) {}
    double length() const { demo_stubs::unavailable("Geodesy::OrthodromicCurveEx::length"); }
    static double length(const Point &, const Point &) {
        demo_stubs::unavailable("Geodesy::OrthodromicCurveEx::length");
    }
    Point forward(double) const { demo_stubs::unavailable("Geodesy::OrthodromicCurveEx::forward"); }
    static Point directCalculate(const Point &, Angle, double) {
        demo_stubs::unavailable("Geodesy::OrthodromicCurveEx::directCalculate");
    }
    std::vector<Point> getPoints(std::size_t) const {
        demo_stubs::unavailable("Geodesy::OrthodromicCurveEx::getPoints");
    }
};
}
namespace Spheroid {
template <class Point> class OrthodromicPolygon {
public:
    OrthodromicPolygon() = default;
    OrthodromicPolygon(std::initializer_list<Point> points) : points_(points) {}
    // Implicit conversion is used by qMakePair(i.first, points) in the listing.
    OrthodromicPolygon(const std::vector<Point> &points) : points_(points) {}
    bool intersects(const OrthodromicPolygon &) const {
        demo_stubs::unavailable("Geodesy::OrthodromicPolygon::intersects");
    }
private:
    std::vector<Point> points_;
};
}
}}
// settings.hpp uses Angle and Degree without namespace qualifiers.
using Geodesy::Math::Angle;
using Geodesy::Math::Degree;
namespace IMath { using VRadian = Geodesy::Math::Angle; }
