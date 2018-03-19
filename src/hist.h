/**
 * Histogram containers: The following classes define different types of
 * histograms, which are reused for e.g. inclusive and exclusive selections.
 */

#include <iostream>
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TPaveText.h>
#include <TString.h>
#include <TStyle.h>
#include <TVector3.h>

/** Create a TPaveText label with information about the GENIE config. */
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

