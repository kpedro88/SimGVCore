#!/bin/bash

SIMS=()

# check arguments
while getopts "s:" opt; do
	case "$opt" in
		s) IFS="," read -a SIMS <<< "$OPTARG"
		;;
	esac
done

BASEDIR=`pwd`
TESTDIR=CMSSW_10_2_0/src/SimGVCore/Application/test
GVDIR=${BASEDIR}/GV
G4DIR=${BASEDIR}/G4

OS=slc6
UNAME=$(uname -a)
if [[ "$UNAME" == *el7* ]]; then
	OS=slc7
fi

for SIM in ${SIMS[@]}; do
	if [ "$SIM" = GV ]; then
		cd $BASEDIR
		mkdir -p $GVDIR
		cd $GVDIR
		export SCRAM_ARCH=${OS}_amd64_gcc630
		scram project CMSSW_10_2_0
		cd CMSSW_10_2_0
		eval `scramv1 runtime -sh`
		mkdir work
		cd work
		git clone git@github.com:kpedro88/install-geant.git -b CMSSW_10_2_X
		ln -s install-geant/* .
		./setup.sh -j 8 -I all
		cd $CMSSW_BASE/src
		# important to cmsenv again after installation because of scram setup for new tools
		eval `scramv1 runtime -sh`
		git cms-init
		git cms-merge-topic kpedro88:GVGeometry102X
		git cms-merge-topic -u kpedro88:GVPassiveHit
		git clone git@github.com:kpedro88/SimGVCore.git -b SensDetTemplateWrapper
		scram b -j 8
		# link to G4
		ln -s ${G4DIR}/${TESTDIR} ${GVDIR}/${TESTDIR}/G4
	elif [ "$SIM" = G4 ]; then
		cd $BASEDIR
		mkdir -p $G4DIR
		cd $G4DIR
		export SCRAM_ARCH=${OS}_amd64_gcc630
		scram project CMSSW_10_2_0
		cd CMSSW_10_2_0/src
		eval `scramv1 runtime -sh`
		git cms-merge-topic kpedro88:GVPhysicsList
		git cms-merge-topic -u kpedro88:GVPassiveHit
		git cms-merge-topic -u kpedro88:GVMagField
		git cms-addpkg SimG4Core SimG4CMS
		git clone git@github.com:kpedro88/SimGVCore.git -b SensDetTemplateWrapper
		cd SimGVCore
		git config core.sparseCheckout true
cat << EOF_SPARSE > .git/info/sparse-checkout
/Calo
/CaloG4
/Application/test
/Application/python
README.md
setup.sh
EOF_SPARSE
		git read-tree -mu HEAD
		cd ..
		scram b disable-biglib
		scram b -j 8
		# link to GV
		ln -s ${GVDIR}/${TESTDIR} ${G4DIR}/${TESTDIR}/GV
	else
		echo "Unknown sim value: $SIM"
	fi
done

