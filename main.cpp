#include <iostream>
#include "Mtx2Graph.hpp"
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include "graph.h"
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/numeric/ublas/vector.hpp>

int
compute_misses(int num_colors, const std::vector<int> &color_vec, boost::numeric::ublas::matrix<int> &m);

using std::cout;
using std::endl;

int main() {
    using boost::numeric::ublas::matrix;
    using boost::numeric::ublas::matrix_column;
    boost::property_map<Graph, boost::vertex_color_t>::type color;
    matrix_market mm("/home/rostam/nos3.mtx");
    matrix<int> m = mm.to_ublas_matrix();

    for (int index = 1; index <= 10; index++) {
        graph g(m.size1());
        for (int i = 0; i < m.size1(); i++) {
            for (int j = i + 1; j < m.size1(); j++) {
                int cnt = 0;
                for (int k = 0; k < m.size2(); k++) {
                    if (m(i, k) != 0 && m(j, k) != 0) {
                        cnt++;
                    }
                }
                if (cnt > index) {
                    g.add_edge(i, j, cnt);
                }
            }
        }

//        boost::iterator_property_map<int *, vertex_index_map> color(&color_vec.front(), get(boost::vertex_index, g));
//        int num_colors = boost::sequential_vertex_coloring(g, color);
//        color(g);
        auto t = g.greedy_color();
        auto ord = g.optimum_order();
//        print(ord);
        auto t2 = g.greedy_color_order(ord);

        int num_colors1 = std::get<0>(t);
        std::vector<int> color_vec1 = std::get<1>(t);

        int num_colors2 = std::get<0>(t2);
        std::vector<int> color_vec2 = std::get<1>(t2);

        int all_sum1 = compute_misses(num_colors1, color_vec1, m);
        int all_sum2 = compute_misses(num_colors2, color_vec2, m);

        //        cout << misses[0] << endl;
        cout << num_colors1 << " " << num_colors2 << endl;
        cout << all_sum1 << " " << all_sum2 << endl << endl;
    }
    return 0;
}

int compute_misses(int num_colors, const std::vector<int> &color_vec, boost::numeric::ublas::matrix<int> &m) {
    int all_sum = 0;
    std::vector<boost::numeric::ublas::vector<int>> misses(num_colors);
    for (int i = 0; i < num_colors; i++) {
        misses[i] = boost::numeric::ublas::zero_vector<int>(m.size2());
    }

    for (int i = 0; i < color_vec.size(); i++) {
        misses[color_vec[i]] += column(m, i);
    }
    for (int i = 0; i < misses.size(); i++) {
        misses[i] /= 2;
        all_sum += sum(misses[i]);
    }

    return all_sum;
}