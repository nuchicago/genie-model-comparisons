#!/bin/bash

###########################################################
# Submit GENIE jobs with various models
#
# Flat flux from 0 to 3.5 GeV. 
#
# A. Mastbaum <mastbaum@uchicago.edu>, 2017/12/01
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
ENERGY="0.01,3.5"
FLUX="1"
NEVT="${1}"
CONFIG="${2}"
GENERATORS="${3}"
SEED="$RANDOM"
echo "Seed: ${SEED}" >> ${LOG} 2>&1

# File I/O Settings
GXML="/pnfs/uboone/persistent/users/mastbaum/genie_xsec/v2_12_6/NULL/tar/${CONFIG}.tar.gz"
OUTDIR="/pnfs/uboone/scratch/users/mastbaum/genie/numu"

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

# Run GENIE
gevgen \
  -r ${PROCESS} -n ${NEVT} -p ${NUPDG} -t ${TGT} -e ${ENERGY} -f ${FLUX} --seed $SEED \
  --cross-sections ${GXMLPATH}/gxspl-small.xml \
  --event-generator-list ${GENERATORS} >>${LOG} 2>&1

ls >>${LOG} 2>&1
date >>${LOG} 2>&1

# Transfer output files to dCache
OUT="${CONFIG}_${GENERATORS}_${CLUSTER}"
mkdir -p ${OUT} >>${LOG} 2>&1
mv gntp.${PROCESS}.ghep.root ${OUT}
mv ${LOG} ${OUT}

ifdh mkdir ${OUTDIR}/${OUT}
ifdh cp -r ${OUT} ${OUTDIR}/${OUT}/

