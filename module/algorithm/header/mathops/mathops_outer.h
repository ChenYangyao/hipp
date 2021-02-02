#ifndef _HIPPALGORITHM_MATHOPS_OUTER_H_
#define _HIPPALGORITHM_MATHOPS_OUTER_H_
#include <hippcntl.h>
namespace HIPP::ALGORITHM {

class OuterProduct {
public:

    template<typename It, typename UnaryOp>
    static void indices(It b, It e, UnaryOp &&op);

    template<typename It, typename UnaryOp, typename ItStk>
    static void indices(It b, It e, UnaryOp &&op, ItStk it_stk);
};

template<typename It, typename UnaryOp>
void OuterProduct::indices(It b, It e, UnaryOp &&op) {
    typedef decltype((*b)++) target_t;

    std::ptrdiff_t n = e-b;
    if( n <= 0 ) return;
    vector<target_t> stk(n+1);
    indices(b, e, std::forward<UnaryOp>(op), stk.data());
}

template<typename It, typename UnaryOp, typename ItStk>
void OuterProduct::indices(It b, It e, UnaryOp &&op, ItStk it_stk){
    typedef decltype((*b)++) target_t;
    
    std::ptrdiff_t n = e - b, cur_pos = n > 0 ? 0 : -1;
    auto b_stk = it_stk + 1;
    b_stk[cur_pos] = 0;
    while( cur_pos >= 0 ){
        if( static_cast<target_t>(b_stk[cur_pos]) == b[cur_pos] ){
            b_stk[--cur_pos]++;
        }else{
            while( ++cur_pos < n ) b_stk[cur_pos] = 0;
            --cur_pos;
            do{
                std::forward<UnaryOp>(op)(b_stk);
            }while( ++b_stk[cur_pos] < b[cur_pos] );
            b_stk[--cur_pos]++;
        }
    }
}

} // namespace HIPP::ALGORITHM

#endif	//_HIPPALGORITHM_MATHOPS_OUTER_H_