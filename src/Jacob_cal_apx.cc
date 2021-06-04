
#include "Jacob_cal_apx.h"
using namespace g2o;
using namespace std;
void linearizeOplus_apx_mono(EdgeSE3ProjectXYZ *edge,Matrix<double,2,3> &jacob_p,Matrix<double,2,6> &jacob_x) 
{
  VertexSE3Expmap * vj = static_cast<VertexSE3Expmap *>((edge->vertices())[1]);
  SE3Quat T(vj->estimate());
  VertexSBAPointXYZ* vi = static_cast<VertexSBAPointXYZ*>((edge->vertices())[0]);
  Vector3d xyz = vi->estimate();
  Vector3d xyz_trans = T.map(xyz);

  double x = xyz_trans[0];
  double y = xyz_trans[1];
  double z = xyz_trans[2];
  double z_2 = z*z;
  double fx = edge->fx;
  double fy = edge->fy;


  Matrix<double,2,3> tmp;
  tmp(0,0) = fx;
  tmp(0,1) = 0;
  tmp(0,2) = -x/z*fx;

  tmp(1,0) = 0;
  tmp(1,1) = fy;
  tmp(1,2) = -y/z*fy;
   

  jacob_p =  -1./z * tmp * T.rotation().toRotationMatrix();

  jacob_x(0,0) =  x*y/z_2 *fx;
  jacob_x(0,1) = -(1+(x*x/z_2)) *fx;
  jacob_x(0,2) = y/z *fx;
  jacob_x(0,3) = -1./z *fx;
  jacob_x(0,4) = 0;
  jacob_x(0,5) = x/z_2 *fx;

  jacob_x(1,0) = (1+y*y/z_2) *fy;
  jacob_x(1,1) = -x*y/z_2 *fy;
  jacob_x(1,2) = -x/z *fy;
  jacob_x(1,3) = 0;
  jacob_x(1,4) = -1./z *fy;
  jacob_x(1,5) = y/z_2 *fy;

}
void linearizeOplus_apx_stereo(EdgeStereoSE3ProjectXYZ *edge,Matrix<double,3,3> &jacob_p,Matrix<double,3,6> &jacob_x) 
{
  VertexSE3Expmap * vj = static_cast<VertexSE3Expmap *>((edge->vertices())[1]);
  SE3Quat T(vj->estimate());
  VertexSBAPointXYZ* vi = static_cast<VertexSBAPointXYZ*>((edge->vertices())[0]);
  Vector3d xyz = vi->estimate();
  Vector3d xyz_trans = T.map(xyz);

  const Matrix3d R =  T.rotation().toRotationMatrix();

  double x = xyz_trans[0];
  double y = xyz_trans[1];
  double z = xyz_trans[2];
  double z_2 = z*z;
  double fx = edge->fx;
  double fy = edge->fy;
  double bf = edge->bf;

// 这里xi是特征点的3d坐标
  jacob_p(0,0) = -fx*R(0,0)/z+fx*x*R(2,0)/z_2;
  jacob_p(0,1) = -fx*R(0,1)/z+fx*x*R(2,1)/z_2;
  jacob_p(0,2) = -fx*R(0,2)/z+fx*x*R(2,2)/z_2;

  jacob_p(1,0) = -fy*R(1,0)/z+fy*y*R(2,0)/z_2;
  jacob_p(1,1) = -fy*R(1,1)/z+fy*y*R(2,1)/z_2;
  jacob_p(1,2) = -fy*R(1,2)/z+fy*y*R(2,2)/z_2;

  jacob_p(2,0) = jacob_p(0,0)-bf*R(2,0)/z_2;
  jacob_p(2,1) = jacob_p(0,1)-bf*R(2,1)/z_2;
  jacob_p(2,2) = jacob_p(0,2)-bf*R(2,2)/z_2;

// 这里xj是camera pose的6dof
  jacob_x(0,0) =  x*y/z_2 *fx;
  jacob_x(0,1) = -(1+(x*x/z_2)) *fx;
  jacob_x(0,2) = y/z *fx;
  jacob_x(0,3) = -1./z *fx;
  jacob_x(0,4) = 0;
  jacob_x(0,5) = x/z_2 *fx;

  jacob_x(1,0) = (1+y*y/z_2) *fy;
  jacob_x(1,1) = -x*y/z_2 *fy;
  jacob_x(1,2) = -x/z *fy;
  jacob_x(1,3) = 0;
  jacob_x(1,4) = -1./z *fy;
  jacob_x(1,5) = y/z_2 *fy;

  jacob_x(2,0) = jacob_x(0,0)-bf*y/z_2;
  jacob_x(2,1) = jacob_x(0,1)+bf*x/z_2;
  jacob_x(2,2) = jacob_x(0,2);
  jacob_x(2,3) = jacob_x(0,3);
  jacob_x(2,4) = 0;
  jacob_x(2,5) = jacob_x(0,5)-bf/z_2;

}

double logdet(MatrixXd & mat)  
{
     LLT<MatrixXd> chol_mat(mat);
     MatrixXd L = chol_mat.matrixL();

     double logdet_res = log10(L.diagonal().array().sum());

     return logdet_res;
}