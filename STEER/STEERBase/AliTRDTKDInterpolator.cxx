#include "AliTRDTKDInterpolator.h"

#include "TClonesArray.h"
#include "TLinearFitter.h"
#include "TMath.h"
#include "TRandom.h"

#include "iostream"
using namespace std;

ClassImp(AliTRDTKDInterpolator)

//_________________________________________________________________
AliTRDTKDInterpolator::AliTRDTKDInterpolator() :
TKDTreeIF(),
fNDataNodes(0),
fNodes(NULL),
fLambda(0),
fNPointsI(0),
fUseHelperNodes(kFALSE),
fUseWeights(kFALSE),
fPDFMode(kInterpolation)
{
  // default constructor
}

//_________________________________________________________________
AliTRDTKDInterpolator::AliTRDTKDInterpolator(Int_t npoints, Int_t ndim, UInt_t bsize, Float_t **data) :
TKDTreeIF(npoints, ndim, bsize, data),
fNDataNodes(0),
fNodes(NULL),
fLambda(1 + ndim + (ndim*(ndim+1)>>1)),
fNPointsI(100),
fUseHelperNodes(kFALSE),
fUseWeights(kFALSE),
fPDFMode(kInterpolation)
{
}

//_________________________________________________________________
AliTRDTKDInterpolator::~AliTRDTKDInterpolator()
{
    if(fNodes){
	fNodes->Delete();
	delete fNodes;
	fNodes=NULL;
    }
}
//_________________________________________________________________
AliTRDTKDInterpolator::AliTRDTKDInterpolator(const AliTRDTKDInterpolator &ref):
TKDTreeIF(),
fNDataNodes(ref.fNDataNodes),
fNodes(ref.fNodes),
fLambda(ref.fLambda),
fNPointsI(ref.fNPointsI),
fUseHelperNodes(ref.fUseHelperNodes),
fUseWeights(ref.fUseWeights),
fPDFMode(ref.fPDFMode)
{
    // Copy constructor
}

//____________________________________________________________

AliTRDTKDInterpolator &AliTRDTKDInterpolator::operator=(const AliTRDTKDInterpolator &ref){
    //
    // Assignment operator
    //
    if(this == &ref) return *this;

    // Make copy
    TObject::operator=(ref);

    return *this;
}

//_________________________________________________________________
Bool_t AliTRDTKDInterpolator::Build()
{
    TKDTreeIF::Build();
    if(!fBoundaries) MakeBoundaries();

    // allocate interpolation nodes
    fNDataNodes = fNPoints/fBucketSize + ((fNPoints%fBucketSize)?1:0);/*TKDTreeIF::GetNTNodes();*/

    if(fNodes){
	Warning("AliTRDTKDInterpolator::Build()", "Data already allocated.");
	fNodes->Delete();
    } else {
	fNodes = new TClonesArray("AliTRDTKDInterpolator::AliTRDTKDNodeInfo", fNDataNodes);
	fNodes->SetOwner();
    }
    for(int in=0; in<fNDataNodes; in++) new ((*fNodes)[in]) AliTRDTKDNodeInfo(fNDim);

    // Set Interpolator nodes

    for(int inode=0, tnode = fNNodes; inode<fNDataNodes-1; inode++, tnode++){
	AliTRDTKDNodeInfo *node =GetNodeInfo(inode);
	memcpy(node->fBounds,GetBoundary(tnode),2*fNDim*sizeof(Float_t));
	node->fVal[0] =  Float_t(fBucketSize)/fNPoints;
	for(int idim=0; idim<fNDim; idim++) node->fVal[0] /= (node->fBounds[2*idim+1] - node->fBounds[2*idim]);
	node->fVal[1] =  node->fVal[0]/TMath::Sqrt(float(fBucketSize));

	Int_t *indexPoints = GetPointsIndexes(tnode);
	// loop points in this terminal node
	for(int idim=0; idim<fNDim; idim++){
	    node->fData[idim] = 0.;
	    for(int ip = 0; ip<fBucketSize; ip++) node->fData[idim] += fData[idim][indexPoints[ip]];
	    node->fData[idim] /= fBucketSize;
	}
    }

    // Analyze last (incomplete) terminal node

    Int_t counts = fNPoints%fBucketSize;
    counts = counts ? counts : fBucketSize;
    Int_t inode = fNDataNodes - 1, tnode = inode + fNNodes;
    AliTRDTKDNodeInfo *ftnode = GetNodeInfo(inode);
    ftnode->fVal[0] =  Float_t(counts)/fNPoints;
    memcpy(ftnode->fBounds,GetBoundary(tnode),2*fNDim*sizeof(Float_t));
 
    for(int idim=0; idim<fNDim; idim++){
	Float_t dx = ftnode->fBounds[2*idim+1]-ftnode->fBounds[2*idim];
	if(dx < 1.e-30){
	    Warning("AliTRDTKDInterpolator::Build()", "Terminal bucket index[%d] too narrow on the %d dimension.", inode, idim);
	    continue;
	}
	ftnode->fVal[0] /= (ftnode->fBounds[2*idim+1] - ftnode->fBounds[2*idim]);
    }
    ftnode->fVal[1] =  ftnode->fVal[0]/TMath::Sqrt(float(counts));

    // loop points in this terminal node
    Int_t *indexPoints = GetPointsIndexes(tnode);
    for(int idim=0; idim<fNDim; idim++){
	ftnode->fData[idim] = 0.;
	for(int ip = 0; ip<counts; ip++) ftnode->fData[idim] += fData[idim][indexPoints[ip]];
	ftnode->fData[idim] /= counts;
    }

    delete [] fBoundaries;
    fBoundaries = NULL;
    // Add Helper Nodes
    if(fUseHelperNodes){BuildBoundaryNodes();}

    if(fNPointsI>GetNTNodes()){fNPointsI=GetNTNodes();}

    BuildInterpolation();

    return kTRUE;
}


//_________________________________________________________________
Bool_t AliTRDTKDInterpolator::Eval(const Double_t *point, Double_t &result, Double_t &error)
{
    Float_t pointF[fNDim]; // local Float_t conversion for "point"
    for(int idim=0; idim<fNDim; idim++) pointF[idim] = (Float_t)point[idim];
    Int_t nodeIndex = GetNodeIndex(pointF);
    if(nodeIndex<0){
	Error("AliTRDTKDInterpolator::Eval()", "Can not retrive node for data point.");
	result = 0.;
	error = 1.E10;
	return kFALSE;
    }
    AliTRDTKDNodeInfo *node =GetNodeInfo(nodeIndex);

    switch(fPDFMode){

    case kInterpolation:
	return node->CookPDF(point, result, error);
    case kMinError:
	node->CookPDF(point, result, error);
	if(error<node->fVal[1]){
	    return kTRUE;
	}
	error=node->fVal[1];
	result=node->fVal[0];
	return kTRUE;
    case kNodeVal:
	error=node->fVal[1];
	result=node->fVal[0];
	return kTRUE;
    default:
	return kFALSE;
    }
}

//__________________________________________________________________
void AliTRDTKDInterpolator::Print(const Option_t */*opt*/) const
{
    for(Int_t i=GetNTNodes(); i--;){
	printf("Node %d of %d: \n",i,GetNTNodes());
	GetNodeInfo(i)->Print();
    }

}

//__________________________________________________________________
Int_t AliTRDTKDInterpolator::GetNodeIndex(const Float_t *p)
{
    Int_t inode=FindNode(p)-fNDataNodes+1;
    if(GetNodeInfo(inode)->Has(p))return inode;

    // Search extra nodes

    for(inode=fNDataNodes;inode<GetNTNodes();inode++){
	if(GetNodeInfo(inode)->Has(p)){return inode;}
    }

    // Search for nearest neighbor
    Float_t dist;
    Float_t closestdist=10000;
    inode=-1;
    for(Int_t ii=0;ii<GetNTNodes();ii++){
	AliTRDTKDNodeInfo *node=GetNodeInfo(ii);
	dist=0;
	for(Int_t idim=0;idim<fNDim;idim++){
	    dist+=TMath::Power((node->fData[idim]-p[idim]),2);
	}
	dist=TMath::Sqrt(dist);
	if(dist<closestdist){closestdist=dist;inode=ii;}
    }
    return inode;
}

//_________________________________________________________________
AliTRDTKDInterpolator::AliTRDTKDNodeInfo* AliTRDTKDInterpolator::GetNodeInfo(Int_t inode) const
{
  if(!fNodes || inode >= GetNTNodes()) return NULL;
  return (AliTRDTKDNodeInfo*)(*fNodes)[inode];
}

//_________________________________________________________________
Int_t AliTRDTKDInterpolator::GetNTNodes() const 
{
  return fNodes?fNodes->GetEntriesFast():0;
}

//_________________________________________________________________
Bool_t AliTRDTKDInterpolator::GetRange(Int_t idim,Float_t range[2]) const
{
    if(!fNodes) return kFALSE;
    if(idim<0 || idim>=fNDim){
    range[0]=0.; range[1]=0.;
    return kFALSE;
    }
    range[0]=1.e10; range[1]=-1.e10;
    for(Int_t in=GetNTNodes(); in--; ){
	AliTRDTKDNodeInfo *node = GetNodeInfo(in);

	if(node->fBounds[2*idim]<range[0]) range[0] = node->fBounds[2*idim];
	if(node->fBounds[2*idim+1]>range[1]) range[1] = node->fBounds[2*idim+1];
    }

    return kTRUE;
}

//_________________________________________________________________
TH2Poly *AliTRDTKDInterpolator::Projection(Int_t xdim,Int_t ydim)
{
    Float_t rangex[2],rangey[2];
    GetRange(xdim,rangex);
    GetRange(ydim,rangey);

    TH2Poly* h2 = new TH2Poly("hTKDnodes","hTKDnodes", rangex[0],rangex[1],rangey[0],rangey[1]);
    h2->GetXaxis()->SetTitle(Form("Q_{%d}", xdim));
    h2->GetYaxis()->SetTitle(Form("Q_{%d}", ydim));

    for(Int_t inode=0;inode<GetNTNodes();inode++){

	AliTRDTKDNodeInfo* node=GetNodeInfo(inode);
	h2->AddBin(node->fBounds[2*xdim],node->fBounds[2*ydim],node->fBounds[2*xdim+1],node->fBounds[2*ydim+1]);
	h2->SetBinContent(inode+1, node->fVal[0]);
    }
    return h2;
}

//_________________________________________________________________
void AliTRDTKDInterpolator::BuildInterpolation()
{

    // Calculate Interpolation

    Double_t buffer[fLambda];

    Float_t **refPoints = new Float_t*[fNDim];
    for(int id=0; id<fNDim; id++){
	refPoints[id] = new Float_t[GetNTNodes()];
	for(int in=0; in<GetNTNodes(); in++) refPoints[id][in] = GetNodeInfo(in)->fData[id];
    }
    TKDTreeIF *KDhelper = new TKDTreeIF(GetNTNodes(), fNDim, 30, refPoints);
    KDhelper->Build();
    KDhelper->MakeBoundariesExact();

    Float_t dist[fNPointsI];
    Int_t ind[fNPointsI];

    TLinearFitter fitter(fLambda, Form("hyp%d", fLambda-1));

    Int_t nodeIndex(0); Float_t param[6], *pp(NULL);
    nodeIndex=GetNTNodes(); pp=&param[0];
    while(nodeIndex--){

	fitter.ClearPoints();

	AliTRDTKDNodeInfo *node = GetNodeInfo(nodeIndex);
	// find nearest neighbors
	KDhelper->FindNearestNeighbors(node->fData,fNPointsI, &ind[0], &dist[0]);

	for(int in=0;in<fNPointsI;in++){
	    AliTRDTKDNodeInfo *nnode = GetNodeInfo(ind[in]);

            Float_t w=1; //weight
	    // calculate tri-cubic weighting function
	    if(fUseWeights){
		Float_t d = dist[in]/dist[fNPointsI-1];
		Float_t w0 = (1. - d*d*d);
		w = w0*w0*w0;
		if(w<1.e-30) continue;
	    }
	    Int_t ipar=0;
	    for(int idim=0; idim<fNDim; idim++){
		buffer[ipar++] = nnode->fData[idim];
		for(int jdim=idim; jdim<fNDim; jdim++) buffer[ipar++] = nnode->fData[idim]*nnode->fData[jdim];
	    }
	    fitter.AddPoint(buffer,nnode->fVal[0], nnode->fVal[1]/w);

	    // Ensure Boundary Condition
	    for(Int_t kdim=0;kdim<fNDim;kdim++){
		if(node->fBounds[2*kdim]==0){
		    Float_t zdata[fNDim];
                    memcpy(&zdata[0],node->fData,fNDim*sizeof(Float_t));
		    zdata[kdim]=0;
		    ipar=0;
		    for(int idim=0; idim<fNDim; idim++){
			buffer[ipar++] = zdata[idim];
			for(int jdim=idim; jdim<fNDim; jdim++) buffer[ipar++] = zdata[idim]*zdata[jdim];
		    }
		    fitter.AddPoint(buffer,0,1);
		}
	    }
	}

	fitter.Eval();

	// retrive fitter results
	TMatrixD cov(fLambda, fLambda);
	TVectorD par(fLambda);
	fitter.GetCovarianceMatrix(cov);
	fitter.GetParameters(par);

	// store results
	node->Store(&par,&cov);
    }

    delete KDhelper;
    for(int id=0; id<fNDim; id++){
	delete refPoints[id];
    }
    delete[] refPoints;
}

//_________________________________________________________________
void AliTRDTKDInterpolator::BuildBoundaryNodes(){

    Int_t nnew=0;

    Float_t treebounds[2*fNDim];
    for(Int_t idim=0;idim<fNDim;idim++){
	GetRange(idim,&treebounds[2*idim]);
    }

    for(int inode=0; inode<GetNTNodes(); inode++){

	AliTRDTKDNodeInfo *node=GetNodeInfo(inode);

	for(Int_t vdim=0;vdim<fNDim;vdim++){

	    // Try expansion to lower and higher values
	    for(Int_t iter=0;iter<2;iter++){
		if(node->fBounds[2*vdim+iter]==treebounds[2*vdim+iter]){

		    // Add new Node
		    new ((*fNodes)[GetNTNodes()]) AliTRDTKDNodeInfo(fNDim);

		    AliTRDTKDNodeInfo *newnode = GetNodeInfo(GetNTNodes()-1);
		    if(iter==0)newnode->fBounds[2*vdim+iter]=0;
                    if(iter==1)newnode->fBounds[2*vdim+iter]=2*treebounds[2*vdim+iter];
		    newnode->fBounds[2*vdim+!iter]=node->fBounds[2*vdim+iter];
		    for(Int_t idim=0;idim<fNDim;idim++){
			if(idim==vdim)continue;
			newnode->fBounds[2*idim]=node->fBounds[2*idim];
			newnode->fBounds[2*idim+1]=node->fBounds[2*idim+1];
		    }
		    newnode->fVal[0]=0;
		    newnode->fVal[1]=Float_t(1)/fNPoints;
		    for(int idim=0; idim<fNDim; idim++){
			newnode->fVal[1] /= (newnode->fBounds[2*idim+1] - newnode->fBounds[2*idim]);
                        newnode->fData[idim]=0.5*(newnode->fBounds[2*idim+1] + newnode->fBounds[2*idim]);
		    }
		    nnew++;
		}
	    }
	}
    }
    printf("%d Boundary Nodes Added \n",nnew);
}

//_________________________________________________________________
AliTRDTKDInterpolator::AliTRDTKDNodeInfo::AliTRDTKDNodeInfo(Int_t ndim):
  TObject()
  ,fNDim(ndim)
  ,fNBounds(2*ndim)
  ,fNPar(1 + ndim + (ndim*(ndim+1)>>1))
  ,fNCov(fNPar*fNPar)
  ,fData(NULL)
  ,fBounds(NULL)
  ,fPar(NULL)
  ,fCov(NULL)
{
  // Default constructor
    fVal[0] = 0.; fVal[1] = 0.;
    fData=new Float_t[fNDim];
    fBounds=new Float_t[fNBounds];
}

//_________________________________________________________________
AliTRDTKDInterpolator::AliTRDTKDNodeInfo::AliTRDTKDNodeInfo(const AliTRDTKDInterpolator::AliTRDTKDNodeInfo &ref):
TObject(ref)
   ,fNDim(ref.fNDim)
   ,fNBounds(ref.fNBounds)
   ,fNPar(ref.fNPar)
   ,fNCov(ref.fNCov)
   ,fData(NULL)
   ,fBounds(NULL)
   ,fPar(NULL)
   ,fCov(NULL)
{
  // Copy constructor

    if(ref.fData){
	fData = new Float_t[fNDim];
	memcpy(fData, ref.fData, fNDim*sizeof(Float_t));
    }
    if(ref.fBounds){
	fBounds = new Float_t[2*fNDim];
	memcpy(fBounds, ref.fBounds, 2*fNDim*sizeof(Float_t));
    }

    fVal[0] = ref.fVal[0];
    fVal[1] = ref.fVal[1];

    if(ref.fPar){
	fPar=new Double_t[fNPar];
	memcpy(fPar, ref.fPar, fNPar*sizeof(Double_t));
    }
    if(ref.fCov){
	fCov=new Double_t[fNCov];
	memcpy(fCov, ref.fCov, fNCov*sizeof(Double_t));
    }
}

//_________________________________________________________________
AliTRDTKDInterpolator::AliTRDTKDNodeInfo::~AliTRDTKDNodeInfo()
{
  // Destructor
  if(fData) delete [] fData;
  if(fBounds) delete [] fBounds;
  if(fPar) delete [] fPar;
  if(fCov) delete [] fCov;
}

//_________________________________________________________________
AliTRDTKDInterpolator::AliTRDTKDNodeInfo &AliTRDTKDInterpolator::AliTRDTKDNodeInfo::operator=(const AliTRDTKDInterpolator::AliTRDTKDNodeInfo &ref)
{
    //
    // Assignment operator
    //

    if(&ref==this) return *this;
    memcpy(fData, ref.fData, fNDim*sizeof(Float_t));
    memcpy(fBounds, ref.fBounds, 2*fNDim*sizeof(Float_t));

    fVal[0] = ref.fVal[0];
    fVal[1] = ref.fVal[1];

    if(ref.fPar){
	fPar=new Double_t[fNPar];
	memcpy(fPar, ref.fPar, fNPar*sizeof(Double_t));
    }
    if(ref.fCov){
	fCov=new Double_t[fNCov];
	memcpy(fCov, ref.fCov, fNCov*sizeof(Double_t));
    }
    return *this;
}


//_________________________________________________________________
void AliTRDTKDInterpolator::AliTRDTKDNodeInfo::Print(const Option_t *opt) const
{
  // Print the content of the node
  printf("x [");
  for(int idim=0; idim<fNDim; idim++) printf("%f ", fData?fData[idim]:0.);
  printf("] f = [%e +- %e]\n", fVal[0], fVal[1]);

  if(fBounds){
      printf("range [");
      for(int idim=0; idim<fNDim; idim++) printf("(%f %f) ", fBounds[2*idim], fBounds[2*idim+1]);
    printf("]\n");
  }
  if(strcmp(opt, "a")!=0) return;

  if(fPar){
    printf("Fit parameters : \n");
    for(int ip=0; ip<fNPar; ip++) printf("p%d[%f] ", ip, fPar[ip]);
    printf("\n");
  }
  if(!fCov) return;
  for(int ip(0), n(0); ip<fNPar; ip++){
    for(int jp(ip); jp<fNPar; jp++) printf("c(%d %d)[%f] ", ip, jp, fCov[n++]);
    printf("\n");
  }
}

//_________________________________________________________________
void AliTRDTKDInterpolator::AliTRDTKDNodeInfo::Store(TVectorD const *par, TMatrixD const *cov)
{
// Store the parameters and the covariance in the node

    if(!fPar){fPar = new Double_t[fNPar];}
    for(int ip=0; ip<fNPar; ip++) fPar[ip] = (*par)[ip];

    if(!cov) return;
    if(!fCov){fCov = new Double_t[fNCov];}
    for(int ip(0), np(0); ip<fNPar; ip++)
	for(int jp=ip; jp<fNPar; jp++) fCov[np++] = (*cov)(ip,jp);
}

//_________________________________________________________________
Bool_t AliTRDTKDInterpolator::AliTRDTKDNodeInfo::CookPDF(const Double_t *point, Double_t &result, Double_t &error) const
{
    // Recalculate the PDF for one node from the results of interpolation (parameters and covariance matrix)

    result =0.; error = 1.;
    if(!fPar) return kFALSE;

    Double_t fdfdp[fNDim];
    Int_t ipar = 0;
    fdfdp[ipar++] = 1.;
    for(int idim=0; idim<fNDim; idim++){
	fdfdp[ipar++] = point[idim];
	for(int jdim=idim; jdim<fNDim; jdim++) fdfdp[ipar++] = point[idim]*point[jdim];
    }

    // calculate estimation
    for(int i=0; i<fNPar; i++) result += fdfdp[i]*fPar[i];

    if(!fCov)return kTRUE;
    // calculate error
    error=0;

    for(int i(0), n(0); i<fNPar; i++){
	error += fdfdp[i]*fdfdp[i]*fCov[n++];
	for(int j(i+1); j<fNPar; j++) error += 2.*fdfdp[i]*fdfdp[j]*fCov[n++];
    }
    if(error>0)error = TMath::Sqrt(error);
    else{error=100;}

    // Boundary condition
    if(result<0){
	result=fVal[0];
	error=fVal[1];
    }

    return kTRUE;
}

//_____________________________________________________________________
Bool_t AliTRDTKDInterpolator::AliTRDTKDNodeInfo::Has(const Float_t *p) const
{
  Int_t n(0);
  for(int id=0; id<fNDim; id++)
      if(p[id]>=fBounds[2*id] && p[id]<fBounds[2*id+1]) n++;
  if(n==fNDim) return kTRUE;
  return kFALSE;
}

