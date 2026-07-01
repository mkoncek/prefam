#!/bin/bash

set -e

################################################################################

testdir='target/testdir'
rm -rf "${testdir}"
mkdir "${testdir}"

function test_command
{
	exec 3>"${testdir}/metafile"
	PREFAM_OUTPUT_FD=3 LD_PRELOAD="${TARGET_LIB}" "$@"
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

echo "Test open"
touch "${testdir}/file_openat" "${testdir}/file_openat_bigfd" "${testdir}/file_openat64"
exec 3>"${testdir}/metafile"
PREFAM_OUTPUT_FD=3 "${TARGET_TEST_BIN_DIR}/test_open" "${testdir}" 2>/dev/null
exec 3>&-
check_result '/file_openat$'
check_result '/file_openat_bigfd$'
check_result '/file_openat64$'
check_result '/dev/null$'
check_result '/dev/zero$'
check_result '/dev/random$'
check_result '/dev/urandom$'

################################################################################

echo "Test exec"
exec 3>"${testdir}/metafile"
PREFAM_OUTPUT_FD=3 "${TARGET_TEST_BIN_DIR}/test_exec" 2>/dev/null
exec 3>&-
check_result '/__prefam_test_execve__$'
check_result '/__prefam_test_execv__$'
check_result '/__prefam_test_execle__$'
check_result '/__prefam_test_execl__$'
check_result '/__prefam_test_posix_spawn__$'

################################################################################

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

################################################################################
# Test record_path_search: failed exec of a nonexistent command.
# When exec fails, the process continues and coverage data is preserved.
# This exercises the execvp interceptor and the PATH iteration loop.
test_command env __prefam_nonexistent__ 2>/dev/null || true

################################################################################
# Test record_path_search: successful match via relative PATH entry.
# A bad-shebang script is found by access() but exec fails (ENOENT from kernel
# for the missing interpreter), so the process exits normally and coverage data
# for the "match found" code path and the relative-PATH branch is preserved.
mkdir -p "${testdir}/bin"
printf '#!/nonexistent_interp\n' > "${testdir}/bin/bad_shebang"
chmod +x "${testdir}/bin/bad_shebang"
exec 3>"${testdir}/metafile"
PATH="${testdir}/bin:/usr/bin" PREFAM_OUTPUT_FD=3 LD_PRELOAD="${TARGET_LIB}" env bad_shebang 2>/dev/null || true
exec 3>&-
check_result '.*bad_shebang$'

################################################################################
# Test record_path_search: path containing '/' delegates to record_path.
# Using a nonexistent relative path so exec fails and coverage is preserved.
test_command env ./__prefam_no_such_script__ 2>/dev/null || true

################################################################################
# Test path containing .. (exercises buffer_derelativize in the library)
test_command cat "${testdir}/../testdir/textfile" > /dev/null
check_result '.*testdir/textfile$'

################################################################################
# Test find -exec which uses openat with directory file descriptors
mkdir -p "${testdir}/finddir"
echo "data" > "${testdir}/finddir/findable"
test_command find "${testdir}/finddir" -name "findable" -exec cat {} \; > /dev/null
check_result '.*findable$'

################################################################################
# Test symbolic link path recording
ln -s textfile "${testdir}/symlink"
test_command cat "${testdir}/symlink" > /dev/null
check_result '.*symlink$'

################################################################################
# Test nonexistent file (path is recorded before the open attempt)
test_command cat "${testdir}/no_such_file" 2>/dev/null || true
check_result '.*no_such_file$'

################################################################################
# Test xargs executing a command (exercises execvp in xargs context)
echo "${testdir}/textfile" | test_command xargs cat > /dev/null
check_result '.*textfile$'

echo 'Tests PASSED'
