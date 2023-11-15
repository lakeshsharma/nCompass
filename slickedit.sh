#!/usr/bin/env bash
set -Eeuo pipefail

# Run SlickEdit using Docker.

: "${DOCKER_SYSTEM_TARGET:?not set -- expected value like lite-i386, pt316-amd64, yocto, etc: DOCKER_SYSTEM_TARGET=lite-i386 $0 $*}"

# https://jenkins.systems.aristocrat.com/job/ncompass-linux-firmware/job/master/
: "${NCOMPASS_FIRMWARE_VERSION:=$(< ncompass-firmware-version)}"

windows=
case "$(uname -o)" in
	Msys) windows=1 ;;
esac

_abs() {
	if [ -n "$windows" ] && command -v cygpath > /dev/null; then
		cygpath --absolute --windows "$@"
	else
		readlink -f "$@"
	fi
}

dir="$(_abs "$PWD")"
home="$(_abs "$HOME")"

mkdir -p "$HOME/.slickedit"

license=
for tryLicense in \
	"$PWD/slickedit.lic" \
	"$HOME/.slickedit/slickedit.lic" \
	/opt/slickedit/bin/slickedit.lic \
; do
	if [ -s "$tryLicense" ]; then
		license="$tryLicense"
		break
	fi
done
if [ ! -s "$license" ]; then
	echo >&2 "error: failed to find 'slickedit.lic' file"
	echo >&2 "  once acquired, place it at '$HOME/.slickedit/slickedit.lic'"
	exit 1
fi
license="$(_abs "$license")"

args=(
	--name slickedit
	--hostname "docker-slickedit-$DOCKER_SYSTEM_TARGET"
	--init

	# see also --tty behavior below
	--interactive --rm

	# extra / prefixes on following paths are for the sake of Windows (to stop MinGW from "helpfully" converting them to C:\... style paths)

	--mount type=bind,src="$license",dst=//opt/slickedit/bin/slickedit.lic,readonly

	--mount type=bind,src="$home/.slickedit",dst="/$HOME/.slickedit"
	--env HOME="/$HOME"

	--mount type=bind,src="$dir",dst="/$PWD"
	--workdir "/$PWD"

	--env DOCKER_SYSTEM_TARGET
	--env NCOMPASS_DEBUG_IMAGE

	--env BUILD_NUMBER="${BUILD_NUMBER:-99}"
	--env MAKEFLAGS="-j4 ${MAKEFLAGS:-}"
)

# on Windows, UID/GID mapping in Docker containers doesn't matter (Docker handles that with their filesystem sharing)
if [ -n "$windows" ]; then
	args+=( --user root )
else
	user="$(id -u):$(id -g)"
	groups="$(id -G | xargs -rn1 echo --group-add)"
	args+=(
		--mount type=bind,src=//etc/passwd,dst=//etc/passwd,readonly
		--mount type=bind,src=//etc/group,dst=//etc/group,readonly
		--user "$user"
		$groups
	)
fi

# add bits for X11 (graphical) support
args+=(
	--env DISPLAY="${DISPLAY:-:0}"
	--ipc host
	--net host
)
if [ -d /tmp/.X11-unix ]; then
	args+=( --mount type=bind,src=//tmp/.X11-unix,dst=//tmp/.X11-unix,readonly )
fi

# peel off some layers for the Debugger
args+=(
	--cap-add SYS_PTRACE
	--security-opt seccomp=unconfined
)
if docker info 2>/dev/null | grep -qi apparmor; then
	args+=(
		--security-opt apparmor=unconfined
	)
fi

dockerRun='docker run'
if [ -t 0 ] && [ -t 1 ]; then
	args+=( --tty )

	if [ -n "$windows" ] && command -v winpty > /dev/null; then
		dockerRun="winpty $dockerRun"

		# Docker Toolbox creates a "docker" function to disable Git for Windows MSYS path conversions, but we handle those all ourselves instead (and work around MSYS auto-path conversion) since we need to for Docker for Windows (where MSYS is not the expected usage)
		unset docker &> /dev/null || :
	fi
fi

DOCKER_IMAGE="us-west1-docker.pkg.dev/cxs-docker-repository/cxs-docker-dev/ncompass/firmware:$NCOMPASS_FIRMWARE_VERSION"
if [ "$DOCKER_SYSTEM_TARGET" = 'yocto' ]; then
	DOCKER_IMAGE_TARGET="$DOCKER_SYSTEM_TARGET-i386"
else
	DOCKER_IMAGE_TARGET="$DOCKER_SYSTEM_TARGET"
fi
DOCKER_IMAGE="$DOCKER_IMAGE-${DOCKER_IMAGE_TARGET##*-}-slickedit-${DOCKER_IMAGE_TARGET%%-*}"

# allow running "bash slickedit.sh bash" to get an interactive shell in the container environment
# (otherwise, we assume arguments should be passed directly to "vs")
if [ "${1:-}" != 'bash' ]; then
	set -- vs "$@"
fi
exec $dockerRun "${args[@]}" "$DOCKER_IMAGE" bash .docker-entrypoint.sh -- "$@"
