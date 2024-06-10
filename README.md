# TAI Assignment 3 - Compression-based method for Music Classification

## Video
**Link:** [https://www.youtube.com/watch?v=tpOJMJ76UCI](https://www.youtube.com/watch?v=tpOJMJ76UCI)

[YouTube could still be processing the video]

## Description

This third assignment's goal was to explore the usage of a compression-based method for music classification. This repository contains the [report](report/report.pdf), and the source code and [executable](bin/what_the_music) of the `what_the_music` program.

**Course:** Algorithmic Theory of Information (2023/2024)

## The `what_the_music` program

The `what_the_music` program is a command-line tool whose purpose is to automatically identify musics using small samples for querying the database. It computes the Normalised Compression Distance (NCD) between a query sample and each music in the database. The query sample is assigned the name of the music in the database that yields the smallest value of the NCD. It is possible to query multiple samples in a single execution of the program, since it considers a whole folder that is passed as an argument.

Before executing the program, one should generate signatures for the music in the database and the query samples with the `GetMaxFreqs` program. It outputs a file with the `.freqs` extension for each file, which the only accepted extension type in the `what_the_music` program.

### Compile

It's optional, since you can use the provided executable inside the `bin` directory.

- Run `cd bin` in root.
- Run `cmake .. && make` to compile the program.

### Execute

- Run `cd bin` in root to change to the executable's directory.
- Run `./what_the_music REQUIRED OPTIONAL` to execute the program.

#### Required arguments

- `-d database_folder`: folder containing the music signatures to be inserted in the database (string).
- `-q queries_folder`: folder containing the music signatures to be queried on the database (string).
- `-c compression_method`: compression method of the music signatures (string). Supported compressors are`gzip`, `bzip2`, `lzma` and `zstd`.
- `-k top_k_results`: number of top-k resutls to be returned (int).

#### Optional arguments

- `-h`: shows how to use the program.
- `-d reduce_factor`: factor to reduce the counts of the Markov model to prevent overflow (int, default is 2).
- `-l log_file_path`: path to the file where the log will be written (string, default is empty).
- `-t temp_folder`: folder to store temporary files (string, default is `queries_folder`).

#### Example

`./what_the_music -d ../example/database -q ../example/queries -c lzma -k 10`

## Authors

- Diogo Paiva, 103183
- João Fonseca, 103154
- Rafael Gonçalves, 102534
