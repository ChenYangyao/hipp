#ifndef _HIPPIO_H5_UTIL_DATAPACKET_IMPL_H_
#define _HIPPIO_H5_UTIL_DATAPACKET_IMPL_H_

#include "../hippio_hdf5/h5_util_datapacket.h"

namespace HIPP::IO::H5 {

inline Datapacket::Datapacket() noexcept
: buff(nullptr), dspace(nullptr), dtype(nullptr) 
{ }

inline Datapacket::Datapacket(void *_buff, 
    Dataspace _dspace, Datatype _dtype) noexcept
: buff(_buff), dspace(std::move(_dspace)), dtype(std::move(_dtype)) 
{ }

template<typename T>
Datapacket::Datapacket(T *_buff, Dataspace _dspace)
: Datapacket(_buff, _dspace, Datatype::from_type<T>() ) 
{ }

template<typename T>
Datapacket::Datapacket(T &x) : Datapacket() {
    constexpr _h5_datapacket_helper::Traits<T> tr;

    typedef Datatype dt_t;
    typedef Dataspace dsp_t;
    typedef GeneralArrayTraits<T> gr_t;

    if constexpr ( tr.is_predef() ) {
        buff = &x;
        dspace = dsp_t::vSCALAR;
        dtype = dt_t::from_type<T>();
    } else if constexpr ( tr.is_gr_predef() ) {
        gr_t gr {x};
        buff = gr.buff();
        dspace = dsp_t( gr.extents() );
        dtype = dt_t::from_type<typename gr_t::value_t >();
    } else if constexpr ( tr.is_gr_rr_predef() ) {
        gr_t gr {x};

        buff = gr.buff();

        typedef RawArrayTraits<typename gr_t::value_t> _rr_t;
        vector<size_t> ext = gr.extents();
        for(const auto &e: _rr_t::extents) ext.push_back(e);
        dspace = dsp_t( ext );

        dtype = dt_t::from_type<typename _rr_t::value_t>();
    } else if constexpr ( tr.is_custom() ) {
        buff = &x;
        dspace = dsp_t::vSCALAR;
        dtype = dt_t::from_type<T>();
    } else if constexpr ( tr.is_gr_custom() ) {
        gr_t gr {x};
        buff = gr.buff();
        dspace = dsp_t( gr.extents() );
        dtype = dt_t::from_type<typename gr_t::value_t >();
    } else {
        gr_t gr {x};

        buff = gr.buff();

        typedef RawArrayTraits<typename gr_t::value_t> _rr_t;
        vector<size_t> ext = gr.extents();
        for(const auto &e: _rr_t::extents) ext.push_back(e);
        dspace = dsp_t( ext );

        dtype = dt_t::from_type<typename _rr_t::value_t>();
    }
}

template<typename T, typename A>
Datapacket Datapacket::resize_buff(vector<T, A> &v, size_t target_sz) {
    
    Datapacket dp;
    size_t mem_sz;
    
    typedef RawArrayTraits<T> rr_t;
    if constexpr ( rr_t::is_array ) {
        const size_t arr_sz = rr_t::size;
        mem_sz = target_sz / arr_sz;
        if( mem_sz * arr_sz != target_sz ) 
            ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
                "  ... target size ", target_sz, 
                " is not multiple of array size ", arr_sz, '\n');
        dp.dtype = TypeCvt<typename rr_t::value_t>::datatype();
        dp.dspace = Dataspace{mem_sz, arr_sz};
    }else {
        mem_sz = target_sz;
        dp.dtype = TypeCvt<T>::datatype();
        dp.dspace = Dataspace{target_sz};
    }
    v.resize(mem_sz);
    dp.buff = v.data();
    
    return dp;
}

inline
std::tuple<void *, Dataspace, Datatype> Datapacket::to_tuple() const {
    return {buff, dspace, dtype};
}

inline DatapacketScalar::DatapacketScalar() noexcept 
{ }

inline DatapacketScalar::DatapacketScalar(void *_buff, 
    Datatype _dtype) noexcept
: Datapacket( _buff, Dataspace::vSCALAR, std::move(_dtype) ) 
{ }

template<typename T>
DatapacketScalar::DatapacketScalar(T &x)
: DatapacketScalar( &x, Datatype::from_type<T>() ) 
{ }

inline DatapacketStr::DatapacketStr() noexcept
{ }

inline DatapacketStr::DatapacketStr(void *_buff, Dataspace _dspace, 
    Datatype _dtype) noexcept
: Datapacket(_buff, std::move(_dspace), std::move(_dtype))
{ }

inline DatapacketStr::DatapacketStr(string &s)
: Datapacket(s.data(), Dataspace::vSCALAR, C_S1_T.resized(s.size()+1) ) 
{ }

template<size_t N_STR>
DatapacketStr::DatapacketStr(string (&s)[N_STR]) 
: DatapacketStr(s, N_STR)
{ }

inline DatapacketStr::DatapacketStr(string *s, size_t n_str) {
    size_t max_n = 0;
    for(size_t i=0; i<n_str; ++i) max_n = std::max(max_n, s[i].size());
    dspace = Dataspace(vector<hsize_t>{n_str});
    dtype = C_S1_T.resized(max_n+1);
}

inline DatapacketStr::DatapacketStr(vector<string> &s)
: DatapacketStr(s.data(), s.size())
{ }

inline DatapacketStr::DatapacketStr(char *s)
: DatapacketStr(s, strlen(s)+1)
{ }

inline DatapacketStr::DatapacketStr(char *s, size_t n)
: DatapacketStr(s, Dataspace::vSCALAR, C_S1_T.resized(n))
{ }

template<size_t N_STR, size_t N>
DatapacketStr::DatapacketStr(char (&s)[N_STR][N]) 
: DatapacketStr(s, N_STR)
{ }

template<size_t N>
DatapacketStr::DatapacketStr(char (*s)[N], size_t n_str)
: DatapacketStr(s, Dataspace(vector<hsize_t>{n_str}), C_S1_T.resized(N))
{ }

inline DatapacketStr::DatapacketStr(char **s, size_t n_str)
{
    size_t max_n = 0;
    for(size_t i=0; i<n_str; ++i) max_n = std::max(max_n, strlen(s[i]));
    dspace = Dataspace(vector<hsize_t>{n_str});
    dtype = C_S1_T.resized(max_n+1);
}

/** Implementation of the const version. */
inline ConstDatapacket::ConstDatapacket() noexcept
: buff(nullptr), dspace(nullptr), dtype(nullptr) 
{ }

inline ConstDatapacket::ConstDatapacket(const void *_buff, 
    Dataspace _dspace, Datatype _dtype) noexcept
: buff(_buff), dspace(std::move(_dspace)), dtype(std::move(_dtype)) 
{ }

template<typename T>
ConstDatapacket::ConstDatapacket(const T *_buff, Dataspace _dspace)
: ConstDatapacket(_buff, _dspace, Datatype::from_type<T>() ) 
{ }

template<typename T>
ConstDatapacket::ConstDatapacket(const T &x) 
: ConstDatapacket()
{
    constexpr _h5_datapacket_helper::Traits<const T> tr;

    typedef Datatype dt_t;
    typedef Dataspace dsp_t;
    typedef GeneralArrayTraits<const T> gr_t;

    if constexpr ( tr.is_predef() ) {
        buff = &x;
        dspace = dsp_t::vSCALAR;
        dtype = dt_t::from_type<const T>();
    } else if constexpr ( tr.is_gr_predef() ) {
        gr_t gr {x};
        buff = gr.buff();
        dspace = dsp_t( gr.extents() );
        dtype = dt_t::from_type<typename gr_t::value_t >();
    } else if constexpr ( tr.is_gr_rr_predef() ) {
        gr_t gr {x};

        buff = gr.buff();

        typedef RawArrayTraits<typename gr_t::value_t> _rr_t;
        vector<size_t> ext = gr.extents();
        for(const auto &e: _rr_t::extents) ext.push_back(e);
        dspace = dsp_t( ext );

        dtype = dt_t::from_type<typename _rr_t::value_t >();
    } else if constexpr ( tr.is_custom() ) {
        buff = &x;
        dspace = dsp_t::vSCALAR;
        dtype = dt_t::from_type<const T>();
    } else if constexpr ( tr.is_gr_custom() ) {
        gr_t gr {x};
        buff = gr.buff();
        dspace = dsp_t( gr.extents() );
        dtype = dt_t::from_type<typename gr_t::value_t >();
    } else {
        gr_t gr {x};

        buff = gr.buff();

        typedef RawArrayTraits<typename gr_t::value_t> _rr_t;
        vector<size_t> ext = gr.extents();
        for(const auto &e: _rr_t::extents) ext.push_back(e);
        dspace = dsp_t( ext );

        dtype = dt_t::from_type<typename _rr_t::value_t>();
    }
}

inline
std::tuple<const void *, Dataspace, Datatype> ConstDatapacket::to_tuple() const 
{
    return {buff, dspace, dtype};
}

inline ConstDatapacketScalar::ConstDatapacketScalar() noexcept 
{ }

inline ConstDatapacketScalar::ConstDatapacketScalar(const void *_buff, 
    Datatype _dtype) noexcept
: ConstDatapacket( _buff, Dataspace::vSCALAR, std::move(_dtype) ) 
{ }

template<typename T>
ConstDatapacketScalar::ConstDatapacketScalar(const T &x)
: ConstDatapacketScalar( &x, Datatype::from_type<T>() ) 
{ }

inline ConstDatapacketStr::ConstDatapacketStr() noexcept
{ }

inline ConstDatapacketStr::ConstDatapacketStr(const void *_buff, 
    Dataspace _dspace, Datatype _dtype) noexcept
: ConstDatapacket(_buff, std::move(_dspace), std::move(_dtype))
{ }

inline ConstDatapacketStr::ConstDatapacketStr(const string &s)
: ConstDatapacket(s.data(), 
    Dataspace::vSCALAR, C_S1_T.resized(s.size()+1) ) 
{ }

template<size_t N_STR>
ConstDatapacketStr::ConstDatapacketStr(const string (&s)[N_STR]) 
: ConstDatapacketStr(s, N_STR)
{ }

inline ConstDatapacketStr::ConstDatapacketStr(const string *s, 
    size_t n_str) 
{
    size_t max_n = 0;
    for(size_t i=0; i<n_str; ++i) max_n = std::max(max_n, s[i].size());
    dspace = Dataspace(vector<hsize_t>{n_str});
    dtype = C_S1_T.resized(max_n+1);
}

inline ConstDatapacketStr::ConstDatapacketStr(const vector<string> &s)
: ConstDatapacketStr(s.data(), s.size())
{ }

inline ConstDatapacketStr::ConstDatapacketStr(const char *s)
: ConstDatapacketStr(s, strlen(s)+1)
{ }

inline ConstDatapacketStr::ConstDatapacketStr(const char *s, size_t n)
: ConstDatapacketStr(s, Dataspace::vSCALAR, C_S1_T.resized(n))
{ }

template<size_t N_STR, size_t N>
ConstDatapacketStr::ConstDatapacketStr(const char (&s)[N_STR][N]) 
: ConstDatapacketStr(s, N_STR)
{ }

template<size_t N>
ConstDatapacketStr::ConstDatapacketStr(const char (*s)[N], size_t n_str)
: ConstDatapacketStr(s, Dataspace(vector<hsize_t>{n_str}), 
    C_S1_T.resized(N))
{ }

inline ConstDatapacketStr::ConstDatapacketStr(const char * const *s, 
    size_t n_str)
{
    size_t max_n = 0;
    for(size_t i=0; i<n_str; ++i) max_n = std::max(max_n, strlen(s[i]));
    dspace = Dataspace(vector<hsize_t>{n_str});
    dtype = C_S1_T.resized(max_n+1);
}

inline void ConstDatapacketStr::buff_to(const string *s, size_t n_str, 
    vector<char> &b) 
{
    const size_t n = dtype.size(), tot_n = n_str * n;
    b.resize(tot_n);
    for(size_t i=0; i<n_str; ++i)
        std::strcpy(b.data() + i * n, s[i].c_str());
}

inline void ConstDatapacketStr::buff_to(const char * const *s, size_t n_str,
    vector<char> &b) 
{
    const size_t n = dtype.size(), tot_n = n_str * n;
    b.resize(tot_n);
    for(size_t i=0; i<n_str; ++i)
        std::strcpy(b.data() + i * n, s[i]);
}


} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_UTIL_DATAPACKET_IMPL_H_