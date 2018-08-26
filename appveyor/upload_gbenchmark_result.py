import argparse
import subprocess
import xml.etree.ElementTree as etree

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--gtest_xml_output", help="XML filename from gtest output")
    parser.add_argument("--debug", action='store_true', help="Echo output instead of sending to appveyor")

    args = parser.parse_args()

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
