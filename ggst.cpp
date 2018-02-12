#include <iostream>
#include <TF1.h>
#include <TPaveText.h>
#include <TString.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TObjArray.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TObjString.h>
#include <TChain.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TError.h>

/**
 * Plot distributions from GENIE MC with different models.
 *
 * Reads a set of GST-format ntuples from a directory, and plots
 * distributions of interesting quantities for model comparison.
 *
 * A. Mastbaum <mastbaum@uchicago.edu>, 2018/01/12
 */

TPaveText* genie_label(TString config, TString gen) {
    TPaveText* l = new TPaveText(0.17, 0.6, 0.62, 0.88, "ndc");
    l->SetTextFont(132);
    l->SetTextAlign(12);
    l->SetBorderSize(0);
    l->SetFillColor(0);
    l->SetFillStyle(0);
    l->AddText("GENIE 2.12.6, BNB #nu_{#mu}");
    l->AddText(config);
    l->AddText(TString("Generators: ") + gen);
    return l;
}


int ggst(TString files, bool fsi=false) {
  gROOT->SetBatch(true);
  gROOT->ProcessLine(".x ~/.rootlogon.C");
  gStyle->SetOptStat(0);
  gErrorIgnoreLevel = kError;

  // Extract configuration string from file path (FRAGILE)
  TObjArray* path = files.Tokenize("/");
  TString dir = ((TObjString*)(path->At(path->GetEntries()-2)))->GetString();
  std::cout << "Directory: " << dir << std::endl;
  TObjArray* conf = dir.Tokenize("_");
  TString config = ((TObjString*)(conf->At(0)))->GetString();
  TString gen = ((TObjString*)(conf->At(1)))->GetString();
  std::cout << "Configuration: " << config << std::endl;
  std::cout << "Generators: " << gen << std::endl;

  // Set up input ROOT trees
  TChain gst("gst");
  gst.Add(files);
  std::cout << "Entries: " << gst.GetEntries() << std::endl;

  const int kNPmax = 250;  // Matches GENIE gntpc

  int neu, tgt, nuance_code;
  bool qel, res, dis, coh, dfr, imd, nuel, cc, nc;
  double pxv, pyv, pzv, pxl, pyl, pzl, x, y, t, q2, w, enu, elep;
  gst.SetBranchAddress("neu", &neu);
  gst.SetBranchAddress("tgt", &tgt);
  gst.SetBranchAddress("nuance_code", &nuance_code);
  gst.SetBranchAddress("qel", &qel);
  gst.SetBranchAddress("res", &res);
  gst.SetBranchAddress("dis", &dis);
  gst.SetBranchAddress("coh", &coh);
  gst.SetBranchAddress("dfr", &dfr);
  gst.SetBranchAddress("imd", &imd);
  gst.SetBranchAddress("nuel", &nuel);
  gst.SetBranchAddress("cc", &cc);
  gst.SetBranchAddress("nc", &nc);
  gst.SetBranchAddress("pxv", &pxv);
  gst.SetBranchAddress("pyv", &pyv);
  gst.SetBranchAddress("pzv", &pzv);
  gst.SetBranchAddress("pxl", &pxl);
  gst.SetBranchAddress("pyl", &pyl);
  gst.SetBranchAddress("pzl", &pzl);
  gst.SetBranchAddress("x", &x);
  gst.SetBranchAddress("y", &y);
  gst.SetBranchAddress("t", &t);
  gst.SetBranchAddress("Q2", &q2);
  gst.SetBranchAddress("W", &w);
  gst.SetBranchAddress("Ev", &enu);
  gst.SetBranchAddress("El", &elep);

  int nip, nin, nipip, nipim, nipi0, nikp, nikm, nik0, niem;
  if (!fsi) {
    gst.SetBranchAddress("nip", &nip);
    gst.SetBranchAddress("nin", &nin);
    gst.SetBranchAddress("nipip", &nipip);
    gst.SetBranchAddress("nipim", &nipim);
    gst.SetBranchAddress("nipi0", &nipi0);
    gst.SetBranchAddress("nikp", &nikp);
    gst.SetBranchAddress("nikm", &nikm);
    gst.SetBranchAddress("nik0", &nik0);
    gst.SetBranchAddress("niem", &niem);
  }
  else {
    gst.SetBranchAddress("nfp", &nip);
    gst.SetBranchAddress("nfn", &nin);
    gst.SetBranchAddress("nfpip", &nipip);
    gst.SetBranchAddress("nfpim", &nipim);
    gst.SetBranchAddress("nfpi0", &nipi0);
    gst.SetBranchAddress("nfkp", &nikp);
    gst.SetBranchAddress("nfkm", &nikm);
    gst.SetBranchAddress("nfk0", &nik0);
    gst.SetBranchAddress("nfem", &niem);
  }

  int ni;
  int pdgi[kNPmax];
  double ei[kNPmax];
  double pxi[kNPmax];
  double pyi[kNPmax];
  double pzi[kNPmax];
  if (!fsi) {
    gst.SetBranchAddress("ni", &ni);
    gst.SetBranchAddress("pdgi", &pdgi);
    gst.SetBranchAddress("Ei", &ei);
    gst.SetBranchAddress("pxi", &pxi);
    gst.SetBranchAddress("pyi", &pyi);
    gst.SetBranchAddress("pzi", &pzi);
  }
  else {
    gst.SetBranchAddress("nf", &ni);
    gst.SetBranchAddress("pdgf", &pdgi);
    gst.SetBranchAddress("Ef", &ei);
    gst.SetBranchAddress("pxf", &pxi);
    gst.SetBranchAddress("pyf", &pyi);
    gst.SetBranchAddress("pzf", &pzi);
  }

  // Output file and histograms
  TString outpath = TString("plots/") + config + "_" + gen + ".root";
  TFile* fout = TFile::Open(outpath, "recreate");


  // Event-level
  // q0/q3
  TH2D* h_q0q3 = new TH2D(
    "h_q0q3",
    ";Three-momentum transfer q^{3} (GeV);Energy transfer q^{0} (GeV)",
    100, 0, 1.250, 100, 0, 1.250);

  // NUANCE interaction code
  gst.Draw("nuance_code>>h_nuance_code(100,0,100)");
  TH1D* h_nuance_code = (TH1D*) gDirectory->Get("h_nuance_code");
  h_nuance_code->SetTitle(";NUANCE interaction code;Entries");

  // Number of things
  if (!fsi) {
    gst.Draw("nip>>h_nip(20,0,20)", "", "goff");
    gst.Draw("nin>>h_nin(20,0,20)", "", "goff");
    gst.Draw("nipip>>h_nipip(20,0,20)", "", "goff");
    gst.Draw("nipim>>h_nipim(20,0,20)", "", "goff");
    gst.Draw("nipi0>>h_nipi0(20,0,20)", "", "goff");
    gst.Draw("nikp>>h_nikp(20,0,20)", "", "goff");
  }
  else {
    gst.Draw("nfp>>h_nip(20,0,20)", "", "goff");
    gst.Draw("nfn>>h_nin(20,0,20)", "", "goff");
    gst.Draw("nfpip>>h_nipip(20,0,20)", "", "goff");
    gst.Draw("nfpim>>h_nipim(20,0,20)", "", "goff");
    gst.Draw("nfpi0>>h_nipi0(20,0,20)", "", "goff");
    gst.Draw("nfkp>>h_nikp(20,0,20)", "", "goff");
  }

  TH1D* h_nip = (TH1D*) gDirectory->Get("h_nip");
  h_nip->SetTitle(";Number of protons;Events");

  TH1D* h_nin = (TH1D*) gDirectory->Get("h_nin");
  h_nin->SetTitle(";Number of neutrons;Events");

  TH1D* h_nipip = (TH1D*) gDirectory->Get("h_nipip");
  h_nipip->SetTitle(";Number of #pi^{+};Events");

  TH1D* h_nipim = (TH1D*) gDirectory->Get("h_nipim");
  h_nipim->SetTitle(";Number of #pi^{-};Events");

  TH1D* h_nipi0 = (TH1D*) gDirectory->Get("h_nipi0");
  h_nipi0->SetTitle(";Number of #pi^{0};Events");

  TH1D* h_nikp = (TH1D*) gDirectory->Get("h_nikp");
  h_nikp->SetTitle(";Number of K^{+};Events");


  // Interaction mode
  TH1I* h_mode = new TH1I(
    "h_mode",
    ";Interaction mode;Entries",
    11, 0, 11);

  const char* modes[11] = {
    "CCQE", "CCMEC", "CCRes", "CCDIS", "CCCoh",
    "NCEL", "NCMEC", "NCRes", "NCDIS", "NCCoh",
    "Other"
  };

  for (int i=1; i<=11; i++) {
    h_mode->GetXaxis()->SetBinLabel(i, modes[i-1]);
  }

  // CCQE
  TH1D* h_ccqe_tmu = new TH1D(
    "h_ccqe_tmu",
    ";#mu Kinetic energy T_{#mu} (GeV)",
    200, 0, 3.500);

  TH1D* h_ccqe_ctmu = new TH1D(
    "h_ccqe_ctmu",
    ";cos#theta_{#mu}",
    100, -1, 1);

  TH2D* h_ccqe_tmu_ctmu = new TH2D(
    "h_ccqe_tmu_ctmu",
    ";#mu Kinetic energy p_{#mu} (GeV);cos#theta_{#mu}",
    100, 0, 3.5, 100, -1, 1);

  TH2D* h_ccqe_q0q3 = new TH2D(
    "h_ccqe_q0q3",
    ";Three-momentum transfer q^{3} (GeV);Energy transfer q^{0} (GeV)",
    100, 0, 1.250, 100, 0, 1.250);

  // MEC
  TH2D* h_mec_pke = new TH2D(
    "h_mec_pke",
    ";Leading p kinetic energy T_{p1} (GeV);Subleading p kinetic energy T_{p2} (GeV)",
    100, 0, 1.000, 100, 0, 1.000);

  TH2D* h_mec_q0q3 = new TH2D(
    "h_mec_q0q3",
    ";Three-momentum transfer q^{3} (GeV);Energy transfer q^{0} (GeV)",
    100, 0, 1.250, 100, 0, 1.250);


  // Event Loop
  for (int i=0; i<gst.GetEntries(); i++) {
    gst.GetEntry(i);
    //if (i%1000 ==0){std::cout << i << std::endl;}

    // Kinematics
    double q0 = (enu - elep);
    double q3 = (TVector3(pxv, pyv, pzv) - TVector3(pxl, pyl, pzl)).Mag();
    TLorentzVector pnu(pxv, pyv, pzv, enu);
    TLorentzVector plep(pxl, pyl, pzl, elep);

    float lmass = -9999;
    if (cc && abs(neu) == 12) {
      lmass = 0.510999e-3;
    }
    else if (cc && abs(neu) == 14) {
      lmass = 105.658e-3;
    }
    else if (cc && abs(neu) == 16) {
      lmass = 1776.82e-3;
    }
    else if (nc) {
      lmass = 0;
    }

    // General plots
    h_q0q3->Fill(q3, q0);

    if      (cc && qel) h_mode->Fill(0);
    else if (cc && res) h_mode->Fill(2);
    else if (cc && dis) h_mode->Fill(3);
    else if (cc && coh) h_mode->Fill(4);
    else if (cc && nuance_code == 0) h_mode->Fill(1);  // Hmm...
    else if (nc && qel) h_mode->Fill(5);
    else if (nc && res) h_mode->Fill(7);
    else if (nc && dis) h_mode->Fill(8);
    else if (nc && coh) h_mode->Fill(9);
    else if (nc && nuance_code == 0) h_mode->Fill(6);
    else h_mode->Fill(10);

    // CCQE
    if (cc && qel) {
      h_ccqe_q0q3->Fill(q3, q0);

      float tmu = elep - lmass;
      float ctmu = plep.Vect().CosTheta();
      h_ccqe_tmu->Fill(tmu);
      h_ccqe_ctmu->Fill(ctmu);
      h_ccqe_tmu_ctmu->Fill(tmu, ctmu);
    }

    // Probably CC MEC
    if (cc && nuance_code == 0) {
      h_mec_q0q3->Fill(q3, q0);

      // Proton kinetic energies: build a stack
      std::vector<double> pke;
      for (int ii=0; ii<ni; ii++) {
        if (pdgi[ii] == 2212) {
          pke.push_back(ei[ii] - 0.938272);
        }
      }

      if (pke.size() > 2) {
        std::cout << "MEC party: " << pke.size() << " intermediate protons (" << i << ")" << std::endl;
      }

      if (pke.size() > 1) {
        double p1 = TMath::Max(pke[0], pke[1]);
        double p2 = TMath::Min(pke[0], pke[1]);
        h_mec_pke->Fill(p1, p2);
      }

    }

  }


  // Output

  TCanvas* c_mode = new TCanvas("c_mode");
  h_mode->Draw();
  fout->cd();
  h_mode->Write();
  c_mode->SaveAs(TString("plots/") + config + "_" + gen + "_c_mode.pdf"); 
  c_mode->SaveAs(TString("plots/") + config + "_" + gen + "_c_mode.C"); 

  // Generic
  std::vector<TH1*> hs_g = { h_ccqe_tmu, h_ccqe_ctmu, h_ccqe_tmu_ctmu };
  for (size_t i=0; i<hs_g.size(); i++) {
    TH1* h = hs_g[i];

    char cname[150];
    snprintf(cname, 150, "c_%s", h->GetName());
    TCanvas* c = new TCanvas();
    c->SetName(cname);
    c->SetLeftMargin(0.15);

    h->Draw("colz");

    // Labels
    //TPaveText* l = genie_label(config, gen);
    //l->Draw();

    c->RedrawAxis();
    c->Update();
    fout->cd();
    c->Write();
    c->SaveAs(TString("plots/") + config + "_" + gen + "_" + cname + ".pdf"); 
    c->SaveAs(TString("plots/") + config + "_" + gen + "_" + cname + ".C"); 
  }

  // Log y
  std::vector<TH1*> hs_gl = { h_nip, h_nin, h_nipip, h_nipim, h_nipi0, h_nikp };
  for (size_t i=0; i<hs_gl.size(); i++) {
    TH1* h = hs_gl[i];

    char cname[150];
    snprintf(cname, 150, "c_%s", h->GetName());
    TCanvas* c = new TCanvas();
    c->SetName(cname);
    c->SetLeftMargin(0.15);
    c->SetLogy();

    h->Draw("colz");
    h->Write();

    // Labels
    //TPaveText* l = genie_label(config, gen);
    //l->Draw();

    c->RedrawAxis();
    c->Update();
    fout->cd();
    c->Write();
    c->SaveAs(TString("plots/") + config + "_" + gen + "_" + cname + ".pdf"); 
    c->SaveAs(TString("plots/") + config + "_" + gen + "_" + cname + ".C"); 
  }

  // q0/q3 Plots
  std::vector<TH2D*> hs_q0q3 = { h_q0q3, h_ccqe_q0q3, h_mec_q0q3 };
  for (size_t i=0; i<hs_q0q3.size(); i++) {
    TH2D* h = hs_q0q3[i];

    char cname[150];
    snprintf(cname, 150, "c_%s", h->GetName());
    TCanvas* c = new TCanvas();
    c->SetName(cname);
    c->SetLeftMargin(0.15);

    h->Draw("colz");
    h->GetXaxis()->SetRangeUser(0, 1.2);
    h->GetYaxis()->SetRangeUser(0, 1.2);

    // Labels
    TPaveText* l = genie_label(config, gen);
    l->AddText("Lines W = 938, 1232, 1535 MeV");
    l->AddText("Lines Q^{2} = 0.2 - 1.0 GeV");
    l->Draw();

    // Q2 and W lines
    float lw = 2;
    std::vector<TF1*> qfs, wfs;
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

    c->RedrawAxis();
    c->Update();
    fout->cd();
    c->Write();
    c->SaveAs(TString("plots/") + config + "_" + gen + "_" + cname + ".pdf"); 
    c->SaveAs(TString("plots/") + config + "_" + gen + "_" + cname + ".C"); 
  }

  // Proton kinematics
  std::vector<TH2D*> hs_pke = { h_mec_pke };
  for (size_t i=0; i<hs_pke.size(); i++) {
    TH2D* h = hs_pke[i];

    char cname[150];
    snprintf(cname, 150, "c_%s", h->GetName());
    TCanvas* c = new TCanvas();
    c->SetName(cname);
    c->SetLeftMargin(0.15);

    h->Draw("colz");
    h->GetXaxis()->SetRangeUser(0, 0.8);
    h->GetYaxis()->SetRangeUser(0, 0.8);

    // Labels
    TPaveText* l = genie_label(config, gen);
    l->Draw();

    c->RedrawAxis();
    c->Update();
    fout->cd();
    c->Write();
    c->SaveAs(TString("plots/") + config + "_" + gen + "_" + cname + ".pdf"); 
    c->SaveAs(TString("plots/") + config + "_" + gen + "_" + cname + ".C"); 
    c->SaveAs(TString("plots/") + config + "_" + gen + "_" + cname + ".pdf"); 
    c->SaveAs(TString("plots/") + config + "_" + gen + "_" + cname + ".C"); 
  }

  return 0;
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " \"files*.root\"" << std::endl;
    return 0;
  }

  bool fsi = false;
  if (argc > 2) {
    fsi = std::string(argv[2]) == "fsi";
  }
  std::cout << "FSI: " << (fsi ? "on" : "off") << std::endl;

  return ggst(argv[1], fsi);
}

