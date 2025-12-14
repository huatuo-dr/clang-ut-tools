#!/usr/bin/env python3
"""
Unity output to JUnit XML converter.

Usage:
    python3 unity_to_junit.py input.txt -o output.xml
    cat test_output.txt | python3 unity_to_junit.py -o output.xml
"""

import sys
import re
import argparse
from xml.etree import ElementTree as ET
from xml.dom import minidom

def parse_unity_output(content):
    """Parse Unity test output and return test results."""
    results = []
    # Unity output format: file:line:test_name:STATUS[:message]
    pattern = r'^(.+?):(\d+):([^:]+):(PASS|FAIL|IGNORE)(?::(.*))?$'

    for line in content.split('\n'):
        match = re.match(pattern, line.strip())
        if match:
            results.append({
                'file': match.group(1),
                'line': match.group(2),
                'name': match.group(3),
                'status': match.group(4),
                'message': match.group(5) or ''
            })

    return results

def results_to_junit_xml(results, suite_name='Unity Tests'):
    """Convert test results to JUnit XML format."""
    # Create root element
    testsuites = ET.Element('testsuites')

    # Group by file
    files = {}
    for r in results:
        if r['file'] not in files:
            files[r['file']] = []
        files[r['file']].append(r)

    for file_name, tests in files.items():
        # Create testsuite element
        testsuite = ET.SubElement(testsuites, 'testsuite')
        testsuite.set('name', file_name)
        testsuite.set('tests', str(len(tests)))

        failures = sum(1 for t in tests if t['status'] == 'FAIL')
        skipped = sum(1 for t in tests if t['status'] == 'IGNORE')

        testsuite.set('failures', str(failures))
        testsuite.set('skipped', str(skipped))
        testsuite.set('errors', '0')

        for test in tests:
            testcase = ET.SubElement(testsuite, 'testcase')
            testcase.set('name', test['name'])
            testcase.set('classname', file_name)
            testcase.set('time', '0.000')

            if test['status'] == 'FAIL':
                failure = ET.SubElement(testcase, 'failure')
                failure.set('message', test['message'])
                failure.text = f"File: {test['file']}, Line: {test['line']}"
            elif test['status'] == 'IGNORE':
                skipped_elem = ET.SubElement(testcase, 'skipped')
                skipped_elem.set('message', test['message'])

    return testsuites

def prettify_xml(elem):
    """Return a pretty-printed XML string."""
    rough_string = ET.tostring(elem, encoding='unicode')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

def main():
    parser = argparse.ArgumentParser(
        description='Convert Unity test output to JUnit XML format'
    )
    parser.add_argument('input', nargs='?', type=str, default='-',
                        help='Input file (default: stdin)')
    parser.add_argument('-o', '--output', type=str, default='unity_result.xml',
                        help='Output XML file (default: unity_result.xml)')
    parser.add_argument('-n', '--name', type=str, default='Unity Tests',
                        help='Test suite name')

    args = parser.parse_args()

    # Read input
    if args.input == '-':
        content = sys.stdin.read()
    else:
        with open(args.input, 'r') as f:
            content = f.read()

    # Parse and convert
    results = parse_unity_output(content)

    if not results:
        print("Warning: No test results found in input", file=sys.stderr)
        sys.exit(1)

    xml_root = results_to_junit_xml(results, args.name)

    # Write output
    xml_string = prettify_xml(xml_root)
    with open(args.output, 'w') as f:
        f.write(xml_string)

    # Print summary
    total = len(results)
    passed = sum(1 for r in results if r['status'] == 'PASS')
    failed = sum(1 for r in results if r['status'] == 'FAIL')
    ignored = sum(1 for r in results if r['status'] == 'IGNORE')

    print(f"Converted {total} tests: {passed} passed, {failed} failed, {ignored} ignored")
    print(f"Output: {args.output}")

if __name__ == '__main__':
    main()
