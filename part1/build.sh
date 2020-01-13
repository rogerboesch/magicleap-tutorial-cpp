mabu ctest.package -t device -s $HOME/test
mldb uninstall com.rogerboesch.test
mldb install .out/ctest/ctest.mpk
mldb launch com.rogerboesch.ctest