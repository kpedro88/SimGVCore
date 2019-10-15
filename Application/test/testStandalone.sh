#!/bin/bash

SIM=$1
TESTDIR=$CMSSW_BASE/work/local/geantv/install/bin/examples/FullCMS
NCPU=$(cat /proc/cpuinfo | grep processor | wc -l)
# check for hyperthreading
if grep -q " ht " /proc/cpuinfo; then
	NCPU=$((NCPU/2))
fi
th=1

if [ -z "$SIM" ]; then
	echo "Must specify sim"
	exit 1
fi

# use other CPUs
declare -A PIDS
for ((busy=$((th+1));busy<=$NCPU;busy++)); do
	yes >& /dev/null &
	PIDS[$busy]=$!
done

if [ "$SIM" == "GV" ]; then
	cd ${TESTDIR}/GeantV
	./FullCMS \
		--det-set-gdml                 cms2018.gdml           "# gdml file "\
		--gun-set-primary-energy                 50           "# kinetic energy of the primary particle [in GeV]"\
		--gun-set-primary-type                   e-           "# primary particle name/type"\
		--gun-set-primary-per-event               2           "# number of primary particles per event"\
		--field-type                              2           "# field type: 0-no field, 1-uniform field, 2- field map"\
		--field-use-RK                            1           "# use Runge-Kutta propagator"\
		--field-eps-RK                       0.0003           "# RK tolerance error"\
		--field-basketized                        1           "# basketized geometry"\
		--config-number-of-buffered-events      $th           "# number of events transported at once"\
		--config-total-number-of-events         500           "# total number of events to be simulated"\
		--config-number-of-threads              $th           "# number of working threads to be used"\
		--config-number-of-propagators            1           "# number of propagators"\
		--config-run-performance                  0           "# flag to activate performance mode i.e. no scoring"\
		--config-vectorized-geom                  0           "# flag to activate vectorized geometry"\
		--config-vectorized-physics               0           "# flag to activate vectorized physics"\
		--config-vectorized-MSC                   1           "# flag to activate vectorized MSC"\
		--config-external-loop                    0           "# flag to run the application in external loop mode"\
		--config-monitoring                       0           "# flag to activate some monitoring"\
		--config-single-track                     0           "# flag to activate single track mode"\
		--config-tracks-per-basket               16           "# default number of tracks per basket"\
		--config-basket-fld                     512           "# basket size for field"\
		--config-basket-msc                     256           "# basket size for MSC"\
		--config-basket-phy                      64           "# basket size for physics"
	TESTEXIT=$?
elif [ "$SIM" == "G4" ]; then
	unset G4FORCENUMBEROFTHREADS
	cd ${TESTDIR}/Geant4
	cat << EOL_BENCH > mybench.g4
/control/verbose 0
/run/verbose 0
/event/verbose 0
/tracking/verbose 0
/run/numberOfThreads $th
/control/cout/prefixString G4-WORKER
/mydet/useFieldMap true
/run/initialize
/mygun/primaryPerEvt 2
/mygun/particle e-
/mygun/energy  50 GeV
/run/beamOn 500
/process/list
EOL_BENCH
	./full_cms -m mybench.g4 -p
	TESTEXIT=$?
fi

# kill busy processes
for PID in ${PIDS[@]}; do
	kill $PID >& /dev/null
	wait $PID >& /dev/null
done

if [[ $TESTEXIT -ne 0 ]]; then
	exit $TESTEXIT
fi

