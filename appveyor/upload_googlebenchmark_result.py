import matplotlib
matplotlib.use('Agg')

import argparse
import matplotlib.pyplot as plt
import numpy as np
import os
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

def get_input_from_name(name):
    #<input>
    templateInputStart = name.find('<')
    if (-1 != templateInputStart):
        return name[templateInputStart + 1:name.find('>')]

    #/input (but can have /threads:n or /input/threads:n)
    argInputStart = name.find('/')
    if (-1 != argInputStart):
        normalizedResult = name[argInputStart + 1:]
        argInputStart = normalizedResult.find('/')

        if (normalizedResult.startswith('threads')):
            return "0"

        argInputStart = normalizedResult.find('/')
        if (-1 != argInputStart):
            return normalizedResult[:argInputStart]
        else:
            return normalizedResult

    return "0"

def get_testname_from_name(name):
    testname = name[name.index('_') + 1:]
    placeHolder1 = testname.find('<')
    placeHolder2 = testname.find('/')

    if (-1 != placeHolder1) and (-1 != placeHolder2):
        return testname[:min(placeHolder1, placeHolder2)]
    elif (-1 != placeHolder1):
        return testname[:placeHolder1]
    elif (-1 != placeHolder2):
        return testname[:placeHolder2]
    else:
        return testname

if __name__ == "__main__":
    args = setup_args()

    df = pd.read_csv(args.googlebenchmark_csv, header=find_csv_header_index(args.googlebenchmark_csv))

    # CATEGORY_TESTNAMEBenchmark[<input>][/input][/more info]
    df['category'] = df.name.map(lambda x: x[:x.index('_')])
    df['testname'] = df.name.map(lambda x: get_testname_from_name(x))
    df['data_size'] = df.name.map(lambda x: get_input_from_name(x))
    df.set_index('data_size', inplace=True)
    df.index = df.index.astype(int)

    for category in df.category.unique():
        category_df = df[df.category == category][['testname', 'real_time']]
        category_df

        fig, ax = plt.subplots()

        fig.set_figheight(7)
        fig.set_figwidth(11)

        ax.set_title(''.join([' {}'.format(c) if c.isupper() else c for c in category]))
        ax.set_xscale('log', basex=2)
        ax.set_ylabel('real_time')

        for testname in category_df.testname.unique():
            testname_df = category_df[category_df.testname == testname]['real_time']
            testname_df.plot(ax=ax, label=testname, legend=True)

        ax.grid(b=True, which='major', color='black', alpha=0.2, linestyle='--')

        filename = os.path.join(os.getcwd(), 'googlebenchmark_graph_{}.png'.format(category))
        fig.savefig(filename)
        subprocess.call('{} appveyor PushArtifact {}'.format('echo ' if args.debug else '', filename), shell=True)
