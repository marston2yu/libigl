// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "lscm.h"

// Bug in unsupported/Eigen/SparseExtra needs iostream first
#include <iostream>
#include <unsupported/Eigen/SparseExtra>

#include <igl/areamatrix.h>
#include <igl/cotmatrix.h>
#include <igl/kronecker_product.h>
#include <igl/boundary_vertices_sorted.h>
#include <igl/min_quad_with_fixed.h>

IGL_INLINE Eigen::MatrixXd igl::lscm(
  const Eigen::MatrixXd& V,
  const Eigen::MatrixXi& F,
  const Eigen::VectorXi& b,
  const Eigen::MatrixXd& bc)
{
  using namespace Eigen;
  
  // Assemble the area matrix (note that A is #Vx2 by #Vx2)
  SparseMatrix<double> A;
  igl::areamatrix(V,F,A);

  // Assemble the cotan laplacian matrix
  SparseMatrix<double> L;
  igl::cotmatrix(V,F,L);

  SparseMatrix<double> I2(2,2);
  I2.insert(0,0) = 1;
  I2.insert(1,1) = 1;

  SparseMatrix<double> L_flat = igl::kronecker_product(I2,L);

  VectorXi b_flat = b;
  MatrixXd bc_flat = bc;
  
  if (b.size() < 2)
  {
    // if no boundary conditions are provided, fix two boundary points
    Eigen::VectorXi bnd;
    igl::boundary_vertices_sorted(V,F,bnd);

    int v1 = bnd(0);
    int v2 = bnd(round(bnd.size()/2));

    b_flat.resize(4);
    b_flat << v1, v1+1, v2, v2+1;
    bc_flat.resize(4,1);
    bc_flat << 0, 1, 0, 0;
  }

  // Minimize the LSCM energy
  SparseMatrix<double> Q = -0.5*L_flat + A;
  const VectorXd B_flat = VectorXd::Zero(V.rows()*2);
  igl::min_quad_with_fixed_data<double> data;
  igl::min_quad_with_fixed_precompute(Q,b_flat,SparseMatrix<double>(),true,data);

  MatrixXd W_flat;
  if(!min_quad_with_fixed_solve(data,B_flat,bc_flat,VectorXd(),W_flat))
    assert(false);

  MatrixXd V_uv(V.rows(),2);
  for (unsigned i=0;i<V.rows();++i)
  {
    V_uv(i,0) = W_flat(i);
    V_uv(i,1) = W_flat(i+V.rows());
  }

  return V_uv;
}

#ifndef IGL_HEADER_ONLY
// Explicit template specialization
// generated by autoexplicit.sh
#endif