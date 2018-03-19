#include <TChain.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TVector3.h>

/**
 * \class Event
 * \brief A container for GST data.
 *
 * This class provides a more object-like interface to the GENIE Summary Tree.
 */
class Event {
public:
  Event(TTree* _t) : gst(_t) {
    gst->SetBranchAddress("neu", &neu);
    gst->SetBranchAddress("tgt", &tgt);
    gst->SetBranchAddress("nuance_code", &nuance_code);
    gst->SetBranchAddress("qel", &qel);
    gst->SetBranchAddress("res", &res);
    gst->SetBranchAddress("dis", &dis);
    gst->SetBranchAddress("coh", &coh);
    gst->SetBranchAddress("dfr", &dfr);
    gst->SetBranchAddress("imd", &imd);
    gst->SetBranchAddress("nuel", &nuel);
    gst->SetBranchAddress("cc", &cc);
    gst->SetBranchAddress("nc", &nc);
    gst->SetBranchAddress("pxv", &pxv);
    gst->SetBranchAddress("pyv", &pyv);
    gst->SetBranchAddress("pzv", &pzv);
    gst->SetBranchAddress("pxl", &pxl);
    gst->SetBranchAddress("pyl", &pyl);
    gst->SetBranchAddress("pzl", &pzl);
    gst->SetBranchAddress("x", &x);
    gst->SetBranchAddress("y", &y);
    gst->SetBranchAddress("t", &t);
    gst->SetBranchAddress("Q2", &q2);
    gst->SetBranchAddress("W", &w);
    gst->SetBranchAddress("Ev", &enu);
    gst->SetBranchAddress("El", &elep);

    gst->SetBranchAddress("ni", &ni);
    gst->SetBranchAddress("pdgi", &pdgi);
    gst->SetBranchAddress("Ei", &ei);
    gst->SetBranchAddress("pxi", &pxi);
    gst->SetBranchAddress("pyi", &pyi);
    gst->SetBranchAddress("pzi", &pzi);
    gst->SetBranchAddress("nip", &nip);
    gst->SetBranchAddress("nin", &nin);
    gst->SetBranchAddress("nipip", &nipip);
    gst->SetBranchAddress("nipim", &nipim);
    gst->SetBranchAddress("nipi0", &nipi0);
    gst->SetBranchAddress("nikp", &nikp);
    gst->SetBranchAddress("nikm", &nikm);
    gst->SetBranchAddress("nik0", &nik0);
    gst->SetBranchAddress("niem", &niem);

    gst->SetBranchAddress("nf", &nf);
    gst->SetBranchAddress("pdgf", &pdgf);
    gst->SetBranchAddress("Ef", &ef);
    gst->SetBranchAddress("pxf", &pxf);
    gst->SetBranchAddress("pyf", &pyf);
    gst->SetBranchAddress("pzf", &pzf);
    gst->SetBranchAddress("nfp", &nfp);
    gst->SetBranchAddress("nfn", &nfn);
    gst->SetBranchAddress("nfpip", &nfpip);
    gst->SetBranchAddress("nfpim", &nfpim);
    gst->SetBranchAddress("nfpi0", &nfpi0);
    gst->SetBranchAddress("nfkp", &nfkp);
    gst->SetBranchAddress("nfkm", &nfkm);
    gst->SetBranchAddress("nfk0", &nfk0);
    gst->SetBranchAddress("nfem", &nfem);
  }

  /// Convenience getters
  int intmode() {
    int imode = 10;
    if      (cc && qel) imode = 0;
    else if (cc && res) imode = 2;
    else if (cc && dis) imode = 3;
    else if (cc && coh) imode = 4;
    else if (cc && nuance_code == 0) imode = 1;  // Hmm...
    else if (nc && qel) imode = 5;
    else if (nc && res) imode = 7;
    else if (nc && dis) imode = 8;
    else if (nc && coh) imode = 9;
    else if (nc && nuance_code == 0) imode = 6;

    return imode;
  }

  double q0() { return enu - elep; }

  double q3() {
    return (TVector3(pxv, pyv, pzv) - TVector3(pxl, pyl, pzl)).Mag();
  }

  TLorentzVector pnu() { return TLorentzVector(pxv, pyv, pzv, enu); }

  TLorentzVector plep() { return TLorentzVector(pxl, pyl, pzl, elep); }

  double lmass() {
    float lm = -9999;

    if (cc && abs(neu) == 12) {
      lm = 0.510999e-3;
    }
    else if (cc && abs(neu) == 14) {
      lm = 105.658e-3;
    }
    else if (cc && abs(neu) == 16) {
      lm = 1776.82e-3;
    }
    else if (nc) {
      lm = 0;
    }

    return lm;
  }

  float tmu() { return elep - lmass(); }

  float ctmu() { return plep().Vect().CosTheta(); }

  /// Event type selections
  typedef bool (*EventType)(Event& e);

  static bool isAny(Event& e) { return true; }

  static bool isCCQE(Event& e) { return e.intmode() == 0; }

  static bool isCCMEC(Event& e) { return e.intmode() == 1; }

  static bool is1l1p0pi0(Event& e) {
    unsigned np = 0;
    unsigned npi0 = 0;

    for (int ii=0; ii<e.nf; ii++) {
      if (e.pdgf[ii] == 2212 && e.ef[ii] - 0.938272 > 0.060) np++;
      if (e.pdgf[ii] == 111) npi0++;
    }

    return (e.cc && np == 1 && npi0 == 0 &&
            ((abs(e.neu) == 12 && e.tmu() > 0.030) ||
             (abs(e.neu) == 14 && e.tmu() > 0.060)));
  }

  static bool is1l1trk0pi0(Event& e) {
    unsigned np = 0;
    unsigned npi0 = 0;
    unsigned npiq = 0;

    for (int ii=0; ii<e.nf; ii++) {
      if (e.pdgf[ii] == 2212 && e.ef[ii] - 0.938272 > 0.060) np++;
      if (e.pdgf[ii] == 111) npi0++;
      if (abs(e.pdgf[ii] == 211)) npiq++;
    }

    return (e.cc && np + npiq == 1 &&
            ((abs(e.neu) == 12 && e.tmu() > 0.030) ||
             (abs(e.neu) == 14 && e.tmu() > 0.060)));
  }

  /// Tree 
  TTree* gst;
  static const int kNPmax = 250;  // Matches GENIE gntpc

  long GetEntries() { return gst->GetEntries(); }
  void GetEntry(long i) { gst->GetEntry(i); }

  // General
  int neu, tgt, nuance_code;
  bool qel, res, dis, coh, dfr, imd, nuel, cc, nc;
  double pxv, pyv, pzv, pxl, pyl, pzl, x, y, t, q2, w, enu, elep;

  // Pre-FSI
  int ni, nip, nin, nipip, nipim, nipi0, nikp, nikm, nik0, niem;
  int pdgi[kNPmax];
  double ei[kNPmax], pxi[kNPmax], pyi[kNPmax], pzi[kNPmax];

  // Post-FSI
  int nf, nfp, nfn, nfpip, nfpim, nfpi0, nfkp, nfkm, nfk0, nfem;
  int pdgf[kNPmax];
  double ef[kNPmax], pxf[kNPmax], pyf[kNPmax], pzf[kNPmax];
};


