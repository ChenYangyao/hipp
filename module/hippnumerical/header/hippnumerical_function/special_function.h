/**
create: Yangyao CHEN, 2021/06/18
    [write   ] SpecialFunction - special functions.
*/

#ifndef _HIPPNUMERICAL_SPECIAL_FUNCTION_H_
#define _HIPPNUMERICAL_SPECIAL_FUNCTION_H_
#include <gsl/gsl_sf_result.h>
#include <gsl/gsl_sf_airy.h>
#include "function_base.h"
namespace HIPP::NUMERICAL {
namespace _hippnumerical_special_function_helper {

struct SFBase {
    typedef ErrGSL::errno_t errno_t;

    typedef double value_t;
    typedef unsigned int uzero_loc_t;
    typedef int order_t;
    typedef gsl_sf_result result_t;
    typedef gsl_sf_result_e10 result_e10_t;
    typedef std::pair<errno_t, result_t> e_result_t;
    typedef std::pair<errno_t, result_e10_t> e_result_e10_t;

    typedef gsl_mode_t mode_t;
    enum : mode_t { 
        PREC_DOUBLE = GSL_PREC_DOUBLE, 
        PREC_SINGLE = GSL_PREC_SINGLE, 
        PREC_APPROX = GSL_PREC_APPROX };
protected:
    template<typename F, typename ...Args>
    static e_result_t _get_e_result(F f, Args &&...args){
        result_t res;
        errno_t e = f(std::forward<Args>(args)..., &res);
        return {e, res};
    }
};

struct Airy : public SFBase {
    static value_t Ai(value_t x, mode_t mode = PREC_DOUBLE);
    static value_t Bi(value_t x, mode_t mode = PREC_DOUBLE);
    static value_t Ai_scaled(value_t x, mode_t mode = PREC_DOUBLE);
    static value_t Bi_scaled(value_t x, mode_t mode = PREC_DOUBLE);
    static value_t zero_Ai(uzero_loc_t s);
    static value_t zero_Bi(uzero_loc_t s);
    static value_t Ai_deriv(value_t x, mode_t mode = PREC_DOUBLE);
    static value_t Bi_deriv(value_t x, mode_t mode = PREC_DOUBLE);
    static value_t Ai_deriv_scaled(value_t x, mode_t mode = PREC_DOUBLE);
    static value_t Bi_deriv_scaled(value_t x, mode_t mode = PREC_DOUBLE);
    static value_t zero_Ai_deriv(uzero_loc_t s);
    static value_t zero_Bi_deriv(uzero_loc_t s);

    static e_result_t Ai_e(value_t x, mode_t mode = PREC_DOUBLE);
    static e_result_t Bi_e(value_t x, mode_t mode = PREC_DOUBLE);
    static e_result_t Ai_scaled_e(value_t x, mode_t mode = PREC_DOUBLE);    
    static e_result_t Bi_scaled_e(value_t x, mode_t mode = PREC_DOUBLE);
    static e_result_t zero_Ai_e(uzero_loc_t s);
    static e_result_t zero_Bi_e(uzero_loc_t s);
    static e_result_t Ai_deriv_e(value_t x, mode_t mode = PREC_DOUBLE);
    static e_result_t Bi_deriv_e(value_t x, mode_t mode = PREC_DOUBLE);
    static e_result_t Ai_deriv_scaled_e(value_t x, mode_t mode = PREC_DOUBLE);
    static e_result_t Bi_deriv_scaled_e(value_t x, mode_t mode = PREC_DOUBLE);
    static e_result_t zero_Ai_deriv_e(uzero_loc_t s);
    static e_result_t zero_Bi_deriv_e(uzero_loc_t s);
};

struct Bessel : public SFBase {
    static value_t J0(value_t x);
    static value_t J1(value_t x);
    static value_t Jn(order_t n, value_t x);
    static errno_t Jn_array(order_t nmin, order_t nmax, value_t x, value_t *res);
    static value_t Y0(value_t x);
    static value_t Y1(value_t x);
    static value_t Yn(order_t n, value_t x);
    static errno_t Yn_array(order_t nmin, order_t nmax, value_t x, value_t *res);
    static value_t I0(value_t x);
    static value_t I1(value_t x);
    static value_t In(order_t n, value_t x);
    static errno_t In_array(order_t nmin, order_t nmax, value_t x, value_t *res);
    static value_t I0_scaled(value_t x);
    static value_t I1_scaled(value_t x);
    static value_t In_scaled(order_t n, value_t x);
    static errno_t In_scaled_array(order_t nmin, order_t nmax, value_t x, value_t *res);
    static value_t K0(value_t x);
    static value_t K1(value_t x);
    static value_t Kn(order_t n, value_t x);
    static errno_t Kn_array(order_t nmin, order_t nmax, value_t x, value_t *res);
    static value_t K0_scaled(value_t x);
    static value_t K1_scaled(value_t x);
    static value_t Kn_scaled(order_t n, value_t x);
    static errno_t Kn_scaled_array(order_t nmin, order_t nmax, value_t x, value_t *res);
    
    static value_t j0(value_t x);
    static value_t j1(value_t x);
    static value_t j2(value_t x);
    static value_t jl(order_t l, value_t x);
    static errno_t jl_array(order_t lmax, value_t x, value_t *res);
    static errno_t jl_steed_array(order_t lmax, value_t x, value_t *res);
    static value_t y0(value_t x);
    static value_t y1(value_t x);
    static value_t y2(value_t x);
    static value_t yl(order_t l, value_t x);
    static errno_t yl_array(order_t lmax, value_t x, value_t *res);
    static value_t i0_scaled(value_t x);
    static value_t i1_scaled(value_t x);
    static value_t i2_scaled(value_t x);
    static value_t il_scaled(order_t l, value_t x);
    static errno_t il_scaled_array(order_t lmax, value_t x, value_t *res);
    static value_t k0_scaled(value_t x);
    static value_t k1_scaled(value_t x);
    static value_t k2_scaled(value_t x);
    static value_t kl_scaled(order_t l, value_t x);
    static errno_t kl_scaled_array(order_t lmax, value_t x, value_t *res);

    static value_t Jnu(value_t nu, value_t x);
    static value_t sequence_Jnu_e(value_t nu, mode_t mode, size_t sz, value_t *res);
    static value_t Ynu(value_t nu, value_t x);
    static value_t Inu(value_t nu, value_t x);
    static value_t Inu_scaled(value_t nu, value_t x);
    static value_t Knu(value_t nu, value_t x);
    static value_t lnKnu(value_t nu, value_t x);
    static value_t Knu_scaled(value_t nu, value_t x);

    static value_t zero_J0(uzero_loc_t s);
    static value_t zero_J1(uzero_loc_t s);
    static value_t zero_Jnu(value_t nu, uzero_loc_t s);

    static e_result_t J0_e(value_t x);
    static e_result_t J1_e(value_t x);
    static e_result_t Jn_e(order_t n, value_t x);
    static e_result_t Y0_e(value_t x);
    static e_result_t Y1_e(value_t x);
    static e_result_t Yn_e(order_t n, value_t x);
    static e_result_t I0_e(value_t x);
    static e_result_t I1_e(value_t x);
    static e_result_t In_e(order_t n, value_t x);
    static e_result_t I0_scaled_e(value_t x);
    static e_result_t I1_scaled_e(value_t x);
    static e_result_t In_scaled_e(order_t n, value_t x);
    static e_result_t K0_e(value_t x);
    static e_result_t K1_e(value_t x);
    static e_result_t Kn_e(order_t n, value_t x);
    static e_result_t K0_scaled_e(value_t x);
    static e_result_t K1_scaled_e(value_t x);
    static e_result_t Kn_scaled_e(order_t n, value_t x);
    
    static e_result_t j0_e(value_t x);
    static e_result_t j1_e(value_t x);
    static e_result_t j2_e(value_t x);
    static e_result_t jl_e(order_t l, value_t x);
    static e_result_t y0_e(value_t x);
    static e_result_t y1_e(value_t x);
    static e_result_t y2_e(value_t x);
    static e_result_t yl_e(order_t l, value_t x);
    static e_result_t i0_scaled_e(value_t x);
    static e_result_t i1_scaled_e(value_t x);
    static e_result_t i2_scaled_e(value_t x);
    static e_result_t il_scaled_e(order_t l, value_t x);
    static e_result_t k0_scaled_e(value_t x);
    static e_result_t k1_scaled_e(value_t x);
    static e_result_t k2_scaled_e(value_t x);
    static e_result_t kl_scaled_e(order_t l, value_t x);

    static e_result_t Jnu_e(value_t nu, value_t x);
    static e_result_t Ynu_e(value_t nu, value_t x);
    static e_result_t Inu_e(value_t nu, value_t x);
    static e_result_t Inu_scaled_e(value_t nu, value_t x);
    static e_result_t Knu_e(value_t nu, value_t x);
    static e_result_t lnKnu_e(value_t nu, value_t x);
    static e_result_t Knu_scaled_e(value_t nu, value_t x);

    static e_result_t zero_J0_e(uzero_loc_t s);
    static e_result_t zero_J1_e(uzero_loc_t s);
    static e_result_t zero_Jnu_e(value_t nu, uzero_loc_t s);
};

struct Clausen : public SFBase {
    static value_t clausen(value_t x);
    static e_result_t clausen_e(value_t x);
};

struct Coulomb : public SFBase {
    static value_t hydrogenicR_1(value_t Z, value_t r);
    static value_t hydrogenicR(order_t n, order_t l, value_t Z, value_t r);
    
    static e_result_t hydrogenicR_1_e(value_t Z, value_t r);
    static e_result_t hydrogenicR_e(order_t n, order_t l, value_t Z, value_t r);
};

struct ErrorFunction : public SFBase {
    static value_t erf(value_t x);
    static value_t erfc(value_t x);
    static value_t log_erfc(value_t x);
    static value_t erf_Z(value_t x);
    static value_t erf_Q(value_t x);
    static value_t hazard(value_t x);

    static e_result_t erf_e(value_t x);
    static e_result_t erfc_e(value_t x);
    static e_result_t log_erfc_e(value_t x);
    static e_result_t erf_Z_e(value_t x);
    static e_result_t erf_Q_e(value_t x);
    static e_result_t hazard_e(value_t x);
};

struct Laguerre : public SFBase {
    static value_t laguerre_1(value_t a, value_t x);
    static value_t laguerre_2(value_t a, value_t x);
    static value_t laguerre_3(value_t a, value_t x);
    static value_t laguerre_n(order_t n, value_t a, value_t x);

    static e_result_t laguerre_1_e(value_t a, value_t x);
    static e_result_t laguerre_2_e(value_t a, value_t x);
    static e_result_t laguerre_3_e(value_t a, value_t x);
    static e_result_t laguerre_n_e(order_t n, value_t a, value_t x);
};

struct Legendre : public SFBase {
    static value_t P1(value_t x);
    static value_t P2(value_t x);
    static value_t P3(value_t x);
    static value_t Pl(order_t l, value_t x);
    static value_t Pl_array(order_t lmax, value_t x, value_t *res);
    static value_t Pl_deriv_array(order_t lmax, value_t x, value_t *res, value_t *res_deriv);
    static value_t Q0(value_t x);
    static value_t Q1(value_t x);
    static value_t Ql(order_t l, value_t x);

    static e_result_t P1_e(value_t x);
    static e_result_t P2_e(value_t x);
    static e_result_t P3_e(value_t x);
    static e_result_t Pl_e(order_t l, value_t x);
    static e_result_t Q0_e(value_t x);
    static e_result_t Q1_e(value_t x);
    static e_result_t Ql_e(order_t l, value_t x);
};

struct Power : public SFBase {
    static value_t pow_int(value_t x, order_t n);
    static e_result_t pow_int_e(value_t x, order_t n);
};

} // namespace _hippnumerical_special_function_helper

/** 
SpecialFunction - special functions.
*/ 
class SpecialFunction: public _hippnumerical_special_function_helper::SFBase {
public:    
    typedef _hippnumerical_special_function_helper::Airy Airy;
    typedef _hippnumerical_special_function_helper::Bessel Bessel;
    typedef _hippnumerical_special_function_helper::Clausen Clausen;
    typedef _hippnumerical_special_function_helper::Coulomb Coulomb;
    typedef _hippnumerical_special_function_helper::ErrorFunction ErrorFunction;
    typedef _hippnumerical_special_function_helper::Laguerre Laguerre;
    typedef _hippnumerical_special_function_helper::Legendre Legendre;
    typedef _hippnumerical_special_function_helper::Power Power;
};

} // namespace HIPP::NUMERICAL

#endif	//_HIPPNUMERICAL_SPECIAL_FUNCTION_H_