#!/usr/bin/env bash
set -xEeuo pipefail

ip="$1"
user='user'
ssh='ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null'

$ssh "$user@$ip" '[ -d /mnt/boot/firmware.new ] || sudo rsync -avP --inplace --modify-window=1 /mnt/boot/firmware/ /mnt/boot/firmware.new/ --delete'

rsync \
	--chown=root:root \
	--inplace \
	--modify-window=1 \
	--progress \
	--recursive \
	--rsh="$ssh" \
	--rsync-path='sudo rsync' \
	build/dist/sentinel.img \
	"$user@$ip:/mnt/boot/firmware.new/"

$ssh "$user@$ip" 'sudo -- bash -Eeuo pipefail -xc "
	cd /mnt/boot/firmware.new
	sed -i -e \"/sentinel[.]img\$/d\" manifest.sha1
	! grep -E "sentinel[.]img" manifest.sha1
	sha1sum ./sentinel.img >> manifest.sha1
	sync
"'
