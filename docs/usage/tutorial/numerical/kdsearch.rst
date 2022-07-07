Space-searching Algorithms
============================

.. include:: /global.rst

.. namespace:: HIPP::NUMERICAL

KDTree
---------

KDTree is a natural extension of one-dimensional binary search tree (BST) to 
arbitrary dimensionality. 
Like BST, it bi-partites the space recursively into small regions, each hosting
a small set of data points. Those regions are ordered and stored in a 
memory-compact and look-up-efficient manner for fast neighbor-based searching.

Steps to use :class:`KDTree` are:

- Choose an appropriate tree type by specifying the template arguments to 
  :class:`KDTree`.
- Construct the tree instance by a set of KD points.
- Make queries on the tree instance.

Each step is detailed in the following.

**Choosing the tree type:** :class:`KDTree` is a class template. To get a specific tree type, 
four pieces of information must be provided through the 
template arguments:

- a floating-point scalar type used to represent space coordinate.
- the dimensionality of the space.
- the size in byte of user-defined data (called padding) attached to each 
  point.
- a signed integer type used to represent the indices of points/nodes 
  on the tree.

For example, a KD tree type in three-dimensional space is declared by::

    using float_t = float;
    constexpr auto DIM = 3;
    using pad_t = float;
    using index_t = int;

    using kd_point_t = nu::KDPoint<float_t, DIM, sizeof(pad_t)>;
    using kd_tree_t = nu::KDTree<kd_point_t, index_t>;

Debrief:

- We use ``float_t = float`` as coordinate representation. 
- We will associate each point with a user-defined ``pad_t = float`` value which 
  represents the weight of this point.
  HIPP knows nothing about this padding field and stores it as bytes. 
  Hence, user only specifies the size of padding as template argument 
  (``sizeof(pad_t)``, 4 in x86 platforms).
  User may attach more complicated information by defining a ``struct`` and use 
  it as the padding field.
- The index type ``index_t`` must be sufficiently large to index all points.
  Hence, if more than billions of points will be used to construct the tree, 
  choose a larger interger type.

**Constructing the tree instance:** To construct a KD tree instance, just pass a contiguous buffer of ``kd_point_t`` 
instances. Each KD point hosts two piece of information: the coordinates of 
the point, and the user-defined padding field.

An instance of KD point can be constructed by::

        kd_point_t::pos_t pos;
        for(int i=0; i<DIM; ++i) pos[i] = nu::rand();
        pad_t weight = 1.0;
        kd_point_t pt (pos, weight);


Here, ``pos_t`` is a ``std:array``-like type, whose member is accessible via the 
``[]`` operator. We set each member to an uniform random number in the above 
code. Iterators like those returned by ``begin()``, ``end()``, 
and ``data()`` are also defined
so that the above ``for``-loop can be replaced by one of the following::

    nu::rand(pos.begin(), pos.end());

    nu::rand(pos.data(), pos.data()+DIM);

The coordinates and padding of a KD point can be retrieved via its 
``pos()`` and ``pad<pad_t>()`` methods, respectively, both return 
a reference to the corresponding data member. Hence,
it is also valid to default-initialize a KD point, and set its members later::

    kd_point_t pt;

    kd_point_t::pos_t &pos = pt.pos();
    pos[0] = 1.0;       // pos[1] = ..., pos[2] = ... 

    float_t &weight = pt.pad<pad_t>();
    weight = 1.0;
    
    // or, call ``fill_pad()`` to directly set the padding
    pt.fill_pad((pad_t)1.0);

    pout << pt.pos(), ", padding = ", pt.pad<pad_t>(), endl;

The output is:

.. code-block:: text
    
    SArray{{1, 0, 0}}, padding = 1

Once we have a contiguous buffer of KD points, pass it to the constructor 
of :class:`KDTree` to get a tree instance. In the following codes, a vector 
of KD points is created to initialize the tree::

    vector<kd_point_t> pts;
    for(int i=0; i<1000000; ++i){
        kd_point_t::pos_t pos;
        nu::rand(pos.begin(), pos.end());     // randomly assign coordinates
        
        pad_t weight = 1.0;                   // all points are equal-weighted

        pts.emplace_back(pos, weight);  
        // or pts.push_back(kd_point_t(pos, weight));
    }
    kd_tree_t kd_tree(pts);
    pout << "Tree: ", kd_tree, endl;

KDTree stores copies of these points. Once the tree is created, it is safe to 
clear the points in the vector.

The output is:

.. code-block:: text
    
    Tree: <KDTree> {impl=<_KDTree> {no. nodes=1000000, buf size=24000000, <_KDTree::tree_info_t> {max depth=20}, <_KDTree::construct_policy_t> {split axis=MAX_EXTREME, random seed=0}}}

**Making Queries:** To get the nearest neighbor of any spatial location, call 
:func:`~KDTree::nearest` on the tree instance::

    kd_tree_t::point_t pt {0.1, 0.2, 0.3};
    kd_tree_t::ngb_t ngb = kd_tree.nearest(pt);
    pout << "Node index = ", ngb.node_idx, ", r^2 to 'pt' = ", ngb.r_sq, endl;

The output is:

.. code-block:: text

    Node index = 36580, r^2 to 'pt' = 3.17662e-06

Debrief:

- The argument of :func:`~KDTree::nearest` is a ``kd_tree_t::point_t`` instance,
  which denotes a spatial location (i.e., a point). A point can be constructed 
  by a initializer list (as in the above codes), or a ``point_t::pos_t`` 
  instance (like in the KD point case), or we can default-initialize it and 
  then refer to its position via ``pos()`` method.
- The returned result of the query is a ``kd_tree_t::ngb_t`` instance, which 
  has two member ``r_sq`` and ``node_idx``. ``r_sq`` is the squared distance
  from the queried location to its nearest neighbor.
  ``node_idx`` is the index of this neighbor into 
  the node array hosted by the tree. Each node is copied from a KD point
  used to construct the tree, with extra fields book-keeping algorithm-specific
  meta-info. Hence, like a KD point, the methods ``pos()`` and ``pad<pad_t>()``
  of the node return its coordinates and padding field, respectively. The 
  following code sample demonstrates how to get the coordinates and padding 
  field of the nearest neighbor we have just found::

    auto &node = kd_tree.nodes()[ngb.node_idx];
    pout << node.pos(), ", padding = ", node.pad<pad_t>(), endl;

The output is:

.. code-block:: text

    SArray{{0.101701, 0.199494, 0.299839}}, padding = 1

We see ``padding`` is ``1.0``, because we have set paddings of all KD points 
to be ``1.0``.


To get k-nearest neighbors of any spatial location, call 
:func:`~KDTree::nearest_k` on the tree instance::

    int k = 8;
    vector<kd_tree_t::ngb_t> ngbs(k);
    kd_tree.nearest_k(pt, ngbs);

        
    std::sort(ngbs.begin(), ngbs.end());

    for(auto &ngb: ngbs){
        pout << "Node index = ", ngb.node_idx, ", r^2 to 'pt' = ", ngb.r_sq, endl;
    }

The output is:

.. code-block:: text

    Node index = 36580, r^2 to 'pt' = 3.17662e-06
    Node index = 36582, r^2 to 'pt' = 1.49361e-05
    Node index = 36581, r^2 to 'pt' = 6.29745e-05
    Node index = 36572, r^2 to 'pt' = 7.42609e-05
    Node index = 36573, r^2 to 'pt' = 0.000156318
    Node index = 36584, r^2 to 'pt' = 0.00016049
    Node index = 36574, r^2 to 'pt' = 0.000177615
    Node index = 36466, r^2 to 'pt' = 0.000195571

Debrief:

- The argument of :func:`~KDTree::nearest_k` is a contiguous buffer of 
  ``ngb_t`` (a ``std::vector`` here). On entry of the call, the size 
  of the buffer denotes the number of neighbors to query (i.e., k).
  On exit, the buffer is filled with information (``node_idx`` and ``r_sq``) 
  of each neighbor.
- The buffer of neighbors is not sorted by default. You may manually sort 
  them, for example, by ``std::sort`` (in increasing ``r_sq``). 

**Performance issue:** Many nuanced factors have impact on the performance of 
:class:`KDTree`. We list some suggestions to help users maximize tree 
performance:

- Fine-tune the tree construction algorithm with construction policies.
- Reuse the tree instance. Construct a new tree by the ``construct()`` method.
- Sort large set of input points before making queries on them.
- Reuse the temporary buffer in the neighbor-searching algorithm by 
  keeping a persistent query policy object and passing it to each query.
- All the neighbor-query methods of KDTree are thread-safe. Parallelizing
  your codes with a thread library generally scales well.
  
The first two suggestions are relevant to the tree construction process.
To manually tune the construction policy, declare a policy instance, set 
its attributes, and pass it to the constructor or ``construct()`` method of 
the tree, like::

        kd_tree_t::construct_policy_t cstr_pl;
        kd_tree.construct(pts, cstr_pl); 

HIPP adopts an optimal construction algorithm, so it is very rare that you need 
to tune the construction. 

The last three suggestions are much more frequently adopted. Sorting points 
before passing them to the query methods allows computer cache to be more 
efficiently used. Because memory latency and throughput are the bottle-lacks 
in most programs, such a trick may significantly boost the performance. 
To achieve this, just call ``argsort()`` on the tree by passing a buffer 
of points, and use the returned indices to reorder the points::


    vector<kd_tree_t::point_t> pts, pts_sorted;
    for(int i=0; i<1000000; ++i){
        kd_tree_t::pos_t pos;
        nu::rand(pos.begin(), pos.end());
        pts.emplace_back(pos);   
    }

    vector<kd_tree_t::idx_pair_t> idxs;
    kd_tree.argsort<kd_tree_t::point_t>(pts, idxs);
    for(auto &idx: idxs){
        pts_sorted.push_back(pts[idx.idx_in]);
    }

Now, call ``nearest()`` or ``nearest_k()`` on ``pts_sorted`` one by one would 
be faster than on ``pts``.

The neighbor-searching algorithms use some temporary buffers. Constructing and 
destroying those buffers on each query may significantly reduce the 
performance. The strategy is to declare a persistent policy object, which hosts 
all buffers used by the algorithm, and pass the same policy object to multiple 
queries. The policy object also allows other fine-controls on the algorithm, 
for example, enabling the sorting of neighbors. The following code sample 
demonstrates how to query the neighbors of the ``pts_sorted`` obtained above,
and accumulate the weights we have assigned to those neighbors::

    kd_tree_t::nearest_k_query_policy_t query_pl;
    query_pl.sort_by_distance_on();
    
    double sum_weight = 0.;
    for(auto &pt: pts_sorted){
        kd_tree.nearest_k(pt, ngbs, query_pl);
        for(auto &ngb: ngbs)
            sum_weight += kd_tree.nodes()[ngb.node_idx].pad<pad_t>();
    }
    pout << "sum_weight = ", sum_weight, endl;

Because we query 1000,000 times with ``k=8``, the output is

.. code-block:: text

    sum_weight = 8e+06

