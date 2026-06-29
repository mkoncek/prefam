#!/bin/bash

set -ex

################################################################################
# Unit tests

${test_derelativize} "a" "a"
${test_derelativize} "." "."
${test_derelativize} "/a" "/a"
${test_derelativize} "/" "/."
${test_derelativize} "/a" "/./a"
${test_derelativize} "/.a" "/./.a"
${test_derelativize} "/a." "/./a."
${test_derelativize} "/a/" "/./a/"
${test_derelativize} "/a" "/./a"
${test_derelativize} "/a/" "/./a/"

${test_derelativize} "/" "/a/.."
${test_derelativize} "/" "/a/b/../.."
${test_derelativize} "/" "/a/../b/.."
${test_derelativize} "/" "/.."
${test_derelativize} "/a" "/../a"
${test_derelativize} "/a/c" "/a/b/../c"
${test_derelativize} "/a/b/c" "/a/./b/./c"
${test_derelativize} "/a/.b" "/a/.b"
${test_derelativize} "/a/..b" "/a/..b"
${test_derelativize} "/.a" "/.a/.b/.."
${test_derelativize} "/c" "/a/b/../../c"
${test_derelativize} "/b" "/a/../../b"
${test_derelativize} "/a/b/c" "/a/b/c"
${test_derelativize} "/a/c/d" "/a/b/../c/./d"
${test_derelativize} "/a/d" "/a/b/../c/../d"

################################################################################

testdir='target/testdir'
rm -rf "${testdir}"
mkdir "${testdir}"

function test_command
{
	exec 3>"${testdir}/metafile"
	PREFAM_OUTPUT_FD=3 LD_PRELOAD="${libprefam}" "$@"
	exec 3>&-
}

function check_result
{
	if ! grep -q -e "${1}" "${testdir}/metafile"; then
		echo "No pattern ${1} found; content of the file is:"
		cat "${testdir}/metafile"
		return 1
	fi
}

echo "content" > "${testdir}/textfile"
test_command cat "${testdir}/textfile" > /dev/null
check_result '.*'"${testdir}"'/textfile$'

test_command touch "${testdir}/file"
check_result '.*'"${testdir}"'/file$'

test_command touch "${testdir}/file"
check_result '.*'"${testdir}"'/file$'

echo '#!/bin/bash
true' > "${testdir}/executable.sh"
chmod a+x "${testdir}/executable.sh"
test_command "./${testdir}/executable.sh"
check_result '.*'"${testdir}"'/executable.sh$'

test_command touch "$(pwd)/${testdir}/file"
check_result '.*'"${testdir}"'/file$'

echo 'Tests PASSED'
