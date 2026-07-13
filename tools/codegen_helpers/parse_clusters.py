#!/usr/bin/env python3
"""Parses a ZAP input file and outputs directories to compile."""

import argparse
import pathlib
import glob
import sys
import os

excluded_files = [
    "TemporaryTestCoupling.cpp",
    "access-control-server/ArlEncoder.cpp",
]

def parse_zapfile_clusters(cluster_file, chip_path):
    """Prints all of the source directories to build for a given ZAP file.
    Arguments:
      zap_file_path - Path to the ZAP input file.
    """
    # Backup current working directory
    cwd = os.getcwd()

    # Store all the clusters in a list
    f = open(cluster_file, 'r')
    cluster_list = f.read().splitlines()
    f.close()

    # Open file, prepend filepath and append .cpp extension to every cluster in the list, write to file
    with open(cluster_file, 'w') as f:
        os.chdir(f'{chip_path}/src/app/clusters/')
        chip_cluster_path = os.getcwd()

        for cluster in cluster_list:
            patterns = [
                f"{chip_cluster_path}/{cluster}/*.cpp",
                f"{chip_cluster_path}/{cluster}/codegen/*.cpp",
            ]

            for pattern in patterns:
                for clusters_cpp in glob.glob(pattern):
                    cpp_filename = os.path.basename(clusters_cpp)

                    if cpp_filename in excluded_files:
                        continue

                    relative_path = os.path.relpath(clusters_cpp, chip_cluster_path)
                    if relative_path in excluded_files:
                        continue

                    f.write(clusters_cpp + '\n')

    os.chdir(cwd)
    f.close()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cluster_file',
                        help='Path to intermediate file containing cluster list',
                        required=True,
                        type=pathlib.Path)
    parser.add_argument('--chip_path',
                        help='Path to connectedhomeip sdk',
                        required=True,
                        type=pathlib.Path)

    args = parser.parse_args()

    parse_zapfile_clusters(args.cluster_file, args.chip_path)

    sys.exit(0)


if __name__ == '__main__':
    main()
