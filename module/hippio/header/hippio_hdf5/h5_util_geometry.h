#ifndef _HIPPIO_H5_UTIL_GEOMETRY_H_
#define _HIPPIO_H5_UTIL_GEOMETRY_H_

#include "../hippio_hdf5_raw/h5_base.h"
#include "../hippio_hdf5_raw/h5_error.h"

namespace HIPP::IO::H5 {

/**
``Dimensions`` describes the extent of the regular array dataspace or a 
coordinate in it.
*/
class Dimensions {
public:
    typedef hsize_t value_t;
    typedef value_t * iterator;
    typedef const value_t * const_iterator;

    /**
    Constructors.

    (1): Default constructor - the instance is initialized to ``ndims = 0`` 
        state.
    (2): Specify the dimensions using a initializer_list.
    (3): Specify the dimensions using ``ndims`` values starting at ``p``. 
    (4): Specify the dimensions by a ContiguousBuffer object.
    */
    Dimensions() noexcept;

    Dimensions(std::initializer_list<value_t> dims);

    template<typename T>
    Dimensions(const T *p, size_t ndims);

    Dimensions(size_t n, value_t value = value_t {});

    template<typename Buff,
        std::enable_if_t<ContiguousBufferTraits<Buff>::is_buffer, int> =0>
    Dimensions(const Buff &dims);

    /**
    ``Dimensions`` is copyable and movable. The copy operation is deep. The 
    move operation sets the move-from object an undefined by valid state.
    */
    Dimensions(const Dimensions &) = default;
    Dimensions(Dimensions &&) = default;
    Dimensions & operator=(const Dimensions &) = default;
    Dimensions & operator=(Dimensions &&) = default;
    ~Dimensions() = default;

    /**
    Print information of the instance to stream ``os``.
    @fmt_cntl: format controller. ``0`` for a inline, short message and ``1``
    for a verbose one.

    ``operator<<()`` is equivalent to ``info()`` with default format controller.
    */
    ostream & info(ostream &os=cout, int fmt_cntl=0) const;
    friend ostream & operator<<(ostream &os, const Dimensions &dims);

    /**
    Set the dimensions of the current instance. Theses four overloads are 
    exactly corresponding to the four constructors.
    */
    Dimensions & set() noexcept;
    Dimensions & set(std::initializer_list<value_t> dims);

    template<typename T>
    Dimensions & set(const T *p, size_t n);

    template<typename Buff,
        std::enable_if_t<ContiguousBufferTraits<Buff>::is_buffer, int> =0>
    Dimensions & set(const Buff &dims);

    /**
    Extend the dimensions at tail.
    (1) by a single element.
    (2-4) by a series of elements.
    */
    Dimensions & push_back(hsize_t x);

    Dimensions & push_back(std::initializer_list<value_t> dims);

    template<typename T>
    Dimensions & push_back(const T *p, size_t n);

    template<typename Buff,
        std::enable_if_t<ContiguousBufferTraits<Buff>::is_buffer, int> =0>
    Dimensions & push_back(const Buff &dims);

    /**
    Change the ndims of the instance. If enlarged, tail elements are 
    value-initialized. If shrinked, the tail elements are truncated.
    */
    void resize(size_t new_ndims);

    /**
    Data access methods.

    ``operator[]()``: visist a single dimension at axis ``i``.
    ``data()``: return the raw buffer of dimensions.
    ``begin()``, ``end()`` and their const-counterpart: return the iterators 
        of the dimensions.
    ``as_vector()`` returns a reference to the underlying vector that stores 
        the dimensions data.
    */
    const value_t & operator[](size_t i) const;
    value_t & operator[](size_t i);
    
    const value_t * data() const noexcept;
    value_t * data() noexcept;

    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

    const vector<value_t> & as_vector() const noexcept;
    vector<value_t> & as_vector() noexcept;
    
    /**
    Retrieve the properties of the instance.
    ``ndims()``: number of dimensions, i.e., rank.
    ``size()``: equal to ``ndims()``.
    ``empty()``: tests whether or not the instance has zero-rank.
    ``n_elems()``: total number of elements in the array, i.e., product of all
        dimensions.
    */
    size_t ndims() const noexcept;
    size_t size() const noexcept;
    bool empty() const noexcept;
    value_t n_elems() const noexcept;

    /**
    Tells whether or not ndims are equal && all dimensions are equal.
    */
    bool operator==(const Dimensions &) const noexcept;
    bool operator!=(const Dimensions &) const noexcept;
protected:
    vector<value_t> _dims;
};

/** Represent the coordinate of a single point. */
using Point = Dimensions;

/**
Represent the coordinates of a list of points.
The instance only keeps a reference to the data whose life-time is maintained by
the user.
*/
class Points {
public:
    typedef hsize_t value_t;

    /**
    Constructors.
    (1): 0 point.
    (2): ``n_points`` points in ``n_dims``-dimensional space. The coordinates
        of them is taken from a contiguous buffer starting at ``coords``.
    (3): the same as (2) but ``n_dims`` is set to ``0`` - to be determined from 
        other sources.
    */
    Points() noexcept;
    Points(size_t n_points, size_t n_dims, const value_t *coords) noexcept;
    Points(size_t n_points, value_t *coords) noexcept;
    
    /**
    Getters: retrieve the coordinates buffer address, number of points and 
    number of dimensions.
    */
    const value_t * coords() const noexcept;
    size_t n_points() const noexcept;
    size_t n_dims() const noexcept;
protected:
    size_t _n_points, _n_dims;
    const value_t *_coords;
};

/**
Chunk represents a continuous chunk (i.e., a regular sub-region) of an array.
*/
class Chunk {
public:
    /**
    Constructors.
    (1): the start and count of the chunk are set to empty.
    (2): specify the start and count manually. They must be in equal length.
    (3): specify the start and count using two buffers, both sized ``ndims``.
        The instance deeply copy the data from these two buffers.
    */
    Chunk() noexcept;
    Chunk(const Dimensions &start, const Dimensions &count);
    template<typename T1, typename T2>
    Chunk(const T1 *start, const T2 *count, size_t ndims);

    /**
    Getters: retrieve the starting coordinates and counts along axes.
    */
    const Dimensions & start() const;
    const Dimensions & count() const;

    Dimensions & start();
    Dimensions & count();
protected:
    Dimensions _start, _count;
};

/**
Hyperslab represents multiple regular sub-regions with regular strides in 
an array.
*/
class Hyperslab {
public:
    typedef hsize_t value_t;

    /**
    Constructors.
    (1): An empty hyperslab (i.e., start, count, stride, block are all 
        length-zero arrays).
    (2): Specifies only start and count, leaving stride and block empty.
    (3): Initialize all four fields. stride and block may be empty or array
        with equal length to other two arguments.
    (4,5): similar to (2,3) but using raw buffers to fill the start, count,
        stride, and block.
        In (5), stride or block may be NULL, indicating empty, or 
        otherwise interpreted as ``ndims`` elements in the buffer.
        The instance deeply copy the data from the buffers.
    
    Empty stride and block, when used in the selection of HDF5 hyperslabs, 
    are interpreted as ``1`` at all dimensions.
    */
    Hyperslab() noexcept;
    explicit Hyperslab(const Chunk &chunk);
    
    Hyperslab(const Dimensions &start, const Dimensions &count);
    
    Hyperslab(const Dimensions &start, const Dimensions &count, 
        const Dimensions &stride, const Dimensions &block);

    template<typename T1, typename T2>
    Hyperslab(const T1 *start, const T2 *count, size_t ndims);
    
    template<typename T1, typename T2, typename T3, typename T4>
    Hyperslab(const T1 *start, const T2 *count, 
        const T3 *stride, const T4 *block, size_t ndims);   

    /**
    Select a hyperslab by subsetting along the dimension ``dim``.
    @full_dims: dimensions of the parent space.
    @select_1d: start, count, stride, block at dimension ``dim``.
    */
    static Hyperslab subsetting_1d(Dimensions full_dims, 
        size_t dim, const std::array<hsize_t, 4> &select_1d);

    /**
    Getters: retrieve the starting coordinates, counts, strides and block 
    lengths along axes.
    */
    const Dimensions & start() const noexcept;
    const Dimensions & count() const noexcept;
    const Dimensions & stride() const noexcept;
    const Dimensions & block() const noexcept;

    Dimensions & start() noexcept;
    Dimensions & count() noexcept;
    Dimensions & stride() noexcept;
    Dimensions & block() noexcept;

    /**
    Get the pointers to the underlying buffer of start, count, stride and 
    block.
    If stride or block is empty, the returned pointer for it is nullptr.
    */
    std::tuple<const value_t*, const value_t*, const value_t*, const value_t*> 
        data() const;
    std::tuple<value_t*, value_t*, value_t*, value_t*> 
        data();
protected:
    Dimensions _start, _count, _stride, _block;
};

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_UTIL_GEOMETRY_H_