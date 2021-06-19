#include <special_function.h>
#include <gsl/gsl_sf.h>
namespace HIPP::NUMERICAL {

namespace _hippnumerical_special_function_helper {

/** Airy sf */
auto Airy::Ai(value_t x, mode_t mode) -> value_t {
    return gsl_sf_airy_Ai(x, mode);
}
auto Airy::Bi(value_t x, mode_t mode) -> value_t {
    return gsl_sf_airy_Bi(x, mode);
}
auto Airy::Ai_scaled(value_t x, mode_t mode) -> value_t {
    return gsl_sf_airy_Ai_scaled(x, mode);
}
auto Airy::Bi_scaled(value_t x, mode_t mode) -> value_t {
    return gsl_sf_airy_Bi_scaled(x, mode);
}
auto Airy::zero_Ai(uzero_loc_t s) -> value_t {
    return gsl_sf_airy_zero_Ai(s);
}
auto Airy::zero_Bi(uzero_loc_t s) -> value_t {
    return gsl_sf_airy_zero_Bi(s);
}
auto Airy::Ai_deriv(value_t x, mode_t mode) -> value_t {
    return gsl_sf_airy_Ai_deriv(x, mode);
}
auto Airy::Bi_deriv(value_t x, mode_t mode) -> value_t {
    return gsl_sf_airy_Bi_deriv(x, mode);
}
auto Airy::Ai_deriv_scaled(value_t x, mode_t mode) -> value_t {
    return gsl_sf_airy_Ai_deriv_scaled(x, mode);
}
auto Airy::Bi_deriv_scaled(value_t x, mode_t mode) -> value_t {
    return gsl_sf_airy_Bi_deriv_scaled(x, mode);
}
auto Airy::zero_Ai_deriv(uzero_loc_t s) -> value_t {
    return gsl_sf_airy_zero_Ai_deriv(s);
}
auto Airy::zero_Bi_deriv(uzero_loc_t s) -> value_t {
    return gsl_sf_airy_zero_Bi_deriv(s);
}
auto Airy::Ai_e(value_t x, mode_t mode) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_Ai_e, x, mode);
}
auto Airy::Bi_e(value_t x, mode_t mode) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_Bi_e, x, mode);
}
auto Airy::Ai_scaled_e(value_t x, mode_t mode) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_Ai_scaled_e, x, mode);
}
auto Airy::Bi_scaled_e(value_t x, mode_t mode) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_Bi_scaled_e, x, mode);
}
auto Airy::zero_Ai_e(uzero_loc_t s) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_zero_Ai_e, s);
}
auto Airy::zero_Bi_e(uzero_loc_t s) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_zero_Bi_e, s);
}
auto Airy::Ai_deriv_e(value_t x, mode_t mode) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_Ai_deriv_e, x, mode);
}
auto Airy::Bi_deriv_e(value_t x, mode_t mode) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_Bi_deriv_e, x, mode);
}
auto Airy::Ai_deriv_scaled_e(value_t x, mode_t mode) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_Ai_deriv_scaled_e, x, mode);
}
auto Airy::Bi_deriv_scaled_e(value_t x, mode_t mode) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_Bi_deriv_scaled_e, x, mode);
}
auto Airy::zero_Ai_deriv_e(uzero_loc_t s) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_zero_Ai_deriv_e, s);
}
auto Airy::zero_Bi_deriv_e(uzero_loc_t s) -> e_result_t {
    return _get_e_result(&gsl_sf_airy_zero_Bi_deriv_e, s);
}

/** Bessel sf */
auto Bessel::J0(value_t x) -> value_t {
    return gsl_sf_bessel_J0(x);
}
auto Bessel::J1(value_t x) -> value_t {
    return gsl_sf_bessel_J1(x);
}
auto Bessel::Jn(order_t n, value_t x) -> value_t {
    return gsl_sf_bessel_Jn(n, x);
}
auto Bessel::Jn_array(order_t nmin, order_t nmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_Jn_array(nmin, nmax, x, res);
}
auto Bessel::Y0(value_t x) -> value_t {
    return gsl_sf_bessel_Y0(x);
}
auto Bessel::Y1(value_t x) -> value_t {
    return gsl_sf_bessel_Y1(x);
}
auto Bessel::Yn(order_t n, value_t x) -> value_t {
    return gsl_sf_bessel_Yn(n, x);
}
auto Bessel::Yn_array(order_t nmin, order_t nmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_Yn_array(nmin, nmax, x, res);
}
auto Bessel::I0(value_t x) -> value_t {
    return gsl_sf_bessel_I0(x);
}
auto Bessel::I1(value_t x) -> value_t {
    return gsl_sf_bessel_I1(x);
}
auto Bessel::In(order_t n, value_t x) -> value_t {
    return gsl_sf_bessel_In(n, x);
}
auto Bessel::In_array(order_t nmin, order_t nmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_In_array(nmin, nmax, x, res);
}
auto Bessel::I0_scaled(value_t x) -> value_t {
    return gsl_sf_bessel_I0_scaled(x);
}
auto Bessel::I1_scaled(value_t x) -> value_t {
    return gsl_sf_bessel_I1_scaled(x);
}
auto Bessel::In_scaled(order_t n, value_t x) -> value_t {
    return gsl_sf_bessel_In_scaled(n, x);
}
auto Bessel::In_scaled_array(order_t nmin, order_t nmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_In_scaled_array(nmin, nmax, x, res);
}
auto Bessel::K0(value_t x) -> value_t {
    return gsl_sf_bessel_K0(x);
}
auto Bessel::K1(value_t x) -> value_t {
    return gsl_sf_bessel_K1(x);
}
auto Bessel::Kn(order_t n, value_t x) -> value_t {
    return gsl_sf_bessel_Kn(n, x);
}
auto Bessel::Kn_array(order_t nmin, order_t nmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_Kn_array(nmin, nmax, x, res);
}
auto Bessel::K0_scaled(value_t x) -> value_t {
    return gsl_sf_bessel_K0_scaled(x);
}
auto Bessel::K1_scaled(value_t x) -> value_t {
    return gsl_sf_bessel_K1_scaled(x);
}
auto Bessel::Kn_scaled(order_t n, value_t x) -> value_t {
    return gsl_sf_bessel_Kn_scaled(n, x);
}
auto Bessel::Kn_scaled_array(order_t nmin, order_t nmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_Kn_scaled_array(nmin, nmax, x, res);
}
auto Bessel::j0(value_t x) -> value_t {
    return gsl_sf_bessel_j0(x);
}
auto Bessel::j1(value_t x) -> value_t {
    return gsl_sf_bessel_j1(x);
}
auto Bessel::j2(value_t x) -> value_t {
    return gsl_sf_bessel_j2(x);
}
auto Bessel::jl(order_t l, value_t x) -> value_t {
    return gsl_sf_bessel_jl(l, x);
}
auto Bessel::jl_array(order_t lmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_jl_array(lmax, x, res);
}
auto Bessel::jl_steed_array(order_t lmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_jl_steed_array(lmax, x, res);
}
auto Bessel::y0(value_t x) -> value_t {
    return gsl_sf_bessel_y0(x);
}
auto Bessel::y1(value_t x) -> value_t {
    return gsl_sf_bessel_y1(x);
}
auto Bessel::y2(value_t x) -> value_t {
    return gsl_sf_bessel_y2(x);
}
auto Bessel::yl(order_t l, value_t x) -> value_t {
    return gsl_sf_bessel_yl(l, x);
}
auto Bessel::yl_array(order_t lmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_yl_array(lmax, x, res);
}
auto Bessel::i0_scaled(value_t x) -> value_t {
    return gsl_sf_bessel_i0_scaled(x);
}
auto Bessel::i1_scaled(value_t x) -> value_t {
    return gsl_sf_bessel_i1_scaled(x);
}
auto Bessel::i2_scaled(value_t x) -> value_t {
    return gsl_sf_bessel_i2_scaled(x);
}
auto Bessel::il_scaled(order_t l, value_t x) -> value_t {
    return gsl_sf_bessel_il_scaled(l, x);
}
auto Bessel::il_scaled_array(order_t lmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_il_scaled_array(lmax, x, res);
}
auto Bessel::k0_scaled(value_t x) -> value_t {
    return gsl_sf_bessel_k0_scaled(x);
}
auto Bessel::k1_scaled(value_t x) -> value_t {
    return gsl_sf_bessel_k1_scaled(x);
}
auto Bessel::k2_scaled(value_t x) -> value_t {
    return gsl_sf_bessel_k2_scaled(x);
}
auto Bessel::kl_scaled(order_t l, value_t x) -> value_t {
    return gsl_sf_bessel_kl_scaled(l, x);
}
auto Bessel::kl_scaled_array(order_t lmax, value_t x, value_t *res) -> errno_t {
    return gsl_sf_bessel_kl_scaled_array(lmax, x, res);
}
auto Bessel::Jnu(value_t nu, value_t x) -> value_t {
    return gsl_sf_bessel_Jnu(nu, x);
}
auto Bessel::sequence_Jnu_e(value_t nu, mode_t mode, size_t sz, value_t *res) -> value_t {
    return gsl_sf_bessel_sequence_Jnu_e(nu, mode, sz, res);
}
auto Bessel::Ynu(value_t nu, value_t x) -> value_t {
    return gsl_sf_bessel_Ynu(nu, x);
}
auto Bessel::Inu(value_t nu, value_t x) -> value_t {
    return gsl_sf_bessel_Inu(nu, x);
}
auto Bessel::Inu_scaled(value_t nu, value_t x) -> value_t {
    return gsl_sf_bessel_Inu_scaled(nu, x);
}
auto Bessel::Knu(value_t nu, value_t x) -> value_t {
    return gsl_sf_bessel_Knu(nu, x);
}
auto Bessel::lnKnu(value_t nu, value_t x) -> value_t {
    return gsl_sf_bessel_lnKnu(nu, x);
}
auto Bessel::Knu_scaled(value_t nu, value_t x) -> value_t {
    return gsl_sf_bessel_Knu_scaled(nu, x);
}
auto Bessel::zero_J0(uzero_loc_t s) -> value_t {
    return gsl_sf_bessel_zero_J0(s);
}
auto Bessel::zero_J1(uzero_loc_t s) -> value_t {
    return gsl_sf_bessel_zero_J1(s);
}
auto Bessel::zero_Jnu(value_t nu, uzero_loc_t s) -> value_t {
    return gsl_sf_bessel_zero_Jnu(nu, s);
}

auto  Bessel::J0_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_J0_e, x);
}
auto  Bessel::J1_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_J1_e, x);
}
auto  Bessel::Jn_e(order_t n, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Jn_e, n, x);
}
auto  Bessel::Y0_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Y0_e, x);
}
auto  Bessel::Y1_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Y1_e, x);
}
auto  Bessel::Yn_e(order_t n, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Yn_e, n, x);
}
auto  Bessel::I0_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_I0_e, x);
}
auto  Bessel::I1_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_I1_e, x);
}
auto  Bessel::In_e(order_t n, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_In_e, n, x);
}
auto  Bessel::I0_scaled_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_I0_scaled_e, x);
}
auto  Bessel::I1_scaled_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_I1_scaled_e, x);
}
auto  Bessel::In_scaled_e(order_t n, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_In_scaled_e, n, x);
}
auto  Bessel::K0_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_K0_e, x);
}
auto  Bessel::K1_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_K1_e, x);
}
auto  Bessel::Kn_e(order_t n, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Kn_e, n, x);
}
auto  Bessel::K0_scaled_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_K0_scaled_e, x);
}
auto  Bessel::K1_scaled_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_K1_scaled_e, x);
}
auto  Bessel::Kn_scaled_e(order_t n, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Kn_scaled_e, n, x);
}
auto  Bessel::j0_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_j0_e, x);
}
auto  Bessel::j1_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_j1_e, x);
}
auto  Bessel::j2_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_j2_e, x);
}
auto  Bessel::jl_e(order_t l, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_jl_e, l, x);
}
auto  Bessel::y0_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_y0_e, x);
}
auto  Bessel::y1_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_y1_e, x);
}
auto  Bessel::y2_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_y2_e, x);
}
auto  Bessel::yl_e(order_t l, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_yl_e, l, x);
}
auto  Bessel::i0_scaled_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_i0_scaled_e, x);
}
auto  Bessel::i1_scaled_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_i1_scaled_e, x);
}
auto  Bessel::i2_scaled_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_i2_scaled_e, x);
}
auto  Bessel::il_scaled_e(order_t l, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_il_scaled_e, l, x);
}
auto  Bessel::k0_scaled_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_k0_scaled_e, x);
}
auto  Bessel::k1_scaled_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_k1_scaled_e, x);
}
auto  Bessel::k2_scaled_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_k2_scaled_e, x);
}
auto  Bessel::kl_scaled_e(order_t l, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_kl_scaled_e, l, x);
}
auto  Bessel::Jnu_e(value_t nu, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Jnu_e, nu, x);
}
auto  Bessel::Ynu_e(value_t nu, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Ynu_e, nu, x);
}
auto  Bessel::Inu_e(value_t nu, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Inu_e, nu, x);
}
auto  Bessel::Inu_scaled_e(value_t nu, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Inu_scaled_e, nu, x);
}
auto  Bessel::Knu_e(value_t nu, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Knu_e, nu, x);
}
auto  Bessel::lnKnu_e(value_t nu, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_lnKnu_e, nu, x);
}
auto  Bessel::Knu_scaled_e(value_t nu, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_Knu_scaled_e, nu, x);
}
auto  Bessel::zero_J0_e(uzero_loc_t s) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_zero_J0_e, s);
}
auto  Bessel::zero_J1_e(uzero_loc_t s) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_zero_J1_e, s);
}
auto  Bessel::zero_Jnu_e(value_t nu, uzero_loc_t s) -> e_result_t {
    return _get_e_result(&gsl_sf_bessel_zero_Jnu_e, nu, s);
}

/** Clausen sf */
auto Clausen::clausen(value_t x) -> value_t {
    return gsl_sf_clausen(x);
}
auto Clausen::clausen_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_clausen_e, x);
}

/** Clausen sf */
auto Coulomb::hydrogenicR_1(value_t Z, value_t r) -> value_t {
    return gsl_sf_hydrogenicR_1(Z, r);
}
auto Coulomb::hydrogenicR(order_t n, order_t l, value_t Z, value_t r) -> value_t {
    return gsl_sf_hydrogenicR(n, l, Z, r);
}
auto Coulomb::hydrogenicR_1_e(value_t Z, value_t r) -> e_result_t {
    return _get_e_result(&gsl_sf_hydrogenicR_1_e, Z, r);
}
auto Coulomb::hydrogenicR_e(order_t n, order_t l, value_t Z, value_t r) -> e_result_t {
    return _get_e_result(&gsl_sf_hydrogenicR_e, n, l, Z, r);
}

/** Error sf */
auto ErrorFunction::erf(value_t x) -> value_t {
    return gsl_sf_erf(x);
}
auto ErrorFunction::erfc(value_t x) -> value_t {
    return gsl_sf_erfc(x);
}
auto ErrorFunction::log_erfc(value_t x) -> value_t {
    return gsl_sf_log_erfc(x);
}
auto ErrorFunction::erf_Z(value_t x) -> value_t {
    return gsl_sf_erf_Z(x);
}
auto ErrorFunction::erf_Q(value_t x) -> value_t {
    return gsl_sf_erf_Q(x);
}
auto ErrorFunction::hazard(value_t x) -> value_t {
    return gsl_sf_hazard(x);
}

auto ErrorFunction::erf_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_erf_e, x);
}
auto ErrorFunction::erfc_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_erfc_e, x);
}
auto ErrorFunction::log_erfc_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_log_erfc_e, x);
}
auto ErrorFunction::erf_Z_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_erf_Z_e, x);
}
auto ErrorFunction::erf_Q_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_erf_Q_e, x);
}
auto ErrorFunction::hazard_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_hazard_e, x);
}

/** Laguerre sf */
auto Laguerre::laguerre_1(value_t a, value_t x) -> value_t {
    return gsl_sf_laguerre_1(a, x);
}
auto Laguerre::laguerre_2(value_t a, value_t x) -> value_t {
    return gsl_sf_laguerre_2(a, x);
}
auto Laguerre::laguerre_3(value_t a, value_t x) -> value_t {
    return gsl_sf_laguerre_3(a, x);
}
auto Laguerre::laguerre_n(order_t n, value_t a, value_t x) -> value_t {
    return gsl_sf_laguerre_n(n, a, x);
}
auto Laguerre::laguerre_1_e(value_t a, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_laguerre_1_e, a, x);
}
auto Laguerre::laguerre_2_e(value_t a, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_laguerre_2_e, a, x);
}
auto Laguerre::laguerre_3_e(value_t a, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_laguerre_3_e, a, x);
}
auto Laguerre::laguerre_n_e(order_t n, value_t a, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_laguerre_n_e, n, a, x);
}

/** Legendre sf */
auto Legendre::P1(value_t x) -> value_t {
    return gsl_sf_legendre_P1(x);
}
auto Legendre::P2(value_t x) -> value_t {
    return gsl_sf_legendre_P2(x);
}
auto Legendre::P3(value_t x) -> value_t {
    return gsl_sf_legendre_P3(x);
}
auto Legendre::Pl(order_t l, value_t x) -> value_t {
    return gsl_sf_legendre_Pl(l, x);
}
auto Legendre::Pl_array(order_t lmax, value_t x, value_t *res) -> value_t {
    return gsl_sf_legendre_Pl_array(lmax, x, res);
}
auto Legendre::Pl_deriv_array(order_t lmax, value_t x, value_t *res, value_t *res_deriv) -> value_t {
    return gsl_sf_legendre_Pl_deriv_array(lmax, x, res, res_deriv);
}
auto Legendre::Q0(value_t x) -> value_t {
    return gsl_sf_legendre_Q0(x);
}
auto Legendre::Q1(value_t x) -> value_t {
    return gsl_sf_legendre_Q1(x);
}
auto Legendre::Ql(order_t l, value_t x) -> value_t {
    return gsl_sf_legendre_Ql(l, x);
}

auto Legendre::P1_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_legendre_P1_e, x);
}
auto Legendre::P2_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_legendre_P2_e, x);
}
auto Legendre::P3_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_legendre_P3_e, x);
}
auto Legendre::Pl_e(order_t l, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_legendre_Pl_e, l, x);
}
auto Legendre::Q0_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_legendre_Q0_e, x);
}
auto Legendre::Q1_e(value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_legendre_Q1_e, x);
}
auto Legendre::Ql_e(order_t l, value_t x) -> e_result_t {
    return _get_e_result(&gsl_sf_legendre_Ql_e, l, x);
}

/** Power sf */
auto Power::pow_int(value_t x, order_t n) -> value_t {
    return gsl_sf_pow_int(x, n);
}
auto Power::pow_int_e(value_t x, order_t n) -> e_result_t {
    return _get_e_result(&gsl_sf_pow_int_e, x, n);
}



} // namespace _hippnumerical_special_function_helper



} // namespace HIPP::NUMERICAL