// Compile with C++17 and the vendored Eigen include directory; see README.MD.
#include "../../src/mas/QuadraObjective.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
using namespace mas::backend;
void close(double got, double expected, double tolerance=1e-6) {
    if (!std::isfinite(got) || std::abs(got-expected)>tolerance) {
        std::cerr << "expected " << expected << ", got " << got << '\n';
        std::abort();
    }
}
struct Quadratic : ObjectiveFunction<double> {
    Variable<double> x{-1.}, y{2.}, derived, cached_x;
    Quadratic() {
        x.SetName("x"); y.SetName("y"); cached_x=x;
        x.SetMinBoundary(-2.); x.SetMaxBoundary(5.);
        RegisterParameter(x,1); RegisterParameter(y,2);
    }
    void Objective_Function(Variable<double>& f) override {
        derived=x+2.*y;
        f=(cached_x-2.)*(cached_x-2.)+2.*(y+1.)*(y+1.);
    }
};
// Integrating u analytically gives NLL = theta^2/2 + log(theta^2+1)/2
// plus a constant. The fixed-effect optimum is 0 and its Hessian is 2.
struct Mixed : ObjectiveFunction<double> {
    Variable<double> theta{0.5}, u{0.2}, derived;
    Mixed() { RegisterParameter(theta); RegisterRandomVariable(u); }
    void Objective_Function(Variable<double>& f) override {
        derived=theta+u;
        f=0.5*theta*theta+0.5*(1.+theta*theta)*u*u;
    }
};
struct CorrelatedMixed : ObjectiveFunction<double> {
    Variable<double> theta{0.5}, u{0.2}, derived;
    CorrelatedMixed() { RegisterParameter(theta); RegisterRandomVariable(u); }
    void Objective_Function(Variable<double>& f) override {
        derived=theta+u;
        f=0.5*theta*theta+0.5*(u-theta)*(u-theta);
    }
};
struct Constrained : ObjectiveFunction<double> {
    Variable<double> x{0.}, y{0.}, derived;
    Constrained() { RegisterParameter(x); RegisterParameter(y); }
    void Objective_Function(Variable<double>& f) override {
        derived=x-y;
        f=0.5*(derived-2.)*(derived-2.);
    }
};
struct Saddle : ObjectiveFunction<double> {
    Variable<double> x{0.}, y{0.};
    Saddle() { RegisterParameter(x); RegisterParameter(y); }
    void Objective_Function(Variable<double>& f) override { f=x*x-y*y; }
};
struct Invalid : ObjectiveFunction<double> {
    Variable<double> x{1.};
    Invalid() { RegisterParameter(x); }
    void Objective_Function(Variable<double>& f) override { f=log(-x*x-1.); }
};
int main() {
    tape.Reset(); tape.recording=true;
    Variable<double> x=1.3,y=0.7;
    auto f=log(x)+exp(x*y)+pow(x,3.);
    tape.Reverse(f.AD().varId);
    close(tape.Value(x.info->id),1./1.3+0.7*std::exp(0.91)+3.*1.3*1.3);
    close(tape.Value(x.info->id,y.info->id),std::exp(0.91)*(1.+0.91));
    double before=f.GetValue();
    tape.Reset(); tape.recording=true;
    auto g=x*x+y;
    tape.Reverse(g.AD().varId);
    close(tape.Value(x.info->id),2.6); close(f.GetValue(),before);
    // MAS parameter copies share storage so cached aliases follow updates.
    auto copy=x; copy=5.; close(x.GetValue(),5.);
    x=x+1.; close(copy.GetValue(),6.);
    tape.recording=false;
    Quadratic q; LBFGS<double> fit;
    fit.SetObjectiveFunction(&q); fit.SetTolerance(1e-8); fit.Run();
    close(q.x.GetValue(),2.); close(q.y.GetValue(),-1.);
    close(q.covariance(0,0),0.5); close(q.covariance(1,1),0.25);
    close(q.GetVarianceOfDerivedValue(q.derived.info->id,q.fixed_ids),1.5);
    assert(q.fixed_effect_report.success_m && q.fixed_effect_report.rows_m.size()==2);
    close(q.fixed_effect_report.rows_m[0].std_error_m,std::sqrt(0.5));
    close(q.fixed_effect_report.rows_m[0].lower95_m,2.-1.96*std::sqrt(0.5));
    Variable<double> unrecorded=0.;
    const double saved_gradient=tape.Value(q.fixed_ids[0]);
    const auto variances=q.BuildDerivedReport({{"linear",&q.derived},{"x",&q.x},{"unrecorded",&unrecorded}});
    assert(q.derived_report.success_m && q.derived_report.names_m.size()==2);
    assert(q.unrecorded_derived_quantities==std::vector<std::string>{"unrecorded"});
    close(variances[0],1.5); close(variances[1],0.5); assert(std::isnan(variances[2]));
    close(q.derived_report.delta_m.covariance_m(0,1),0.5);
    close(q.derived_report.delta_m.std_error_m[0],std::sqrt(1.5));
    close(tape.Value(q.fixed_ids[0]),saved_gradient);
    tape.recording=false;
    Mixed mixed; fit.SetObjectiveFunction(&mixed); fit.Run();
    close(mixed.theta.GetValue(),0.,1e-5); close(mixed.u.GetValue(),0.,1e-5);
    close(mixed.covariance(0,0),0.5,1e-4);
    close(mixed.GetVarianceOfDerivedValue(mixed.derived.info->id,mixed.fixed_ids),0.5,1e-4);
    CorrelatedMixed correlated; fit.SetObjectiveFunction(&correlated); fit.Run();
    close(correlated.covariance(0,0),1.,1e-4);
    close(correlated.mode_sensitivity(0,0),1.);
    close(correlated.GetVarianceOfDerivedValue(correlated.derived.info->id,correlated.fixed_ids),4.,1e-4);
    const auto profiled=correlated.BuildDerivedReport({{"theta_plus_mode",&correlated.derived}});
    assert(correlated.derived_report.success_m);
    close(profiled[0],4.,1e-4);
    close(correlated.derived_report.delta_m.std_error_m[0],2.,1e-4);
    close(correlated.derived_report.delta_m.jacobian_m(0,0),2.);
    Constrained constrained; fit.SetObjectiveFunction(&constrained); fit.Run();
    close(constrained.derived.GetValue(),2.);
    assert(!constrained.covariance_available && !constrained.covariance_result.success_m);
    assert(!constrained.fixed_effect_report.success_m);
    assert(std::isnan(constrained.GetVarianceOfDerivedValue(constrained.derived.info->id,constrained.fixed_ids)));
    const auto unavailable=constrained.BuildDerivedReport({{"difference",&constrained.derived}});
    assert(!constrained.derived_report.success_m && std::isnan(unavailable[0]));
    Saddle saddle; fit.SetObjectiveFunction(&saddle); fit.Run();
    assert(!saddle.covariance_available && std::isnan(saddle.covariance(0,0)));
    assert(std::isnan(saddle.GetVarianceOfDerivedValue(saddle.x.info->id,saddle.fixed_ids)));
    Invalid invalid; fit.SetObjectiveFunction(&invalid);
    bool rejected=false;
    try { fit.Run(); } catch (const std::runtime_error&) { rejected=true; }
    assert(rejected && !tape.recording && had::g_ADGraph==&tape.graph);
    Variable<double> after_failure=4.; close((after_failure*2.).GetValue(),8.);
    Variable<double> lower=3.; lower.SetMinBoundary(1.);
    close(physical_parameter(optimizer_parameter(lower),lower),3.);
    Variable<double> upper=3.; upper.SetMaxBoundary(5.);
    close(physical_parameter(optimizer_parameter(upper),upper),3.);
    auto flat_gradient=[](const std::vector<double>&) { return std::vector<double>{0.}; };
    const auto rejected_covariance=quadra::estimate_fixed_effect_covariance_from_gradient(
            flat_gradient,std::vector<double>{1.});
    assert(!rejected_covariance.success_m);
    assert(rejected_covariance.message_m.find("not positive definite")!=std::string::npos);
    std::cout << "Quadra backend tests passed\n";
}
