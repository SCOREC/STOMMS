#/bin/bash

# Step 1: First run doxygen inside /STOMMS/build
# Make sure Doxyfile is in build folder. If not run config-files according to your system.
echo "Running Doxygen ..."
cd ../build
doxygen ../build/Doxyfile

# Step 2: Run python envirnoment script (if not already loaded). Make sure to update 
# requirements.txt and env_sphinx.sh according to the python version available on the 
# system.
cd ../docs
rm -rf build # clear the cache
if [ -z "$VIRTUAL_ENV"]; then 
  echo "Activating pyEnv ..."
  source env_sphinx.sh
else
  echo "pyEnv already activated"
fi

# Step 3: Make sphinx html
echo "Running Sphinx to generate html ..."
make html

# The output html files are in /STOMMS/docs/build.
