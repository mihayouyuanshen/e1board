#!/bin/bash

EXTRA_MAKE_FLAGS=-j4

#build libfrm.a
make ${EXTRA_MAKE_FLAGS} -C frm/ clean
make ${EXTRA_MAKE_FLAGS} -C frm/
make ${EXTRA_MAKE_FLAGS} -C frm/ install

#build wdt and libwdt.a
make ${EXTRA_MAKE_FLAGS} -C wdt/ clean
make ${EXTRA_MAKE_FLAGS} -C wdt/
make ${EXTRA_MAKE_FLAGS} -C wdt/ install

#build mainApp
make -C app/ clean
make -C app/