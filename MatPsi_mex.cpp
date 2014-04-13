#include "mex.h"
#include "class_handle.hpp"
#include "MatPsi.h"
#include "MatPsi.cc"

using namespace std;
using namespace psi;
using namespace boost;

SharedMatrix InputMatrix(mxArray*& Mat_m) {
	int nrow = mxGetM(Mat_m);
	int ncol = mxGetN(Mat_m);
    SharedMatrix Mat_c(new Matrix(nrow, ncol));
	double* Mat_m_pt = mxGetPr(Mat_m);
	double** Mat_c_pt = Mat_c->pointer();
	for(int i = 0; i < ncol; i++)
        for(int j = 0; j < nrow; j++)
            Mat_c_pt[j][i] = *Mat_m_pt++; // Matlab loops over a column first, but C++ loops over a row first 
    return Mat_c;
}

void OutputMatrix(mxArray*& Mat_m, SharedMatrix Mat_c) {
	int nrow = Mat_c->nrow();
	int ncol = Mat_c->ncol();
	Mat_m = mxCreateDoubleMatrix( nrow, ncol, mxREAL);
	double* Mat_m_pt = mxGetPr(Mat_m);
	double** Mat_c_pt = Mat_c->pointer();
	for(int i = 0; i < ncol; i++)
        for(int j = 0; j < nrow; j++)
            *Mat_m_pt++ = Mat_c_pt[j][i];
}

void OutputSymmMatrix(mxArray*& Mat_m, SharedMatrix Mat_c) {
	int nrow = Mat_c->nrow();
	int ncol = Mat_c->ncol();
    if (nrow != ncol)
        mexErrMsgTxt("OutputSymmMatrix: Matrix column/row dimensions do not agree. This function can only be used for symmetric matrices.");
	Mat_m = mxCreateDoubleMatrix( nrow, ncol, mxREAL);
	double* Mat_m_pt = mxGetPr(Mat_m);
	double* Mat_c_pt = Mat_c->get_pointer();
	for(int i = 0; i < nrow * ncol; i++) {
		*Mat_m_pt++ = *Mat_c_pt++;
	}
}

void OutputVector(mxArray*& Mat_m, SharedVector Vec_c) {
	int dim = Vec_c->dim();
	Mat_m = mxCreateDoubleMatrix( 1, dim, mxREAL);
	double* Mat_m_pt = mxGetPr(Mat_m);
	double* Vec_c_pt = Vec_c->pointer();
	for(int i = 0; i < dim; i++) {
		*Mat_m_pt++ = *Vec_c_pt++;
	}
}

void OutputScalar(mxArray*& Mat_m, double scalar) {
	Mat_m = mxCreateDoubleMatrix( 1, 1, mxREAL);
	double* Mat_m_pt = mxGetPr(Mat_m);
    *Mat_m_pt = scalar;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {	
    // Get the command string
    char cmd[64];
	if (nrhs < 1 || mxGetString(prhs[0], cmd, sizeof(cmd)))
		mexErrMsgTxt("First input should be a command string less than 64 characters long.");
    
    // Constructor
    if (!strcmp("new", cmd)) {
        // Check parameters
        if (nlhs != 1)
            mexErrMsgTxt("Constructor: One output expected.");
        if (nrhs!=2 || !mxIsCell(prhs[1]) || mxGetNumberOfElements(prhs[1]) != 2)
            mexErrMsgTxt("Constructor: Cell array {mol_string, basis_name} input expected.");
        try {
            // Return a handle to a new C++ instance
            mxArray *tmp;
            tmp = mxGetCell(prhs[1], 0);
            std::string mol_string = mxArrayToString(tmp);
            tmp = mxGetCell(prhs[1], 1);
            std::string basis_name = mxArrayToString(tmp);
            plhs[0] = convertPtr2Mat<MatPsi>(new MatPsi(mol_string, basis_name));
        } 
        catch(...) {
            mexErrMsgTxt("Constructor failed. Sorry!!");
        }
        return;
    }
    
    // Check there is a second input, which should be the class instance handle
    if (nrhs < 2)
		mexErrMsgTxt("Second input should be a class instance handle.");
    
    // Delete
    if (!strcmp("delete", cmd)) {
        // Destroy the C++ object
        destroyObject<MatPsi>(prhs[1]);
        // Warn if other commands were ignored
        if (nlhs != 0 || nrhs != 2)
            mexWarnMsgTxt("Delete: Unexpected arguments ignored.");
        return;
    }
    
    // Get the class instance pointer from the second input
    MatPsi* MatPsi_obj = convertMat2Ptr<MatPsi>(prhs[1]);
    
    // Call the various class methods 
	// Copy Constructor
	if (!strcmp("MatPsiCopy", cmd)) {
        // Check parameters
        if (nlhs != 1)
            mexErrMsgTxt("Copy Constructor: One output expected.");
        try {
            // Return a handle to a new C++ instance
            plhs[0] = convertPtr2Mat<MatPsi>(new MatPsi(MatPsi_obj));
        } 
        catch(...) {
            mexErrMsgTxt("Copy Constructor failed");
        }
        return;
    }
    
    // testmol
    if (!strcmp("testmol", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("testmol: Unexpected arguments.");
        // Call the method
        MatPsi_obj->testmol();
        return;
    }
	
    // Molecule properties
    // natom
    if (!strcmp("natom", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("natom: Unexpected arguments.");
        // Call the method
        OutputScalar(plhs[0], (double)MatPsi_obj->natom());
        return;
    }
    
    // nelec
    if (!strcmp("nelec", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("nelec: Unexpected arguments.");
        // Call the method
        OutputScalar(plhs[0], (double)MatPsi_obj->nelec());
        return;
    }
    
    // coord(natom, 3)
    if (!strcmp("coord", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("coord: Unexpected arguments.");
        // Call the method
        OutputMatrix(plhs[0], MatPsi_obj->coord());
        return;
    }
    
    // Zlist(natom, 1) 
    if (!strcmp("Zlist", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("Zlist: Unexpected arguments.");
        // Call the method
		OutputVector(plhs[0], MatPsi_obj->Zlist());
        return;
    }
    
    // Enuc
    if (!strcmp("Enuc", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("Enuc: Unexpected arguments.");
        // Call the method
        OutputScalar(plhs[0], MatPsi_obj->Enuc());
        return;
    }
    
    // Basis set properties 
    // nbasis
    if (!strcmp("nbasis", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("nbasis: Unexpected arguments.");
        // Call the method
        OutputScalar(plhs[0], (double)MatPsi_obj->nbasis());
        return;
    }
    
    // func2center 
    if (!strcmp("func2center", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("func2center: Unexpected arguments.");
        // Call the method
        SharedVector func2centerVec = MatPsi_obj->func2center();
        for(int i = 0; i < func2centerVec->dim(); i++)
            func2centerVec->add(i, 1.0); // + 1 convert C++ convention to Matlab convention 
        OutputVector(plhs[0], func2centerVec);
        return;
    }
    
    // func2am 
    if (!strcmp("func2am", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("func2am: Unexpected arguments.");
        // Call the method
        OutputVector(plhs[0], MatPsi_obj->func2am());
        return;
    }
	
    // One-electron integrals 
	// overlap(nbasis, nbasis)
    if (!strcmp("overlap", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("overlap: Unexpected arguments.");
        // Call the method
		OutputSymmMatrix(plhs[0], MatPsi_obj->overlap());
        return;
    }
    
    // kinetic(nbasis, nbasis)
    if (!strcmp("kinetic", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("kinetic: Unexpected arguments.");
        // Call the method
		OutputSymmMatrix(plhs[0], MatPsi_obj->kinetic());
        return;
    }
    
    // potential(nbasis, nbasis)
    if (!strcmp("potential", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("potential: Unexpected arguments.");
        // Call the method
		OutputSymmMatrix(plhs[0], MatPsi_obj->potential());
        return;
    }
    
    // potential_sep(nbasis, nbasis, natom)
    if (!strcmp("potential_sep", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("potential_sep: Unexpected arguments.");
        // Call the method
        boost::shared_array<SharedMatrix> viMatArray = MatPsi_obj->potential_sep();
        int ncol = viMatArray[0]->ncol();
        int nrow = viMatArray[0]->nrow();
        int natom = MatPsi_obj->natom();
        int dims[3] = {ncol, nrow, natom};
        plhs[0] = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);
        double* matlab_pt = mxGetPr(plhs[0]);
        for(int iatom = 0; iatom < natom; iatom++) {
            double* tmp_pt = viMatArray[iatom]->get_pointer();
            for(int i = 0; i < ncol * nrow; i++) {
                matlab_pt[iatom*ncol*nrow + i] = tmp_pt[i];
            }
        }
        return;
    }
    
    // potential_zxyz(nbasis, nbasis)
    if (!strcmp("potential_zxyz", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("potential_zxyz: Unexpected arguments.");
        if (nrhs!=3 || !mxIsCell(prhs[2]) || mxGetNumberOfElements(prhs[2]) != 4)
            mexErrMsgTxt("potential_zxyz: Cell array {Z, x, y, z} input expected.");
        // Call the method
        double Zxyz_array[4];
        for(int i = 0; i < 4; i++) {
            Zxyz_array[i] = (double)mxGetScalar(mxGetCell(prhs[2], i));
        }
		OutputSymmMatrix(plhs[0], MatPsi_obj->potential_zxyz(Zxyz_array));
        return;
    }
    
    // potential_zxyzlist(nbasis, nbasis)
    if (!strcmp("potential_zxyzlist", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("potential_zxyzlist: Unexpected arguments.");
        if (nrhs!=3 || !mxIsCell(prhs[2]) || mxGetNumberOfElements(prhs[2]) != 1)
            mexErrMsgTxt("potential_zxyzlist: Cell array {Zxyz_mat} input expected.");
        if ( mxGetN(mxGetCell(prhs[2], 0)) != 4 )
            mexErrMsgTxt("potential_zxyzlist: Zxyz list matrix dimension does not agree.");
        // Call the method
        mxArray* tmp = mxGetCell(prhs[2], 0);
		OutputSymmMatrix(plhs[0], MatPsi_obj->potential_zxyzlist(InputMatrix(tmp)));
        return;
    }
    
    // dipole 3 * (nbasis, nbasis) 
    if (!strcmp("dipole", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("dipole: Unexpected arguments.");
        // Call the method
        std::vector<SharedMatrix> dipole = MatPsi_obj->dipole();
		OutputSymmMatrix(plhs[0], dipole[0]);
        OutputSymmMatrix(plhs[1], dipole[1]);
        OutputSymmMatrix(plhs[2], dipole[2]);
        return;
    }
    
    // Two-electron integrals 
    // tei_ijkl
    if (!strcmp("tei_ijkl", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("tei_ijkl: Unexpected arguments.");
        if (nrhs!=3 || !mxIsCell(prhs[2]) || mxGetNumberOfElements(prhs[2]) != 4)
            mexErrMsgTxt("tei_ijkl: Cell array {i, j, k, l} input expected.");
        // Call the method
        mxArray *tmp;
        int ind[4];
        for(int i = 0; i < 4; i++) {
            tmp = mxGetCell(prhs[2], i);
            ind[i] = (int)mxGetScalar(tmp) - 1; // -1 convert Matlab convention to C++ convention
            if(ind[i] < 0 || ind[i] >= MatPsi_obj->nbasis())
                mexErrMsgTxt("tei_ijkl: Required index not within scale.");
        }
        OutputScalar(plhs[0], MatPsi_obj->tei_ijkl(ind[0], ind[1], ind[2], ind[3]));
        return;
    }
    
    // tei_uniqN 
    if (!strcmp("tei_uniqN", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("tei_uniqN: Unexpected arguments.");
        // Call the method
        OutputScalar(plhs[0], (double)MatPsi_obj->tei_uniqN());
        return;
    }
    
    // tei_alluniq 
    if (!strcmp("tei_alluniq", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("tei_alluniq: Unexpected arguments.");
        // Call the method
        plhs[0] = mxCreateDoubleMatrix( 1, MatPsi_obj->tei_uniqN(), mxREAL);
        double* matpt = mxGetPr(plhs[0]);
        MatPsi_obj->tei_alluniq(matpt);
        return;
    }
    
    // tei_allfull 
    if (!strcmp("tei_allfull", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("tei_alluniq: Unexpected arguments.");
        // Call the method
        int dims[4] = {MatPsi_obj->nbasis(), MatPsi_obj->nbasis(), MatPsi_obj->nbasis(), MatPsi_obj->nbasis()};
        plhs[0] = mxCreateNumericArray(4, dims, mxDOUBLE_CLASS, mxREAL);
        double* matpt = mxGetPr(plhs[0]);
        MatPsi_obj->tei_allfull(matpt);
        return;
    }
    
    // tei_alluniqJK 
    if (!strcmp("tei_alluniqJK", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("tei_alluniqJK: Unexpected arguments.");
        // Call the method
        plhs[0] = mxCreateDoubleMatrix( 1, MatPsi_obj->tei_uniqN(), mxREAL);
        double* matptJ = mxGetPr(plhs[0]);
        plhs[1] = mxCreateDoubleMatrix( 1, MatPsi_obj->tei_uniqN(), mxREAL);
        double* matptK = mxGetPr(plhs[1]);
        MatPsi_obj->tei_alluniqJK(matptJ, matptK);
        return;
    }
    
    // SCF related 
    // OccMO2J(nbasis, nbasis) 
    if (!strcmp("OccMO2J", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("OccMO2J: Unexpected arguments.");
        if (nrhs!=3 || !mxIsCell(prhs[2]) || mxGetNumberOfElements(prhs[2]) != 1)
            mexErrMsgTxt("OccMO2J: Cell array containing a (nbasis by noccupy) matrix {MOmat} input expected.");
        if ( mxGetM(mxGetCell(prhs[2], 0)) != MatPsi_obj->nbasis() )
            mexErrMsgTxt("OccMO2J: Occupied MO matrix dimension does not agree.");
        // Call the method
        mxArray* tmp = mxGetCell(prhs[2], 0);
		OutputSymmMatrix(plhs[0], MatPsi_obj->OccMO2J(InputMatrix(tmp)));
        return;
    }
    
    // OccMO2K(nbasis, nbasis) 
    if (!strcmp("OccMO2K", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("OccMO2K: Unexpected arguments.");
        if (nrhs!=3 || !mxIsCell(prhs[2]) || mxGetNumberOfElements(prhs[2]) != 1)
            mexErrMsgTxt("OccMO2K: Cell array containing a (nbasis by noccupy) matrix {MOmat} input expected.");
        if ( mxGetM(mxGetCell(prhs[2], 0)) != MatPsi_obj->nbasis() )
            mexErrMsgTxt("OccMO2K: Occupied MO matrix dimension does not agree.");
        // Call the method
        mxArray* tmp = mxGetCell(prhs[2], 0);
		OutputSymmMatrix(plhs[0], MatPsi_obj->OccMO2K(InputMatrix(tmp)));
        return;
    }
    
    // OccMO2G(nbasis, nbasis) 
    if (!strcmp("OccMO2G", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("OccMO2G: Unexpected arguments.");
        if (nrhs!=3 || !mxIsCell(prhs[2]) || mxGetNumberOfElements(prhs[2]) != 1)
            mexErrMsgTxt("OccMO2G: Cell array containing a (nbasis by noccupy) matrix {MOmat} input expected.");
        if ( mxGetM(mxGetCell(prhs[2], 0)) != MatPsi_obj->nbasis() )
            mexErrMsgTxt("OccMO2G: Occupied MO matrix dimension does not agree.");
        // Call the method
        mxArray* tmp = mxGetCell(prhs[2], 0);
		OutputSymmMatrix(plhs[0], MatPsi_obj->OccMO2G(InputMatrix(tmp)));
        return;
    }
    
    // RHF
    if (!strcmp("RHF", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("RHF: Unexpected arguments.");
        // Call the method
        OutputScalar(plhs[0], MatPsi_obj->RHF());
        return;
    }
    
    // RHF_finalize
    if (!strcmp("RHF_finalize", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("RHF_finalize: Unexpected arguments.");
        // Call the method
        MatPsi_obj->RHF_finalize();
        return;
    }
    
    // RHF_EHF 
    if (!strcmp("RHF_EHF", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("RHF_EHF: Unexpected arguments.");
        // Call the method
        OutputScalar(plhs[0], MatPsi_obj->RHF_EHF());
        return;
    }
    
    // RHF_C 
    if (!strcmp("RHF_C", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("RHF_C: Unexpected arguments.");
        // Call the method
        OutputMatrix(plhs[0], MatPsi_obj->RHF_C());
        return;
    }
    
    // RHF_EMO 
    if (!strcmp("RHF_EMO", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("RHF_EMO: Unexpected arguments.");
        // Call the method
        OutputVector(plhs[0], MatPsi_obj->RHF_EMO());
        return;
    }
    
    // RHF_D 
    if (!strcmp("RHF_D", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("RHF_density: Unexpected arguments.");
        // Call the method
        OutputSymmMatrix(plhs[0], MatPsi_obj->RHF_D());
        return;
    }
    
    // RHF_H 
    if (!strcmp("RHF_H", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("RHF_H: Unexpected arguments.");
        // Call the method
        OutputSymmMatrix(plhs[0], MatPsi_obj->RHF_H());
        return;
    }
    
    // RHF_J 
    if (!strcmp("RHF_J", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("RHF_J: Unexpected arguments.");
        // Call the method
        OutputSymmMatrix(plhs[0], MatPsi_obj->RHF_J());
        return;
    }
    
    // RHF_K 
    if (!strcmp("RHF_K", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("RHF_K: Unexpected arguments.");
        // Call the method
        OutputSymmMatrix(plhs[0], MatPsi_obj->RHF_K());
        return;
    }
    
    // RHF_F 
    if (!strcmp("RHF_F", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("RHF_F: Unexpected arguments.");
        // Call the method
        OutputSymmMatrix(plhs[0], MatPsi_obj->RHF_F());
        return;
    }
    
    // Got here, so command not recognized
    mexErrMsgTxt("Command not recognized.");
}

