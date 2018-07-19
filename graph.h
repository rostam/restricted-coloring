//
// Created by rostam on 14.06.18.
//

#ifndef MY_GCOL_GRAPH_H
#define MY_GCOL_GRAPH_H

#include "prettyprint.hpp"
#include "boost/config.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/cstdlib.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <boost/dynamic_bitset.hpp>
#include <tuple>

using boost::property;
using boost::edge_name;
using boost::edge_weight;
using boost::vertex_color_t;
using boost::edge_weight_t;
using boost::edge_name_t;
using boost::property_map;
using boost::graph_traits;
using boost::vertex_color;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
        property<boost::vertex_color_t, int>,
        property<boost::edge_weight_t, int,
                property<boost::edge_name_t, std::string>>> Graph;

typedef boost::graph_traits<Graph>::vertex_iterator V_iter;
typedef boost::graph_traits<Graph>::edge_iterator E_iter;
typedef Graph::vertex_descriptor Ver;
typedef Graph::edge_descriptor Edge;
typedef boost::property_map<Graph, boost::edge_weight_t>::type edge_weight_type;
typedef boost::dynamic_bitset<> dynbit;

class graph {
    Graph g;
    property_map<Graph, vertex_color_t>::type color;
public:
    graph(unsigned long num_vertices) : g(Graph(num_vertices)) { init(); }

    graph() { init(); }

    void add_edge(int i, int j, int weight) { boost::add_edge(i, j, weight, g); }

    void init() {
        color = get(vertex_color, g);
        for_each_v([&](Ver v) { put_color_v(v, 0); });
    }

    unsigned long num_v() { return boost::num_vertices(g); }

    int get_color_v(int v) { return boost::get(vertex_color, g, v); }

    void put_color_v(int v, int c) { put(color, v, c); }


    /**
     * for each vertex
     * @tparam Lambda
     * @param g
     * @param func
     */
    template<typename Lambda>
    void for_each_v(Lambda func) {
        V_iter vi, vi_end;
        std::tie(vi, vi_end) = boost::vertices(g);
        std::for_each(vi, vi_end, func);
    }

    /**
     * for each edge
     * @tparam Lambda
     * @param g
     * @param func
     */
    template<typename Lambda>
    void for_each_e(Lambda func) {
        E_iter ei, ei_end;
        tie(ei, ei_end) = edges(g);
        std::for_each(ei, ei_end, func);
    }

    /**
     * for each neighbor of v
     * @tparam Lambda
     * @param g
     * @param v
     * @param func
     */
    template<typename Lambda>
    void for_each_n(const Ver &v, Lambda func) {
        auto av = adjacent_vertices(v, g);
        std::for_each(av.first, av.second, func);
    }

    /**
     * for each distance-2 neighbor of v
     * @tparam Lambda
     * @param g
     * @param v
     * @param func
     */
    template<typename Lambda>
    void for_each_2n(int v, Lambda func) {
        std::set<int> tmp;
        for_each_n(v, [&](Ver a) {
            for_each_n(v, [&](Ver a2) {
                if (v != a2)
                    tmp.insert(a2);
            });
        });

        for (int i : tmp) func(i);
    }

    std::tuple<int,std::vector<int>> greedy_color_order(std::vector<int> order, int max_color) {
        std::vector<unsigned int> forbiddenColors(num_v(), -1);
        for(int v : order) {
            forbiddenColors[0] = v;
            for_each_n(v, [&](int n) {
                int c = get_color_v(n);
                if (c > 0)forbiddenColors[c] = v;
            });
            //Find first color which can be assigned to v
            auto result = find_if(forbiddenColors.begin(), forbiddenColors.end(), [&](int i) {return i != v;});
            auto res_color = distance(forbiddenColors.begin(),result);
            if(res_color < max_color)
                put_color_v(v, res_color);
        }
        std::vector<int> colors;
        std::set<int> unique_colors;
        for_each_v([&](int v){
            colors.push_back(get_color_v(v)-1);
            int the_color = get_color_v(v);
            if(the_color != -1)
                unique_colors.insert(the_color);
        });
        return {unique_colors.size(), colors};
    }

    std::tuple<int,std::vector<int>> greedy_color(int max_color) {
        std::vector<unsigned int> forbiddenColors(num_v(), -1);
        for_each_v([&](int v) {
            forbiddenColors[0] = v;
            for_each_n(v, [&](int n) {
                int c = get_color_v(n);
                if (c > 0)forbiddenColors[c] = v;
            });
            //Find first color which can be assigned to v
            auto result = find_if(forbiddenColors.begin(), forbiddenColors.end(), [&](int i) {return i != v;});
            auto res_color = distance(forbiddenColors.begin(),result);
            if(res_color < max_color)
                put_color_v(v, res_color);
        });

        std::vector<int> colors;
        std::set<int> unique_colors;
        for_each_v([&](int v){
            colors.push_back(get_color_v(v) - 1);
            int the_color = get_color_v(v);
            if(the_color != -1)
                unique_colors.insert(the_color);
        });
        return {unique_colors.size(), colors};
    }

    std::vector<int> optimum_order() {
        std::vector<std::tuple<int,int>> vertex_weight;
        for_each_v([&](int v){
            int sum = 0;
            for_each_n(v, [&](int n) {
                int w = get(boost::edge_weight_t(),g, edge(v,n,g).first);
                sum+= w;
            });
            vertex_weight.push_back({v,sum});
        });

        std::sort(begin(vertex_weight), end(vertex_weight), [](auto const &t1, auto const &t2) {
            return get<1>(t1) > get<1>(t2); // or use a custom compare function
        });

        std::vector<int> ret;
        for(auto &t : vertex_weight)
            ret.push_back(get<0>(t));
        return ret;
    }
};


#endif //MY_GCOL_GRAPH_H
