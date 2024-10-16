# RemoteControlViaEmail

## Requirements (external libraries):
- libcurl
- OpenSSL
- wxWidget

## Prerequisites:
- MinGW Minimal GNU
- CMake
- IDE: CLion (for bundled components, debugger, compiler, builder)

# Libraries installation:

## wxWidget
- Step 1: Install ```wxWidget-X.X.X.zip``` from [wxWidget official website](https://www.wxwidgets.org/downloads/).
- Step 2: Extract into ```root/lib/```.
- (Step 3: Config ```CMakeLists.txt```.)

## libcurl
- Step 1: Install ```curl-X.X.X_X-win64-mingw.zip``` from [libcurl official website](https://curl.se/windows/).
- Step 2: Extract into ```root/lib/```.
- (Step 3: Config ```CMakeLists.txt```.)

## OpenSSL
- Step 1: Install ```Win64 OpenSSL vX.X.X``` from [this website](https://slproweb.com/products/Win32OpenSSL.html).
- Step 2: Run the installer and locate its directory.
- Step 3: Copy ```OpenSSL-Win64``` and place it into ```root/lib/```.
- (Step 4: Config ```CMakeLists.txt```.)

# Setup

- Install ```ca-bundle.crt``` from [this website](https://github.com/bagder/ca-bundle/blob/master/ca-bundle.crt).
- Place the file into ```root/cert/```
- Config your desired credentials inside the ```config.ini``` file (use [app password](https://support.google.com/accounts/answer/185833?hl=en) for password).