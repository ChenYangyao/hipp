#include <hippnumerical.h>

using namespace HIPP;
namespace nu = HIPP::NUMERICAL;

int main(int argc, char const *argv[])
{
    /**
    Specify the compile-time arguments (i.e. template arguments) to KDTree.
    (1) a floating-point scalar type used to represent space coordinate.
    (2) the dimensionality of the space.
    (3) the size in byte of user-defined data (called padding) attached to each 
    point.
    (4) a signed integer type used to represent the indices of points/nodes 
    on the tree.
    */
    using float_t = float;
    constexpr auto DIM = 3;
    using pad_t = float;
    using index_t = int;

    using kd_point_t = nu::KDPoint<float_t, DIM, sizeof(pad_t)>;
    using kd_tree_t = nu::KDTree<kd_point_t, index_t>;

    /**
    Examples of using KDPoint.
    */
    {
        kd_point_t::pos_t pos;
        
        for(int i=0; i<DIM; ++i) pos[i] = nu::rand();

        nu::rand(pos.begin(), pos.end());

        nu::rand(pos.data(), pos.data()+DIM);

        pad_t weight = 1.0;
        kd_point_t pt (pos, weight);
    }
    {
        kd_point_t pt;

        kd_point_t::pos_t &pos = pt.pos();
        pos[0] = 1.0;

        float_t &weight = pt.pad<pad_t>();
        weight = 1.0;

        pt.fill_pad((pad_t)1.0);

        pout << pt.pos(), ", padding = ", pt.pad<pad_t>(), endl;
    }


    /**
    Construct the tree instance by passing a contiguous buffer of KD points.
    The tree instance stores copies of these points.
    */
    vector<kd_point_t> pts;
    for(int i=0; i<1000000; ++i){
        kd_point_t::pos_t pos;
        nu::rand(pos.begin(), pos.end());
        
        pad_t weight = 1.0;

        pts.emplace_back(pos, weight);  
        // or pts.push_back(kd_point_t(pos, weight));
    }
    kd_tree_t kd_tree(pts);
    pout << "Tree: ", kd_tree, endl;


    /**
    Query the nearest neighbor.
    */
    kd_tree_t::point_t pt {0.1, 0.2, 0.3};
    kd_tree_t::ngb_t ngb = kd_tree.nearest(pt);

    {
        auto ngb = kd_tree.nearest(pts[0]);
    }

    pout << "Node index = ", ngb.node_idx, ", r^2 to 'pt' = ", ngb.r_sq, endl;

    auto &node = kd_tree.nodes()[ngb.node_idx];
    pout << node.pos(), ", padding = ", node.pad<pad_t>(), endl;


    /**
    Query the k-nearest neighbors.
    */
    int k = 8;
    vector<kd_tree_t::ngb_t> ngbs(k);
    kd_tree.nearest_k(pt, ngbs);

    // Not sorted by default. May manually sort by std::sort (in increasing 
    // distances). 
    std::sort(ngbs.begin(), ngbs.end());

    for(auto &ngb: ngbs){
        pout << "Node index = ", ngb.node_idx, ", r^2 to 'pt' = ", ngb.r_sq, endl;
    }

    /**
    Maximizing the performance.
    (1) Fine-tune the tree construction algorithm with construction policies.
    (2) Reuse the tree instance. Construct a new tree by the 
    `construct()` method.
    */
    {
        kd_tree_t::construct_policy_t cstr_pl;
        kd_tree.construct(pts, cstr_pl); 
    }

    {
        /** 
        (3) Sort large set of input points before making queries on them.
        */
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

        /**
        (4) Reuse the temporary buffer in the neighbor-searching algorithm by 
        keeping a persitent query policy object and passing it to each query.
        */
        kd_tree_t::nearest_k_query_policy_t query_pl;
        query_pl.sort_by_distance_on();
        
        double sum_weight = 0.;
        for(auto &pt: pts_sorted){
            kd_tree.nearest_k(pt, ngbs, query_pl);
            for(auto &ngb: ngbs)
                sum_weight += kd_tree.nodes()[ngb.node_idx].pad<pad_t>();
        }
        pout << "sum_weight = ", sum_weight, endl;
    }
    
    return 0;
}
