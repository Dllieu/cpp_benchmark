import argparse
import pandas as pd
import subprocess

def setup_args():
    parser = argparse.ArgumentParser()
    parser._action_groups.pop()

    requiredArguments = parser.add_argument_group('required arguments')
    requiredArguments.add_argument("--googlebenchmark_csv", help="CSV filename from googlebenchmark output", required=True)

    optionalArguments = parser.add_argument_group('optional arguments')
    optionalArguments.add_argument("--debug", action='store_true', help="Echo output instead of sending to appveyor")

    return parser.parse_args()

def find_csv_header_index(csvFile):
    with open(csvFile) as f:
        index = 0
        for line in f.readlines():
            if line.startswith('name,iterations'):
                return index

            index += 1

    raise Exception('No header found! [File={}]'.format(csvFile))

if __name__ == "__main__":
    args = setup_args()

    bla = pd.read_csv(args.googlebenchmark_csv, header=find_csv_header_index(args.googlebenchmark_csv))
    print(bla)

    subprocess.call('echo appveyor PushArtifact BLABLA.JPG', shell=True)
