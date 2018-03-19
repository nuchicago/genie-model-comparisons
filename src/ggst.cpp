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
#include "event.h"
#include "hist.h"

int ggst(std::vector<TString> files);


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

