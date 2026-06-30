#!/bin/bash

set -e

################################################################################

testdir='target/testdir'
rm -rf "${testdir}"
mkdir "${testdir}"

function test_command
{
	exec 3>"${testdir}/metafile"
	PREFAM_OUTPUT_FD=3 LD_PRELOAD="${TARGET_LIB_DIR}/libprefam.so" "$@"
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

################################################################################
# Unit tests

echo "Test derelativize"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "a" "a"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "." "."
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a" "/a"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/" "/."
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a" "/./a"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/.a" "/./.a"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a." "/./a."
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a/" "/./a/"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a" "/./a"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a/" "/./a/"

"${TARGET_TEST_BIN_DIR}/test_derelativize" "/" "/a/.."
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/" "/a/b/../.."
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/" "/a/../b/.."
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/" "/.."
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a" "/../a"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a/c" "/a/b/../c"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a/b/c" "/a/./b/./c"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a/.b" "/a/.b"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a/..b" "/a/..b"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/.a" "/.a/.b/.."
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/c" "/a/b/../../c"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/b" "/a/../../b"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a/b/c" "/a/b/c"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a/c/d" "/a/b/../c/./d"
"${TARGET_TEST_BIN_DIR}/test_derelativize" "/a/d" "/a/b/../c/../d"

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
