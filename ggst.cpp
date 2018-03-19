/**
 * Plot distributions from GENIE MC with different models.
 *
 * Reads a set of GST-format ntuples from a directory, and plots
 * distributions of interesting quantities for model comparison.
 *
 * A. Mastbaum <mastbaum@uchicago.edu>, 2018/01/12
 */

#include <cassert>
#include <iostream>
#include <TCanvas.h>
#include <TChain.h>
#include <TError.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLorentzVector.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TString.h>
#include <TStyle.h>
#include <TVector3.h>

TPaveText* genie_label(TString config, TString gen) {
    TPaveText* l = new TPaveText(0.17, 0.6, 0.62, 0.88, "ndc");
    l->SetTextFont(132);
    l->SetTextAlign(12);
    l->SetBorderSize(0);
    l->SetFillColor(0);
    l->SetFillStyle(0);
    l->AddText("GENIE 2.12.10, BNB #nu_{#mu}");
    l->AddText(config);
    l->AddText(TString("Generators: ") + gen);
    return l;
}


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

  double q3() { return (TVector3(pxv, pyv, pzv) - TVector3(pxl, pyl, pzl)).Mag(); }

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


/// Histogram containers
class Hist {
public:
  Hist(TH1* _h) : h(_h) {}

  virtual void Fill(Event& ev) = 0;

  virtual void Write(TString config, TString gen, TFile* f, bool lines=false, bool label=false) {
    char cname[150];
    snprintf(cname, 150, "c_%s", h->GetName());
    TCanvas* c = new TCanvas();
    c->SetName(cname);
    c->SetLeftMargin(0.15);
    h->Draw("colz");

    TPaveText* l1 = NULL;
    TPaveText* l2 = NULL;
    std::vector<TF1*> qfs, wfs;
    if (std::string(h->GetName()).find("q0q3") != std::string::npos) {
      // Labels
      l1 = genie_label(config, gen);
      l1->AddText("Lines W = 938, 1232, 1535 MeV");
      l1->AddText("Lines Q^{2} = 0.2 - 1.0 GeV");
      l1->Draw();

      // Q2 and W lines
      float lw = 2;
      std::vector<float> qs = {200, 400, 600, 800, 1000};
      for (size_t j=0; j<qs.size(); j++) {
        char fname[150];
        snprintf(fname, 150, "fq_%f", qs[j]);
        char ff[350];
        snprintf(ff, 350, "sqrt(TMath::Max(x**2-%f, 0.0))", qs[j]/1000);
        TF1* f = new TF1(fname, ff, qs[j]/1000, 1.3);
        f->SetNpx(1000);
        f->SetLineColor(kWhite);
        f->SetLineWidth(lw);
        f->Draw("same");
        qfs.push_back(f);
      }

      std::vector<float> ws = {0.938, 1.232, 1.535};
      for (size_t j=0; j<ws.size(); j++) {
        char fname[150];
        snprintf(fname, 150, "fw_%f", qs[j]);
        char ff[350];
        snprintf(ff, 350, "sqrt(%f**2 + x**2) - 0.938", ws[j]);
        TF1* f = new TF1(fname, ff, 0, 1.3);
        f->SetNpx(1000);
        f->SetLineColor(kWhite);
        f->SetLineWidth(lw);
        f->Draw("same");
        qfs.push_back(f);
      }
    }
    else if (label) {
      l2 = genie_label(config, gen);
      l2->Draw();
    }

    c->RedrawAxis();
    c->Update();
    f->cd();
    h->Write();
    c->Write();
    c->SaveAs(TString("./") + config + "_" + gen + "_" + cname + ".pdf"); 
  }

  TH1* h;
};


class Hist_q0q3 : public Hist {
public:
  Hist_q0q3(TString name) : Hist(
    new TH2D(name,
             ";Three-momentum transfer q^{3} (GeV);Energy transfer q^{0} (GeV)",
             100, 0, 1.250, 100, 0, 1.250)) {}
 
  void Fill(Event& ev) {
    h->Fill(ev.q3(), ev.q0());
  }

  void Write(TString config, TString gen, TFile* f) {
    Hist::Write(config, gen, f, true);
  }
};


class Hist_nuanceCode : public Hist {
public:
  Hist_nuanceCode(TString name) : Hist(
    new TH1D(name,
             ";NUANCE interaction code;Entries",
             100, 0, 100)) {}

  void Fill(Event& ev) {
    h->Fill(ev.nuance_code);
  }
};


class Hist_number : public Hist {
public:
  Hist_number(TString name, TString title, int* _number) : Hist(
    new TH1D(name,
             TString(";Number of ") + title + ";Entries",
             21, 0, 21)), number(_number) {}

  void Fill(Event& ev) {
    h->Fill(*number);
  }

  int* number;
};


class Hist_intmode : public Hist {
public:
  Hist_intmode(TString name) : Hist(
    new TH1I(name,
             ";Interaction mode;Entries",
             11, 0, 11)) {
    const char* modes[11] = {
      "CCQE", "CCMEC", "CCRes", "CCDIS", "CCCoh",
      "NCEL", "NCMEC", "NCRes", "NCDIS", "NCCoh",
      "Other"
    };

    for (int i=1; i<=11; i++) {
      h->GetXaxis()->SetBinLabel(i, modes[i-1]);
    }
  }

  void Fill(Event& ev) {
    h->Fill(ev.intmode());
  }
};


class Hist_ke : public Hist {
public:
  Hist_ke(TString name, TString particle) : Hist(
    new TH1D(name,
             TString(";")  + particle + " Kinetic energy T_{" + particle + "} (GeV)",
             200, 0, 3.500)) {}

  void Fill(Event& ev) {
    h->Fill(ev.tmu());
  }
};


class Hist_cosTheta : public Hist {
public:
  Hist_cosTheta(TString name, TString particle) : Hist(
    new TH1D(name,
             TString(";cos#theta_{") + particle + "}",
             100, -1, 1)) {}

  void Fill(Event& ev) {
    h->Fill(ev.ctmu());
  }
};


class Hist_Tct : public Hist {
public:
  Hist_Tct(TString name, TString particle) : Hist(
    new TH2D(name,
             TString(";") + particle + " Kinetic energy p_{" + particle + "} (GeV);cos#theta_{" + particle + "}",
             100, 0, 3.5, 100, -1, 1)) {}

  void Fill(Event& ev) {
    h->Fill(ev.tmu(), ev.ctmu());
  }
};


class Hist_leadpKE : public Hist {
public:
  Hist_leadpKE(TString name, TString particle) : Hist(
    new TH1D(name,
             TString(";")  + particle + " Kinetic energy T_{" + particle + "} (GeV)",
             200, 0, 1.0)) {}

  void Fill(Event& ev) {
    std::vector<double> pke;
    for (int ii=0; ii<ev.ni; ii++) {
      if (ev.pdgi[ii] == 2212) {
        pke.push_back(ev.ei[ii] - 0.938272);
      }
    }

    std::sort(pke.begin(), pke.end());

    if (!pke.empty()) {
      h->Fill(pke[pke.size() - 1]);
    }
  }
};


class Hist_pKE : public Hist {
public:
  Hist_pKE(TString name) : Hist(
    new TH2D(name,
             ";Leading p kinetic energy T_{p1} (GeV);Subleading p kinetic energy T_{p2} (GeV)",
             100, 0, 1.000, 100, 0, 1.000)) {}

  void Fill(Event& ev) {
    // Proton kinetic energies: build a stack
    std::vector<double> pke;
    for (int ii=0; ii<ev.ni; ii++) {
      if (ev.pdgi[ii] == 2212) {
        pke.push_back(ev.ei[ii] - 0.938272);
      }
    }

    if (pke.size() > 2) {
      std::cout << "MEC party: " << pke.size() << " intermediate protons" << std::endl;
    }

    if (pke.size() > 1) {
      double p1 = TMath::Max(pke[0], pke[1]);
      double p2 = TMath::Min(pke[0], pke[1]);
      h->Fill(p1, p2);
    }
  }

  void Write(TString config, TString gen, TFile* f) { 
    h->GetXaxis()->SetRangeUser(0, 0.8);
    h->GetYaxis()->SetRangeUser(0, 0.8);
    TPaveText* l = genie_label(config, gen);
    l->Draw();
    Hist::Write(config, gen, f, false, true);
  }
};


// Main function
int ggst(std::vector<TString> files) {
  gROOT->SetBatch(true);
  gROOT->ProcessLine(".x ~/.rootlogon.C");
  gStyle->SetOptStat(0);
  gErrorIgnoreLevel = kError;

  assert(!files.empty());

  // Extract configuration name (FRAGILE!)
  TString dir, config, gen;
  if (files.size() > 1) {
    // Many files: use first file path
    TObjArray* path = files[0].Tokenize("/");
    dir = ((TObjString*)(path->At(path->GetEntries()-2)))->GetString();
    TObjArray* conf = dir.Tokenize("_");
    config = ((TObjString*)(conf->At(0)))->GetString();
    gen = ((TObjString*)(conf->At(1)))->GetString();
  }
  else {
    // One file: use file name
    TObjArray* path = files[0].Tokenize("/.");
    dir = ((TObjString*)(path->At(path->GetEntries()-2)))->GetString();
    TObjArray* conf = dir.Tokenize("_");
    config = ((TObjString*)(conf->At(0)))->GetString();
    gen = ((TObjString*)(conf->At(1)))->GetString();
  }
  std::cout << "File: " << dir << std::endl;
  std::cout << "Configuration: " << config << std::endl;
  std::cout << "Generators: " << gen << std::endl;

  // Set up input ROOT trees
  TChain* gst = new TChain("gst");
  for (size_t i=0; i<files.size(); i++) {
    std::cout << "Add: " << files[i] << std::endl;
    gst->Add(files[i]);
  }
  std::cout << "Entries: " << gst->GetEntries() << std::endl;
  Event ev(gst);

  // Output file and histograms
  TString outpath = TString("./") + config + "_" + gen + ".root";
  TFile* fout = TFile::Open(outpath, "recreate");

  // Book histograms.
  // The mapping is {"selection": [selection_function, {"histname": hist}]}
  std::map<std::string, std::pair<Event::EventType, std::map<std::string, Hist*> > > hists;

  if (gen == "CCMEC") {
    hists = {
      {
        "ccmec", {
          Event::isCCMEC, {
            {"q0q3", new Hist_q0q3("h_ccmec_q0q3")},
            {"pke", new Hist_pKE("h_ccmec_pke")},
            {"leadpke", new Hist_leadpKE("h_ccmec_leadpke", "p_{lead}")}
          }
        }
      }
    };
  }
  else {
    hists = {
      {
        "all", {
          Event::isAny, {
            {"q0q3", new Hist_q0q3("h_all_q0q3")},
            {"nuanceCode", new Hist_nuanceCode("h_all_nuanceCode")},
            {"intMode", new Hist_intmode("h_all_intmode")},
            {"nip", new Hist_number("h_all_nip", "p", &ev.nip)},
            {"nin", new Hist_number("h_all_nin", "n", &ev.nin)},
            {"nipip", new Hist_number("h_all_nipip", "#pi^{+}", &ev.nipip)},
            {"nipim", new Hist_number("h_all_nipim", "#pi^{-}", &ev.nipim)},
            {"nipi0", new Hist_number("h_all_nipi0", "#pi^{0}", &ev.nipi0)},
            {"nikp", new Hist_number("h_all_nikp", "K^{+}", &ev.nikp)},
            {"nikm", new Hist_number("h_all_nikm", "K^{-}", &ev.nikm)},
            {"nik0", new Hist_number("h_all_nik0", "K^{0}", &ev.nik0)},
            {"nfp", new Hist_number("h_all_nfp", "p, post-FSI", &ev.nfp)},
            {"nfn", new Hist_number("h_all_nfn", "n, post-FSI", &ev.nfn)},
            {"nfpip", new Hist_number("h_all_nfpip", "#pi^{+}, post-FSI", &ev.nfpip)},
            {"nfpim", new Hist_number("h_all_nfpim", "#pi^{-}, post-FSI", &ev.nfpim)},
            {"nfpi0", new Hist_number("h_all_nfpi0", "#pi^{0}, post-FSI", &ev.nfpi0)},
            {"nfkp", new Hist_number("h_all_nfkp", "K^{+}, post-FSI", &ev.nfkp)},
            {"nfkm", new Hist_number("h_all_nfkm", "K^{-}, post-FSI", &ev.nfkm)},
            {"nfk0", new Hist_number("h_all_nfk0", "K^{0}, post-FSI", &ev.nfk0)}
          }
        },
      },
      {
        "ccqe", {
          Event::isCCQE, {
            {"q0q3", new Hist_q0q3("h_ccqe_q0q3")},
            {"tmu", new Hist_ke("h_ccqe_tmu", "#mu")},
            {"ctmu", new Hist_cosTheta("h_ccqe_ctmu", "#mu")},
            {"tctmu", new Hist_Tct("h_ccqe_tctmu", "#mu")},
          }
        },
      },
      {
        "1l1p", {
          Event::is1l1p0pi0, {
            {"q0q3", new Hist_q0q3("h_1l1p_q0q3")},
            {"nuanceCode", new Hist_nuanceCode("h_1l1p_nuanceCode")},
            {"intMode", new Hist_intmode("h_1l1p_intmode")}
          }
        },
      },
      {
        "1l1trk", {
          Event::is1l1trk0pi0, {
            {"q0q3", new Hist_q0q3("h_1l1trk_q0q3")},
            {"nuanceCode", new Hist_nuanceCode("h_1l1trk_nuanceCode")},
            {"intMode", new Hist_intmode("h_1l1trk_intmode")}
          }
        },
      }
    };
  }

  // Event Loop
  for (int i=0; i<ev.GetEntries(); i++) {
    ev.GetEntry(i);

    // Loop over "selections"
    for (auto const& h : hists) {
      if ((*h.second.first)(ev)) {
        // Loop over histograms for this selection
        for (auto const& hist : h.second.second) {
          hist.second->Fill(ev);
        }
      }
    }
  }

  // Output
  for (auto const& h : hists) {
    for (auto const& hist : h.second.second) {
      std::cout << "Writing " << h.first << " " << hist.first << std::endl;
      hist.second->Write(config, gen, fout);
    }
  }

  return 0;
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " \"files*.root\"" << std::endl;
    return 0;
  }

  std::vector<TString> files;
  for (int i=1; i<argc; i++) {
    files.push_back(argv[i]);
  }

  return ggst(files);
}

