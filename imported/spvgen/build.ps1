Push-Location

Push-Location external
python fetch_external_sources.py
Pop-Location

cmake -S . -B build
cmake --build build --target spvgen --config Release

Pop-Location
