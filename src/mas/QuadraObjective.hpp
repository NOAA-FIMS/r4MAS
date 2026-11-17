#ifndef MAS_QUADRA_OBJECTIVE_HPP
#define MAS_QUADRA_OBJECTIVE_HPP

#include "QuadraBackend.hpp"
#include "third_party/Quadra/core/optimizer.hpp"
#include "third_party/Quadra/core/inference/fixed_effect_report.hpp"
#include "third_party/Quadra/core/laplace/laplace_mode_sensitivity.hpp"
#include "third_party/Quadra/core/laplace/laplace_profiled_delta_method.hpp"
#include "third_party/Quadra/core/laplace/laplace_profiled_derived_report.hpp"

namespace mas { namespace backend {

template<class T> class ObjectiveFunction {
public:
    using variable = Variable<T>;
    int phase_m = 1;
    std::vector<variable*> parameters;
    std::vector<int> phases;
    std::vector<variable*> random;
    RealMatrix<T> covariance;
    quadra::FixedEffectCovarianceResult covariance_result;
    quadra::FixedEffectReport fixed_effect_report;
    quadra::LaplaceProfiledDerivedReport derived_report;
    std::vector<std::string> unrecorded_derived_quantities;
    int covariance_rank = 0;
    bool covariance_available = true;
    bool converged = false;
    std::string covariance_status = "not computed";
    std::string optimizer_status = "not fitted";
    Eigen::MatrixXd mode_sensitivity;
    std::vector<unsigned> fixed_ids, random_ids;
    virtual ~ObjectiveFunction() = default;
    virtual void Initialize() {}
    virtual variable Evaluate() { variable f; Objective_Function(f); return f; }
    virtual void Objective_Function(variable&) = 0;
    virtual void Finalize() {}
    void RegisterParameter(variable& v, int phase = 1) {
        if (std::find(parameters.begin(), parameters.end(), &v) == parameters.end()) {
            parameters.push_back(&v); phases.push_back(phase);
        }
    }
    void RegisterRandomVariable(variable& v) {
        if (std::find(random.begin(), random.end(), &v) == random.end()) random.push_back(&v);
    }
    const RealMatrix<T>& GetVarianceCovariance() const { return covariance; }
    // Rebuild once on the physical scale after fitting. MAS output, goodness of
    // fit and delta-method calculations then share valid graph vertex IDs.
    variable RecordPhysical() {
        tape.Reset(); tape.recording = true;
        fixed_ids.clear(); random_ids.clear();
        for (auto p : parameters) fixed_ids.push_back(p->AD().varId);
        for (auto p : random) random_ids.push_back(p->AD().varId);
        auto f = Evaluate();
        tape.Reverse(f.AD().varId);
        return f;
    }
    quadra::ProfiledADGradientResult DerivedGradient(unsigned id) {
        quadra::ProfiledADGradientResult result;
        auto& graph = *had::g_ADGraph;
        if (id >= graph.vertices.size()) {
            result.message_m = "Derived quantity has no current graph vertex.";
            return result;
        }
        // The MAS adapter reads gradients from its already recorded Quadra
        // graph. Quadra's reporting routines own uncertainty propagation.
        struct RestoreAdjoints {
            had::ADGraph& graph;
            std::vector<double> saved;
            explicit RestoreAdjoints(had::ADGraph& g) : graph(g) {
                for (const auto& vertex : graph.vertices) saved.push_back(vertex.w);
            }
            ~RestoreAdjoints() {
                for (std::size_t i=0; i<saved.size(); ++i) graph.vertices[i].w=saved[i];
            }
        } restore(graph);
        result.estimate_m = graph.vertices[id].primal;
        tape.Reverse(id, false);
        std::vector<quadra::AD> fixed, random_effects;
        for (unsigned vertex : fixed_ids) fixed.emplace_back(0.,vertex);
        for (unsigned vertex : random_ids) random_effects.emplace_back(0.,vertex);
        result.gradient_fixed_m = quadra::extract_gradient(fixed);
        result.gradient_random_m = quadra::extract_gradient(random_effects);
        result.success_m = std::isfinite(result.estimate_m) &&
                result.gradient_fixed_m.allFinite() && result.gradient_random_m.allFinite();
        result.message_m = result.success_m ? "Gradients extracted from the MAS Quadra graph."
                : "Derived quantity or gradient is non-finite.";
        return result;
    }
    T GetVarianceOfDerivedValue(unsigned id, const std::vector<uint32_t>& ids) {
        if (!covariance_available) return std::numeric_limits<T>::quiet_NaN();
        if (ids != fixed_ids)
            throw std::runtime_error("Quadra: derived parameter IDs do not match the fitted graph");
        const auto blocks = DerivedGradient(id);
        if (!blocks.success_m) return std::numeric_limits<T>::quiet_NaN();
        const auto result = quadra::compute_laplace_profiled_delta_method(
                blocks.estimate_m, blocks.gradient_fixed_m, blocks.gradient_random_m,
                mode_sensitivity, covariance);
        return result.success_m ? result.variance_m : std::numeric_limits<T>::quiet_NaN();
    }
    // Returned variances follow the supplied order. Missing graph outputs and
    // failed Quadra reports remain unavailable rather than becoming zero SEs.
    std::vector<T> BuildDerivedReport(
            const std::vector<std::pair<std::string,variable*>>& quantities) {
        const T missing = std::numeric_limits<T>::quiet_NaN();
        std::vector<T> variances(quantities.size(),missing);
        derived_report = quadra::LaplaceProfiledDerivedReport();
        unrecorded_derived_quantities.clear();
        std::vector<std::size_t> indices;
        for (std::size_t i=0;i<quantities.size();++i) {
            const auto& quantity=quantities[i];
            if (!quantity.second->IsRecorded()) {
                unrecorded_derived_quantities.push_back(quantity.first);
                continue;
            }
            indices.push_back(i);
            derived_report.names_m.push_back(quantity.first);
        }
        if (!covariance_available) {
            derived_report.message_m="Derived report unavailable: "+covariance_status;
            return variances;
        }
        if (indices.empty()) {
            derived_report.success_m=quantities.empty();
            derived_report.message_m=quantities.empty() ? "No derived quantities supplied."
                    : "No derived quantities are recorded in the fitted graph.";
            return variances;
        }
        Eigen::VectorXd estimates(indices.size());
        Eigen::MatrixXd fixed(indices.size(),fixed_ids.size());
        Eigen::MatrixXd random_effects(indices.size(),random_ids.size());
        for (std::size_t k=0;k<indices.size();++k) {
            auto block=DerivedGradient(quantities[indices[k]].second->info->id);
            if (!block.success_m) {
                derived_report.message_m=block.message_m;
                return variances;
            }
            estimates[k]=block.estimate_m;
            fixed.row(k)=block.gradient_fixed_m.transpose();
            random_effects.row(k)=block.gradient_random_m.transpose();
        }
        derived_report.delta_m=quadra::compute_laplace_profiled_delta_method_vector(
                estimates,fixed,random_effects,mode_sensitivity,covariance);
        derived_report.success_m=derived_report.delta_m.success_m;
        derived_report.message_m=derived_report.delta_m.message_m;
        if (derived_report.success_m)
            for (std::size_t k=0;k<indices.size();++k)
                variances[indices[k]]=derived_report.delta_m.covariance_m(k,k);
        return variances;
    }

};

// Map finite bounds onto an unconstrained optimizer coordinate. Fixed-effect
// transforms are numerical parameterizations and do not add a Jacobian to NLL.
template<class Scalar, class T>
Scalar physical_parameter(const Scalar& x, const Variable<T>& p) {
    using std::exp; using had::exp;
    const double lo=p.GetMinBoundary(), hi=p.GetMaxBoundary();
    if (std::isfinite(lo) && std::isfinite(hi)) {
        if (lo >= hi) throw std::runtime_error("Quadra: parameter bounds must have min < max");
        if (quadra::value_of(x) >= 0.) return lo+(hi-lo)/(1.+exp(-x));
        auto e=exp(x); return lo+(hi-lo)*e/(1.+e);
    }
    if (std::isfinite(lo)) return lo+exp(x);
    if (std::isfinite(hi)) return hi-exp(x);
    return x;
}
template<class T> double optimizer_parameter(const Variable<T>& p) {
    double x=p.GetValue(), lo=p.GetMinBoundary(), hi=p.GetMaxBoundary();
    if (!std::isfinite(x) || x<lo || x>hi || lo>=hi)
        throw std::runtime_error("Quadra: invalid initial value or parameter bounds: "+p.GetName());
    if (std::isfinite(lo) && std::isfinite(hi)) {
        double r=std::max(1e-8,std::min(1.-1e-8,(x-lo)/(hi-lo)));
        return std::log(r/(1.-r));
    }
    if (std::isfinite(lo)) return std::log(std::max(1e-8,x-lo));
    if (std::isfinite(hi)) return std::log(std::max(1e-8,hi-x));
    return x;
}

template<class T> struct ModelAdapter {
    ObjectiveFunction<T>& objective;
    std::vector<Variable<T>*> active;
    bool transform = true;
    template<class Scalar> Scalar operator()(const std::vector<Scalar>& x) {
        constexpr bool third = std::is_same<Scalar,had::ThirdOrderScalar>::value;
        tape.Attach(third); tape.recording = true;
        for (std::size_t i=0; i<active.size(); ++i) {
            Scalar value=transform ? physical_parameter(x[i],*active[i]) : x[i];
            *active[i]=Variable<T>::From(value);
        }
        auto loss=objective.Evaluate();
        if constexpr (third) return loss.Third();
        else return loss.AD();
    }
};

template<class T> class LBFGS {
    ObjectiveFunction<T>* objective = nullptr;
    double tolerance = 1e-5;
public:
    int max_iterations = 1000;
    int max_line_searches = 50;
    int print_interval = 25;
    bool converged = false;
    void SetPrintWidth(int) {}
    void SetTolerance(double v) { tolerance=v; }
    void SetObjectiveFunction(ObjectiveFunction<T>* p) { objective=p; }
    void Run() {
        struct GraphGuard {
            GraphGuard() { tape.Reset(); tape.recording=false; }
            ~GraphGuard() {
                tape.recording=false; tape.third_order=false;
                had::g_ADGraph=&tape.graph;
            }
        } graph_guard;
        if (!objective) throw std::runtime_error("Quadra: objective was not set");
        if (max_iterations<=0 || max_line_searches<=0 || tolerance<=0.)
            throw std::runtime_error("Quadra: invalid optimizer controls");
        if (objective->parameters.empty() && objective->random.empty()) {
            objective->converged=true;
            objective->optimizer_status="no estimated parameters";
        }
        int last=1;
        for (int phase: objective->phases) last=std::max(last,phase);
        // Random effects are integrated in their original coordinate system.
        // Transforming their bounds would change the modeled integration domain.
        for (auto p: objective->random)
            if (p->bounded_m) throw std::runtime_error("Quadra: bounded random effects are unsupported");
        for (int phase=1; phase<=last; ++phase) {
            objective->phase_m=phase;
            ModelAdapter<T> model{*objective,{},true};
            quadra::ParameterVector params;
            for (std::size_t i=0;i<objective->parameters.size();++i)
                if (objective->phases[i]<=phase) {
                    auto p=objective->parameters[i]; model.active.push_back(p);
                    params.add(quadra::Parameter(p->GetName(),optimizer_parameter(*p)));
                }
            const std::size_t nfixed=model.active.size();
            for (auto p: objective->random) {
                model.active.push_back(p);
                params.add(quadra::Parameter(p->GetName(),p->GetValue(),quadra::ParameterTransform::Identity,true));
            }
            if (!nfixed) {
                if (!objective->random.empty())
                    throw std::runtime_error("Quadra: random effects require at least one active fixed parameter");
                continue;
            }
            auto fixed=quadra::build_fixed_index(params), random=quadra::build_random_index(params);
            quadra::LaplaceOptions options;
            options.use_hutchinson_trace=false;
            quadra::LBFGSObjective<ModelAdapter<T>> fun(model,params,fixed,random,options);
            fun.print_every=print_interval>0 ? print_interval : std::numeric_limits<int>::max();
            fun.epsilon=tolerance;
            LBFGSpp::LBFGSParam<double> control;
            control.max_iterations=max_iterations;
            control.max_linesearch=max_line_searches;
            control.epsilon=tolerance; control.epsilon_rel=0.;
            control.past=5; control.delta=1e-14;
            LBFGSpp::LBFGSSolver<double> solver(control);
            Eigen::VectorXd x(nfixed);
            for (std::size_t i=0;i<nfixed;++i) x[i]=params.params[i].value;
            double value=0.;
            std::string stop_reason="stopped before gradient tolerance";
            Eigen::VectorXd grad;
            if (random.empty()) {
                // Fixed-effect fits need only a first-order reverse sweep.
                // Quadra's mixed-effects objective also evaluates Hessians and
                // third-order Laplace terms when the random block is empty.
                Eigen::VectorXd converged_point, best_point;
                double best_value=std::numeric_limits<double>::infinity();
                bool stop_on_gradient=true;
                int evaluations=0;
                auto fixed_objective = [&](const Eigen::VectorXd& point, Eigen::VectorXd& gradient) {
                    quadra::TapeContext context;
                    auto independent=quadra::to_ad(point);
                    auto loss=model(independent);
                    tape.Reverse(loss.varId,false);
                    gradient=quadra::extract_gradient(independent);
                    if (!std::isfinite(loss.val) || !gradient.allFinite())
                        throw std::runtime_error("Quadra: non-finite fixed-effect objective or gradient");
                    if (loss.val<best_value) { best_value=loss.val; best_point=point; }
                    ++evaluations;
                    if (print_interval>0 && (evaluations==1 || evaluations%print_interval==0))
                        std::cout << "Quadra evaluation " << evaluations << ": objective=" << loss.val
                                  << ", |gradient|=" << gradient.norm() << std::endl;
                    if (stop_on_gradient && gradient.norm()<=tolerance) {
                        converged_point=point;
                        throw quadra::LBFGSConvergedByGradient();
                    }
                    return loss.val;
                };
                try { solver.minimize(fixed_objective,x,value); }
                catch (const quadra::LBFGSConvergedByGradient&) { x=converged_point; }
                catch (const std::runtime_error& error) {
                    std::string message=error.what();
                    if (message.find("line search")==std::string::npos || best_point.size()!=x.size()) throw;
                    x=best_point;
                    stop_reason="line search stopped before gradient tolerance";
                }
                stop_on_gradient=false;
                value=fixed_objective(x,grad);
            } else {
                try { solver.minimize(fun,x,value); }
                catch (const quadra::LBFGSConvergedByGradient&) {
                    x=fun.best_converged_x; value=fun.best_converged_fx;
                }
                catch (const std::runtime_error& error) {
                    std::string message=error.what();
                    if (message.find("line search")==std::string::npos || !fun.best_available) throw;
                    x=fun.best_x;
                    stop_reason="line search stopped before gradient tolerance";
                }
                fun.epsilon=-1.;
                value=fun(x,grad);
            }
            if (!std::isfinite(value) || value>=1e100 || !grad.allFinite())
                throw std::runtime_error("Quadra: final objective or gradient is not finite");
            converged=std::isfinite(value) && grad.norm()<=tolerance;
            objective->converged=converged;
            objective->optimizer_status=converged ? "converged" : stop_reason;
            std::cout << "Quadra phase " << phase << ": "
                      << (converged ? "converged" : stop_reason)
                      << ", objective=" << value << ", |gradient|=" << grad.norm() << "\n";
        }
        ComputeCovariance();
    }
private:
    void ComputeCovariance() {
        const std::size_t n=objective->parameters.size(), m=objective->random.size();
        Eigen::VectorXd fitted(n), modes(m);
        for (std::size_t i=0;i<n;++i) fitted[i]=objective->parameters[i]->GetValue();
        for (std::size_t i=0;i<m;++i) modes[i]=objective->random[i]->GetValue();
        auto loss=objective->RecordPhysical();
        Eigen::MatrixXd H(n,n), Huu(m,m), Huf(m,n);
        for (std::size_t i=0;i<n;++i)
            for (std::size_t j=0;j<n;++j) H(i,j)=tape.Value(objective->fixed_ids[i],objective->fixed_ids[j]);
        Eigen::VectorXd scales(n);
        for (std::size_t i=0;i<n;++i)
            scales[i]=H(i,i)>0. ? 1./std::sqrt(H(i,i)) : 1.;
        objective->mode_sensitivity=Eigen::MatrixXd::Zero(m,n);
        quadra::LaplaceModeSensitivityResult sensitivity;
        if (m) {
            for (std::size_t i=0;i<m;++i) {
                for (std::size_t j=0;j<m;++j) Huu(i,j)=tape.Value(objective->random_ids[i],objective->random_ids[j]);
                for (std::size_t j=0;j<n;++j) Huf(i,j)=tape.Value(objective->random_ids[i],objective->fixed_ids[j]);
            }
            sensitivity=quadra::solve_dense_laplace_mode_sensitivity(Huu,Huf);
            if (sensitivity.success_m) objective->mode_sensitivity=sensitivity.du_dtheta_m;
            ModelAdapter<T> model{*objective,objective->parameters,false};
            quadra::ParameterVector params;
            for (auto p: objective->parameters) params.add(quadra::Parameter(p->GetName(),p->GetValue()));
            for (auto p: objective->random) {
                model.active.push_back(p);
                params.add(quadra::Parameter(p->GetName(),p->GetValue(),quadra::ParameterTransform::Identity,true));
            }
            quadra::LaplaceOptions options; options.use_hutchinson_trace=false;
            quadra::LBFGSObjective<ModelAdapter<T>> fun(model,params,
                    quadra::build_fixed_index(params),quadra::build_random_index(params),options);
            fun.print_every=std::numeric_limits<int>::max();
            fun.epsilon=-1.;
            // Center and scale coordinates for catchability-sized parameters.
            // Quadra chooses difference steps and validates/inverts the Hessian.
            auto gradient=[&](const std::vector<double>& z) {
                Eigen::VectorXd point=fitted+scales.asDiagonal()*quadra::to_eigen(z), g;
                const double value=fun(point,g);
                if (!std::isfinite(value) || value>=1e100 || !g.allFinite())
                    throw std::runtime_error("Quadra: invalid Laplace evaluation during uncertainty reporting");
                g=scales.asDiagonal()*g;
                return std::vector<double>(g.data(),g.data()+g.size());
            };
            objective->covariance_result=quadra::estimate_fixed_effect_covariance_from_gradient(
                    gradient,std::vector<double>(n,0.));
        } else if (n) {
            const Eigen::MatrixXd normalized=scales.asDiagonal()*H*scales.asDiagonal();
            objective->covariance_result=quadra::estimate_fixed_effect_covariance_from_hessian(normalized);
        } else {
            objective->covariance_result=quadra::FixedEffectCovarianceResult();
            objective->covariance_result.covariance_m.resize(0,0);
            objective->covariance_result.hessian_m.resize(0,0);
            objective->covariance_result.success_m=true;
            objective->covariance_result.message_m="No estimated parameters.";
        }
        auto& result=objective->covariance_result;
        if (result.success_m && m && !sensitivity.success_m) {
            result.success_m=false;
            result.message_m=sensitivity.message_m;
        }
        if (result.success_m && n) {
            result.covariance_m=scales.asDiagonal()*result.covariance_m*scales.asDiagonal();
            result.hessian_m=scales.cwiseInverse().asDiagonal()*result.hessian_m*scales.cwiseInverse().asDiagonal();
            result.correlation_m=quadra::covariance_to_correlation(result.covariance_m);
            if (result.steps_m.size()==static_cast<Eigen::Index>(n))
                result.steps_m=scales.cwiseProduct(result.steps_m);
        }
        objective->covariance_available=result.success_m;
        objective->covariance_rank=result.success_m ? static_cast<int>(n) : 0;
        objective->covariance_status=result.success_m ? "available: Quadra covariance report"
                : "unavailable: Quadra covariance validation failed: "+result.message_m;
        objective->covariance.resize(n,n);
        if (result.success_m) objective->covariance=result.covariance_m;
        else {
            objective->covariance.setConstant(std::numeric_limits<T>::quiet_NaN());
            std::cerr << objective->covariance_status << "\n";
        }
        for (std::size_t i=0;i<n;++i) objective->parameters[i]->SetValue(fitted[i]);
        for (std::size_t i=0;i<m;++i) objective->random[i]->SetValue(modes[i]);
        objective->RecordPhysical();
        std::vector<std::string> names;
        std::vector<double> estimates;
        for (auto p: objective->parameters) {
            names.push_back(p->GetName()); estimates.push_back(p->GetValue());
        }
        objective->fixed_effect_report=quadra::build_fixed_effect_report(names,estimates,result);
    }
};
}} // namespace mas::backend
#endif
