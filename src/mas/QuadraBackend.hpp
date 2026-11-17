#ifndef MAS_QUADRA_BACKEND_HPP
#define MAS_QUADRA_BACKEND_HPP

#include "third_party/Quadra/core/autodiff.hpp"
#include "IntrusivePtr.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <type_traits>

// Quadra's graph pointer must have one definition per program/shared library.
// C++17 inline thread-local storage also supports header-only MAS clients.
namespace had { inline threadDefine ADGraph* g_ADGraph = nullptr; }

namespace mas { namespace backend {

// MAS objects survive many graph recordings. An epoch invalidates their old
// vertex IDs while retaining their scalar values and parameter metadata.
struct Tape {
    had::ADGraph graph;
    std::size_t epoch = 1;
    bool recording = false;
    bool third_order = false;
    void Reset() {
        had::g_ADGraph = &graph;
        graph.Clear();
        ++epoch;
        third_order = false;
    }
    void Attach(bool third = false) { ++epoch; third_order = third; }
    void SetRecording(bool value) { recording = value; }
    void Reverse(unsigned id, bool second = true) {
        if (!had::g_ADGraph || id >= had::g_ADGraph->vertices.size())
            throw std::runtime_error("Quadra: invalid objective vertex");
        had::ZeroAdjoints();
        had::SetAdjoint(had::AReal(0., id), 1.);
        if (second) had::PropagateAdjoint();
        else {
            auto& vertices = had::g_ADGraph->vertices;
            for (std::size_t i = vertices.size(); i-- > 0;) {
                const auto v = vertices[i];
                if (v.e1.to != i) vertices[v.e1.to].w += v.w * v.e1.w;
                if (v.e2.to != i) vertices[v.e2.to].w += v.w * v.e2.w;
            }
        }
    }
    double Value(unsigned id) const {
        return had::g_ADGraph->vertices.at(id).w;
    }
    double Value(unsigned i, unsigned j) const {
        return had::GetAdjoint(had::AReal(0., i), had::AReal(0., j));
    }
};
inline thread_local Tape tape;

template<class T> class Variable {
    static_assert(std::is_same<T, double>::value,
                  "Quadra uses double precision; MAS must use double");
    struct Info {
        double value = 0.;
        mutable unsigned id = 0;
        mutable had::AReal ad{0.,0};
        mutable had::ThirdOrderScalar third{0.};
        mutable std::size_t epoch = 0;
    };
    std::string name_;
    double min_ = -std::numeric_limits<double>::infinity();
    double max_ = std::numeric_limits<double>::infinity();
public:
    inline static thread_local Tape& tape = backend::tape;
    std::shared_ptr<Info> info;
    bool bounded_m = false;
    Variable(T value, bool record) : info(new Info{value, 0}) {
        if (record && tape.recording) {
            if (tape.third_order) { info->third=had::ThirdOrderScalar(value); info->epoch=tape.epoch; }
            else { info->ad=had::AReal(value); info->id=info->ad.varId; info->epoch=tape.epoch; }
        }
    }
    Variable(T value = 0.) : Variable(value,true) {}
    // MAS caches copies of parameters throughout the population model. As in
    // ATL, lvalue copies share parameter storage. Expression temporaries update
    // that storage so all aliases observe the new value and graph vertex.
    Variable(const Variable&) = default;
    Variable(Variable&&) = default;
    Variable& operator=(const Variable&) = default;
    Variable& operator=(Variable&& other) {
        if (this != &other) *info = *other.info;
        return *this;
    }
    Variable& operator=(T value) { SetValue(value); return *this; }
    static Variable From(const had::AReal& ad) {
        Variable v(ad.val,false); v.info->ad = ad; v.info->id = ad.varId;
        v.info->epoch = tape.epoch; return v;
    }
    static Variable From(const had::ThirdOrderScalar& ad) {
        Variable v(ad.val,false); v.info->third = ad; v.info->epoch = tape.epoch; return v;
    }
    had::AReal AD() const {
        if (info->epoch != tape.epoch) {
            if (!had::g_ADGraph) had::g_ADGraph = &tape.graph;
            info->ad = had::AReal(info->value); info->epoch = tape.epoch;
            info->id = info->ad.varId;
        }
        return info->ad;
    }
    had::ThirdOrderScalar Third() const {
        if (info->epoch != tape.epoch) {
            info->third = had::ThirdOrderScalar(info->value); info->epoch = tape.epoch;
        }
        return info->third;
    }
    bool IsRecorded() const {
        return info->epoch==tape.epoch && !tape.third_order && had::g_ADGraph &&
                info->id<had::g_ADGraph->vertices.size();
    }
    T GetValue() const { return info->value; }
    void SetValue(T v) { info->value = v; info->epoch = 0; }
    void SetName(const std::string& s) { name_ = s; }
    const std::string& GetName() const { return name_; }
    void SetMinBoundary(T v) { min_ = v; bounded_m = true; }
    void SetMaxBoundary(T v) { max_ = v; bounded_m = true; }
    T GetMinBoundary() const { return min_; }
    T GetMaxBoundary() const { return max_; }
    const Variable& Cast() const { return *this; }
    Variable& operator++() { return *this += Variable(1.); }
    Variable operator++(int) { Variable old(GetValue(),false); *old.info=*info; ++*this; return old; }
    Variable& operator--() { return *this -= Variable(1.); }
    Variable operator--(int) { Variable old(GetValue(),false); *old.info=*info; --*this; return old; }
    Variable operator-() const {
        if (!tape.recording) return Variable(-GetValue());
        return tape.third_order ? From(-Third()) : From(-AD());
    }
    friend Variable operator+(const Variable& a, const Variable& b) {
        if (!tape.recording) return Variable(a.GetValue() + b.GetValue());
        return tape.third_order ? From(a.Third() + b.Third()) : From(a.AD() + b.AD());
    }
    Variable& operator+=(const Variable& b) { return *this = *this + b; }
    friend Variable operator-(const Variable& a, const Variable& b) {
        if (!tape.recording) return Variable(a.GetValue() - b.GetValue());
        return tape.third_order ? From(a.Third() - b.Third()) : From(a.AD() - b.AD());
    }
    Variable& operator-=(const Variable& b) { return *this = *this - b; }
    friend Variable operator*(const Variable& a, const Variable& b) {
        if (!tape.recording) return Variable(a.GetValue() * b.GetValue());
        return tape.third_order ? From(a.Third() * b.Third()) : From(a.AD() * b.AD());
    }
    Variable& operator*=(const Variable& b) { return *this = *this * b; }
    friend Variable operator/(const Variable& a, const Variable& b) {
        if (!tape.recording) return Variable(a.GetValue() / b.GetValue());
        return tape.third_order ? From(a.Third() / b.Third()) : From(a.AD() / b.AD());
    }
    Variable& operator/=(const Variable& b) { return *this = *this / b; }
    friend bool operator<(const Variable& a, const Variable& b) { return a.GetValue() < b.GetValue(); }
    friend bool operator<=(const Variable& a, const Variable& b) { return a.GetValue() <= b.GetValue(); }
    friend bool operator>(const Variable& a, const Variable& b) { return a.GetValue() > b.GetValue(); }
    friend bool operator>=(const Variable& a, const Variable& b) { return a.GetValue() >= b.GetValue(); }
    friend bool operator==(const Variable& a, const Variable& b) { return a.GetValue() == b.GetValue(); }
    friend bool operator!=(const Variable& a, const Variable& b) { return a.GetValue() != b.GetValue(); }
    friend Variable operator+(const Variable& a, double b) {
        if (!tape.recording) return Variable(a.GetValue() + b,false);
        return tape.third_order ? From(a.Third() + b) : From(a.AD() + b);
    }
    friend Variable operator+(double a, const Variable& b) {
        if (!tape.recording) return Variable(a + b.GetValue(),false);
        return tape.third_order ? From(a + b.Third()) : From(a + b.AD());
    }
    friend Variable operator-(const Variable& a, double b) {
        if (!tape.recording) return Variable(a.GetValue() - b,false);
        return tape.third_order ? From(a.Third() - b) : From(a.AD() - b);
    }
    friend Variable operator-(double a, const Variable& b) {
        if (!tape.recording) return Variable(a - b.GetValue(),false);
        return tape.third_order ? From(a - b.Third()) : From(a - b.AD());
    }
    friend Variable operator*(const Variable& a, double b) {
        if (!tape.recording) return Variable(a.GetValue() * b,false);
        return tape.third_order ? From(a.Third() * b) : From(a.AD() * b);
    }
    friend Variable operator*(double a, const Variable& b) {
        if (!tape.recording) return Variable(a * b.GetValue(),false);
        return tape.third_order ? From(a * b.Third()) : From(a * b.AD());
    }
    friend Variable operator/(const Variable& a, double b) {
        if (!tape.recording) return Variable(a.GetValue() / b,false);
        return tape.third_order ? From(a.Third() / b) : From(a.AD() / b);
    }
    friend Variable operator/(double a, const Variable& b) {
        if (!tape.recording) return Variable(a / b.GetValue(),false);
        return tape.third_order ? From(a / b.Third()) : From(a / b.AD());
    }
    friend bool operator<(const Variable& a,double b) { return a.GetValue()<b; }
    friend bool operator<(double a,const Variable& b) { return a<b.GetValue(); }
    friend bool operator<=(const Variable& a,double b) { return a.GetValue()<=b; }
    friend bool operator<=(double a,const Variable& b) { return a<=b.GetValue(); }
    friend bool operator>(const Variable& a,double b) { return a.GetValue()>b; }
    friend bool operator>(double a,const Variable& b) { return a>b.GetValue(); }
    friend bool operator>=(const Variable& a,double b) { return a.GetValue()>=b; }
    friend bool operator>=(double a,const Variable& b) { return a>=b.GetValue(); }
    friend bool operator==(const Variable& a,double b) { return a.GetValue()==b; }
    friend bool operator==(double a,const Variable& b) { return a==b.GetValue(); }
    friend bool operator!=(const Variable& a,double b) { return a.GetValue()!=b; }
    friend bool operator!=(double a,const Variable& b) { return a!=b.GetValue(); }
    friend std::ostream& operator<<(std::ostream& out, const Variable& v) { return out << v.GetValue(); }
};

template<class T> Variable<T> exp(const Variable<T>& x) {
    if (!tape.recording) return Variable<T>(std::exp(x.GetValue()));
    return tape.third_order ? Variable<T>::From(had::exp(x.Third()))
                            : Variable<T>::From(had::exp(x.AD()));
}
template<class T> Variable<T> log(const Variable<T>& x) {
    if (!tape.recording) return Variable<T>(std::log(x.GetValue()));
    return tape.third_order ? Variable<T>::From(had::log(x.Third()))
                            : Variable<T>::From(had::log(x.AD()));
}
template<class T> Variable<T> sqrt(const Variable<T>& x) {
    if (!tape.recording) return Variable<T>(std::sqrt(x.GetValue()));
    return tape.third_order ? Variable<T>::From(had::sqrt(x.Third()))
                            : Variable<T>::From(had::sqrt(x.AD()));
}
template<class T> Variable<T> sin(const Variable<T>& x) {
    if (!tape.recording) return Variable<T>(std::sin(x.GetValue()));
    return tape.third_order ? Variable<T>::From(had::sin(x.Third()))
                            : Variable<T>::From(had::sin(x.AD()));
}
template<class T> Variable<T> cos(const Variable<T>& x) {
    if (!tape.recording) return Variable<T>(std::cos(x.GetValue()));
    return tape.third_order ? Variable<T>::From(had::cos(x.Third()))
                            : Variable<T>::From(had::cos(x.AD()));
}
template<class T> Variable<T> tan(const Variable<T>& x) {
    if (!tape.recording) return Variable<T>(std::tan(x.GetValue()));
    return tape.third_order ? Variable<T>::From(had::tan(x.Third()))
                            : Variable<T>::From(had::tan(x.AD()));
}
template<class T> Variable<T> asin(const Variable<T>& x) {
    if (!tape.recording) return Variable<T>(std::asin(x.GetValue()));
    return tape.third_order ? Variable<T>::From(had::asin(x.Third()))
                            : Variable<T>::From(had::asin(x.AD()));
}
template<class T> Variable<T> acos(const Variable<T>& x) {
    if (!tape.recording) return Variable<T>(std::acos(x.GetValue()));
    return tape.third_order ? Variable<T>::From(had::acos(x.Third()))
                            : Variable<T>::From(had::acos(x.AD()));
}
template<class T> Variable<T> pow(const Variable<T>& x, double p) {
    // Integer powers at zero must not create 0 * infinity local derivatives.
    if (p == 0.) return Variable<T>(1.);
    if (p == 1.) return x;
    if (p == 2.) return x*x;
    if (p == 3.) return x*x*x;
    if (!tape.recording) return Variable<T>(std::pow(x.GetValue(), p));
    return tape.third_order ? Variable<T>::From(had::pow(x.Third(), p))
                            : Variable<T>::From(had::pow(x.AD(), p));
}
template<class T> Variable<T> pow(const Variable<T>& x, const Variable<T>& p) { return exp(p*log(x)); }
template<class T> Variable<T> pow(double x, const Variable<T>& p) { return exp(p*std::log(x)); }
template<class T> Variable<T> fabs(const Variable<T>& x) { return x.GetValue() < 0. ? -x : x; }
template<class T> Variable<T> abs(const Variable<T>& x) { return fabs(x); }
template<class T> Variable<T> ceil(const Variable<T>& x) { return Variable<T>(std::ceil(x.GetValue())); }
template<class T> Variable<T> floor(const Variable<T>& x) { return Variable<T>(std::floor(x.GetValue())); }
template<class T> Variable<T> log10(const Variable<T>& x) { return log(x)/std::log(10.); }
template<class T> Variable<T> sinh(const Variable<T>& x) { return (exp(x)-exp(-x))/2.; }
template<class T> Variable<T> cosh(const Variable<T>& x) { return (exp(x)+exp(-x))/2.; }
template<class T> Variable<T> tanh(const Variable<T>& x) {
    if (x.GetValue() < 0.) { auto e = exp(2.*x); return (e-1.)/(e+1.); }
    auto e = exp(-2.*x); return (1.-e)/(1.+e);
}
template<class T> Variable<T> atan(const Variable<T>& x) {
    double v=x.GetValue(), q=1.+v*v;
    if (!tape.recording) return Variable<T>(std::atan(v));
    double d1=1./q, d2=-2.*v/(q*q), d3=(6.*v*v-2.)/(q*q*q);
    if (tape.third_order) return Variable<T>::From(had::unary_chain(x.Third(),std::atan(v),d1,d2,d3));
    auto input=x.AD();
    auto result=had::NewAReal(std::atan(v)); had::AddEdge(result,input,d1,d2,d3);
    return Variable<T>::From(result);
}
template<class T> Variable<T> mfexp(const Variable<T>& x) {
    const double limit=60., e=std::exp(limit);
    if (x.GetValue() > limit) return e*(1.+2.*(x-limit))/(1.+x-limit);
    if (x.GetValue() < -limit) return (1.-x-limit)/(e*(1.+2.*(-x-limit)));
    return exp(x);
}

// Retain the small matrix/vector access interface used by MAS output code.
template<class T> struct RealMatrix : Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> {
    using Base=Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic>;
    using Base::Base; using Base::operator=;
    int GetRows() const { return this->rows(); }
    int GetColumns() const { return this->cols(); }
};
template<class T> struct RealVector : Eigen::Matrix<T,Eigen::Dynamic,1> {
    using Base=Eigen::Matrix<T,Eigen::Dynamic,1>;
    using Base::Base; using Base::operator=;
    void Resize(int n) { this->resize(n); }
    int GetSize() const { return this->size(); }
};
}} // namespace mas::backend

namespace mas {
using std::acos;
using backend::acos;
using std::asin;
using backend::asin;
using std::atan;
using backend::atan;
using std::ceil;
using backend::ceil;
using std::cos;
using backend::cos;
using std::cosh;
using backend::cosh;
using std::exp;
using backend::exp;
using backend::mfexp;
using std::fabs;
using backend::fabs;
using std::abs;
using backend::abs;
using std::floor;
using backend::floor;
using std::log;
using backend::log;
using std::log10;
using backend::log10;
using std::sin;
using backend::sin;
using std::sinh;
using backend::sinh;
using std::sqrt;
using backend::sqrt;
using std::tan;
using backend::tan;
using std::tanh;
using backend::tanh;
using std::pow;
using backend::pow;
}
#endif
