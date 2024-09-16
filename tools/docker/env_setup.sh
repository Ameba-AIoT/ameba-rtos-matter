#!/usr/bin/env bash
set -e

basedir=$1
chipdir=${basedir}/connectedhomeip

cd ${chipdir}
source ${chipdir}/scripts/bootstrap.sh
source ${chipdir}/scripts/activate.sh
cd ${basedir}
