# Generate summary plots from GENIE model analysis output
#
# A. Mastbaum <mastbaum@uchicago.edu> 2018/03

import os
import sys
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

# Directory with input ROOT files (from ggst)
pdir = sys.argv[1]

# Input ROOT file names
files = [
    'DefaultPlusMECWithNC_Default+CCMEC+NCMEC.root',
    'DefaultPlusMECWithNCAndhN2015_Default+CCMEC+NCMEC.root',
    'DefaultPlusMECWithNCAndhA2015_Default+CCMEC+NCMEC.root',
    #'ValenciaQEBergerSehgalCOHRES_Default+CCMEC+1K.root',
    'ValenciaQEBergerSehgalCOHRES_Default+CCMEC.root',
    'DefaultPlusValenciaMEC_Default+CCMEC.root',
    'LocalFGNievesQEAndMEC_Default+MEC.root',
    'EffSFTEM_Default.root',
    'AltPion_Default.root',
]

# Labels for the interaction channels (matching the mapping in ggst.cpp)
labels = [
    "CCQE", "CCMEC", "CCRes", "CCDIS", "CCCoh",
    "NCEL", "NCMEC", "NCRes", "NCDIS", "NCCoh",
    "Other"
]

hm = ROOT.TH2F('hm', '', len(files), 0, len(files), 11, 0, 11)
hsm = ROOT.TH2F('hsm', '', len(files), 0, len(files), 11, 0, 11)

hinp = ROOT.TH2F('hinp', '', len(files), 0, len(files), 20, 0, 20)
hinn = ROOT.TH2F('hinn', '', len(files), 0, len(files), 20, 0, 20)
hinpip = ROOT.TH2F('hinpip', '', len(files), 0, len(files), 8, 0, 8)
hinpim = ROOT.TH2F('hinpim', '', len(files), 0, len(files), 8, 0, 8)
hinpi0 = ROOT.TH2F('hinpi0', '', len(files), 0, len(files), 8, 0, 8)
hinkp = ROOT.TH2F('hinkp', '', len(files), 0, len(files), 8, 0, 8)

hfnp = ROOT.TH2F('hfnp', '', len(files), 0, len(files), 20, 0, 20)
hfnn = ROOT.TH2F('hfnn', '', len(files), 0, len(files), 20, 0, 20)
hfnpip = ROOT.TH2F('hfnpip', '', len(files), 0, len(files), 8, 0, 8)
hfnpim = ROOT.TH2F('hfnpim', '', len(files), 0, len(files), 8, 0, 8)
hfnpi0 = ROOT.TH2F('hfnpi0', '', len(files), 0, len(files), 8, 0, 8)
hfnkp = ROOT.TH2F('hfnkp', '', len(files), 0, len(files), 8, 0, 8)

# Histogram titles
ts = [
    '', '',
    'N_{p}^{i}', 'N_{n}^{i}', 'N_{#pi^{+}}^{i}', 'N_{#pi^{-}}^{i}', 'N_{#pi^{0}}^{i}', 'N_{K^{+}}^{i}',
    'N_{p}^{f}', 'N_{n}^{f}', 'N_{#pi^{+}}^{f}', 'N_{#pi^{-}}^{f}', 'N_{#pi^{0}}^{f}', 'N_{K^{+}}^{f}',
]

# Histogram object names
obs = [
    'h_all_intmode', 'h_1l1p_intmode',
    'h_all_nip', 'h_all_nin', 'h_all_nipip', 'h_all_nipim', 'h_all_nipi0', 'h_all_nikp',
    'h_all_nfp', 'h_all_nfn', 'h_all_nfpip', 'h_all_nfpim', 'h_all_nfpi0', 'h_all_nfkp',
]

# Histogram objects
hs = [
    hm, hsm,
    hinp, hinn, hinpip, hinpim, hinpi0, hinkp,
    hfnp, hfnn, hfnpip, hfnpim, hfnpi0, hfnkp,
]

for i in range(hm.GetNbinsY()):
    hm.GetYaxis().SetBinLabel(i+1, labels[i])
    hsm.GetYaxis().SetBinLabel(i+1, labels[i])

for j, ff in enumerate(files):
    f = ROOT.TFile(os.path.join(pdir, ff))
    if not f.IsOpen(): continue

    model = ff.split('_')[0]
    for hh in hs:
        hh.GetXaxis().SetBinLabel(j+1, model)

    h = f.Get('h_all_intmode')
    n = h.Integral()
    print(model, n)

    for hh, oo, tt in zip(*(hs, obs, ts)):
        hhn = f.Get(oo)
        n = hhn.Integral()
        if 'oo' != 'h_all_intmode': hh.GetYaxis().SetTitle(tt)
        for i in range(hhn.GetNbinsX()):
            hh.Fill(j, i, 1.0 * hhn.GetBinContent(i+1) / n)

stuff = []
for hh in hs:
    print(hh)
    c = ROOT.TCanvas()
    hh.Draw('colz text')
    c.SetBottomMargin(0.22)
    c.SetRightMargin(0.15)
    c.Update()
    c.SaveAs(pdir + '/models_%s.pdf' % hh.GetName())
    c.SaveAs(pdir + '/models_%s.C' % hh.GetName())
    stuff.append(c)

#input()

