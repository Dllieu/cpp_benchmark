import argparse
import subprocess
import xml.etree.ElementTree as etree

def setup_args():
    parser = argparse.ArgumentParser()
    parser._action_groups.pop()

    requiredArguments = parser.add_argument_group('required arguments')
    requiredArguments.add_argument("--gtest_xml", help="XML filename from gtest output", required=True)

    optionalArguments = parser.add_argument_group('optional arguments')
    optionalArguments.add_argument("--debug", action='store_true', help="Echo output instead of sending to appveyor")

    return parser.parse_args()

def format_filename(testsuite_name):
    formalizedName = ''.join(['_{}'.format(c.lower()) if c.isupper() else c for c in testsuite_name])
    formalizedName = formalizedName.replace('_test', '')

    return 'test{}.cpp'.format(formalizedName)

if __name__ == "__main__":
    args = setup_args()

    testsuites = etree.parse(args.gtest_xml_output)

    appVeyorTestReportTemplate = 'appveyor AddTest {} -Framework NUnit -Filename {} -Outcome {} -Duration {}'
    if args.debug:
        appVeyorTestReportTemplate = 'echo {}'.format(appVeyorTestReportTemplate)

    appVeyorTestReportResults = []

    for testsuite in testsuites.getroot():
        for testcase in testsuite:
          outcome = 'Passed' if 'run' == testcase.get('status') else 'Failed'
          testcaseName = '{}/{}'.format(testsuite.get('name'), testcase.get('name'))

          appVeyorTestReportResults.append(appVeyorTestReportTemplate.format(testcaseName, format_filename(testsuite.get('name')), outcome, int(float(testsuite.get('time')))))

    # Useless as appveyor don't receive them in order
    appVeyorTestReportResults.sort()

    for appVeyorTestReportResult in appVeyorTestReportResults:
        subprocess.call(appVeyorTestReportResult, shell=True)
