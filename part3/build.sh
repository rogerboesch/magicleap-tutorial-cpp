mabu ctest.package -t device -s $HOME/test
mldb uninstall com.rogerboesch.ctest
mldb install .out/ctest/ctest.mpk
mldb launch com.rogerboesch.ctest