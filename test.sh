#!/bin/bash

set -e

testdir='target/testdir'
rm -rf "${testdir}"
mkdir "${testdir}"

libprefam="${1}"

function test_command
{
	exec 3>"${testdir}/metafile"
	PREFAM_OUTPUT_FD=3 LD_PRELOAD="${libprefam}" "$@"
	exec 3>&-
}

function check_result
{
	grep -q -e "${1}" "${testdir}/metafile"
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

echo 'Tests PASSED'
