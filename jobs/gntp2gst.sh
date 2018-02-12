#!/bin/bash

############################################################
# Convert GENIE gntp files to GENIE summary table (GST)
#
# A. Mastbaum <mastbaum@uchicago.edu>, 2018/01
############################################################

BASE="/pnfs/uboone/scratch/users/mastbaum/genie/numu"

for DIR in `ls $BASE`; do
  for f in `ls $BASE/$DIR/gntp*.root`; do
    echo $f
    gntpc -i $f -f gst -o $BASE/$DIR/$(basename $f .root).gst.root
  done
done

