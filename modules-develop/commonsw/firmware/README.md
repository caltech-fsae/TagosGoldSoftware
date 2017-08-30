# Caltech FSAE Firmware
This repository contains all firmware and board support for our hardware. This is a regular Git repository that should be updated using Gitflow.

## Organization

config/ - Contains configuration constants for each board.
inc/ - Include files that contain the interface exposed to software. Software should *only* be including these.
src/ - Source files and include files for internal firmware implementation. Worth looking at to understand how something works, but should not be used directly.
unitTest/ - Tests for functionality that runs on the hardware. Also serves as an example of how to use functionality.
funcTests/ - Tests that run through Unity.
