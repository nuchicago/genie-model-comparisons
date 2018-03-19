#!/bin/bash

###########################################################
# Submit GENIE jobs with various models
# 
# Monoenergetic neutrinos (10 GeV).
#
# A. Mastbaum <mastbaum@uchicago.edu>, 2018/02/01
###########################################################

# Log file
LOG="${CLUSTER}_${PROCESS}.log"
echo "Running ${0} on ${HOSTNAME}" >>${LOG} 2>&1
echo "Cluster: ${CLUSTER}" >>${LOG} 2>&1
echo "Process: ${PROCESS}" >>${LOG} 2>&1
date >>${LOG} 2>&1

# GENIE gEvGen settings
TGT="1000180400"
ENERGY="10.0"
NEVT="${1}"
CONFIG="${2}"
GENERATORS="${3}"
SEED="$RANDOM"
echo "Seed: ${SEED}" >> ${LOG} 2>&1

# File I/O Settings
GXML="/pnfs/uboone/persistent/users/mastbaum/genie_xsec/v2_12_10/NULL/tar/${CONFIG}.tar.gz"

# nue/numu
NUPDG="14"
OUTDIR="/pnfs/uboone/scratch/users/mastbaum/genie/mono_ng_num"
#FLUXFILE="/pnfs/uboone/persistent/users/mastbaum/genie/flux_num.root"
#FLUXOBJ="numu_CV_AV_TPC"

#NUPDG="12"
#FLUXFILE="/pnfs/uboone/persistent/users/mastbaum/genie/flux_nue.root"
#OUTDIR="/pnfs/uboone/scratch/users/mastbaum/genie/ubflux_ng_nue"
#FLUXOBJ="nue_CV_AV_TPC"

# Set up GENIE and ifdh environment
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh
setup ifdhc

setup root v6_12_04e -q e15:prof
setup lhapdf v5_9_1j -q e15:prof
setup libxml2 v2_9_5 -q prof
setup pythia v6_4_28j -q gcc640:prof
setup log4cpp v1_1_3a -q e15:prof
setup cmake v3_10_1

ifdh cp /pnfs/uboone/persistent/users/mastbaum/tars/genie_R-2_12_10.tar.gz genie_R-2_12_10.tar.gz
tar zxf genie_R-2_12_10.tar.gz
export GENIE=$PWD/R-2_12_10
export PATH=$GENIE/local/bin:$PATH
export LD_LIBRARY_PATH=$GENIE/local/lib:$LD_LIBRARY_PATH
export LIBXML2_LIB=/cvmfs/fermilab.opensciencegrid.org/products/larsoft/libxml2/v2_9_5/Linux64bit+2.6-2.12-prof/lib/
export PYTHIA6=$PYTHIA6_LIBRARY

# Transfer cross section splines and settings
ifdh cp -D ${GXML} . >>${LOG} 2>&1
tar xzvf ${CONFIG}.tar.gz >>${LOG} 2>&1
export GXMLPATH="${CONFIG}/data"

ifdh cp ${FLUXFILE} ./flux.root >>${LOG} 2>&1

# Run GENIE
gevgen \
  -r ${PROCESS} -n ${NEVT} -p ${NUPDG} -t ${TGT} --seed ${SEED} \
  -e ${ENERGY} \
  --cross-sections ${GXMLPATH}/gxspl-small.xml \
  --event-generator-list ${GENERATORS} >>${LOG} 2>&1

FFILE="gntp.${PROCESS}.ghep.root"
gntpc -i ${FFILE} -f gst -o $(basename ${FFILE} .root).gst.root

ls >>${LOG} 2>&1
date >>${LOG} 2>&1

# Transfer output files to dCache
OUT="${CONFIG}_${GENERATORS}_${CLUSTER}"
mkdir -p ${OUT} >>${LOG} 2>&1
mv gntp.${PROCESS}.ghep.root ${OUT}
mv gntp.${PROCESS}.ghep.gst.root ${OUT}
mv ${LOG} ${OUT}

ifdh mkdir ${OUTDIR}/${OUT}
ifdh cp -r ${OUT} ${OUTDIR}/${OUT}/

