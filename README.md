GENIE Model Studies
===================
A. Mastbaum <mastbaum@uchicago.edu>, 2018/02

This repository contains scripts for comparing different models implemented
in the [GENIE](http://genie-mc.org) neutrino event generator.

Production Scripts
------------------
There are *production scripts* in `jobs`, set up to generate event samples
on the Fermilab grid. If using these, be sure to adjust output directories
to suit your needs! Scripts are:

    gevgen_models_flat.sh

        Ar40, choose nue/numu
        Flat flux, 0-3.5 GeV

    gevgen_models_mono.sh

        Ar40, choose nue/numu
        Monoenergetic beam at 10 GeV

    gevgen_models_ubflux.sh

        Ar40, choose nue/numu
        BNB-like flux (provided in data; place it somewhere in pnfs)

    submit_gevgen_models

        Submit jobs for one of the event generation scripts, for all the GENIE
        model combinations.

The `gevgen` scripts take three arguments:

    ./gevgen_models_(type) [NVENTS] [CONFIG] [GENERATORS]

    NEVENTS - Number of events to generate
    CONFIG - A GENIE configuration (e.g. DefaultPlusMECWithNC)
    GENERATORS - Specifies the generator list (e.g. Default+CCMEC+NCMEC)

Note that in order to enable a particular configuration, the cross section
tables and physics list are read from a specified file location. In the
current versions of the scripts, it reads and unpacks a tarball with
"minified" data (Ar40 num/nue only) and settings for each configuration,
from persistent pnfs (dCache).

Analysis Scripts
----------------
An *analysis script* named `ggst` reads in GENIE GST (summary tree) ntuples and
plots of kinematic parameters, depending on the enabled generators (e.g. CCQE,
CCMEC, all events). To build the `ggst` program, run `make`.

The basic workflow is to generate event samples incusively (e.g.
Default+CCMEC+NCMEC) and with single generators enabled (e.g. CCMEC only)
and feed those into `ggst`, which will compute for example an overall q0/q3
distribution for all events, and the joint proton kinetic energy distribution
for CCMEC events with two-proton final states. There are a couple of additional
scripts which take the per-generator output and condense it into summary
plots.

There are two input modes, for reading a set of files as they come out of the
production scripts (i.e. 1000 `.gst.root` files) or one file where they are
`hadd`-ed together. In either case, the configuration and generator names
are divined from the file path/name, which makes this somewhat fragile.
See the file loading section of `ggst.cpp` for complete details.

