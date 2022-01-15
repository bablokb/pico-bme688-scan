pico-bme688-scan
================

Read the BME688-sensor in _forced-mode_ on a Raspberry Pi Pico using the official API of Bosch-Sensortec.

The program takes multiple readings using different heat/duration-combinations to scan the
parameter-space for sensitive parameter values.

You need to configure `CMakeLists.txt` for

  - hardware-setup
  - altitude at your location
  - heat/duration parameters to scan


Preparation
===========

After checkout, run

    git submodule update --init --recursive

This pulls in the [bme688-library-project](https://github.com/bablokb/pico-bme688) and the git-repo from Bosch-Sensortec.


Build
=====

Either use VSCode (preferred) or use:

    mkdir -p build
    cd build
    cmake ..
    make
    cd ..

The executable is `build/pico-bme688-scan.uf2`.
