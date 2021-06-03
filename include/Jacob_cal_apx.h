#include "Thirdparty/g2o/g2o/types/types_six_dof_expmap.h"
using namespace Eigen;

void linearizeOplus_apx_mono(g2o::EdgeSE3ProjectXYZ *,Matrix<double,2,3> &,Matrix<double,2,6> &) ;

double logdet(MatrixXd &);