#include "Mtx2Graph.hpp"
#include <memory>

/**
 * \brief Write the matrix into the file with format mtx
 *
 * @param filename the name of file
 * @return true if it works correctly
 */
bool matrix_market::write_to_file(char* filename) {
    const int size =nz;
    double val[size];
    std::fill(val,val+size,1);
    mm_write_mtx_crd(filename, M, N, nz, (int *) I, (int *) J, val, matcode);
    return true;
}

/**
 * \brief gets the name of a mtx file and makes the matrix
 *
 * @param filename the name of the matrix file with format mtx
 * @return
 */
matrix_market::matrix_market(const char* filename) {
    int ret_code;
    FILE *file;
    int i;

    if ((file = fopen(filename, "r")) == NULL)
        exit(1);

    if (mm_read_banner(file, &matcode) != 0) {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }

    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) &&
        mm_is_sparse(matcode)) {
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(1);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(file, &M, &N, &nz)) != 0)
        exit(1);

    /* reseve memory for matrices */
    I = (unsigned int *) malloc(nz * sizeof(unsigned int));
    J = (unsigned int *) malloc(nz * sizeof(unsigned int));

    if (mm_is_pattern(matcode)) {
        for (i = 0; i < nz; i++) {
            fscanf(file, "%u %u\n", &I[i], &J[i]);
            I[i]--;  /* adjust from 1-based to 0-based */
            J[i]--;
        }
    } else {
        double *val;
        val = (double *) malloc(nz * sizeof(double));
        for (i = 0; i < nz; i++) {
            fscanf(file, "%u %u %lg\n", &I[i], &J[i], &val[i]);
            I[i]--;  /* adjust from 1-based to 0-based */
            J[i]--;
        }
        free(val);
    }

    if (file != stdin) fclose(file);
}

/**
 * \brief return the bipartite graph generated from the matrix
 *
 * @param G_b the result matrix
 * @return
 */
boost::numeric::ublas::matrix<int> matrix_market::to_ublas_matrix() {
    boost::numeric::ublas::matrix<int> m = boost::numeric::ublas::zero_matrix<int>(M, N);
    if (mm_is_symmetric(matcode)) {
        for (int i = 0; i < nz; ++i) {
            m(I[i], J[i]) = 1;
            m(J[i], I[i]) = 1;
        }
    } else {
        for (int i = 0; i < nz; ++i) {
            m(I[i], J[i]) = 1;
        }
    }

    return m;
}

matrix_market::~matrix_market() {
  free(I);
  free(J);
}
