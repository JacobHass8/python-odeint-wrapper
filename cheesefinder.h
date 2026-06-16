#include <boost/numeric/odeint.hpp>
#include <vector>
#include <iostream>
#include <functional>
#include <utility>

using namespace boost::numeric::odeint;
typedef std::vector<double> (*cheesefunc)(char *name, void *user_data);
typedef std::vector<double> (*derv_func)(std::vector<double> vec);
typedef std::vector<double> (*py_derv_func)(std::vector<double> vec, void *user_data);
typedef std::pair<std::vector<double>, std::vector<double> > state_t;
typedef symplectic_rkn_sb3a_mclachlan<std::vector<double> > stepper_type;
typedef std::tuple<
    std::vector<std::vector<double> >, 
    std::vector<std::vector<double> >, 
    std::vector<double>> result;

static char *cheeses[] = {
  "cheddar",
  "camembert",
  "that runny one",
  0
};

void find_cheeses(cheesefunc user_func, void *user_data) {
  char **p = cheeses;
  std::vector<double> val;
  auto new_user_func = std::bind(user_func, std::placeholders::_1, user_data);
  while (*p) {
    val = new_user_func(*p);
    ++p;
  }
}

template <class FuncType>
struct D_wrapper {
    const FuncType f_pq2dpq;

    D_wrapper (const FuncType f) : f_pq2dpq(f) {}

    void operator()(const std::vector<double> &pq, std::vector<double> &dpqdt) const {
        std::vector<double> tmp = f_pq2dpq(pq);
        for (size_t i = 0; i < pq.size(); ++i) dpqdt[i] = tmp[i];
    }
};

struct Observer
{
    std::vector<std::vector<double> >& m_p;
    std::vector<std::vector<double> >& m_q;
    std::vector<double>& m_times;

    Observer(std::vector<std::vector<double> > &p, std::vector<std::vector<double> > &q, std::vector< double > &times)
    : m_p(p) , m_q(q), m_times(times) { }

    void operator()(const state_t &x , double t) const
    {
        m_q.push_back( x.first );
        m_p.push_back( x.second );
        m_times.push_back( t );
    }
};

struct ResultCpp
{
    std::vector<std::vector<double> > m_p;
    std::vector<std::vector<double> > m_q;
    std::vector<double> m_times;

    ResultCpp(std::vector<std::vector<double> > &p, std::vector<std::vector<double> > &q, std::vector<double> &times)
    : m_p(p), m_q(q), m_times(times) { }
};

ResultCpp* solve_ivp_symplectic (
    double dt, double t0, double tn,
    std::vector<double> q0, std::vector<double>  p0,
    py_derv_func py_q2dp, void* user_data,
    py_derv_func py_p2dq, void* user_data2)
{
    std::vector<std::vector<double> > m_p;
    std::vector<std::vector<double> > m_q;
    std::vector<double> m_t;
    Observer observer(m_p, m_q, m_t);

    auto q2dp = [&py_q2dp, &user_data](std::vector<double> vec) -> std::vector<double>
    {
      return py_q2dp(vec, user_data);
    };

    auto p2dq = [&py_p2dq, &user_data2](std::vector<double> vec) -> std::vector<double>
    {
      return py_p2dq(vec, user_data2);
    };

    integrate_const(
        stepper_type() ,
        std::make_pair( D_wrapper( q2dp ) , D_wrapper( p2dq ) ) ,
        std::make_pair( boost::ref( q0 ) , boost::ref( p0 ) ) ,
        t0 , 
        tn , 
        dt ,  
        observer);
    return new ResultCpp(m_p, m_q, m_t);
}