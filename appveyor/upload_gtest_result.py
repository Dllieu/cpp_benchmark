import argparse
import subprocess
import xml.etree.ElementTree as etree

def format_filename(testsuite_name):
    formalizedName = ''.join(['_{}'.format(c.lower()) if c.isupper() else c for c in testsuite_name])
    formalizedName = formalizedName.replace('_test', '')

    return 'test{}.cpp'.format(formalizedName)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--gtest_xml_output", help="XML filename from gtest output")
    args = parser.parse_args()

    testsuites = etree.parse(args.gtest_xml_output)

    appVeyorTestReportTemplate = 'appveyor AddTest {} -Framework NUnit -Filename {} -Outcome {} -Duration {}'
    appVeyorTestReportResults = []

    for testsuite in testsuites.getroot():
        for testcase in testsuite:
          outcome = 'Passed' if 'run' == testsuite.get('status') else 'Failed'
          testcaseName = '{}/{}'.format(testsuite.get('name'), testcase.get('name'))

          appVeyorTestReportResults.append(appVeyorTestReportTemplate.format(testcaseName, format_filename(testsuite.get('name')), outcome, testsuite.get('time')))

    appVeyorTestReportResults.sort()

    for appVeyorTestReportResult in appVeyorTestReportResults:
        subprocess.call(appVeyorTestReportResult, shell=True)
