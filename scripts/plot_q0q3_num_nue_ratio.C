/**
 * Plot the double ratio of num/nue q0/q3 for two models.
 *
 * A. Mastbaum <mastbaum@uchicago.edu>, 2018/03
 */

TH1D* get_hist(TString fname, TString name) {
  TFile f1(fname);
  TCanvas* c1 = (TCanvas*) f1.Get("c_h_1l1p_q0q3");
  TH1D* h1 = (TH1D*) c1->FindObject("h_1l1p_q0q3");
  TH1D* h1a = (TH1D*) h1->Clone(name);
  h1a->SetDirectory(0);
  f1.Close();

  return h1a;
}


void plot_q0q3_num_nue_ratio() {
  gStyle->SetOptStat(0);

  TH1D* h1a = \
    get_hist("plots/num/DefaultPlusMECWithNC_Default+CCMEC+NCMEC.root",
             "h_1l1p_num");
  TH1D* h2a = \
    get_hist("plots/nue/DefaultPlusMECWithNC_Default+CCMEC+NCMEC.root",
             "h_1l1p_nue");

  TCanvas* c1 = new TCanvas();
  TH1D* hr = (TH1D*) h1a->Clone("hr");
  hr->Divide(h2a);
  hr->Draw("colz");
  hr->GetZaxis()->SetRangeUser(0.5, 2.0);

  TCanvas* c2 = new TCanvas();
  h1a->SetMarkerColorAlpha(kBlue, 0.25);
  h2a->SetMarkerColorAlpha(kRed,0.05);
  h1a->DrawNormalized();
  h2a->DrawNormalized("same");

  //c1->SaveAs("ccqe_temu_ratio.pdf");
  //c1->SaveAs("ccqe_temu_ratio.C");

  // Double ratio
  TH1D* h1b = \
    get_hist("plots/num/ValenciaQEBergerSehgalCOHRES_Default+CCMEC.root",
             "h_1l1p_num_2");
  TH1D* h2b = \
    get_hist("plots/nue/ValenciaQEBergerSehgalCOHRES_Default+CCMEC.root",
             "h_1l1p_nue_2");

  TCanvas* c3 = new TCanvas();
  TH1D* hrb = (TH1D*) h1b->Clone("hrb");
  hrb->Divide(h2b);
  hrb->Draw("colz");
  hrb->GetZaxis()->SetRangeUser(0.5, 2.0);

  TCanvas* c4 = new TCanvas();
  TH1D* hdr = (TH1D*) hrb->Clone("hdr");
  hdr->Divide(hr);
  hdr->Draw("colz");
  hdr->GetZaxis()->SetRangeUser(0.0, 2.0);
}

