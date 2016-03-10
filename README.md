# Clusterboot

A simple boot loader for large scale OpenPOWER based clusters.

## Concept

A small C program, `clusterbootstrap`, waits for network availability then uses UDP broadcasts to request a URL to a bootstrap script from a `clusterbootmaster`. The bootstrap script is a simple bash file that completes any user-defined boot procedure.
