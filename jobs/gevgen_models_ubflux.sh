#!/bin/bash

###########################################################
# Submit GENIE jobs with various models
# 
# Approximately BNB flux at MicroBooNE.
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
NUPDG="14"
ENERGY="0.00,6.75"
NEVT="${1}"
CONFIG="${2}"
GENERATORS="${3}"
SEED="$RANDOM"
echo "Seed: ${SEED}" >> ${LOG} 2>&1

# File I/O Settings
GXML="/pnfs/uboone/persistent/users/mastbaum/genie_xsec/v2_12_6/NULL/tar/${CONFIG}.tar.gz"
OUTDIR="/pnfs/uboone/scratch/users/mastbaum/genie/ubflux_num"
FLUXFILE="/pnfs/uboone/persistent/users/mastbaum/genie/flux_num.root"

# Set up GENIE and ifdh environment
source /cvmfs/uboone.opensciencegrid.org/products/setup_uboone.sh
setup ifdhc

NUISCVMFS=/cvmfs/minerva.opensciencegrid.org/minerva/NUISANCE_080117/
source $NUISCVMFS/external/setupexternal.sh
source $NUISCVMFS/nuisance/v2r6/builds/genie2126-nuwrov11qrw/Linux/setupcvmfs.sh
source $GENIE/setup.sh

# Transfer cross section splines and settings
ifdh cp -D ${GXML} . >>${LOG} 2>&1
tar xzvf ${CONFIG}.tar.gz >>${LOG} 2>&1
export GXMLPATH="${CONFIG}/data"

ifdh cp ${FLUXFILE} ./flux.root >>${LOG} 2>&1

# Run GENIE
gevgen \
  -r ${PROCESS} -n ${NEVT} -p ${NUPDG} -t ${TGT} --seed $SEED \
  -e ${ENERGY} -f ./flux.root,numu_CV_AV_TPC \
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

