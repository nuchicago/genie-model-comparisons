/**
 * Plot the ratio of Tmu/costhetamu for CCQE events for two models.
 *
 * A. Mastbaum <mastbaum@uchicago.edu>, 2018/03
 */

TH1D* get_hist(TString fname) {
  TFile f1(fname);
  TCanvas* c1 = (TCanvas*) f1.Get("c_h_ccqe_tctmu");
  TH1D* h1 = (TH1D*) c1->FindObject("h_ccqe_tctmu");
  TH1D* h1a = (TH1D*) h1->Clone("h_ccqe_default");
  h1a->SetDirectory(0);
  f1.Close();

  return h1a;
}


void plot_ccqe_temu_ratio() {
  gStyle->SetOptStat(0);

  TH1D* h1a = get_hist("plots_num_2/DefaultPlusMECWithNC_Default+CCMEC+NCMEC.root");
  TH1D* h2a = get_hist("plots_num_2/ValenciaQEBergerSehgalCOHRES_Default+CCMEC.root");

  TCanvas* c1 = new TCanvas();
  TH1D* hr = (TH1D*) h2a->Clone("hr");
  hr->Divide(h1a);
  hr->Draw("colz");
  hr->GetZaxis()->SetRangeUser(0.5, 2.0);

  c1->SaveAs("ccqe_temu_ratio.pdf");
  c1->SaveAs("ccqe_temu_ratio.C");
}

