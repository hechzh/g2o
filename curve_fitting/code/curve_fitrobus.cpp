// g2o - General Graph Optimization
// Copyright (C) 2012 R. Kümmerle
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <Eigen/Core>
#include <iostream>

#include "g2o/stuff/sampler.h"
#include "g2o/stuff/command_args.h"
#include "g2o/core/sparse_optimizer.h"
#include "g2o/core/block_solver.h"
#include "g2o/core/solver.h"
#include "g2o/core/optimization_algorithm_levenberg.h"
#include "g2o/core/base_vertex.h"
#include "g2o/core/base_unary_edge.h"
#include "g2o/solvers/dense/linear_solver_dense.h"

using namespace std;

/**
 * \brief the params, a, b, and lambda for a * exp(-lambda * t) + b
 */
class VertexParams : public g2o::BaseVertex<4, Eigen::Vector4d>
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
    VertexParams()
    {
    }

    virtual bool read(std::istream& /*is*/)
    {
      cerr << __PRETTY_FUNCTION__ << " not implemented yet" << endl;
      return false;
    }

    virtual bool write(std::ostream& /*os*/) const
    {
      cerr << __PRETTY_FUNCTION__ << " not implemented yet" << endl;
      return false;
    }

    virtual void setToOriginImpl()
    {
      cerr << __PRETTY_FUNCTION__ << " not implemented yet" << endl;
    }

    virtual void oplusImpl(const double* update)
    {
      Eigen::Vector4d::ConstMapType v(update);
      _estimate += v;
    }
};

/**
 * \brief measurement for a point on the curve
 *
 * Here the measurement is the point which is lies on the curve.
 * The error function computes the difference between the curve
 * and the point.
 */
class EdgePointOnCurve : public g2o::BaseUnaryEdge<1, Eigen::Vector2d, VertexParams>
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    EdgePointOnCurve()
    {
    }
    virtual bool read(std::istream& /*is*/)
    {
      cerr << __PRETTY_FUNCTION__ << " not implemented yet" << endl;
      return false;
    }
    virtual bool write(std::ostream& /*os*/) const
    {
      cerr << __PRETTY_FUNCTION__ << " not implemented yet" << endl;
      return false;
    }

    void computeError()
    {
      const VertexParams* params = static_cast<const VertexParams*>(vertex(0));
      const double& a = params->estimate()(0);
      const double& b = params->estimate()(1);
      const double& lambda = params->estimate()(2);
      const double& c = params->estimate()(3);
      double fval = a * exp(-lambda * measurement()(0)) + b + c * measurement()(0);
      _error(0) = fval - measurement()(1);
      ///error+=abs(_error(0));
      ///cout << error << endl;
    }
};

int main(int argc, char** argv)
{
    double inf=0.01;
    double error=0;
    for (int j=0 ;j<10;++j) {
        int numPoints;
        int maxIterations;
        bool verbose;
        std::vector<int> gaugeList;
        string dumpFilename;
        g2o::CommandArgs arg;
        arg.param("dump", dumpFilename, "", "dump the points into a file");
        arg.param("numPoints", numPoints, 100, "number of points sampled from the curve");
        arg.param("i", maxIterations, 20, "perform n iterations");
        arg.param("v", verbose, false, "verbose output of the optimization process");

        arg.parseArgs(argc, argv);
        // generate random data
        double a = 2.1;
        double b = 0.4;
        double lambda = 0.2;
        double c = 0;
        Eigen::Vector2d *points = new Eigen::Vector2d[numPoints];
        cout << "y = [";
        for (int i = 0; i < numPoints; ++i) {
            double xup = g2o::Sampler::uniformRand(0, 10);
            double y = a * exp(-lambda * x) + b + c * x;
            // add Gaussian noise
            y += g2o::Sampler::gaussRand(0, inf + 0.01 * j);
            points[i].x() = x;
            points[i].y() = y;
            if ( i < numPoints-1){cout << x << "," << points[i].y() << ";";}
            else{cout<< x << "," << points[i].y() << "];" << endl;}
        }
/**
  for (int i=0;i<numPoints;i++){
      double x=i/10.0;
      points[i].x()=x;
      points[i].y()=a * exp(-lambda * x) + b+g2o::Sampler::gaussRand(0, 0.2);
      cout<<points[i].y()<<",";
  }
*/
        if (dumpFilename.size() > 0) {
            ofstream fout(dumpFilename.c_str());
            for (int i = 0; i < numPoints; ++i)
                fout << points[i].transpose() << endl;
        }

        // some handy typedefs
        typedef g2o::BlockSolver<g2o::BlockSolverTraits<Eigen::Dynamic, Eigen::Dynamic> > MyBlockSolver;
        typedef g2o::LinearSolverDense<MyBlockSolver::PoseMatrixType> MyLinearSolver;

        // setup the solver
        g2o::SparseOptimizer optimizer;
        optimizer.setVerbose(false);
        MyLinearSolver *linearSolver = new MyLinearSolver();
        MyBlockSolver *solver_ptr = new MyBlockSolver(linearSolver);
        g2o::OptimizationAlgorithmLevenberg *solver = new g2o::OptimizationAlgorithmLevenberg(solver_ptr);
        optimizer.setAlgorithm(solver);

        // build the optimization problem given the points
        // 1. add the parameter vertex
        VertexParams *params = new VertexParams();
        params->setId(0);
        params->setEstimate(Eigen::Vector4d(1, 1, 1, 1)); // some initial value for the params
        optimizer.addVertex(params);
        // 2. add the points we measured to be on the curve
        for (int i = 0; i < numPoints; ++i) {
            EdgePointOnCurve *e = new EdgePointOnCurve;
            e->setInformation(Eigen::Matrix<double, 1, 1>::Identity());
            e->setVertex(0, params);
            e->setMeasurement(points[i]);
            optimizer.addEdge(e);
        }

        // perform the optimization
        optimizer.initializeOptimization();
        optimizer.setVerbose(verbose);
        optimizer.optimize(maxIterations);

        if (verbose)
            cout << endl;
        for (int i = 0; i < numPoints; ++i) {
            error+= pow(params->estimate()(0) * exp(- params->estimate()(2)*points[i].x() ) + params->estimate()(1) + params->estimate()(3)*points[i].x() - points[i].y(),2);
        }
        cout << "a      = " << params->estimate()(0) << ";"<<endl;
        cout << "b      = " << params->estimate()(1) << ";"<<endl;
        cout << "error  = " << pow(error,0.5)        << ";"<<endl;
        cout << "lambda = " << params->estimate()(2) << ";"<<endl;
        cout << "c      = " << params->estimate()(3) << ";"<<endl;
        delete[] points;
        error=0;
        cout << "x=0:0.2:10;"<<endl;
        cout << "Y_moni=a * exp(- lambda *x) + b + c*x;" <<endl;
        cout << "Y_true=2.1 * exp(- 0.2 *x) + 0.4 + 0*x;" <<endl;
        cout << "plot(y(:,1),y(:,2),\"o\",x,y_moni,x,y_true);" <<endl;
        cout << "legend(\"data_set\",\"simulated curve\",\"real curve\");" <<endl;
    }
  // clean up

  return 0;
}
